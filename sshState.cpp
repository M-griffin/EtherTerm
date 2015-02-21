
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
#include "menuFunction.h"

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
        // Synchronet sends anyoing amount of null bits
        // becasue the \0 in rows of 50 or so!
        // when waiting for input!
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

bool SSHState::onEnter()
{
    TheAnsiParser::Instance()->setCursorActive(true);
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

    // Check if we need to Ask for UserName before starting SSH Connection
    if (sysconn.login == "")
    {
        std::string initConnection = "|CS|15|16Initiating connection to: [|07";
        initConnection += sysconn.name + "|15]";

        MenuFunction::sequenceToAnsi((char *)initConnection.c_str());
        MenuFunction::sequenceToAnsi((char *)"|CR|10Login credentials not found. Enter your login below.");

        char loginId[1024] = {0};
        char rBuffer[1024] = {0};
        sprintf(loginId, "|CR|15Use [|07ESC|15] to abort login prompts. |CR|CR|15SSH Login: ");
        int len = 30;

        // Setup the Input Field after the Text
        MenuFunction::inputField(loginId,len);
        MenuFunction::sequenceToAnsi(loginId);

        // Loop Input
        while(!TheInputHandler::Instance()->isGlobalShutdown())
        {
            // Ask for Users Real Name
            MenuFunction::getLine(rBuffer, len);
            if (rBuffer[0] == '\x1b') // Abort
            {
                shutdown = true;
                TheTerminal::Instance()->clearScreenSurface();
                TheTerminal::Instance()->renderScreen();
                TheAnsiParser::Instance()->reset();
                return false;
            }
            else if (strcmp(rBuffer,"") != 0 && strcmp(rBuffer,"\n") != 0)
            {
                sysconn.login = rBuffer;
                break;
            }
        }

        // Received Shutdown.
        if (TheInputHandler::Instance()->isGlobalShutdown())
        {
            shutdown = true;
            TheTerminal::Instance()->clearScreenSurface();
            TheTerminal::Instance()->renderScreen();
            TheAnsiParser::Instance()->reset();
            return false;
        }

        char passwordId[1024] = {0};
        memset(rBuffer,0,sizeof(rBuffer));
        sprintf(passwordId,
            "|CR|CR|03(|07Leave password blank to use public key Authenticate|03)|CR|15SSH Password: ");

        // Setup the Input Field after the Text
        MenuFunction::inputField(passwordId,len);
        MenuFunction::sequenceToAnsi(passwordId);

        // Loop Input
        while(!TheInputHandler::Instance()->isGlobalShutdown())
        {
            // Ask for Users Real Name
            MenuFunction::getLine(rBuffer, len, 0, true);
            if (rBuffer[0] == '\x1b') // Abort
            {
                shutdown = true;
                TheTerminal::Instance()->clearScreenSurface();
                TheTerminal::Instance()->renderScreen();
                TheAnsiParser::Instance()->reset();
                return false;
            }
            else if (strcmp(rBuffer,"\n") != 0)
            {
                // Enter, Completed!
                sysconn.password = rBuffer;
                break;
            }
        }
    }
    else
    {
        std::string initConnection = "|CS|15|16Initiating connection to: [|07";
        initConnection += sysconn.name + "|15]";

        MenuFunction::sequenceToAnsi((char *)initConnection.c_str());
        MenuFunction::sequenceToAnsi((char *)"|CR|10Loading credentials from dialdirectory.xml");
    }

    // Received Shutdown.
    if (TheInputHandler::Instance()->isGlobalShutdown())
    {
        shutdown = true;
        TheTerminal::Instance()->clearScreenSurface();
        TheTerminal::Instance()->renderScreen();
        TheAnsiParser::Instance()->reset();
        return false;
    }

    std::string initConnection = "|CR|CR|15Establishing connection.";
    MenuFunction::sequenceToAnsi((char *)initConnection.c_str());

    SDL_Delay(100);

    if(TheSocketHandler::Instance()->initSSH(sysconn.ip, sysconn.port, sysconn.login, sysconn.password)
        && !TheInputHandler::Instance()->isGlobalShutdown())
    {
        // Connection Success.
        std::string initConnection = "|CR|CR|08[|07*|08] |10Connection Successful!";
        MenuFunction::sequenceToAnsi((char *)initConnection.c_str());
        SDL_Delay(1000);
    }
    else
    {
        // Connection Failed.
        std::string initConnection = "|CR|CR|08[|07*|08] |12Unable to connect!";
        initConnection += "|CR|CR|15Hit any key to continue.";
        MenuFunction::sequenceToAnsi((char *)initConnection.c_str());
        MenuFunction::getKey();

        std::cout << "Error Connecting!" << std::endl;        
        shutdown = true;
        TheTerminal::Instance()->clearScreenSurface();
        TheTerminal::Instance()->renderScreen();
        TheAnsiParser::Instance()->reset();
        return false;
    }
 
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
