
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "inputHandler.h"
#include "socketHandler.h"
#include "ansiParser.h"
#include "terminal.h"

#include <iostream>
#include <string>

InputHandler* InputHandler::globalInstance = 0;

InputHandler::InputHandler() :
    globalShutdown(false),
    isWindowMode(false),
    fullScreenWindowSize(0),
    isMouseSelected(false),
    mouseSourceXPosition(0),
    mouseSourceYPosition(0),
    mouseReleaseXPosition(0),
    mouseReleaseYPosition(0)
{
    SDL_StartTextInput();
}

InputHandler::~InputHandler()
{
    SDL_StopTextInput();
    std::cout << "InputHandler Released" << std::endl;
}

bool InputHandler::update()
{
    SDL_Event event;
    std::string sequence;

    TheTerminal::SystemConnection sysConection;
    sysConection = TheTerminal::Instance()->getSystemConnection();

    // Set default KeyMapping when there is no connection
    // ie.. Dialing Directory for Menu System.
    if (sysConection.keyMap == "")
        sysConection.keyMap = "ANSI";

    //Handle events on queue
    while(SDL_PollEvent(&event) != 0 && !isGlobalShutdown())
    {
        //User requests quit
        switch (event.type)
        {
            // If no Keypress check now for Window Events.
            case SDL_WINDOWEVENT:
                switch(event.window.event)
                {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                    /*
                        SDL_Log("Window %d size changed to %dx%d",
                                event.window.windowID, event.window.data1,
                                event.window.data2);*/
                        break;

                    case SDL_WINDOWEVENT_SHOWN:
                        //SDL_Log("Window %d shown", event.window.windowID);
                        break;

                    case SDL_WINDOWEVENT_HIDDEN:
                        //SDL_Log("Window %d hidden", event.window.windowID);
                        break;

                    case SDL_WINDOWEVENT_EXPOSED:
                        //SDL_Log("Window %d exposed", event.window.windowID);
                        //TheTerminal::Instance()->clearScreen();
                        //SDL_RenderPresent(TheTerminal::Instance()->getRenderer());
                        TheTerminal::Instance()->drawTextureScreen();                        
                        break;

                    case SDL_WINDOWEVENT_MOVED:
                    /*
                        SDL_Log("Window %d moved to %d,%d",
                            event.window.windowID, event.window.data1,
                            event.window.data2);*/

                        break;

                    case SDL_WINDOWEVENT_RESIZED:
                    /*
                        SDL_Log("Window %d resized to %dx%d",
                                event.window.windowID, event.window.data1,
                                event.window.data2);*/
                        TheTerminal::Instance()->drawTextureScreen();
                        break;

                    case SDL_WINDOWEVENT_MINIMIZED:
                        //SDL_Log("Window %d minimized", event.window.windowID);
                        break;

                    case SDL_WINDOWEVENT_MAXIMIZED:
                        //SDL_Log("Window %d maximized", event.window.windowID);
                        //TheTerminal::Instance()->clearScreen();
                        //SDL_RenderPresent(TheTerminal::Instance()->getRenderer());
                        TheTerminal::Instance()->drawTextureScreen();
                        break;

                    case SDL_WINDOWEVENT_RESTORED:
                        //SDL_Log("Window %d restored", event.window.windowID);
                        //TheTerminal::Instance()->clearScreen();
                        //SDL_RenderPresent(TheTerminal::Instance()->getRenderer());
                        TheTerminal::Instance()->drawTextureScreen();
                        break;

                    case SDL_WINDOWEVENT_CLOSE:
                        globalShutdown = true;
                        SDL_Log("Window %d closed by user!", event.window.windowID);
                        return false;

                    case SDL_WINDOWEVENT_ENTER:
                        //SDL_Log("Mouse entered window %d",
                        //event.window.windowID);
                        break;

                    case SDL_WINDOWEVENT_LEAVE:
                        //SDL_Log("Mouse left window %d", event.window.windowID);
                        break;

                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        //SDL_Log("Window %d gained keyboard focus",
                        //event.window.windowID);
                        break;

                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        //SDL_Log("Window %d lost keyboard focus",
                        //event.window.windowID);
                        break;

                    default:
                        //SDL_Log("Window %d got unknown event %d",
                        //event.window.windowID, event.window.event);
                        break;

                }// End of Switch
                break;

            case SDL_TEXTINPUT:

                // Check for NumKey and Number Input,  IF Numlock if off
                // Then ignore numbers
                //if (!(event.key.keysym.mod & KMOD_NUM))

                setInputSequence(event.text.text);
                return true;

            case SDL_QUIT:
                return false;

            // Mouse Release and Motion Left is how to select text.
            case SDL_MOUSEBUTTONUP:
                //If the left mouse button was released
                if( event.button.button == SDL_BUTTON_LEFT )
                {
                    isMouseSelected = false;

                    // Get the mouse offsets
                    mouseReleaseXPosition = event.button.x;
                    mouseReleaseYPosition = event.button.y;

                    // Clear the screen of the selection box.
                    TheTerminal::Instance()->drawTextureScreen();

                    // Copy Selected Text to Clipboard. Make sure click
                    // in window with no movement is not considered a selection
                    if (mouseReleaseXPosition != mouseSourceXPosition &&
                        mouseReleaseYPosition != mouseSourceYPosition)
                    {
                        TheTerminal::Instance()->pullSelectionBuffer(
                            mouseReleaseXPosition, mouseReleaseYPosition);
                    }

                    // Needed for Windows Full Screen Mode Switches, otherwise it doesn't
                    // Repopulate properly.  Clear so texture is refreshed each selection
                    TheTerminal::Instance()->clearSelectionTexture();

                    /*
                    std::cout << "Mouse released at: " << mouseReleaseXPosition << ","
                        << mouseReleaseYPosition << std::endl;
                    */
                }
                //SDL_Log("SDL_MOUSEBUTTONUP %d shown", event.button.button);
                break;

            case SDL_MOUSEMOTION:
                if( event.button.button == SDL_BUTTON_LEFT
                    && isMouseSelected)
                {
                    /* debugging
                    std::cout << "Mouse moved by: "
                        << event.motion.xrel << ", " << event.motion.yrel
                        << " : "
                        << event.motion.x << "," << event.motion.y
                        << std::endl;
                    */
                    // Process the Mouse Position from the Origin
                    // To Highligh the selected screen segment.
                    TheTerminal::Instance()->renderSelectionScreen(
                        event.motion.x, event.motion.y);
                }
                break;

            case SDL_MOUSEBUTTONDOWN:

                if( event.button.button == SDL_BUTTON_LEFT )
                {
                    //Get the mouse offsets
                    mouseSourceXPosition = event.button.x;
                    mouseSourceYPosition = event.button.y;

                    //std::cout << "Mouse pressed at: " << mouseSourceXPosition << ","
                    //    << mouseSourceYPosition << std::endl;

                    // Fix for clicking on title bar, 0 pixels will not register
                    // for text selection
                    if (mouseSourceXPosition > 0 && mouseSourceYPosition > 0)
                        isMouseSelected = true;

                    //SDL_Log("SDL_MOUSEBUTTONDOWN %d shown", event.button.button);
                    break;
                }

                //If the Right mouse button was pressed Paste Text.
                else if(event.button.button == SDL_BUTTON_RIGHT)
                {
                    std::cout << "Paste Clipboard. " << std::endl;
                    SDL_StartTextInput();
                    inputText = SDL_GetClipboardText();
                    // Some input filtering
                    std::string::size_type id1 = 0;
                    while(1)
                    {   // Replace \r\n with \r
                        id1 = inputText.find("\r\n", 0);
                        if(id1 != std::string::npos)
                        {
                            inputText.erase(id1+1,1);
                        }
                        else break;
                    }
                    while(1)
                    {   // Replace Tabs with (4) Spaces.
                        // Need Toggle in INI for \t or 4 spaces
                        // On Paste, not all systems handle \t.
                        id1 = inputText.find("\t", 0);
                        if(id1 != std::string::npos)
                        {
                            inputText.replace(id1,1,"    ");
                        }
                        else break;
                    }
                    while(1)
                    {   // Change \n to \r
                        id1 = inputText.find("\n", 0);
                        if(id1 != std::string::npos)
                        {
                            inputText[id1] = '\r';
                        }
                        else break;
                    }
                    setInputSequence(inputText);
                    return true;
                }
                break;

            case SDL_KEYDOWN:

                // Start with CTRL and ALT Keys combo's. TESTING!
                //if( event.key.keysym.mod  & KMOD_NUM ) printf( "\r\nNUMLOCK \r\n" );
                //if( event.key.keysym.mod  & KMOD_CAPS ) printf( "\r\nCAPSLOCK \r\n" );
                //if( event.key.keysym.mod  & KMOD_LCTRL ) printf( "LCTRL " );
                //if( event.key.keysym.mod  & KMOD_RCTRL ) printf( "RCTRL " );
                //if( event.key.keysym.mod  & KMOD_RSHIFT ) printf( "RSHIFT " );
                //if( event.key.keysym.mod  & KMOD_LSHIFT ) printf( "LSHIFT " );
                //if( event.key.keysym.mod  & KMOD_RALT ) printf( "RALT " );
                //if( event.key.keysym.mod  & KMOD_LALT ) printf( "LALT " );
                //if( event.key.keysym.mod  & KMOD_CTRL ) printf( "\r\nCTRL \r\n" );
                //if( event.key.keysym.mod  & KMOD_SHIFT ) printf( "\r\nSHIFT \r\n" );
                //if( event.key.keysym.mod  & KMOD_ALT ) printf( "\r\nALT \r\n" );
                //std::cout << "done!" << std::endl;

                if (event.key.keysym.mod & KMOD_SHIFT &&
                    event.key.keysym.mod & KMOD_CTRL)
                {
                    switch (event.key.keysym.sym)
                    {

                        case 45: //SDLK_UNDERSCORE:
                            //std::cout << "createUnderScoreSequence" << std::endl;
                            setInputSequence("\x1f");
                            return true;

                        default:
                            // Testing
                            //std::cout << "event.key.keysym.sym: "
                                //<< event.key.keysym.sym << std::endl;
                            break;
                    }
                }

                if (event.key.keysym.mod & KMOD_CTRL)
                {
                    switch (event.key.keysym.sym)
                    {
                        // Control Keys should passthrough for SSH
                        // And other Telnet Servers to handle.
                        default:
                            // CTRL Keys have lowercase letters -
                            // translate CTRL Key combo and
                            // Grab the Current Key Symbol.
                            char ch = static_cast<char>(event.key.keysym.sym);
                            sequence = ch;
                            if((event.key.keysym.sym >= (Uint16)'a') &&
                                (event.key.keysym.sym <= (Uint16)'z'))
                            {
                                // Translate Letter to CTRL Letter value
                                ch = CTRLKEYTABLE[ch-97];
                                sequence = ch;
                                setInputSequence(sequence);
                                return true;
                            }
                            break;

                    } // End Switch
                } // END CTRL

                else if (event.key.keysym.mod & KMOD_ALT)
                {
                    switch (event.key.keysym.sym)
                    {

                        case 'h': // ALT H - Hangup
                            TheSocketHandler::Instance()->reset();
                            return false;

                        case SDLK_RETURN:
                            // If not Full Screen Then Toggle to Next Mode.
                            if(!isWindowMode)
                            {
                                //TheTerminal::Instance()->setWindowWidth(640);
                                //TheTerminal::Instance()->setWindowHeight(400);

                                // Have to reset to 640 so when we come out of full screen,
                                // so it will display this rez properly! Otherwise it skips to last size.
                                //SDL_SetWindowSize(TheTerminal::Instance()->getWindow(), 640, 400);

                                // New work around of SDL issues, we now destory the Window
                                // And Renderer and recreated them in our desired resolutions
                                // Due to SDL being retarded on different platforms.
#ifndef _WIN32
                                TheTerminal::Instance()->restartWindowSize(true);
                                TheTerminal::Instance()->restartWindowRenderer("1");
#else
                                TheTerminal::Instance()->restartWindowRenderer("1");
                                SDL_SetWindowFullscreen(TheTerminal::Instance()->getWindow(), SDL_WINDOW_FULLSCREEN_DESKTOP);
#endif
                                SDL_Log("Setting window to FULLSCREEN.");
                                isWindowMode = true; // Reset so next ALT+ENTER we switch to windowed mode.
                                fullScreenWindowSize = 0;
                                TheTerminal::Instance()->clearScreen();
                                SDL_RenderPresent(TheTerminal::Instance()->getRenderer());
                                TheTerminal::Instance()->drawTextureScreen();
                                return false;
                            }
                            else
                            {

                                // OSX Doesn't do fullscreen properly!
                                // Reset Texture Filtering when in windowed mode.
                                // Set Window Mode
                                if (SDL_SetWindowFullscreen(TheTerminal::Instance()->getWindow(), 0) < 0)
                                {
                                    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                                        "Error setting window to Windowed Mode: %s", SDL_GetError());
                                    return false;
                                }

                                //Toggle Between Window Sizes.
                                switch(fullScreenWindowSize)
                                {
                                    // Texture Filtering OFF.
                                    // These (2) Resolutions work perfect for 8x16 fonts
                                    // When the screen is resized the pixels are doubled
                                    // without needing to do any fancy recalculations
                                    // WE use hthese multiple to keep shaded blocks with
                                    // Correct looking pixel size and layout.
                                    case 0:
                                        TheTerminal::Instance()->setWindowWidth(640);
                                        TheTerminal::Instance()->setWindowHeight(400);
                                        SDL_SetWindowSize(TheTerminal::Instance()->getWindow(), 640, 400);

#ifndef _WIN32
                                        TheTerminal::Instance()->restartWindowSize(false);
                                        TheTerminal::Instance()->restartWindowRenderer("0");
#else
                                        TheTerminal::Instance()->restartWindowRenderer("0");
#endif
                                        TheTerminal::Instance()->clearScreen();
                                        SDL_RenderPresent(TheTerminal::Instance()->getRenderer());
                                        TheTerminal::Instance()->drawTextureScreen();
                                        SDL_Log("Setting window size to 640 x 400.");
                                        ++fullScreenWindowSize;
                                        break;

                                    case 1:
                                        TheTerminal::Instance()->setWindowWidth(1280);
                                        TheTerminal::Instance()->setWindowHeight(800);
                                        SDL_SetWindowSize(TheTerminal::Instance()->getWindow(), 1280, 800);
#ifndef _WIN32
                                        TheTerminal::Instance()->restartWindowSize(false);
                                        TheTerminal::Instance()->restartWindowRenderer("0");
#else
                                        TheTerminal::Instance()->restartWindowRenderer("0");
#endif
                                        TheTerminal::Instance()->clearScreen();
                                        SDL_RenderPresent(TheTerminal::Instance()->getRenderer());
                                        TheTerminal::Instance()->drawTextureScreen();
                                        SDL_Log("Setting window size to 1280 x 800.");
                                        ++fullScreenWindowSize;
                                        isWindowMode = false;
                                        break;
                                }
                                return false;
                            }
                            break;

                        default:
                            break;
                    }
                } // End ALT

                // Get remaining function keys not handled on TextInput()
                // Then Translate to ESC Sequences for Telnet.
                else
                {
                    switch(event.key.keysym.sym)
                    {
                        // Handle KeyPad Keys without numlock
                        // Numlock Numbers are read by Text Input already.
                        /* Caught by TextInput()
                        case SDLK_KP_PERIOD:
                            if (event.key.keysym.mod & KMOD_NUM)
                            {
                                setInputSequence(".");
                                return true;
                            }
                            break;*/
                        case SDLK_KP_8: // UP
                            if (!(event.key.keysym.mod & KMOD_NUM))
                            {
                                setInputSequence("\x1b[A");
                                return true;
                            }
                            break;
                        case SDLK_KP_2: // DN
                           if (!(event.key.keysym.mod & KMOD_NUM))
                            {
                                setInputSequence("\x1b[B");
                                return true;
                            }
                            break;
                        case SDLK_KP_6: // RIGHT
                            if (!(event.key.keysym.mod & KMOD_NUM))
                            {
                                setInputSequence("\x1b[C");
                                return true;
                            }
                            break;
                        case SDLK_KP_4: // LEFT
                            if (!(event.key.keysym.mod & KMOD_NUM))
                            {
                                setInputSequence("\x1b[D");
                                return true;
                            }
                            break;

                        case SDLK_ESCAPE:
                            setInputSequence("\x1b");
                            return true;

                        // Add Toggle for Hardware Keys ESC0A etc..
                        case SDLK_UP:
                            if (event.key.keysym.mod & KMOD_SHIFT)
                                setInputSequence("\x1b[1;2A");
                            else
                                setInputSequence("\x1b[A");
                            return true;
                        case SDLK_DOWN:
                            if (event.key.keysym.mod & KMOD_SHIFT)
                                setInputSequence("\x1b[1;2B");
                            else
                                setInputSequence("\x1b[B");
                            return true;
                        case SDLK_RIGHT:
                            if (event.key.keysym.mod & KMOD_SHIFT)
                                setInputSequence("\x1b[1;2C");
                            else
                                setInputSequence("\x1b[C");
                            return true;
                        case SDLK_LEFT:
                            if (event.key.keysym.mod & KMOD_SHIFT)
                                setInputSequence("\x1b[1;2D");
                            else
                                setInputSequence("\x1b[D");
                            return true;
                        case SDLK_RETURN:
                        case SDLK_KP_ENTER:
                            setInputSequence("\r");
                            return true;

                        // Add Swap for BS and DEL (Win vs Nix Terms)                        
                        case SDLK_TAB:
                            if (event.key.keysym.mod & KMOD_SHIFT)
                                setInputSequence("\x1b[Z");
                            else
                                setInputSequence("\t");
                            return true;

                        default:
                            break;
                    }

                    // Term specific key mappings
                    if (sysConection.keyMap == "ANSI")
                    {
                        // screen2|old VT 100/ANSI X3.64 virtual terminal:
                        // Used By Synchronet/Mystic and DOS Type Systems!
                        switch(event.key.keysym.sym)
                        {
                            case SDLK_KP_BACKSPACE:
                            case SDLK_BACKSPACE:
                                setInputSequence("\x08");
                                return true;
                            case SDLK_DELETE:
                                setInputSequence("\x7f");
                                return true;

                            case SDLK_INSERT:   // insert
                                setInputSequence("\x1b[@");
                                return true;
                            case SDLK_HOME:     // home
                                setInputSequence("\x1b[H");
                                return true;
                            case SDLK_END:      // end
                                setInputSequence("\x1b[K");
                                return true;
                            case SDLK_PAGEUP:   // page up
                                setInputSequence("\x1b[V");
                                return true;
                            case SDLK_PAGEDOWN: // page down
                                setInputSequence("\x1b[U");
                                return true;

                            // Handle KeyPad Keys without numlock
                            // Numlock Numbers are read by Text Input already.
                            case SDLK_KP_PERIOD: // Delete
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x7f");
                                    return true;
                                }
                                break;
                            case SDLK_KP_0: // INS
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x1b[@");
                                    return true;
                                }
                                break;
                            case SDLK_KP_1: // END
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x1b[K");
                                    return true;
                                }
                                break;
                            case SDLK_KP_3: // PGDN
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x1b[U");
                                    return true;
                                }
                                break;
                            case SDLK_KP_5: // Empty, space?
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {

                                }
                                break;
                            case SDLK_KP_7: // HOME
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x1b[H");
                                    return true;
                                }
                                break;
                            case SDLK_KP_9: // PAGEUP
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x1b[V");
                                    return true;
                                }
                                break;

                            // Function Keys
                            case SDLK_F1:
                                setInputSequence("\x1b[OP");
                                return true;
                            case SDLK_F2:
                                setInputSequence("\x1b[OQ");
                                return true;
                            case SDLK_F3:
                                setInputSequence("\x1b[OR");
                                return true;
                            case SDLK_F4:
                                setInputSequence("\x1b[OS");
                                return true;
                            case SDLK_F5:
                                // Windows Console Telnet \\x1b[[15~
                                setInputSequence("\x1b[OT");
                                return true;
                            case SDLK_F6:
                                setInputSequence("\x1b[[17~");
                                return true;
                            case SDLK_F7:
                                setInputSequence("\x1b[[18~");
                                return true;
                            case SDLK_F8:
                                setInputSequence("\x1b[[19~");
                                return true;
                            case SDLK_F9:
                                setInputSequence("\x1b[[20~");
                                return true;
                            case SDLK_F10:
                                setInputSequence("\x1b[[21~");
                                return true;
                            case SDLK_F11:
                                setInputSequence("\x1b[[23~");
                                return true;
                            case SDLK_F12:
                                setInputSequence("\x1b[[24~");
                                return true;

                            default:
                                break;
                        }
                    }
                    else if (sysConection.keyMap == "VT100")
                    {
                        // VT-100 Putty
                        switch(event.key.keysym.sym)
                        {
                            // \x7f = ^?  // 0x08 = ^H
                            case SDLK_KP_BACKSPACE:
                            case SDLK_BACKSPACE:
                                setInputSequence("\x7f");
                                return true;
                            case SDLK_DELETE:
                                setInputSequence("\x1b[3~");
                                return true;

                            case SDLK_INSERT:   // insert
                                setInputSequence("\x1b[2~");
                                return true;
                            case SDLK_HOME:     // home
                                setInputSequence("\x1b[1~");
                                return true;
                            case SDLK_END:      // end
                                setInputSequence("\x1b[4~");
                                return true;
                            case SDLK_PAGEUP:   // page up
                                setInputSequence("\x1b[5~");
                                return true;
                            case SDLK_PAGEDOWN: // page down
                                setInputSequence("\x1b[6~");
                                return true;

                            // Handle KeyPad Keys without numlock
                            // Numlock Numbers are read by Text Input already.
                            case SDLK_KP_PERIOD: // Delete
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x1b[3~");
                                    return true;
                                }
                                break;
                            case SDLK_KP_0: // INS
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x1b[2~");
                                    return true;
                                }
                                break;
                            case SDLK_KP_1: // END
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x1b[4~");
                                    return true;
                                }
                                break;
                            case SDLK_KP_3: // PGDN
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x1b[6~");
                                    return true;
                                }
                                break;
                            case SDLK_KP_5: // Empty, space?
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {

                                }
                                break;
                            case SDLK_KP_7: // HOME
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x1b[1~");
                                    return true;
                                }
                                break;
                            case SDLK_KP_9: // PAGEUP
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x1b[5~");
                                    return true;
                                }
                                break;

                            // Function Keys
                            case SDLK_F1:
                                setInputSequence("\x1b[OP");
                                return true;
                            case SDLK_F2:
                                setInputSequence("\x1b[OQ");
                                return true;
                            case SDLK_F3:
                                setInputSequence("\x1b[OR");
                                return true;
                            case SDLK_F4:
                                setInputSequence("\x1b[OS");
                                return true;
                            case SDLK_F5:                                
                                setInputSequence("\x1b[OT");
                                return true;
                            case SDLK_F6:
                                setInputSequence("\x1b[OU");
                                return true;
                            case SDLK_F7:
                                setInputSequence("\x1b[OV");
                                return true;
                            case SDLK_F8:
                                setInputSequence("\x1b[OW");
                                return true;
                            case SDLK_F9:
                                setInputSequence("\x1b[OX");
                                return true;
                            case SDLK_F10:
                                setInputSequence("\x1b[OY");
                                return true;
                            case SDLK_F11:
                                setInputSequence("\x1b[OZ");
                                return true;
                            case SDLK_F12:
                                setInputSequence("\x1b[O[");
                                return true;

                            default:
                                break;
                        }
                    }
                    else if (sysConection.keyMap == "LINUX")
                    {
                        // Linux Terminal Putty
                        switch(event.key.keysym.sym)
                        {
                            // \x7f = ^?  // 0x08 = ^H
                            case SDLK_KP_BACKSPACE:
                            case SDLK_BACKSPACE:
                                setInputSequence("\x7f");
                                return true;
                            case SDLK_DELETE:
                                setInputSequence("\x1b[3~");
                                return true;

                            case SDLK_INSERT:   // insert
                                setInputSequence("\x1b[2~");
                                return true;
                            case SDLK_HOME:     // home
                                setInputSequence("\x1b[1~");
                                return true;
                            case SDLK_END:      // end
                                setInputSequence("\x1b[4~");
                                return true;
                            case SDLK_PAGEUP:   // page up
                                setInputSequence("\x1b[5~");
                                return true;
                            case SDLK_PAGEDOWN: // page down
                                setInputSequence("\x1b[6~");
                                return true;

                            // Handle KeyPad Keys without numlock
                            // Numlock Numbers are read by Text Input already.
                            case SDLK_KP_PERIOD: // Delete
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x1b[3~");
                                    return true;
                                }
                                break;
                            case SDLK_KP_0: // INS
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x1b[2~");
                                    return true;
                                }
                                break;
                            case SDLK_KP_1: // END
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x1b[4~");
                                    return true;
                                }
                                break;
                            case SDLK_KP_3: // PGDN
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x1b[6~");
                                    return true;
                                }
                                break;
                            case SDLK_KP_5: // Empty, space?
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {

                                }
                                break;
                            case SDLK_KP_7: // HOME
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x1b[1~");
                                    return true;
                                }
                                break;
                            case SDLK_KP_9: // PAGEUP
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x1b[5~");
                                    return true;
                                }
                                break;

                            // Function Keys
                            case SDLK_F1:
                                setInputSequence("\x1b[[A");
                                return true;
                            case SDLK_F2:
                                setInputSequence("\x1b[[B");
                                return true;
                            case SDLK_F3:
                                setInputSequence("\x1b[[C");
                                return true;
                            case SDLK_F4:
                                setInputSequence("\x1b[[D");
                                return true;
                            case SDLK_F5:
                                setInputSequence("\x1b[[E");
                                return true;
                            case SDLK_F6:
                                setInputSequence("\x1b[[17~");
                                return true;
                            case SDLK_F7:
                                setInputSequence("\x1b[[18~");
                                return true;
                            case SDLK_F8:
                                setInputSequence("\x1b[[19~");
                                return true;
                            case SDLK_F9:
                                setInputSequence("\x1b[[20~");
                                return true;
                            case SDLK_F10:
                                setInputSequence("\x1b[[21~");
                                return true;
                            case SDLK_F11:
                                setInputSequence("\x1b[[23~");
                                return true;
                            case SDLK_F12:
                                setInputSequence("\x1b[[24~");
                                return true;

                            default:
                                break;
                        }
                    }
                    else if (sysConection.keyMap == "SCO")
                    {
                        // SCO Putty
                        switch(event.key.keysym.sym)
                        {
                            // \x7f = ^?  // 0x08 = ^H
                            case SDLK_KP_BACKSPACE:
                            case SDLK_BACKSPACE:
                                setInputSequence("\x7f");
                                return true;
                            case SDLK_DELETE:
                                setInputSequence("\x08");
                                return true;
                            case SDLK_INSERT:   // insert
                                setInputSequence("\x1b[L");
                                return true;
                            case SDLK_HOME:     // home
                                setInputSequence("\x1b[H");
                                return true;
                            case SDLK_END:      // end
                                setInputSequence("\x1b[F");
                                return true;
                            case SDLK_PAGEUP:   // page up
                                setInputSequence("\x1b[I");
                                return true;
                            case SDLK_PAGEDOWN: // page down
                                setInputSequence("\x1b[G");
                                return true;

                            // Handle KeyPad Keys without numlock
                            // Numlock Numbers are read by Text Input already.
                            case SDLK_KP_PERIOD: // Delete
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x08");
                                    return true;
                                }
                                break;
                            case SDLK_KP_0: // INS
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x1b[L");
                                    return true;
                                }
                                break;
                            case SDLK_KP_1: // END
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x1b[F");
                                    return true;
                                }
                                break;
                            case SDLK_KP_3: // PGDN
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x1b[G");
                                    return true;
                                }
                                break;
                            case SDLK_KP_5: // Empty, space?
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {

                                }
                                break;
                            case SDLK_KP_7: // HOME
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x1b[H");
                                    return true;
                                }
                                break;
                            case SDLK_KP_9: // PAGEUP
                                if (!(event.key.keysym.mod & KMOD_NUM))
                                {
                                    setInputSequence("\x1b[I");
                                    return true;
                                }
                                break;

                            // Function Keys
                            case SDLK_F1:
                                setInputSequence("\x1b[[M");
                                return true;
                            case SDLK_F2:
                                setInputSequence("\x1b[[N");
                                return true;
                            case SDLK_F3:
                                setInputSequence("\x1b[[O");
                                return true;
                            case SDLK_F4:
                                setInputSequence("\x1b[[P");
                                return true;
                            case SDLK_F5:
                                setInputSequence("\x1b[[Q");
                                return true;
                            case SDLK_F6:
                                setInputSequence("\x1b[[R");
                                return true;
                            case SDLK_F7:
                                setInputSequence("\x1b[[S");
                                return true;
                            case SDLK_F8:
                                setInputSequence("\x1b[[T");
                                return true;
                            case SDLK_F9:
                                setInputSequence("\x1b[[U");
                                return true;
                            case SDLK_F10:
                                setInputSequence("\x1b[[V");
                                return true;
                            case SDLK_F11:
                                setInputSequence("\x1b[[W");
                                return true;
                            case SDLK_F12:
                                setInputSequence("\x1b[[X");
                                return true;

                            default:
                                break;
                        }
                    }
                }
                break;

            default:
                break;

        } // End Switch event.type
    }// End While
    
    // If we get here, then there is no input!
    return false;
}

