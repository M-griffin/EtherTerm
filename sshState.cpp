
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "sshState.h"
#include "mainMenuState.h"
#include "inputHandler.h"
#include "ansiParser.h"
#include "sequenceParser.h"
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
    char msgBuffer[8193]= {'\0'};
    std::string newstring;
    unsigned char ch;

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

    //std::cout << "Received SSH Socket Data: " << msgBuffer << std::endl;
    // Loop through data remove any nulls.
    for(int i = 0; i < len; i++)
    {
        ch = msgBuffer[i];

#ifdef _DEBUG
        if (ch == '\r')
            std::cout << "^r" << std::flush;
        else if (ch == '\n')
            std::cout << "^n" << std::flush;
        else if (ch == '\0')
            std::cout << "^0" << std::flush;
        else
            std::cout << ch << std::flush;
#endif

        if(ch == '\0')
            continue;
        output += ch;
    }

    // If available, push Data through ANSI Pasrser then Screen.
    if(output.size() > 0)
    {
        // Parse Incoming Screen Data
        TheSequenceParser::Instance()->processSequence(output);       
        output.erase();
    }
}

void SSHState::update()
{
    //std::cout << "SSHState::update()" << std::endl;
    int ret = 0;

    // Keeps a count of how many loops we run with no server data, if were waiting
    // For input, add a slight Delay in the loop so were not hogging CPU Usage.
    // reset the count when we get data or type input.
    static int inputCount = 0;

    if(shutdown || TheInputHandler::Instance()->isGlobalShutdown())
    {
        std::cout << "SSHState::shutdown - changeState(new MainMenuState()" << std::endl;
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
        std::cout << "ERROR SSHState::update() >isActive()" << std::endl;
        shutdown = true;
    }
}

void SSHState::render()
{
    //std::cout << "SSHState::render()" << std::endl;
    // Turns off the Rendering Loop when not being used.
    TheTerminal::Instance()->setRenderReady(false);

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
    TheAnsiParser::Instance()->setCursorActive(true);
    TheTerminal::Instance()->setRenderReady(true);
    std::cout << "entering SSHState\n";
    shutdown = false;

    // Test Systems
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

    if(TheSocketHandler::Instance()->initSSH(sysconn.ip, sysconn.port, sysconn.login, sysconn.password))
    {
        TheTerminal::Instance()->clearScreenSurface();
        std::cout << "SSH Initalized, Now Connecting to " << sysconn.name << "... " << std::endl;
        TheTerminal::Instance()->drawString(0,16*2,(char *)"Connecting...");
        TheTerminal::Instance()->renderScreen();
        TheTerminal::Instance()->drawTextureScreen();
        SDL_Delay(500);
    }
    else
    {
        std::cout << "Error Connecting!" << std::endl;
        SDL_Delay(100);
        shutdown = true;
        TheTerminal::Instance()->clearScreenSurface();
        TheTerminal::Instance()->renderScreen();
        TheAnsiParser::Instance()->reset();
        return false;
    }

    std::cout << "Connection Successful. " << std::endl;
    SDL_Delay(100);

    // Clear Renderer and Ansi Parser for Fresh Connection.
    TheTerminal::Instance()->clearScreenSurface();
    TheTerminal::Instance()->renderScreen();
    TheAnsiParser::Instance()->reset();
    return true;
}

bool SSHState::onExit()
{
    std::cout << "SSHState::onExit()" << std::endl;
    
    // reset the handler(s)
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
