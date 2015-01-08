
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "inputHandler.h"
#include "ansiParser.h"
#include "terminal.h"
#include <iostream>

InputHandler* InputHandler::globalInstance = 0;

InputHandler::InputHandler() :
    globalShutdown(false),
    fullScreen(false),
    fullScreenWindowSize(0)
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


    //Handle events on queue
    while(SDL_PollEvent(&event) != 0)
    {
        //User requests quit
        switch (event.type)
        {
            case SDL_TEXTINPUT:
                setInputSequence(event.text.text);
                return true;

            case SDL_QUIT:
                return false;

            case SDL_MOUSEBUTTONDOWN:
                //If the left mouse button was pressed Paste Text.
                if(event.button.button == SDL_BUTTON_RIGHT)
                {
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
                // Start with CTRL and ALT Keys combo's.
                if (event.key.keysym.mod & KMOD_CTRL)
                {
                    switch (event.key.keysym.sym)
                    {
                        //Handle copy
                        case SDLK_c:
                            if (SDL_GetModState() & KMOD_CTRL)
                            {
                                inputText =
                                    SDL_SetClipboardText(inputText.c_str());
                                return false;
                            }
                            break;

                        //Handle paste
                        case SDLK_v:
                            if (SDL_GetModState() & KMOD_CTRL)
                            {
                                inputText = SDL_GetClipboardText();
                                // Some input filtering
                                std::string::size_type id1 = 0;
                                while(1)
                                {
                                    id1 = inputText.find("\r\n", 0);
                                    if(id1 != std::string::npos)
                                    {
                                        inputText.erase(id1+1,1);
                                    }
                                    else
                                        break;
                                }
                                while(1)
                                {
                                    id1 = inputText.find("    ", 0);
                                    if(id1 != std::string::npos)
                                    {
                                        inputText.replace(id1,1,"    ");
                                    }
                                    else
                                        break;
                                }
                                while(1)
                                {
                                    // Windows.
                                    id1 = inputText.find("\n", 0);
                                    if(id1 != std::string::npos)
                                    {
                                        inputText[id1] = '\r';
                                    }
                                    else
                                        break;
                                }
                                setInputSequence(inputText);
                                return true;
                            }
                            break;

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
                                //Append the character
                                setInputSequence(sequence.c_str());
                                return true;
                            }
                            break;

                    } // End Switch
                } // END CTRL

                else if (event.key.keysym.mod & KMOD_ALT)
                {
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_RETURN:
                            // If not Full Screen Then Toggle to Next Mode.
                            if(!fullScreen)
                            {
                                // Texture Filtering OFF.
                                // FullScreen Desktop fits the window to the desktop size,
                                // This throws off the pixels.  Using the resolution switch
                                // so that 640, or 1280 is made to full screen fixes this
                                // properly without needing to do any fancy recalculations
                                fullScreen = true; // Can be Full Screen or Full Screen Desktop.
                                SDL_SetWindowFullscreen(TheTerm::Instance()->getWindow(),
                                    SDL_WINDOW_FULLSCREEN); // _DESKTOP);
                                fullScreenWindowSize = 0;
                                TheTerm::Instance()->drawTextureScreen();
                                return false;
                            }
                            else
                            {
                                // Set Full Screen Window.
                                SDL_SetWindowFullscreen(TheTerm::Instance()->getWindow(), 0);
                                //Toggle Between Window Sizes.
                                switch(fullScreenWindowSize)
                                {
                                    // Texture Filtering OFF.
                                    // These (2) Resolutions work perfect for 8x16 fonts
                                    // When the screen is resized the pixels are doubled
                                    // properly without needing to do any fancy recalculations
                                    // to make Shaded blocks or fonts look better!
                                    case 0:
                                        SDL_SetWindowSize(TheTerm::Instance()->getWindow(), 640, 400);
                                        ++fullScreenWindowSize;
                                        break;

                                    case 1:
                                        SDL_SetWindowSize(TheTerm::Instance()->getWindow(), 1280, 800);
                                        ++fullScreenWindowSize;
                                        fullScreen = false;
                                        break;
                                }
                                TheTerm::Instance()->drawTextureScreen();
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
                        case SDLK_ESCAPE:
                            setInputSequence("\x1b");
                            return true;

                        // Add Toggle for Hardware Keys ESC0A etc..
                        case SDLK_UP:
                            setInputSequence("\x1b[A");
                            return true;
                        case SDLK_DOWN:
                            setInputSequence("\x1b[B");
                            return true;
                        case SDLK_RIGHT:
                            setInputSequence("\x1b[C");
                            return true;
                        case SDLK_LEFT:
                            setInputSequence("\x1b[D");
                            return true;
                        case SDLK_RETURN:
                            setInputSequence("\r");
                            return true;

                        // Add Swap for BS and DEL (Win vs Nix Terms)
                        case SDLK_BACKSPACE:
                            setInputSequence("\x08");
                            return true;
                        case SDLK_DELETE:
                            setInputSequence("\x7f");
                            return true;
                        case SDLK_TAB:
                            setInputSequence("    ");
                            return true;

                        // Add Swap for Number Sequences for Nix terms.
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

                        default:
                            break;
                    }
                }
                break;            

            // If no Keypress check now for Window Events.
            case SDL_WINDOWEVENT:
                switch(event.window.event)
                {
                //
                //case SDL_WINDOWEVENT_SHOWN:
                //      SDL_Log("Window %d shown", event.window.windowID);
                //      break;
                //  case SDL_WINDOWEVENT_HIDDEN:
                //      SDL_Log("Window %d hidden", event.window.windowID);
                //      break;
                //  case SDL_WINDOWEVENT_EXPOSED:
                //      SDL_Log("Window %d exposed", event.window.windowID);
                //      break;

                    case SDL_WINDOWEVENT_MOVED:
                        SDL_Log("Window %d moved to %d,%d",
                            event.window.windowID, event.window.data1,
                            event.window.data2);
                        TheTerm::Instance()->drawTextureScreen();
                        break;

                    case SDL_WINDOWEVENT_SHOWN:
                        SDL_Log("Window %d shown", event.window.windowID);
                        TheTerm::Instance()->drawTextureScreen();
                        break;

                    case SDL_WINDOWEVENT_RESIZED:
                        SDL_Log("Window %d resized to %dx%d",
                                event.window.windowID, event.window.data1,
                                event.window.data2);
                        TheTerm::Instance()->drawTextureScreen();
                        break;

                //  case SDL_WINDOWEVENT_MINIMIZED:
                //      SDL_Log("Window %d minimized", event.window.windowID);
                //      break;

                    case SDL_WINDOWEVENT_MAXIMIZED:
                        SDL_Log("Window %d maximized", event.window.windowID);
                        TheTerm::Instance()->drawTextureScreen();
                        break;

                    case SDL_WINDOWEVENT_RESTORED:
                        SDL_Log("Window %d restored", event.window.windowID);
                        TheTerm::Instance()->drawTextureScreen();
                        break;

                    case SDL_WINDOWEVENT_CLOSE:
                        globalShutdown = true;
                        SDL_Log("Window %d closed", event.window.windowID);
                        break;

                //  case SDL_WINDOWEVENT_ENTER:
                //      SDL_Log("Mouse entered window %d",
                //              event.window.windowID);
                //      break;
                /// case SDL_WINDOWEVENT_LEAVE:
                //      SDL_Log("Mouse left window %d", event.window.windowID);
                //      break;
                //  case SDL_WINDOWEVENT_FOCUS_GAINED:
                //      SDL_Log("Window %d gained keyboard focus",
                //              event.window.windowID);
                //      break;
                //  case SDL_WINDOWEVENT_FOCUS_LOST:
                //      SDL_Log("Window %d lost keyboard focus",
                //              event.window.windowID);
                //      break;
                //
                //  default:
                //      SDL_Log("Window %d got unknown event %d",
                //              event.window.windowID, event.window.event);
                //      break;

                    default:
                        break;

                }// End of Switch
                break;
        } // End Switch event.type
    }// End While
    return false;
}
            
