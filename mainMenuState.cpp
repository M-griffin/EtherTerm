
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "mainMenuState.hpp"
#include "telnetState.hpp"
#include "sshState.hpp"
#include "inputHandler.hpp"
#include "sequenceParser.hpp"

#include <cstdio>
#include <iostream>
#include <string>

const std::string MainMenuState::m_menuID = "MENU";

// Callbacks
void MainMenuState::menuStartTelnet()
{
    TheTerminal::Instance()->getStateMachine()->changeState(new TelnetState());
}

void MainMenuState::menuStartSSH()
{
    TheTerminal::Instance()->getStateMachine()->changeState(new SSHState());
}
// end callbacks

/*
 * Change this to handle input, checkKey for non-blocking
 *
 * We need to not have the system freese in the menu loop
 * So we can handle input/output like the rest of the system.
 */
void MainMenuState::update()
{
    // Draw Updates for Page changes etc..
    // Should run this easily after changes received.
    if (TheInputHandler::Instance()->isGlobalShutdown())
    {
        MenuFunction::displayAnsiFile("outro.ans");
        SDL_Delay(1500);
        TheTerminal::Instance()->quit();
        return;
    }

    if(TheInputHandler::Instance()->update())
    {
        std::string inputSequence = TheInputHandler::Instance()->getInputSequence();
        int ret = m_menuManager.handleMenuUpdates(inputSequence);
        m_menuManager.updateDialDirectory();

        TheTerminal::SystemConnection sysCon;
        if(!TheInputHandler::Instance()->isGlobalShutdown())
        {
            switch(ret)
            {
                case EOF:
                    // default.
                    TheTerminal::Instance()->setCurrentFont("vga8x16.bmp");
                    if(TheTerminal::Instance()->didFontChange())
                        TheTerminal::Instance()->loadBitmapImage(TheTerminal::Instance()->getCurrentFont());

                    MenuFunction::displayAnsiFile("outro.ans");
                    SDL_Delay(1500);
                    TheTerminal::Instance()->quit();
                    break;

                default:
                    sysCon = TheTerminal::Instance()->getSystemConnection();
                    if(sysCon.protocol == "TELNET")
                    {
                        TheTerminal::Instance()->setCurrentFont(sysCon.font);
                        if(TheTerminal::Instance()->didFontChange())
                            TheTerminal::Instance()->loadBitmapImage(TheTerminal::Instance()->getCurrentFont());
                        // Switch to Telnet State
                        menuStartTelnet();
                    }
                    else if(sysCon.protocol == "SSH")
                    {
                        TheTerminal::Instance()->setCurrentFont(sysCon.font);
                        if(TheTerminal::Instance()->didFontChange())
                            TheTerminal::Instance()->loadBitmapImage(TheTerminal::Instance()->getCurrentFont());
                        // Switch to SSH State
                        menuStartSSH();
                    }
                    break;
            }
        }
    }
    SDL_Delay(10);
}

bool MainMenuState::onEnter()
{
    // No Cursor for Menu System.
    TheSequenceParser::Instance()->setCursorActive(false);

    // Setup Defaults for Interface.    
    std::cout << "entering MainMenuState\n";

    // Set Render to Ready so we draw menu, Once it's drawn we toggle
    // This off, so it doesn't keep looping since it's not a game
    // with animation, this saves cpu usage.
    TheTerminal::Instance()->clearSystemConnection();

    // Startup the Dialing directory system
    m_menuManager.setupDialDirectory();

    // Draw the inital Lightbar Listing
    m_menuManager.updateDialDirectory();
    return true;
}

bool MainMenuState::onExit()
{
    // reset the input handler
    TheInputHandler::Instance()->reset();
    std::cout << "exiting MainMenuState\n";
    return true;
}

/*
void MainMenuState::setCallbacks(const std::vector<Callback>& callbacks)
{

}*/

