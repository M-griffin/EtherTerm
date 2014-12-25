
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "telnetState.h"
#include "mainMenuState.h"
#include "inputHandler.h"
#include "ansiParser.h"
#include "terminal.h"
#include "socketHandler.h"

#include <cstdio>

const std::string TelnetState::telnetID = "TELNET";

/*
 * Handles Reading the Socket for Data, then Pasring the Data.
 */

void TelnetState::handleSession()
{
    std::string ansiret;
    std::string output;

    int len = 0;
    char msgBuffer[8193]={'\0'};
    std::string newstring;
    std::string::size_type id1;

    // Escape parsing
    static std::string esc_sequence;
    static char more_params = 0;
    unsigned char c, ch;

    // Get Socket Data From Server
    len = TheSocketHandler::Instance()->recv(msgBuffer);
    if ( len < 0)
    {
        // Error - Lost Connection
        shutdown = true;
        return;
    }
    // Else No Data just return
    else if (len == 0)
        return;

    // Loop through data for IAC Telnet Commands
    for (int i = 0; i < len; i++ )
    {
        c = msgBuffer[i];
        //ch = c;
        //std::cout << c << std::flush;

        // ←[0;0 D
        // ←[0;40 D

        try
        {
            // Run the Telnet Options Parser
            ch = telnet_process_char(c);
        }
        catch (std::exception& e)
        {
            std::cout << "Exception telnet_process_char: " << e.what() << std::endl;
        }

        // Skip all NULL Bytes, returned on IAC characters.
        if (ch == '\0')
            continue;

        //Handle escape sequence Parsing, Make Sure we get Complete ESC Sequences
        // Before throwing data at the ansi pasrser.
        if (ch == '\x1b' || more_params == TRUE)
        {
            // Debug ESC Sequences.
            //if (ch==27) myfile << "\r\n" << (int)ch << "," << ch;
            //else myfile << (int)c << "," << ch;
            //if (ch==27) myfile << "\r\n" << ch;
            //else myfile << ch;

            more_params = TRUE;

            // Loop through Ending Sequence of
            // of Escape Code once we have this, then add the ESC sequence
            // to the DataBuffer
            switch(ch)
            {
                case CURSOR_POSITION:
                case CURSOR_POSITION_ALT:
                case CURSOR_UP:
                case CURSOR_DOWN:
                case CURSOR_FORWARD:
                case CURSOR_BACKWARD:
                case CURSOR_X_POSITION:
                case CURSOR_NEXT_LINE:
                case CURSOR_PREV_LIVE:
                case ERASE_DISPLAY:
                case ERASE_TO_EOL:
                case SAVE_CURSOR_POS:
                case RESTORE_CURSOR_POS:
                case SET_GRAPHICS_MODE:
                case SET_MODE:
                case RESET_MODE:
                case SET_KEYBOARD_STRINGS:
                case ANSI_DETECTION:
                case SCROLL_REGION:

                    esc_sequence += ch;

                    //printf("\r\n ESC SEQUENCE - %s \r\n", esc_sequence.c_str());

                    // Detect Font Switching.
                    if (esc_sequence == "\x1b[0;0 D")
                    {
                        std::cout << "switch to CP437 Font" << endl;
                        TheTerm::Instance()->setCurrentFont("vga8x16.bmp");
                        if (TheTerm::Instance()->didFontChange())
                            TheTerm::Instance()->loadBitmapImage(TheTerm::Instance()->getCurrentFont());
                    }
                    //37 - P0T NOoDLE (Amiga)
                    else
                    if (esc_sequence == "\x1b[0;37 D")
                    {
                        std::cout << "switch to Pot-Noodle Font" << endl;
                        TheTerm::Instance()->setCurrentFont("potNoodle-8x16.bmp");
                        if (TheTerm::Instance()->didFontChange())
                            TheTerm::Instance()->loadBitmapImage(TheTerm::Instance()->getCurrentFont());

                    }
                    //38 - mO'sOul (Amiga)
                    else
                    if (esc_sequence == "\x1b[0;38 D")
                    {
                        std::cout << "switch to mO'sOul Font" << endl;
                        TheTerm::Instance()->setCurrentFont("mo'soul-8x16.bmp");
                        if (TheTerm::Instance()->didFontChange())
                            TheTerm::Instance()->loadBitmapImage(TheTerm::Instance()->getCurrentFont());
                    }
                    //39 - MicroKnight (Amiga)
                    else
                    if (esc_sequence == "\x1b[0;39 D")
                    {
                        std::cout << "switch to Micro-Knight+ Font" << endl;
                        TheTerm::Instance()->setCurrentFont("microKnightPlus-8x16.bmp");
                        if (TheTerm::Instance()->didFontChange())
                            TheTerm::Instance()->loadBitmapImage(TheTerm::Instance()->getCurrentFont());
                    }
                    //40 - Topaz (Amiga)
                    else
                    if (esc_sequence == "\x1b[0;40 D")
                    {
                        std::cout << "switch to Topaz+ Font" << endl;
                        TheTerm::Instance()->setCurrentFont("topazPlus-8x16.bmp");
                        if (TheTerm::Instance()->didFontChange())
                            TheTerm::Instance()->loadBitmapImage(TheTerm::Instance()->getCurrentFont());
                    }
                    else
                    {
                        output += esc_sequence;
                    }

                    //std::cout << esc_sequence << std::endl;
                    esc_sequence.erase();
                    more_params = FALSE;
                    break;
                default:
                    // continue with the next character
                    esc_sequence += ch;

                    // Bad Sequence, push it though
                    // So were not stuck in endless loop.
                    if (esc_sequence.size() > 15)
                    {
                        printf("\r\n BAD ESC SEQUENCE - %s \r\n", esc_sequence.c_str());
                        output += esc_sequence;
                        esc_sequence.erase();
                        more_params = FALSE;
                    }
                    break;
            }
        }
        else
        {
            //myfile << ch;
            //if (ch == 10 || ch == 13)
                //printf("\r\n ch - %i \r\n",ch);
            output += ch;
        }
    }


    // If available, push Data through ANSI Pasrser then Screen.
    if (output.size() > 0)
    {
        // Turn off Cursor Before rendering new screen.
        TheTerm::Instance()->RenderCursorOffScreen(TheAnsiParser::Instance()->x_position-1,
                                                   TheAnsiParser::Instance()->y_position-1);

        // Parse Ansi
        TheAnsiParser::Instance()->ansiparse((unsigned char *)output.c_str(),0);

        // Setup cursor in current x/y position Cursor.
        TheTerm::Instance()->SetupCursorChar(TheAnsiParser::Instance()->x_position-1,
                                             TheAnsiParser::Instance()->y_position-1);


        TheTerm::Instance()->RenderCursorOnScreen(TheAnsiParser::Instance()->x_position-1,
                                                  TheAnsiParser::Instance()->y_position-1);

        TheTerm::Instance()->DrawTextureScreen();
    }

    // Look at cursor timing. If were getting input that we've typed
    // Back from server, then we want to display the cursor,
    // Otherwise for Ansi screens and animation we don't want to show it
    /*
    if (output.size() <= 5)
    {
        TheTerm::Instance()->RenderCursorOnScreen(TheAnsiParser::Instance()->x_position-1,
                                                  TheAnsiParser::Instance()->y_position-1);

        TheTerm::Instance()->DrawTextureScreen();

    }*/
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
    // Reset the count when we get data or type input.
    static int inputCount = 0;

    if (shutdown || TheInputHandler::Instance()->isGlobalShutdown() )
    {
        std::cout << "TelnetState::shutdown - changeState(new MainMenuState()" << std::endl;
        TheTerm::Instance()->getStateMachine()->changeState(new MainMenuState());
        shutdown = false;
        return;
    }

    // Handles User Input and Window Events.
    if(TheInputHandler::Instance()->update())
    {
        inputCount = 0;  // Reset counter.
        inputSequence = TheInputHandler::Instance()->getInputSequence();

        if (TheSocketHandler::Instance()->isActive())
        {
            ret = TheSocketHandler::Instance()->send((unsigned char *)inputSequence.c_str(), (int)inputSequence.size());
            // Check return value lateron on, not used at the moment.
        }
        else
        {
            std::cout << "ERROR !TheSocketHandler::Instance()->isActive()" << std::endl;
            shutdown = true;
        }
    }

    // Polling the Socket for Connection Status and Data, -1 on lost connection
    // 0 = No Data, and -1 = Dead Connection

    if (TheSocketHandler::Instance()->isActive())
    {
        // Poll the Socket for Incoming Data from Server.
        ret = TheSocketHandler::Instance()->update();
        switch (ret)
        {
            case 0:
                // No Data Waiting from Server, Return
                if (inputCount % 3 == 0)
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
    TheTerm::Instance()->setRenderReady(false);
}

bool TelnetState::onEnter()
{
    TheTerm::Instance()->setRenderReady(true);
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

    TheTerm::SystemConnection sysconn;
    sysconn = TheTerm::Instance()->getSystemConnection();

    if (!TheSocketHandler::Instance()->initTelnet(sysconn.ip,sysconn.port))
    {
        std::cout << "Error Connecting!" << std::endl;
        shutdown = true;
    }

    std::cout << "Connection Successful. " << std::endl;

    // Clear Renderer and Ansi Parser for Fresh Connection.
    TheTerm::Instance()->ClearScreenSurface();
    TheTerm::Instance()->RenderScreen();
    TheTerm::Instance()->DrawTextureScreen();
    TheAnsiParser::Instance()->reset();


    return true;
}

bool TelnetState::onExit()
{
    std::cout << "TelnetState::onExit()" << std::endl;

     // Reset the handler(s)
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
unsigned char TelnetState::telnet_opt_ack(unsigned char cmd)
{
    switch(cmd) {
        case DO:    return WILL;
        case DONT:    return WONT;
        case WILL:    return DO;
        case WONT:    return DONT;
    }
    return 0;
}

unsigned char TelnetState::telnet_opt_nak(unsigned char cmd)
{
    switch(cmd) {
        case DO:    return WONT;
        case DONT:    return WILL;
        case WILL:    return DONT;
        case WONT:    return DO;
    }
    return 0;
}


/**
 * Sends IAC Sequence reply for NAWS or Terminal Window Size 80x50
 */
int TelnetState::telnet_build_NAWS_reply()
{
    unsigned char temp[50]={0};
    sprintf ((char *) temp, "%c%c%c%c%c%c%c%c%c",
        IAC, SB, TELOPT_NAWS, 0, 80, 0, NUM_LINES, IAC, SE); // 9

    if (TheSocketHandler::Instance()->isActive())
    {
        TheSocketHandler::Instance()->send(temp, 9);
        // Check return value lateron on, not used at the moment.
    }
    else
    {
        std::cout << "ERROR TelnetState::telnet_build_NAWS_reply() (INPUT) ->isActive()" << std::endl;
        shutdown = true;
    }
//    socket_send(temp, 9);
    return 0;
}

/**
 * Sends IAC Sequence reply for Terminal Type
 */
int TelnetState::telnet_build_TTYPE_reply()
{
    unsigned char temp[50]={0};
    sprintf ((char *) temp, "%c%c%c%c%s%c%c",
        IAC, SB, TELOPT_TTYPE, TELQUAL_IS, "ansi", IAC, SE); // 10

    if (TheSocketHandler::Instance()->isActive())
    {
        TheSocketHandler::Instance()->send(temp, 10);
        // Check return value lateron on, not used at the moment.
    }
    else
    {
        std::cout << "ERROR TelnetState::telnet_build_NAWS_reply() (INPUT) ->isActive()" << std::endl;
        shutdown = true;
    }
//    socket_send(temp, 10);
    return 0;
}


/**
 * Sends IAC Sequence back to Users Client for Terminal Negoation.
 */
int TelnetState::send_iac(Uint32 command, Uint32 option)
{
    unsigned char temp[50]={0};
    sprintf ((char *) temp, "%c%c%c", IAC, command, option);

    if (TheSocketHandler::Instance()->isActive())
    {
        TheSocketHandler::Instance()->send(temp, 3);
        // Check return value lateron on, not used at the moment.
    }
    else
    {
        std::cout << "ERROR TelnetState::telnet_build_NAWS_reply() (INPUT) ->isActive()" << std::endl;
        shutdown = true;
    }
//    socket_send(temp, 3);
    return 0;
}


/*
 * Parses Telnet Options negoations between client / severs
 * On which Features are supports and terminal inforamtion.
 */
unsigned char TelnetState::telnet_process_char(unsigned char c)
{

    //ofstream myfile;
    //myfile.open ("example.txt", ios::app );
    //if (c==255) myfile << "\r\n" << (int)c << ",";
    //else myfile << (int)c << ",";
    //myfile.close();

    //printf("\r\n [***] %i \r\n, ",c);

    // TELOPT Pasrer
    switch (stage)
    {
        // Find IAC
        case 0:
            // Check first Char in Fresh Sequence
            if (c != IAC)
            {
                // Add character to output buffer.
                return c;
            }
            else
            {
                // Sequence Found with IAC, set to next Stage.
                printf("\r\n ========================================== \r\n");
                printf("\r\n Received #255 = IAC \r\n");
                stage++;
            }
            break;

        // Find Command
        case 1:        
            if (c == IAC)
            {
                // Restart on next character
                // Double IAC = IAC.
                std::cout << "\r\n Got double IAC!!\r\n" << std::endl;
                return '\0';
            }
            if (c != IAC)
            {

                switch (c)
                {
                    // Catch Passthrough commands.
                    case GA:    //     249        /* you may reverse the line */
                    case EL:    //     248        /* erase the current line */
                    case EC:    //     247        /* erase the current character */
                    case AYT:   //     246        /* are you there */
                    case AO:    //     245        /* abort output--but let prog finish */
                    case IP:    //     244        /* interrupt process--permanently */
                    case BREAK: //   243        /* break */
                    case DM:    //   242        /* data mark--for connect. cleaning */
                    case NOP:   //      241        /* nop */
                    case SE:    //      240        /* end sub negotiation */
                    case EOR:   //   239        /* end of record (transparent mode) */
                    case ABORT: //   238        /* Abort process */
                    case SUSP:  //   237        /* Suspend process */
                    case xEOF:  //   236        /* End of file: EOF is already used... */

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
            if (TELCMD_OK(cmd))
                printf("\r\n [*****] [IAC][%i][%i] \r\n", cmd,c);
            else
            {
                // Hopefully won't get here!
                printf("\r\n [*** BAD ***] [IAC][%i][%i] \r\n", cmd,c);
                stage = 0;
                break;
            }

            switch (cmd)
            {
                // No responses needed, just stuff these.
                case DONT:

                    printf("\r\n [DONT telnet request received] \r\n");

                    switch (c)
                    {
                        case IAC :
                            stage = 1;
                            break;

                        default:
                            printf("\r\n [DONT - responsed WONT %i] \r\n",c);
                            send_iac(telnet_opt_ack(cmd),c);
                            stage = 0;
                            break;
                    }
                    break;

                case DO: // Replies WILL / WON'T

                    printf("\r\n [DO telnet request received] \r\n");

                    switch (c)
                    {

                        case TELOPT_ECHO:
                            printf("\r\n [DO - TELOPT_ECHO responsed WONT] \r\n");
                            send_iac(telnet_opt_nak(cmd),c);
                            break;

                        case TELOPT_BINARY:
                            printf("\r\n [DO - TELOPT_BINARY responsed WILL] \r\n");
                            send_iac(telnet_opt_ack(cmd),c);
                            binary_mode = 1;
                            break;

                        case TELOPT_SGA:
                            printf("\r\n [DO - TELOPT_SGA responsed WILL] \r\n");
                            send_iac(telnet_opt_ack(cmd),c);
                            did_SGA = 1;
                            break;

                        case TELOPT_TTYPE:
                            printf("\r\n [DO - TELOPT_TTYPE responsed WILL] \r\n");
                            send_iac(telnet_opt_ack(cmd),c);
                            did_TERM = 1;
                            break;

                        case TELOPT_NAWS:
                            printf("\r\n [DO - TELOPT_NAWS responsed WILL] \r\n");
                            send_iac(telnet_opt_ack(cmd),c);
                            telnet_build_NAWS_reply();
                            did_NAWS = 1;
                            break;

                        /*
                        case IAC :
                            printf("\r\n [DO - INCORRECT IAC Received, resetting.] \r\n");
                            stage = 1;
                            return 255;
                        */

                        default:
                            printf("\r\n [DO - responsed WONT %i] \r\n",c);
                            send_iac(telnet_opt_nak(cmd),c);
                            break;
                    }
                    stage = 0;
                    break;

                case WILL: // Replies DO And DONT

                    printf("\r\n [WILL telnet request received] \r\n");

                    // Don't response to WILL Requests.
                    switch (c)
                    {

                        case TELOPT_ECHO:
                            if (!did_ECHO)
                            {
                                printf("\r\n [WILL - TELOPT_ECHO responsed DO] \r\n");
                                send_iac(telnet_opt_ack(cmd),c);
                                did_ECHO = 1;
                            }
                            break;

                        case TELOPT_BINARY :
                            if (!did_BIN)
                            {
                                printf("\r\n [WILL - TELOPT_BINARY responsed DO] \r\n");
                                send_iac(telnet_opt_ack(cmd),c);
                                did_BIN = 1;
                            }
                            break;

                        case TELOPT_SGA :
                            if (!did_SGA)
                            {
                                printf("\r\n [WILL - TELOPT_SGA responsed DO] \r\n");
                                send_iac(telnet_opt_ack(cmd),c);
                                did_SGA = 1;
                            }
                            break;
                        /*
                        case IAC :
                            stage = 1;
                            return 255;
                        */
                        default :
                            send_iac(telnet_opt_nak(cmd),c);
                            printf("\r\n [WILL - responsed DONT %i] \r\n",c);
                            break;
                    }

                    stage = 0;
                    break;

                case WONT:

                    // Don't responset to WONT
                    printf("\r\n [WONT telnet request received] \r\n");
                    send_iac(telnet_opt_ack(cmd),c);
                    printf("\r\n [WONT - responsed DONT %i] \r\n",c);

                    stage = 0;
                    break;

                // Start of Sub Negoations and Stages 3 - 4
                case SB: // 250

                    printf("\r\n [TELNET_STATE_SB ENTERED] \r\n");
                    if (c == TELOPT_TTYPE)
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
            switch (opt)
            {
                case TELOPT_TTYPE:
                    if (c == TELQUAL_SEND) // SEND
                    {
                        printf("\r\n [Stage 3 - TELQUAL_SEND] goto Stage 4");
                        stage = 4;
                    }
                    else
                        stage = 0;
                    break;

                default:
                    //printf("\r\n [Stage 3 - unregistered suff it] - %i, %i \r\n",opt, c);
                    if (c == SE)
                    {
                        printf("\r\n [TELNET_STATE_SB SE] \r\n");
                        stage = 0;
                    }
                    else
                    {
                        // Reset
                        stage = 0;
                    }
                    break;
            }
            break;

        // Only Gets here on TTYPE Subnegoation.
        case 4:

            printf("\r\n [Stage 4] - %i \r\n",c);

            switch (c)
            {

                case IAC:
                    printf("\r\n [TELNET_STATE_SB IAC] \r\n");
                    break;

                case SE:
                    printf("\r\n [TELNET_STATE_SB SE] \r\n");

                    // Send TTYPE After End of Compelte Sequence is Registered.
                    telnet_build_TTYPE_reply();

                    stage = 0;
                    break;

            }
            break;
    }
    return '\0';
}

