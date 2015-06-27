
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
#include "termStateMachine.hpp"

#include <cstdio>
#include <iostream>
#include <string>

const std::string MainMenuState::m_menuID = "MENU";

// Callbacks
void MainMenuState::menuStartTelnet()
{
    TheStateMachine::Instance()->changeState(new TelnetState());
}

void MainMenuState::menuStartSSH()
{
    TheStateMachine::Instance()->changeState(new SSHState());
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
    // Make sure the System has been reloaded before
    // checking for updates.
    if (isActive)
    {
        // Draw Updates for Page changes etc..
        // Should run this easily after changes received.
        if (TheInputHandler::Instance()->isGlobalShutdown())
        {
            MenuFunction::displayAnsiFile("outro.ans");
            SDL_Delay(1500);
            TheRenderer::Instance()->quit();
            return;
        }

        if(TheInputHandler::Instance()->update())
        {
            std::string inputSequence;
            int ret = 0;

            if (TheInputHandler::Instance()->getInputSequence(inputSequence))
            {
                ret = handleMenuUpdates(inputSequence);
                updateDialDirectory();

                TheRenderer::SystemConnection sysCon;
                if(!TheInputHandler::Instance()->isGlobalShutdown())
                {
                    switch(ret)
                    {
                        case EOF:
                            // default.
                            TheRenderer::Instance()->setCurrentFont("vga8x16.bmp");
                            if(TheRenderer::Instance()->didFontChange())
                            {
                                if (!TheRenderer::Instance()->loadBitmapImage(TheRenderer::Instance()->getCurrentFont()))
                                { }
                                    // On exit Anyways!
                            }


                            MenuFunction::displayAnsiFile("outro.ans");
                            SDL_Delay(1500);
                            TheRenderer::Instance()->quit();
                            break;

                        default:
                            sysCon = TheRenderer::Instance()->getSystemConnection();
                            if(sysCon.protocol == "TELNET")
                            {
                                // Switch to Telnet State
                                menuStartTelnet();
                                return;
                            }
                            else if(sysCon.protocol == "SSH")
                            {
                                TheRenderer::Instance()->setCurrentFont(sysCon.font);
                                if(TheRenderer::Instance()->didFontChange())
                                {
                                    if (!TheRenderer::Instance()->loadBitmapImage(
                                        TheRenderer::Instance()->getCurrentFont()))
                                        {
                                            SDL_Delay(1500);
                                            TheRenderer::Instance()->quit();
                                            break;
                                        }
                                }
                                // Switch to SSH State
                                menuStartSSH();
                                return;
                            }
                            break;
                    }
                }
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
    TheRenderer::Instance()->clearSystemConnection();

    // Startup the Dialing directory system
    setupDialDirectory();

    // Draw the inital Lightbar Listing
    updateDialDirectory();

    isActive = true;
    return true;
}

bool MainMenuState::onExit()
{
    // reset the input handler
    TheInputHandler::Instance()->reset();
    std::cout << "exiting MainMenuState\n";
    isActive = false;
    return true;
}

/*
void MainMenuState::setCallbacks(const std::vector<Callback>& callbacks)
{

}*/

