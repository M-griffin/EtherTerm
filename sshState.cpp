
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "sshState.h"
#include "mainMenuState.h"
#include "inputHandler.h"
#include "ansiParser.h"
#include "terminal.h"
#include "socketHandler.h"

#include <cstdio>

const std::string SSHState::sshID = "SSH";


/*
 * Handles Reading the Socket for Data, then Pasring the Data.
 */

void SSHState::handleSession()
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
    unsigned char ch;

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


    std::cout << "Received SSH Socket Data: " << msgBuffer << std::endl;

    // Loop through data for IAC Telnet Commands
    for (int i = 0; i < len; i++ )
    {
        ch = msgBuffer[i];

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
                    //printf("\r\n ESC SEQUENCE - %s \r\n", esc_sequence.c_str());

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

        // Enable Renderer.
        TheTerm::Instance()->setRenderReady(true); // Enable Rendering of new screen.

        // Parse Ansi
        //std::cout << output << std::flush;
        TheAnsiParser::Instance()->ansiparse((unsigned char *)output.c_str(),0);


        // Setup cursor in current x/y position Cursor.
        TheTerm::Instance()->SetupCursorChar(TheAnsiParser::Instance()->x_position-1,
                                             TheAnsiParser::Instance()->y_position-1);

        TheTerm::Instance()->RenderCursorOnScreen(TheAnsiParser::Instance()->x_position-1,
                                              TheAnsiParser::Instance()->y_position-1);
        // Look at cursor timing. If were getting input that we've typed
        // Back from server, then we want to display the cursor,
        // Otherwise for Ansi screens and animation we don't want to show it

        /*
        if (output.size() <= 2)
            TheTerm::Instance()->RenderCursorOnScreen(TheAnsiParser::Instance()->x_position-1,
                                                      TheAnsiParser::Instance()->y_position-1);
        */

        // Write out final screen.
        TheTerm::Instance()->DrawTextureScreen();
        output.erase();

        // Enable Renderer.
        TheTerm::Instance()->setRenderReady(false); // Enable Rendering of new screen.
    }
}

void SSHState::update()
{

    //std::cout << "SSHState::update()" << std::endl;
    int ret = 0;

    // Keeps a count of how many loops we run with no server data, if were waiting
    // For input, add a slight Delay in the loop so were not hogging CPU Usage.
    // Reset the count when we get data or type input.
    static int inputCount = 0;

    if (shutdown || TheInputHandler::Instance()->isGlobalShutdown() )
    {
        std::cout << "SSHState::shutdown - changeState(new MainMenuState()" << std::endl;
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
        std::cout << "ERROR SSHState::update() >isActive()" << std::endl;
        shutdown = true;
    }
}

void SSHState::render()
{
    //std::cout << "SSHState::render()" << std::endl;

    // Turns off the Rendering Loop when not being used.
    TheTerm::Instance()->setRenderReady(false);

    // Testing Term, and setup for inital menu / dialing directory.
    //TheTerm::Instance()->ClearScreenSurface();
    //TheTerm::Instance()->DrawString(0,0,(char *)"EtherTerm Telnet Initalized.");
    //TheTerm::Instance()->DrawString(0,16*2,(char *)"[Q] to Quit.");
    //TheTerm::Instance()->RenderScreen();
    //TheTerm::Instance()->DrawTextureScreen();

    // After Screen is draw disable renderer untill new data is received.
    //TheTerm::Instance()->setRenderReady(false);


    // Need To handle Rendering of Dialing Directory here.
    /*
    if(m_loadingComplete && !m_gameObjects.empty())
    {
        for(int i = 0; i < m_gameObjects.size(); i++)
        {
            m_gameObjects[i]->draw();
        }
    }*/
}

bool SSHState::onEnter()
{
    TheTerm::Instance()->setRenderReady(true);
    std::cout << "entering SSHState\n";
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

    if (TheSocketHandler::Instance()->initSSH(sysconn.ip, sysconn.port, sysconn.login, sysconn.password))
    {
        TheTerm::Instance()->ClearScreenSurface();
        std::cout << "SSH Initalized, Now Connecting to " << sysconn.name << "... " << std::endl;
        TheTerm::Instance()->DrawString(0,16*2,(char *)"Connecting...");
        TheTerm::Instance()->RenderScreen();
        TheTerm::Instance()->DrawTextureScreen();
        SDL_Delay(500);
    }
    else
    {
        std::cout << "Error Connecting!" << std::endl;
        shutdown = true;
        return false;
    }

    std::cout << "Connection Successful. " << std::endl;

    // Clear Renderer and Ansi Parser for Fresh Connection.
    TheTerm::Instance()->ClearScreenSurface();
    TheTerm::Instance()->RenderScreen();
    TheAnsiParser::Instance()->reset();

    /*
    // parse the state
    StateParser stateParser;
    stateParser.parseState("assets/attack.xml", menuID, &termObjects, &textureIDList);

    m_callbacks.push_back(0);
    m_callbacks.push_back(s_menuToPlay);
    m_callbacks.push_back(s_exitFromMenu);

    // set the callbacks for menu items
    setCallbacks(m_callbacks);

    m_loadingComplete = true;
    std::cout << "entering MenuState\n";
    */
    return true;
}

bool SSHState::onExit()
{

    std::cout << "SSHState::onExit()" << std::endl;

    /*
    m_exiting = true;

    // clean the game objects
    if(loadingComplete && !termObjects.empty())
    {
        termObjects.back()->clean();
        termObjects.pop_back();
    }

    m_gameObjects.clear();


    // clear the texture manager
    //for(int i = 0; i < m_textureIDList.size(); i++)
    //{
     //   TheTextureManager::Instance()->clearFromTextureMap(m_textureIDList[i]);
    //}
    /
    */

     // Reset the handler(s)
     TheInputHandler::Instance()->reset();
     TheAnsiParser::Instance()->reset();
     TheSocketHandler::Instance()->reset();

     shutdown = true;

     std::cout << "exiting SSHState" << std::endl;
     return true;
}

void SSHState::setCallbacks(const std::vector<Callback>& callbacks)
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


/*
 * Handles Reading the Socket for Data, then Pasring the Data.
 */
 /*
void SSHState::handleSession()
{
    // Output file, testing to caputure ESC Sequences from Server.
    //ofstream myfile1;
    //myfile.open ("esc_codes.txt", ios::app );

    //ofstream myfile1;
    //myfile1.open ("esc_codes1.txt", ios::app );

    std::string ansiret;
    std::string output;

    int len = 0;
    //char message[16385]={'\0'};
    //char message[16385]={'\0'};
    char message[4097]={'\0'};
    std::string newstring;

    // Escape parsing
    static std::string esc_sequence;
    static char more_params = 0;
    unsigned char c, ch;

    int translateUTF8 = FALSE; // WIP!


    // Get Socket Data From Server
//    len = socket_recv(message);
    if ( len < 0)
    {

        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                         "Closed Connection",
                         "Server has shutdown the connection.",
                         NULL);

        if (!useSSH)
        {
            //SDLNet_TCP_Close(sock); // maybe remove.
            SDLNet_Quit();
        }

//        renderer->Close();
        //delete renderer;
        exit(1);
    }

    //myfile1 << message;
    //myfile1.close();

    // If char is utf8 then translate to cp437
    //if (translateUTF8)
    //{
    //    newstring = UTF8tocp437(message);
//    }


    // Loop through data for IAC Telnet Commands
    for (int i = 0; i < len; i++ )
    {

        if (translateUTF8)
            c = newstring[i];
        else
            c = message[i];

        try
        {
            ch = telnet_process_char(sock, c);
        }
        catch (exception& e)
        {
            cout << "Exception telnet_process_char: " << e.what() << '\n';
        }

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
                output += esc_sequence;

                //printf("\r\n ESC SEQUENCE - %s \r\n", esc_sequence.c_str());

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
        renderer->RenderCursorOffScreen(x_position-1, y_position-1);

        // Parse Ansi
        try
        {
            ansiparse(sock, renderer, (unsigned char *)output.c_str(), FALSE);
        }
        catch (exception& e)
        {
            cout << "Exception ansiparse: " << e.what() << '\n';
        }


        // Setup cursor in current x/y position Cursor.
        renderer->SetupCursorChar(x_position-1, y_position-1);

        // Look at cursor timing. If were getting input that we've typed
        // Back from server, then we want to display the cursor,
        // Otherwise for Ansi screens and animation we don't want to show it
        if (output.size() <= 2)
            renderer->RenderCursorOnScreen(x_position-1, y_position-1);

        // Write out final screen.
        renderer->DrawTextureScreen();
        output.erase();
    }

    //myfile.close();
    //myfile1.close();
}
*/
