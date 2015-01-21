
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "telnetState.h"
#include "mainMenuState.h"
#include "inputHandler.h"
#include "ansiParser.h"
#include "sequenceParser.h"
#include "terminal.h"
#include "socketHandler.h"

#include <cstdio>
#include <fstream>

const std::string TelnetState::telnetID = "TELNET";

TelnetState::TelnetState() :
    stage(0),
    cmd(0),
    isSGA(false),
    isBIN(false),
    isECHO(true),  // Default to True for MUDS etc.
    didSGA(false),
    didTERM(false),
    didNAWS(false),
    didBIN(false),
    didECHO(false)
{ }
/*
 * Handles Reading the Socket for Data, then Pasring the Data.
 */

void TelnetState::handleSession()
{
    std::string output;
    int len = 0;
    char msgBuffer[8193]= {'\0'};

    unsigned char c, ch;

    // Get Socket Data From Server
    len = TheSocketHandler::Instance()->recv(msgBuffer);
    if(len < 0)
    {
        // Error - Lost Connection
        shutdown = true;
        return;
    }
    // Else No Data just return
    else if(len == 0)
        return;

// Debugging, write all data received to file
//    std::fstream sout;
//    sout.open( "screen.ans", ios::out | ios::app );
//    sout << msgBuffer << std::flush;
//    sout.close();

    // Loop through data for IAC Telnet Commands
    for(int i = 0; i < len; i++)
    {
        c = msgBuffer[i];

        // for debugging server output, mainly esc seqeucens
        // and output that might mess up screens when needed.
#ifdef _DEBUG
        if (c == '\r')
            std::cout << "^r" << std::flush;
        else if (c == '\n')
            std::cout << "^n" << std::flush;
        else if (c == '\0')
            std::cout << "^0" << std::flush;
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
        if(ch == '\0')
        {
            continue;
        }
        // Append to buffer
        output += ch;
    }

    // If available, push Data through ANSI Pasrser then Screen.
    if(output.size() > 0)
    {
        // Parse Incoming Screen Data
        TheSequenceParser::Instance()->processSequence(output);
    }    
    output.erase();
}

/*
 * Handles User Input.
 */
void TelnetState::update()
{
    //std::cout << "TelnetState::update()" << std::endl;
    int ret = 0;
    // Keeps a count of how many loops we run with no server data, if were waiting
    // For input, add a slight Delay in the loop so were not hogging CPU Usage.
    // reset the count when we get data or type input.
    static int inputCount = 0;
    if(shutdown || TheInputHandler::Instance()->isGlobalShutdown())
    {
        std::cout << "TelnetState::shutdown - changeState(new MainMenuState()" << std::endl;
        TheTerminal::Instance()->getStateMachine()->changeState(new MainMenuState());
        shutdown = false;
        return;
    }

    // Handles User Input and Window Events.
    if(TheInputHandler::Instance()->update())
    {
        inputCount = 0;  // reset counter.
        inputSequence = TheInputHandler::Instance()->getInputSequence();
        if(TheSocketHandler::Instance()->isActive())
        {
            ret = TheSocketHandler::Instance()->send((unsigned char *)inputSequence.c_str(), (int)inputSequence.size());
            // Check return value lateron on, not used at the moment.
            // Server is not echoing input back to us, we need to do it ourselves.
            // Jump into LINEMODE Editing here.
            //***
            if(isECHO)
            {
                TheSequenceParser::Instance()->processSequence(inputSequence);
            }
        }
        else
        {
            std::cout << "ERROR !TheSocketHandler::Instance()->isActive()" << std::endl;
            shutdown = true;
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
                // Lost connection, return to mainMenu. Maybe Propmpt or freeze?
                inputCount = 0;
                shutdown = true;
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
        shutdown = true;
    }
}

void TelnetState::render()
{
    // Turns off the Rendering Loop when not being used.
    TheTerminal::Instance()->setRenderReady(false);
}

bool TelnetState::onEnter()
{
    TheTerminal::Instance()->setRenderReady(true);
    std::cout << "entering TelnetState\n";
    shutdown = false;
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

    TheTerminal::SystemConnection sysconn;
    sysconn = TheTerminal::Instance()->getSystemConnection();
    if(!TheSocketHandler::Instance()->initTelnet(sysconn.ip,sysconn.port))
    {
        std::cout << "Error Connecting!" << std::endl;
        shutdown = true;
    }

    std::cout << "Connection Successful. " << std::endl;
    // Clear Renderer and Ansi Parser for Fresh Connection.
    TheTerminal::Instance()->clearScreenSurface();
    TheTerminal::Instance()->renderScreen();
    TheTerminal::Instance()->drawTextureScreen();
    TheAnsiParser::Instance()->reset();
    return true;
}

bool TelnetState::onExit()
{
    std::cout << "TelnetState::onExit()" << std::endl;
    // reset the handler(s)
    TheInputHandler::Instance()->reset();
    TheAnsiParser::Instance()->reset();
    TheSocketHandler::Instance()->reset();
    shutdown = true;
    std::cout << "exiting TelnetState" << std::endl;
    return true;
}

void TelnetState::setCallbacks(const std::vector<Callback>& callbacks)
{
    /*
    // go through the game objects
    if(!termObjects.empty())
    {
        for(int i = 0; i < termObjects.size(); i++)
        {
            // if they are of type MenuButton then assign a callback based on the id passed in from the file
            if(dynamic_cast<MenuButton*>(termObjects[i]))
            {
                MenuButton* pButton = dynamic_cast<MenuButton*>(termObjects[i]);
                pButton->setCallback(callbacks[pButton->getCallbackID()]);
            }
        }
    }*/
}


// Telnet State Functions
unsigned char TelnetState::telnetOptionAcknowledge(unsigned char cmd)
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

unsigned char TelnetState::telnetOptionDeny(unsigned char cmd)
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
 * Sends IAC Sequence reply for NAWS or Terminal Window Size 80x50
 */
void TelnetState::telnetOptionNawsReply()
{
    unsigned char temp[50]= {0};
    sprintf((char *) temp, "%c%c%c%c%c%c%c%c%c",
            IAC, SB, TELOPT_NAWS, 0, 80, 0, NUM_LINES, IAC, SE); // 9

    if(TheSocketHandler::Instance()->isActive())
    {
        TheSocketHandler::Instance()->send(temp, 9);
        // Check return value lateron on, not used at the moment.
    }
    else
    {
        std::cout << "ERROR TelnetState::telnet_build_NAWS_reply() (INPUT) ->isActive()" << std::endl;
        shutdown = true;
    }
}

/**
 * Sends IAC Sequence reply for Terminal Type
 */
void TelnetState::telnetOptionTerminalTypeReply()
{
    unsigned char temp[50]= {0};
    sprintf((char *) temp, "%c%c%c%c%s%c%c",
            IAC, SB, TELOPT_TTYPE, TELQUAL_IS, "ansi", IAC, SE); // 10

    if(TheSocketHandler::Instance()->isActive())
    {
        TheSocketHandler::Instance()->send(temp, 10);
        // Check return value lateron on, not used at the moment.
    }
    else
    {
        std::cout << "ERROR TelnetState::telnet_build_NAWS_reply() (INPUT) ->isActive()" << std::endl;
        shutdown = true;
    }
}


/**
 * Sends IAC Sequence back to Users Client for Terminal Negoation.
 */
void TelnetState::telnetSendIAC(Uint32 command, Uint32 option)
{
    unsigned char temp[50]= {0};
    sprintf((char *) temp, "%c%c%c", IAC, command, option);

    if(TheSocketHandler::Instance()->isActive())
    {
        TheSocketHandler::Instance()->send(temp, 3);
        // Check return value lateron on, not used at the moment.
    }
    else
    {
        std::cout << "ERROR TelnetState::telnet_build_NAWS_reply() (INPUT) ->isActive()" << std::endl;
        shutdown = true;
    }
}


/*
 * Parses Telnet Options negoations between client / severs
 * On which Features are supports and terminal inforamtion.
 */
unsigned char TelnetState::telnetOptionParse(unsigned char c)
{
    // TELOPT Pasrer
    switch(stage)
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
                stage++;
            }
            break;

            // Find Command
        case 1:
            if(c == IAC)
            {
                // Restart on next character
                // Double IAC = IAC.
                // Telnet states binary mode, double IAC mean pass through
                // Char 255.  But this doesn't equal any text in ExtASCII
                // So we going to stuff it.
                //std::cout << "\r\n Got double IAC!!\r\n" << std::endl;
                stage = 0;
                return IAC;
            }
            if(c != IAC)
            {
                switch(c)
                {
                        // Catch Passthrough commands.
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
                        printf("\r\n [IAC][%d] 'PASSTHROUGH' \r\n", c);
                        stage = 0;
                        break;

                    default:
                        // Move to Comamnd Parsing.
                        printf("\r\n [IAC][%d] \r\n", c);
                        cmd = c;
                        stage++;
                        break;
                }
            }
            break;

            // Find Option
        case 2:
            stage = 0;
            // Catch if were getting Invalid Commands.
            if(TELCMD_OK(cmd))
                printf("\r\n [*****] [IAC][%i][%i] \r\n", cmd,c);
            else
            {
                // Hopefully won't get here!
                printf("\r\n [*** BAD ***] [IAC][%i][%i] \r\n", cmd,c);
                stage = 0;
                break;
            }
            switch(cmd)
            {
                    // No responses needed, just stuff these.
                case DONT:
                    printf("\r\n [DONT telnet request received] \r\n");
                    switch(c)
                    {
                        case IAC :
                            stage = 1;
                            break;

                        default:
                            printf("\r\n [DONT - responsed WONT %i] \r\n",c);
                            telnetSendIAC(telnetOptionAcknowledge(cmd),c);
                            stage = 0;
                            break;
                    }
                    break;

                case DO: // Replies WILL / WON'T
                    printf("\r\n [DO telnet request received] \r\n");
                    switch(c)
                    {
                        case TELOPT_ECHO:
                            printf("\r\n [DO - TELOPT_ECHO responsed WILL] \r\n");
                            telnetSendIAC(telnetOptionDeny(cmd),c);
                            didECHO = true;
                            isECHO = true;
                            break;

                        case TELOPT_BINARY:
                            printf("\r\n [DO - TELOPT_BINARY responsed WILL] \r\n");
                            telnetSendIAC(telnetOptionAcknowledge(cmd),c);
                            isBIN = true;
                            didBIN = true;
                            break;

                        case TELOPT_SGA:
                            printf("\r\n [DO - TELOPT_SGA responsed WILL] \r\n");
                            telnetSendIAC(telnetOptionAcknowledge(cmd),c);
                            isSGA = true;
                            didSGA = true;
                            break;

                        case TELOPT_TTYPE:
                            printf("\r\n [DO - TELOPT_TTYPE responsed WILL] \r\n");
                            telnetSendIAC(telnetOptionAcknowledge(cmd),c);
                            didTERM = true;
                            break;

                        case TELOPT_NAWS:
                            printf("\r\n [DO - TELOPT_NAWS responsed WILL] \r\n");
                            telnetSendIAC(telnetOptionAcknowledge(cmd),c);
                            telnetOptionNawsReply();
                            didNAWS = true;
                            break;
                            /*
                            case IAC :
                                printf("\r\n [DO - INCORRECT IAC Received, resetting.] \r\n");
                                stage = 1;
                                return 255;
                            */

                        default:
                            printf("\r\n [DO - responsed WONT %i] \r\n",c);
                            telnetSendIAC(telnetOptionDeny(cmd),c);
                            break;
                    }
                    stage = 0;
                    break;

                    // WILL means the Server Will DO IT!
                    // We reply Fine, do it!
                case WILL: // Replies DO And DONT
                    printf("\r\n [WILL telnet request received] \r\n");
                    // Don't response to WILL Requests.
                    switch(c)
                    {
                        case TELOPT_ECHO:
                            if(!didECHO)
                            {
                                printf("\r\n [WILL - TELOPT_ECHO responsed DO] \r\n");
                                telnetSendIAC(telnetOptionAcknowledge(cmd),c);
                                didECHO = true;
                                isECHO = false;
                            }
                            break;

                        case TELOPT_BINARY :
                            if(!didBIN)
                            {
                                printf("\r\n [WILL - TELOPT_BINARY responsed DO] \r\n");
                                telnetSendIAC(telnetOptionAcknowledge(cmd),c);
                                didBIN = true;
                                isBIN = true;
                            }
                            break;

                        case TELOPT_SGA :
                            if(!didSGA)
                            {
                                printf("\r\n [WILL - TELOPT_SGA responsed DO] \r\n");
                                telnetSendIAC(telnetOptionAcknowledge(cmd),c);
                                didSGA = true;
                                isSGA = true;
                            }
                            break;
                            /*
                            case IAC :
                                stage = 1;
                                return 255;
                            */
                        default :
                            telnetSendIAC(telnetOptionDeny(cmd),c);
                            printf("\r\n [WILL - responsed DONT %i] \r\n",c);
                            break;
                    }
                    stage = 0;
                    break;

                case WONT:
                    // Don't responset to WONT
                    printf("\r\n [WONT telnet request received] \r\n");
                    telnetSendIAC(telnetOptionAcknowledge(cmd),c);
                    printf("\r\n [WONT - responsed DONT %i] \r\n",c);
                    stage = 0;
                    break;

                    // Start of Sub Negoations and Stages 3 - 4
                case SB: // 250
                    printf("\r\n [TELNET_STATE_SB ENTERED] \r\n");
                    if(c == TELOPT_TTYPE)
                    {
                        opt = c;
                        stage = 3;
                    }
                    else
                    {
                        printf("\r\n [TELNET_STATE_SB LEAVING] \r\n");
                        // Invalid, reset back.
                        stage = 0;
                    }
                    break;

                default:
                    // Options or Conmmands Not Pasrsed, RESET.
                    printf("\r\n [*****] DEFAULT CMD - %i / %i \r\n", cmd,c);
                    stage = 0;
                    break;
            }
            break;

        case 3:
            printf("\r\n [Stage 3] - %i, %i \r\n",opt, c);
            //Options will be 1 After SB
            //IAC SB TTYPE SEND IAC SE
            switch(opt)
            {
                case TELOPT_TTYPE:
                    if(c == TELQUAL_SEND)  // SEND
                    {
                        printf("\r\n [Stage 3 - TELQUAL_SEND] goto Stage 4");
                        stage = 4;
                    }
                    else
                        stage = 0;
                    break;

                default:
                    //printf("\r\n [Stage 3 - unregistered suff it] - %i, %i \r\n",opt, c);
                    if(c == SE)
                    {
                        printf("\r\n [TELNET_STATE_SB SE] \r\n");
                        stage = 0;
                    }
                    else
                    {
                        // reset
                        stage = 0;
                    }
                    break;
            }
            break;

            // Only Gets here on TTYPE Subnegoation.
        case 4:
            printf("\r\n [Stage 4] - %i \r\n",c);
            switch(c)
            {
                case IAC:
                    printf("\r\n [TELNET_STATE_SB IAC] \r\n");
                    break;

                case SE:
                    printf("\r\n [TELNET_STATE_SB SE] \r\n");

                    // Send TTYPE After End of Compelte Sequence is Registered.
                    telnetOptionTerminalTypeReply();
                    stage = 0;
                    break;
            }
            break;
    }
    return '\0';
}

