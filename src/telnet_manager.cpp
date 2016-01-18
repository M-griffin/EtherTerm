#include "telnet.hpp"
#include "telnet_manager.hpp"
#include "session.hpp"

#include <boost/algorithm/string.hpp>

#include <cstdio>
#include <iostream>
#include <fstream>

TelnetManager::TelnetManager(session_ptr session)
    : m_weak_session(session)
    , m_teloptStage(0)
    , m_teloptCommand(0)
    , m_isSGACompleted(false)
    , m_isBINCompleted(false)
    , m_isECHOCompleted(true) // Default to True for MUD'S etc.
    , m_finishedSGA(false)
    , m_finishedTTYPE(false)
    , m_finishedNAWS(false)
    , m_finishedBIN(false)
    , m_finishedECHO(false)
    , m_isShutdown(false)
    , m_currentOption(0)
{
    std::cout << "TelnetManager Created." << std::endl;
}

TelnetManager::~TelnetManager()
{
    std::cout << "Shutting Down ~TelnetManager" << std::endl;
}

/**
 * @brief Handles Telnet Options and Replies, Returns text data
 * @param msg_buffer
 * @return
 */
std::string TelnetManager::parseIncomingData(std::string msg_buffer)
{
    // Ignore empty buffers.
    if(msg_buffer.size() == 0)
        return "";

    std::string output;
    unsigned char ch = 0;

// Debugging, write all data received to file
//    std::fstream sout;
//    sout.open( "screen.ans", ios::out | ios::app );
//    sout << msgBuffer << std::flush;
//    sout.close();

    // Loop through data for IAC Telnet Commands
    for(unsigned char c : msg_buffer)
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

/*
 * Handles User Input.
 *
void TelnetManager::update()
{
    //std::cout << "TelnetState::update()" << std::endl;
    int ret = 0;
    // Keeps a count of how many loops we run with no server data, if were waiting
    // For input, add a slight Delay in the loop so were not hogging CPU Usage.
    // reset the count when we get data or type input.
    static int inputCount = 0;
    if(m_isShutdown || TheInputHandler::Instance()->isGlobalShutdown())
    {
        std::cout << "TelnetState::shutdown - changeState(new MainMenuState()" << std::endl;
        TheStateMachine::Instance()->changeState(new MainMenuState());
        m_isShutdown = false;
        return;
    }

    // Handles User Input and Window Events.
    if(TheInputHandler::Instance()->update())
    {
        inputCount = 0;  // reset counter.
        if (TheInputHandler::Instance()->getInputSequence(inputSequence))
        {
            if(TheSocketHandler::Instance()->isActive())
            {
                ret = TheSocketHandler::Instance()->send((unsigned char *)inputSequence.c_str(), (int)inputSequence.size());
                // Check return value later on on, not used at the moment.
                // Server is not echoing input back to us, we need to do it ourselves.
                // Jump into LINE MODE Editing here.

                if(m_isECHOCompleted)
                {
                    TheSequenceManager::Instance()->decode(inputSequence);
                }
            }
            else
            {
                std::cout << "ERROR !TheSocketHandler::Instance()->isActive()" << std::endl;
                m_isShutdown = true;
            }
        }
    }

    // Polling the Socket for Connection Status and Data, -1 on lost connection
    // 0 = No Data, and -1 = Dead Connection
    if(TheSocketHandler::Instance()->isActive())
    {
        // Poll the Socket for Incoming Data from Server.
        ret = TheSocketHandler::Instance()->update();
        switch(ret)
        {
            case 0:
                // No Data Waiting from Server, Return
                if(inputCount % 3 == 0)
                {
                    // If we loop 3 times with no data, add input delay
                    // To Save CPU Usage.
                    SDL_Delay(10);
                }
                else
                    ++inputCount;
                break;
            case -1:
                // Lost connection, return to mainMenu. Maybe Prompt or freeze?
                inputCount = 0;
                m_isShutdown = true;
                break;
            default:
                // received data, process it.
                inputCount = 0;
                handleSession();
                break;
        }
    }
    else
    {
        std::cout << "ERROR TelnetState::update() >isActive()" << std::endl;
        m_isShutdown = true;
    }
}
*/

/*
bool TelnetManager::onEnter()
{
    std::cout << "entering TelnetState\n";
    m_isShutdown = false;

    // initialize SDL_net
    if(SDLNet_Init() < 0)
    {
        SDL_Log(
            "Terminal::init() SDLNet_Init: %s",
            SDL_GetError());
        return false;
    }

    std::cout << "SDLNet_Init success" << std::endl;

    //char host[255]= {"entropybbs.co.nz"};
    //char host[255]= {"montereybbs.ath.cx"};
    //char host[255]= {"1984.ws"};
    //char host[255]= {"clutch.darktech.org"};
    //char host[255]= {"fluph.darktech.org"};
    //char host[255]= {"oddnetwork.org"};
    //char host[255]= {"192.168.1.131"};
    //char host[255]= {"blackflag.acid.org"};
    //char host[255]= {"d1st.org"};
    //char host[255]= {"bbs.pharcyde.org"};
    //char host[255]= {"bloodisland.ph4.se"};
    //char host[255]= {"maze.io"};
    //char host[255]= {"skulls.sytes.net"};
    //char host[255]= {"cyberia.darktech.org"};
    //char host[255]= {"eob-bbs.com"};
    //char host[255]= {"192.168.1.126"};
//    char host[255]= {"127.0.0.1"};
    //char host[255]= {"192.168.1.145"}; // vmware
    //char host[255]= {"arcade.demon.co.uk"};
    //char host[255]= {"hq.pyffle.com"};
    //char host[255]= {"bbs.outpostbbs.net"};
    //char host[255]= {"htc.zapto.org"};
    //char host[255]= {"darksorrow.us"};

    TheRenderer::SystemConnection sysconn;
    sysconn = TheRenderer::Instance()->getSystemConnection();

    TheRenderer::Instance()->setCurrentFont(sysconn.font);
    if(TheRenderer::Instance()->didFontChange())
    {
        std::cout << "TelnetState: font: " << TheRenderer::Instance()->getCurrentFont() << std::endl;
        if (!TheRenderer::Instance()->loadBitmapImage(
            TheRenderer::Instance()->getCurrentFont()))
            {
                SDL_Delay(1500);
                TheRenderer::Instance()->quit();
                std::cout << "TelnetState: Error Changeing Font in syscon!" << std::endl;
                return false;
            }
    }

    std::string initConnection = "|CS|15|16Initiating connection to: [|07";
    initConnection += sysconn.name + "|15]";

    MenuFunction::sequenceToAnsi((char *)initConnection.c_str());

    if(!TheSocketHandler::Instance()->initTelnet(sysconn.ip, sysconn.port))
    {
        initConnection = "|CR|CR|04|16Unable to establish connection.";
        MenuFunction::sequenceToAnsi((char *)initConnection.c_str());
        SDL_Delay(1000);
        std::cout << "Error Connecting!" << std::endl;
        m_isShutdown = true;
        return false;
    }
    else
    {
        std::cout << "Connection Successful. " << std::endl;
        initConnection = "|CR|CR|10|16Connection established.";
        MenuFunction::sequenceToAnsi((char *)initConnection.c_str());
        SDL_Delay(1000);
    }

    TheSequenceParser::Instance()->setCursorActive(true);
    // Clear Renderer and ANSI Parser for Fresh Connection.
    TheRenderer::Instance()->clearScreenSurface();
    TheRenderer::Instance()->renderScreen();
    TheRenderer::Instance()->drawTextureScreen();
    TheSequenceParser::Instance()->reset();
    return true;
}
*/

/*
bool TelnetManager::onExit()
{
    // reset the handler(s)
    TheInputHandler::Instance()->reset();
    TheSequenceParser::Instance()->reset();
    TheSocketHandler::Instance()->reset();

    std::cout << "TelnetState::onExit()" << std::endl;

    std::cout << "Shutting Down SDL_Net" << std::endl;
    SDLNet_Quit();
    std::cout << "SDL_Net Done." << std::endl;

    std::cout << "exiting TelnetState" << std::endl;
    m_isShutdown = true;
    return true;
}
*/

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
    unsigned char temp[50] = {0};
    session_ptr session = m_weak_session.lock();
    if(session)
    {
        // Setup the Response String
        sprintf(
            (char *) temp, "%c%c%c%c%c%c%c%c%c",
            IAC,
            SB,
            TELOPT_NAWS,
            // If Row Height above 255, split into 255 and remaining
            (session->m_renderer->m_term_height) > 255 ?
            session->m_renderer->m_term_height - 255 :
            0,
            (session->m_renderer->m_term_height) > 255 ?
            255 :
            session->m_renderer->m_term_height,
            // If Row Width above 255, split into 255 and remaining
            (session->m_renderer->m_term_width) > 255 ?
            session->m_renderer->m_term_width - 255 :
            0,
            (session->m_renderer->m_term_width) > 255 ?
            255 :
            session->m_renderer->m_term_width,
            IAC,
            SE
        ); // 9

        // Check if Connection is good, Send the Option Response to Server.
        if(session->m_connection->is_open())
        {
            std::string outgoing_data;
            outgoing_data = (signed)temp;
           // session->deliver(outgoing_data);
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
    unsigned char temp[50] = {0};
    session_ptr session = m_weak_session.lock();
    if(session)
    {
        // Grab the Terminal Type, make sure we send lowercase.
        std::string terminal = session->m_system_connection->termType;
        boost::algorithm::to_lower(terminal);

        sprintf((char *) temp, "%c%c%c%c%s%c%c",
                IAC,
                SB,
                TELOPT_TTYPE,
                TELQUAL_IS,
                // Pass the current Terminal Type ANSI/XTERM etc..
                terminal.c_str(),
                IAC,
                SE
               ); // 10

        // Check if Connection is good, Send the Option Response to Server.
        if(session->m_connection->is_open())
        {
            std::string outgoing_data;
            outgoing_data = (signed)temp;
          //  session->deliver(outgoing_data);
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
    unsigned char temp[50]= {0};
    session_ptr session = m_weak_session.lock();
    if(session)
    {
        // Grab the Terminal Type, make sure we send lowercase.
        std::string terminal = session->m_system_connection->termType;
        boost::algorithm::to_lower(terminal);

        sprintf((char *) temp, "%c%c%c", IAC, command, option);

        // Check if Connection is good, Send the Option Response to Server.
        if(session->m_connection->is_open())
        {
            std::string outgoing_data;
            outgoing_data = (signed)temp;
          //  session->deliver(outgoing_data);
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
                // Sequence Found with IAC, set to next Stage.
                //printf("\r\n ========================================== \r\n");
                //printf("\r\n Received #255 = IAC \r\n");
                m_teloptStage++;
            }
            break;

            // Find Command
        case 1:
            if(c == IAC)
            {
                // Restart on next character
                // Double IAC = IAC character, not valid so skip it for now.
                // Telnet states binary mode, double IAC mean pass through
                // Char 255.  But this doesn't equal any text in ExtASCII
                // So we going to stuff it.
                //std::cout << "\r\n Got double IAC!!\r\n" << std::endl;
                m_teloptStage = 0;
                return IAC;
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
                            m_finishedECHO = true;
                            m_isECHOCompleted = true;
                            break;

                        case TELOPT_BINARY:
                            std::cout << "[IAC] DO TELOPT_BINARY [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                            telnetSendIAC(telnetOptionAcknowledge(m_teloptCommand),c);
                            m_isBINCompleted = true;
                            m_finishedBIN = true;
                            break;

                        case TELOPT_SGA:
                            std::cout << "[IAC] DO TELOPT_SGA [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                            telnetSendIAC(telnetOptionAcknowledge(m_teloptCommand),c);
                            m_isSGACompleted = true;
                            m_finishedSGA = true;
                            break;

                        case TELOPT_TTYPE:
                            std::cout << "[IAC] DO TELOPT_TTYPE [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                            telnetSendIAC(telnetOptionAcknowledge(m_teloptCommand),c);
                            m_finishedTTYPE = true;
                            break;

                        case TELOPT_NAWS:
                            std::cout << "[IAC] DO TELOPT_NAWS [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                            telnetSendIAC(telnetOptionAcknowledge(m_teloptCommand),c);
                            telnetOptionNawsReply();
                            m_finishedNAWS = true;
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
                            if(!m_finishedECHO)
                            {
                                std::cout << "[IAC] WILL TELOPT_ECHO [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                                telnetSendIAC(telnetOptionAcknowledge(m_teloptCommand),c);
                                m_finishedECHO = true;
                                m_isECHOCompleted = false;
                            }
                            break;

                        case TELOPT_BINARY :
                            if(!m_finishedBIN)
                            {
                                std::cout << "[IAC] WILL TELOPT_BINARY [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                                telnetSendIAC(telnetOptionAcknowledge(m_teloptCommand),c);
                                m_finishedBIN = true;
                                m_isBINCompleted = true;
                            }
                            break;

                        case TELOPT_SGA :
                            if(!m_finishedSGA)
                            {
                                std::cout << "[IAC] WILL TELOPT_SGA [" << (int)m_teloptCommand << "] [" << (int)c << "]" << std::endl;
                                telnetSendIAC(telnetOptionAcknowledge(m_teloptCommand),c);
                                m_finishedSGA = true;
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
    return '\0';
}
