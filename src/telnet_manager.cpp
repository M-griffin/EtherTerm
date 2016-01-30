#include "telnet.hpp"
#include "telnet_manager.hpp"
#include "session.hpp"

#include <boost/algorithm/string.hpp>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

TelnetManager::TelnetManager(session_ptr session)
    : m_weak_session(session)
    , m_teloptStage(0)
    , m_teloptCommand(0)
    , m_isSGACompleted(false)
    , m_isBINCompleted(false)
    , m_isECHOCompleted(true) // Default to True for MUD'S etc.
    , m_is_active_sga(false)
    , m_is_active_ttype(false)
    , m_is_active_naws(false)
    , m_is_active_bin(false)
    , m_is_active_echo(false)
    , m_isShutdown(false)
    , m_currentOption(0)
{
    std::cout << "TelnetManager Created." << std::endl;
}

TelnetManager::~TelnetManager()
{
    std::cout << "~TelnetManager" << std::endl;
}

/**
 * @brief Handles Telnet Options and Replies, Returns text data
 * @param msg_buffer
 * @return
 */
std::string TelnetManager::parseIncomingData(const std::vector<unsigned char> &msg_buffer)
{
    std::string output;
    unsigned char ch = 0;

// Debugging, write all data received to file
//    std::fstream sout;
//    sout.open( "screen.ans", ios::out | ios::app );
//    sout << msgBuffer << std::flush;
//    sout.close();

    // Make sure buffer isn't empty
    if (msg_buffer.size() == 0)
    {
        return "";
    }

    // Loop through data for IAC Telnet Commands (Unsigned Characters)
    for (auto c : msg_buffer)
    {
        // for debugging server output, mainly esc sequence
        // and output that might mess up screens when needed.
#ifdef _DEBUG
        if(c == '\r')
            std::cout << "^r" << std::flush;
        else if(c == '\n')
            std::cout << "^n" << std::endl;
        else if(c == '\0')
        {
            //std::cout << "^0" << std::flush;
            //continue;
        }
        else
            std::cout << c << std::flush;
#endif
        try
        {
            // Run the Telnet Options Parser
            ch = telnetOptionParse(c);
        }
        catch(std::exception& e)
        {
            std::cout << "Exception telnet_process_char: " << e.what() << std::endl;
        }

        // Skip all NULL Bytes, returned on IAC characters.

        // Also:
        // Synchronet even with suppress go-ahead
        // Sends annoying amount of null bits Not "\r\0"
        // because the \0 in rows of 50 or so!
        // when waiting for input!
        if(ch == '\0')
        {
            continue;
        }
        // Append to buffer
        output += ch;
    }

    // If available, push Data through ANSI Passer then Screen.
    if(output.size() > 0)
    {
        return output;
    }
    output.erase();
    return "";
}

/**
 * @brief Telnet Acknowledge Responses.
 * @param cmd
 * @return
 */
unsigned char TelnetManager::telnetOptionAcknowledge(unsigned char cmd)
{
    switch(cmd)
    {
        case DO:
            return WILL;
        case DONT:
            return WONT;
        case WILL:
            return DO;
        case WONT:
            return DONT;
    }
    return 0;
}

/**
 * @brief Telnet Deny Responses.
 * @param cmd
 * @return
 */
unsigned char TelnetManager::telnetOptionDeny(unsigned char cmd)
{
    switch(cmd)
    {
        case DO:
            return WONT;
        case DONT:
            return WILL;
        case WILL:
            return DONT;
        case WONT:
            return DO;
    }
    return 0;
}

/**
 * @brief Sends IAC Sequence reply for NAWS or Terminal Window Size 80x50
 */
void TelnetManager::telnetOptionNawsReply()
{
    session_ptr session = m_weak_session.lock();
    if(session)
    {
        std::stringstream stm;
        std::string       buf;

        stm << static_cast<char>(IAC);
        stm << static_cast<char>(SB);
        stm << static_cast<char>(TELOPT_NAWS);

        //I believe it's [0] + [1] * 256 for resolution.  Fix this!!

        // If Col Width above 255, split into 255 and remaining
        stm << static_cast<char>(
                (session->m_renderer->m_term_width) > 255 ?
                session->m_renderer->m_term_width - 255 : 0
            );

        stm << static_cast<char>(
                (session->m_renderer->m_term_width) > 255 ?
                255 : session->m_renderer->m_term_width
            );

        // If Row Height above 255, split into 255 and remaining
        stm << static_cast<char>(
                (session->m_renderer->m_term_height) > 255 ?
                session->m_renderer->m_term_height - 255 : 0
            );

        stm << static_cast<char>(
                (session->m_renderer->m_term_height) > 255 ?
                255 : session->m_renderer->m_term_height
            );

        stm << static_cast<char>(IAC);
        stm << static_cast<char>(SE);
        buf = stm.str();

        if(session->m_connection->is_open())
        {
            session->deliver(buf);
        }
        else
        {
            std::cout << "Error: TelnetManager telnetOptionNawsReply()" << std::endl;
            m_isShutdown = true;
        }
    }
}

/**
 * @brief Sends IAC Sequence reply for Terminal Type
 */
void TelnetManager::telnetOptionTerminalTypeReply()
{
    session_ptr session = m_weak_session.lock();
    if(session)
    {
        // Grab the Terminal Type, make sure we send lowercase.
        std::string terminal = session->m_system_connection->termType;
        boost::algorithm::to_lower(terminal);

        std::cout << "TermType: " << terminal << std::endl;

        std::stringstream stm;
        std::string       buf;

        stm << static_cast<char>(IAC);
        stm << static_cast<char>(SB);
        stm << static_cast<char>(TELOPT_TTYPE);
        stm << static_cast<char>(TELQUAL_IS);
        stm << terminal;
        stm << static_cast<char>(IAC);
        stm << static_cast<char>(SE);
        buf = stm.str();

        if(session->m_connection->is_open())
        {
            session->deliver(buf);
        }
        else
        {
            std::cout << "Error: TelnetManager telnetOptionTerminalTypeReply()" << std::endl;
            m_isShutdown = true;
        }
    }
}

/**
 * @brief Sends IAC Sequence Responses back to Server
 * @param command
 * @param option
 */
void TelnetManager::telnetSendIAC(unsigned char command, unsigned char option)
{
    session_ptr session = m_weak_session.lock();
    if(session)
    {
        std::stringstream stm;
        std::string       buf;

        stm << static_cast<char>(IAC);
        stm << command;
        stm << static_cast<char>(option);
        buf = stm.str();

        if(session->m_connection->is_open())
        {
            session->deliver(buf);
        }
        else
        {
            std::cout << "Error: TelnetManager telnetSendIAC()" << std::endl;
            m_isShutdown = true;
        }
    }
}


/**
 * @brief  Parses Telnet Options negotiations
 * @param c
 * @return
 */
unsigned char TelnetManager::telnetOptionParse(unsigned char c)
{
    // TEL-OPT Parser
    switch(m_teloptStage)
    {
            // Find IAC
        case 0:
            // Check first Char in Fresh Sequence
            if(c != IAC)
            {
                // Add character to output buffer.
                return c;
            }
            else
            {
                m_teloptStage++;
            }
            break;

            // Find Command
        case 1:
            if(c == IAC)
            {
                // If binary, then we expect double IAC for actual characer
                if (m_is_active_bin)
                {
                    std::cout << "\r\n Got double IAC w/ Binary!!\r\n" << std::endl;
                    m_teloptStage = 0;
                    return IAC;
                }
                else
                {
                    // Restart on next character
                    // Double IAC = IAC character, not valid so skip it for now.
                    // Telnet states binary mode, double IAC mean pass through
                    // Char 255.  But this doesn't equal any text in ExtASCII
                    // So we going to stuff it.
                    std::cout << "\r\n Got double IAC!!\r\n" << std::endl;
                    break;
                }
            }
            if(c != IAC)
            {
                switch(c)
                {
                        // Catch Pass-through commands.
                    case GA:    //     249        /* you may reverse the line */
                    case EL:    //     248        /* erase the current line */
                    case EC:    //     247        /* erase the current character */
                    case AYT:   //     246        /* are you there */
                    case AO:    //     245        /* abort output--but let prog finish */
                    case IP:    //     244        /* interrupt process--permanently */
                    case BREAK: //     243        /* break */
                    case DM:    //     242        /* data mark--for connect. cleaning */
                    case NOP:   //     241        /* nop */
                    case SE:    //     240        /* end sub negotiation */
                    case EOR:   //     239        /* end of record (transparent mode) */
                    case ABORT: //     238        /* Abort process */
                    case SUSP:  //     237        /* Suspend process */
                    case xEOF:  //     236        /* End of file: EOF is already used... */
                        // Pass Through commands that don't need Response.
                        std::cout << "[IAC] [" << (int)c << "] PASS-THROUGH" << std::endl;
                        m_teloptStage = 0;
                        break;

                    default:
                        // Move to Command Parsing.
                        m_teloptCommand = c;
                        m_teloptStage++;
                        break;
                }
            }
            break;

            // Find Option
        case 2:
            m_teloptStage = 0;
            // Catch if were getting Invalid Commands.
            if(TELCMD_OK(m_teloptCommand))
                std::cout << "[IAC] [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
            else
            {
                // Hopefully won't get here!
                std::cout << "[IAC] [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                m_teloptStage = 0;
                break;
            }
            switch(m_teloptCommand)
            {
                    // No responses needed, just stuff these.
                case DONT:
                    std::cout << "[IAC] RECEIVED DONT [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                    switch(c)
                    {
                        case IAC :
                            m_teloptStage = 1;
                            break;

                        default:
                            std::cout << "[IAC] DONT -> WONT [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                            telnetSendIAC(telnetOptionAcknowledge(m_teloptCommand),c);
                            m_teloptStage = 0;
                            break;
                    }
                    break;

                case DO: // Replies WILL / WON'T
                    std::cout << "[IAC] RECEIVED DO [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                    switch(c)
                    {
                        case TELOPT_ECHO:
                            std::cout << "[IAC] DO TELOPT_ECHO [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                            telnetSendIAC(telnetOptionDeny(m_teloptCommand),c);
                            m_is_active_echo = true;
                            m_isECHOCompleted = true;
                            break;

                        case TELOPT_BINARY:
                            std::cout << "[IAC] DO TELOPT_BINARY [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                            telnetSendIAC(telnetOptionAcknowledge(m_teloptCommand),c);
                            m_isBINCompleted = true;
                            m_is_active_bin = true;
                            break;

                        case TELOPT_SGA:
                            std::cout << "[IAC] DO TELOPT_SGA [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                            telnetSendIAC(telnetOptionAcknowledge(m_teloptCommand),c);
                            m_isSGACompleted = true;
                            m_is_active_sga = true;
                            break;

                        case TELOPT_TTYPE:
                            std::cout << "[IAC] DO TELOPT_TTYPE [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                            telnetSendIAC(telnetOptionAcknowledge(m_teloptCommand),c);
                            m_is_active_ttype = true;
                            break;

                        case TELOPT_NAWS:
                            std::cout << "[IAC] DO TELOPT_NAWS [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                            telnetSendIAC(telnetOptionAcknowledge(m_teloptCommand),c);
                            telnetOptionNawsReply();
                            m_is_active_naws = true;
                            break;
                            /*
                            case IAC :
                                printf("\r\n [DO - INCORRECT IAC Received, resetting.] \r\n");
                                stage = 1;
                                return 255;
                            */

                        default:
                            std::cout << "[IAC] DO -> WONT [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                            telnetSendIAC(telnetOptionDeny(m_teloptCommand),c);
                            break;
                    }
                    m_teloptStage = 0;
                    break;

                    // WILL means the Server Will DO IT!
                    // We reply Fine, do it!
                case WILL: // Replies DO And DONT
                    std::cout << "[IAC] RECEIVED WILL [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                    // Don't response to WILL Requests.
                    switch(c)
                    {
                        case TELOPT_ECHO:
                            if(!m_is_active_echo)
                            {
                                std::cout << "[IAC] WILL TELOPT_ECHO [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                                telnetSendIAC(telnetOptionAcknowledge(m_teloptCommand),c);
                                m_is_active_echo = true;
                                m_isECHOCompleted = false;
                            }
                            break;

                        case TELOPT_BINARY :
                            if(!m_is_active_bin)
                            {
                                std::cout << "[IAC] WILL TELOPT_BINARY [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                                telnetSendIAC(telnetOptionAcknowledge(m_teloptCommand),c);
                                m_is_active_bin = true;
                                m_isBINCompleted = true;
                            }
                            break;

                        case TELOPT_SGA :
                            if(!m_is_active_sga)
                            {
                                std::cout << "[IAC] WILL TELOPT_SGA [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                                telnetSendIAC(telnetOptionAcknowledge(m_teloptCommand),c);
                                m_is_active_sga = true;
                                m_isSGACompleted = true;
                            }
                            break;
                            /*
                            case IAC :
                                stage = 1;
                                return 255;
                            */
                        default :
                            std::cout << "[IAC] WILL -> DONT [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                            telnetSendIAC(telnetOptionDeny(m_teloptCommand),c);
                            break;
                    }
                    m_teloptStage = 0;
                    break;

                case WONT:
                    // Don't respond to WONT
                    std::cout << "[IAC] RECEIVED WONT [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                    telnetSendIAC(telnetOptionAcknowledge(m_teloptCommand),c);
                    //printf("\r\n [WONT - responded DONT %i] \r\n",c);
                    m_teloptStage = 0;
                    break;

                    // Start of Sub Negotiations and Stages 3 - 4
                case SB: // 250
                    std::cout << "[IAC] TELNET_STATE_SB [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                    if(c == TELOPT_TTYPE)
                    {
                        m_currentOption = c;
                        m_teloptStage = 3;
                    }
                    else
                    {
                        std::cout << "[IAC] TELNET_STATE_SB INVALID [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                        // Invalid, reset back.
                        m_teloptStage = 0;
                    }
                    break;

                default:
                    // Options or Commands Not Parsed, RESET.
                    std::cout << "[IAC] INVALID [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                    m_teloptStage = 0;
                    break;
            }
            break;

        case 3:
            std::cout << "--> STAGE 3 [" << (int)c << "]" << std::endl;
            //Options will be 1 After SB
            //IAC SB TTYPE SEND IAC SE
            switch(m_currentOption)
            {
                case TELOPT_TTYPE:
                    if(c == TELQUAL_SEND)  // SEND
                    {
                        std::cout << "[IAC] TELQUAL_SEND [" << (int)m_currentOption << "] [" << (int)c << "]" << std::endl;
                        m_teloptStage = 4;
                    }
                    else
                        m_teloptStage = 0;
                    break;

                default:
                    //printf("\r\n [Stage 3 - unregistered stuff it] - %i, %i \r\n",opt, c);
                    if(c == SE)
                    {
                        std::cout << "[IAC] SB END [" << (int)m_currentOption << "] [" << (int)c << "]" << std::endl;
                        m_teloptStage = 0;
                    }
                    else
                    {
                        // reset
                        m_teloptStage = 0;
                    }
                    break;
            }
            break;

            // Only Gets here on TTYPE Sub-Negotiation.
        case 4:
            std::cout << "--> STAGE 4 [" << (int)c << "]" << std::endl;
            switch(c)
            {
                case IAC:
                    std::cout << "[IAC] TTYPE TELNET_STATE_SB IAC [" << (int)m_currentOption << "] [" << (int)c << "]" << std::endl;
                    break;

                case SE:
                    std::cout << "[IAC] TTYPE TELNET_STATE_SB SE [" << (int)m_currentOption << "] [" << (int)c << "]" << std::endl;

                    // Send TTYPE After End of Complete Sequence is Registered.
                    telnetOptionTerminalTypeReply();
                    m_teloptStage = 0;
                    break;
            }
            break;
    }

    //std::cout << "[IAC] null return [" << (int)c << "] " << std::endl;
    return '\0';
}
