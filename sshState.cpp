
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "sshState.hpp"
#include "mainMenuState.hpp"
#include "inputHandler.hpp"
#include "sequenceParser.hpp"
#include "sequenceManager.hpp"
#include "terminal.hpp"
#include "socketHandler.hpp"
#include "menuFunction.hpp"

#include <cstdio>

const std::string SSHState::sshID = "SSH";

/*
 * Handles Reading the Socket for Data, then Parsing the Data.
 */
void SSHState::handleSession()
{
    int len = 0;
    char msgBuffer[8193] = {'\0'};
    std::string output;

    // Get Socket Data From Server
    len = TheSocketHandler::Instance()->recv(msgBuffer);
    if(len < 0)
    {
        // Error - Lost Connection
        b_isShutdown = true;
        return;
    }
    // Else No Data just return
    else if(len == 0)
        return;

    // Loop through data remove any nulls.
    unsigned char ch;
    for(int i = 0; i < len; i++)
    {
        ch = msgBuffer[i];

#ifdef _DEBUG
        if(ch == '\r')
            std::cout << "^r" << std::flush;
        else if(ch == '\n')
            std::cout << "^n" << std::flush;
        else if(ch == '\0')
            std::cout << "^0" << std::flush;
        else
            std::cout << ch << std::flush;
#endif
        // Synchronet sends annoying amount of null bits
        // because the \0 in rows of 50 or so!
        // when waiting for input!
        if(ch == '\0')
            continue;

        output += ch;
    }

    // If available, push Data through ANSI Parser then Screen.
    if(output.size() > 0)
    {
        // Parse Incoming Screen Data
        TheSequenceManager::Instance()->decode(output);
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

    if(b_isShutdown || TheInputHandler::Instance()->isGlobalShutdown())
    {
        std::cout << "SSHState::shutdown - changeState(new MainMenuState()" << std::endl;
        TheTerminal::Instance()->getStateMachine()->changeState(new MainMenuState());
        b_isShutdown = false;
        return;
    }

    // Handles User Input and Window Events.
    if(TheInputHandler::Instance()->update())
    {
        inputCount = 0;  // reset counter.
        if(TheInputHandler::Instance()->getInputSequence(m_inputSequence))
        {
            if(TheSocketHandler::Instance()->isActive())
            {
                ret = TheSocketHandler::Instance()->send(
                          (unsigned char *)m_inputSequence.c_str(),
                          (int)m_inputSequence.size());
                // Check return value later on on, not used at the moment.
            }
            else
            {
                std::cout << "ERROR !TheSocketHandler::Instance()->isActive()" << std::endl;
                b_isShutdown = true;
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
                b_isShutdown = true;
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
        b_isShutdown = true;
    }
}

bool SSHState::onEnter()
{
    TheSequenceParser::Instance()->setCursorActive(true);
    std::cout << "entering SSHState\n";
    b_isShutdown = false;

    TheTerminal::SystemConnection sysconn;
    sysconn = TheTerminal::Instance()->getSystemConnection();

    // Check if we need to Ask for User-name before starting SSH Connection
    if(sysconn.login == "")
    {
        std::string initConnection = "|CS|15|16Initiating connection to: [|07";
        initConnection += sysconn.name + "|15]";

        MenuFunction::sequenceToAnsi((char *)initConnection.c_str());
        MenuFunction::sequenceToAnsi((char *)"|CR|10Login credentials not found. Enter your login below.");


        char rBuffer[1024] = {0};
        std::string loginId = "|CR|15Use [|07ESC|15] to abort login prompts. |CR|CR|15SSH Login: ";
        int len = 30;

        // Setup the Input Field after the Text
        MenuFunction::inputField(loginId, len);
        MenuFunction::sequenceToAnsi(loginId);

        // Loop Input
        while(!TheInputHandler::Instance()->isGlobalShutdown())
        {
            // Ask for Users Real Name
            MenuFunction::getLine(rBuffer, len);
            if(rBuffer[0] == '\x1b')  // Abort
            {
                b_isShutdown = true;
                TheTerminal::Instance()->clearScreenSurface();
                TheTerminal::Instance()->renderScreen();
                TheSequenceParser::Instance()->reset();
                return false;
            }
            else if(strcmp(rBuffer,"") != 0 && strcmp(rBuffer,"\n") != 0)
            {
                sysconn.login = rBuffer;
                break;
            }
        }

        // Received Shutdown.
        if(TheInputHandler::Instance()->isGlobalShutdown())
        {
            b_isShutdown = true;
            TheTerminal::Instance()->clearScreenSurface();
            TheTerminal::Instance()->renderScreen();
            TheSequenceParser::Instance()->reset();
            return false;
        }

        memset(rBuffer,0,sizeof(rBuffer));
        std::string passwordId =
            "|CR|CR|03(|07Leave password blank to use public key authentication|03)|CR|15SSH Password: ";

        // Setup the Input Field after the Text
        MenuFunction::inputField(passwordId, len);
        MenuFunction::sequenceToAnsi(passwordId);

        // Loop Input
        while(!TheInputHandler::Instance()->isGlobalShutdown())
        {
            // Ask for Users Real Name
            MenuFunction::getLine(rBuffer, len, 0, true);
            if(rBuffer[0] == '\x1b')  // Abort
            {
                b_isShutdown = true;
                TheTerminal::Instance()->clearScreenSurface();
                TheTerminal::Instance()->renderScreen();
                TheSequenceParser::Instance()->reset();
                return false;
            }
            else if(strcmp(rBuffer,"\n") != 0)
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
    if(TheInputHandler::Instance()->isGlobalShutdown())
    {
        b_isShutdown = true;
        TheTerminal::Instance()->clearScreenSurface();
        TheTerminal::Instance()->renderScreen();
        TheSequenceParser::Instance()->reset();
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

        // Get input (Blocking!) This should be rewritten later on.
        MenuFunction::getKey();

        std::cout << "Error Connecting!" << std::endl;
        b_isShutdown = true;
        TheTerminal::Instance()->clearScreenSurface();
        TheTerminal::Instance()->renderScreen();
        TheSequenceParser::Instance()->reset();
        return false;
    }

    // Clear Renderer and ANSI Parser for Fresh Connection.
    TheTerminal::Instance()->clearScreenSurface();
    TheTerminal::Instance()->renderScreen();
    TheSequenceParser::Instance()->reset();
    return true;
}

bool SSHState::onExit()
{
    std::cout << "SSHState::onExit()" << std::endl;

    // reset the handler(s)
    TheInputHandler::Instance()->reset();
    TheSequenceParser::Instance()->reset();
    TheSocketHandler::Instance()->reset();

    b_isShutdown = true;
    std::cout << "exiting SSHState" << std::endl;
    return true;
}

/*
void SSHState::setCallbacks(const std::vector<Callback>& callbacks)
{
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
    }
}
*/
