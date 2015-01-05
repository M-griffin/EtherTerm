
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

{ }

InputHandler::~InputHandler()

{ }


bool InputHandler::update()
{
    SDL_Event event;

    //Handle events on queue
    while(SDL_PollEvent(&event) != 0)
    {
        //User requests quit
        if(event.type == SDL_QUIT)
        {
            return false;
        }
        //If a mouse button was pressed
        if(event.type == SDL_MOUSEBUTTONDOWN)
        {
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
                    else
                        break;
                }

                while(1)
                {   // Replace Tabs with (4) Spaces.
                    id1 = inputText.find("\t", 0);
                    if(id1 != std::string::npos)
                    {
                        inputText.replace(id1,1,"    ");
                    }
                    else
                        break;
                }

                while(1)
                {   // Change \n to \r
                    id1 = inputText.find("\n", 0);
                    if(id1 != std::string::npos)
                    {
                        inputText[id1] = '\r';
                    }
                    else
                        break;
                }

                setInputSequence(inputText);
                SDL_StopTextInput();
                return true;
            }
        }
        // Check for CTRL Key Sequences
        else if(event.type == SDL_KEYDOWN && event.key.keysym.mod & KMOD_CTRL)
        {
            //Handle copy
            if(event.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
            {
                SDL_StartTextInput();
                inputText = SDL_SetClipboardText(inputText.c_str());
                SDL_StopTextInput();
                return false;
            }
            //Handle paste
            else if(event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
            {
                SDL_StartTextInput();
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
                    id1 = inputText.find("\n", 0);
                    if(id1 != std::string::npos)
                    {
                        inputText[id1] = '\r';
                    }
                    else
                        break;
                }
                setInputSequence(inputText);
                SDL_StopTextInput();
                return true;
            }
        }
        // Handle Screen Resizing ALT Keys.
        else if(event.type == SDL_KEYDOWN &&
                event.key.keysym.sym == SDLK_RETURN &&
                event.key.keysym.mod & KMOD_ALT)
        {
            // If not Full Screen Then Toggle to Next Mode.
            if(!fullScreen)
            {
                fullScreen = true; // Can be Full Screen or Full Screen Desktop.
                SDL_SetWindowFullscreen(TheTerm::Instance()->getWindow(), SDL_WINDOW_FULLSCREEN_DESKTOP);
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
                    case 0:
                        SDL_SetWindowSize(TheTerm::Instance()->getWindow(), 640, 400);
                        ++fullScreenWindowSize;
                        break;

                    case 1:
                        SDL_SetWindowSize(TheTerm::Instance()->getWindow(), 800, 520);
                        ++fullScreenWindowSize;
                        break;

                    case 2:
                        SDL_SetWindowSize(TheTerm::Instance()->getWindow(), 1440, 800);
                        ++fullScreenWindowSize;
                        fullScreen = false;
                        break;
                }
                TheTerm::Instance()->drawTextureScreen();
                return false;
            }
        }
        // Catch Remaining Key Events.
        if(event.type == SDL_KEYDOWN)
        {
            // Grab the Current Key Symbol.
            char ch = static_cast<char>(event.key.keysym.sym);
            std::string sequence;
            sequence = ch;

            // Check for Keys that need Special ESC Sequences Sent.
            switch(event.key.keysym.sym)
            {
                case SDLK_ESCAPE: // Passthrough as int (27)
                    setInputSequence(sequence.c_str());
                    return true;
                case SDLK_LEFT:
                    setInputSequence("\x1b[D");
                    return true;
                case SDLK_RIGHT:
                    setInputSequence("\x1b[C");
                    return true;
                case SDLK_UP:
                    setInputSequence("\x1b[A");
                    return true;
                case SDLK_DOWN:
                    setInputSequence("\x1b[B");
                    return true;
                case SDLK_RETURN:
                    setInputSequence("\r");
                    return true;
                case SDLK_BACKSPACE:
                    setInputSequence("\x08");
                    return true;
                case SDLK_DELETE:
                    setInputSequence("\x7f");
                    return true;
                case SDLK_TAB:
                    setInputSequence("    ");
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

                    // Check for Special Characters
                    // If one of these is found, then we will
                    // Pass right through to the bottom
                    // Where we can check for Modifier or use oringal value.
                case SDLK_SEMICOLON:  //';' colon
                case SDLK_LEFTBRACKET: //'[' left bracket
                case SDLK_RIGHTBRACKET: //']' right bracket
                case SDLK_EQUALS:  //'=' equals sign
                case SDLK_SLASH: // '/' forward slash
                case SDLK_BACKSLASH: //'\' backslash
                case SDLK_QUOTE: // ''' quote
                case SDLK_BACKQUOTE: //'`' grave
                case SDLK_COMMA: // ',' comma
                case SDLK_MINUS: // '-' minus sign
                case SDLK_PERIOD: // '.' period
                    // If we get here, then we know it's
                    // One of these keys, Check if shift Modifier.
                    if(event.key.keysym.mod & KMOD_SHIFT || event.key.keysym.mod & KMOD_CAPS)
                    {
                        switch(event.key.keysym.sym)
                        {
                            case SDLK_SEMICOLON:  //':' colon
                                ch = ':';
                                break;
                            case SDLK_LEFTBRACKET: //'[' left bracket
                                ch = '{';
                                break;
                            case SDLK_RIGHTBRACKET: //']' right bracket
                                ch = '}';
                                break;
                            case SDLK_EQUALS:  //'=' equals sign
                                ch = '+';
                                break;
                            case SDLK_SLASH: // '/' forward slash
                                ch = '?';
                                break;
                            case SDLK_BACKSLASH: //'\' backslash
                                ch = '|';
                                break;
                            case SDLK_QUOTE: // ''' quote
                                ch = '"';
                                break;
                            case SDLK_BACKQUOTE: //'`' grave
                                ch = '~';
                                break;
                            case SDLK_COMMA: // ',' comma
                                ch = '<';
                                break;
                            case SDLK_MINUS: // '-' minus sign
                                ch = '_';
                                break;
                            case SDLK_PERIOD: // '.' period
                                ch = '>';
                        }
                    }
                    // Send Keys Either Original or Modified.
                    sequence = ch;
                    setInputSequence(sequence.c_str());
                    return true;
            }

            // Now start normal key processing, Space, Letters and Numbers.
            // Also Checking Shift Modifiers.
            if(event.key.keysym.sym == (Uint16)' ')
            {
                //Append the character
                setInputSequence(sequence.c_str());
                return true;
                //printf("\r\nInput: %c - %i\r\n", ch, ch);
            }
            //If the key is a number
            else if((event.key.keysym.sym >= (Uint16)'0') && (event.key.keysym.sym <= (Uint16)'9'))
            {
                // Handle Shift Number Keys
                if(event.key.keysym.mod & KMOD_SHIFT || event.key.keysym.mod & KMOD_CAPS)
                {
                    switch(ch)
                    {
                        case 48: // 0
                            ch = 41; // )
                            break;
                        case 49: // 1
                            ch = 33; // !
                            break;
                        case 50: // 2
                            ch = 64; // @ ..
                            break;
                        case 51: // 3
                            ch = 35;
                            break;
                        case 52: // 4
                            ch = 36;
                            break;
                        case 53: // 5
                            ch = 37;
                            break;
                        case 54: // 6
                            ch = 94;
                            break;
                        case 55: // 7
                            ch = 38;
                            break;
                        case 56: // 8
                            ch = 42;
                            break;
                        case 57: // 9
                            ch = 40;
                            break;
                        default:
                            break;

                    }
                }
                //Append the character
                sequence = ch;
                setInputSequence(sequence.c_str());
                return true;
                //printf("\r\n Input: %c - %i\r\n", ch, ch);
            }
            //If the key is a lowercase letter
            else if((event.key.keysym.sym >= (Uint16)'a') && (event.key.keysym.sym <= (Uint16)'z'))
            {
                if(event.key.keysym.mod & KMOD_SHIFT || event.key.keysym.mod & KMOD_CAPS)
                {
                    ch = toupper(ch);
                    sequence = ch;
                    setInputSequence(sequence.c_str());
                    return true;
                    //printf("\r\n Input: KMOD_SHIFT %c - %i\r\n", ch, ch);
                }
                else if(event.key.keysym.mod & KMOD_CTRL)
                {
                    // Translate Letter to CTRL Letter.
                    ch = CTRLKEYTABLE[ch-97];
                    sequence = ch;
                    //Append the character
                    setInputSequence(sequence.c_str());
                    return true;
                    //printf("\r\n Input: KMOD_CTRL %c - %i\r\n", ch, ch);
                }
                else if(event.key.keysym.mod & KMOD_ALT)
                {
                    // Nothing Stuff ALT Keys.
                    //printf("\r\n Input KMOD_ALT: %c - %i\r\n", ch, ch);
                }
                else
                {
                    //Append the character
                    setInputSequence(sequence.c_str());
                    return true;
                    //printf("\r\n Input: %c - %i\r\n", ch, ch);
                }
            }
        }
        // If no Keypress check now for Window Events.
        else if(event.type == SDL_WINDOWEVENT)
        {
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
                    //  case SDL_WINDOWEVENT_MOVED:
                    //      SDL_Log("Window %d moved to %d,%d",
                    //              event.window.windowID, event.window.data1,
                    //              event.window.data2);
                    //      break;
                    //

                case SDL_WINDOWEVENT_SHOWN:
                    SDL_Log("Window %d shown", event.window.windowID);
                    TheTerm::Instance()->drawTextureScreen();
                    break;

                case SDL_WINDOWEVENT_RESIZED:
                    SDL_Log("Window %d resized to %dx%d",
                            event.window.windowID, event.window.data1,
                            event.window.data2);

                    TheTerm::Instance()->drawTextureScreen();
                    return false;

                    //case SDL_WINDOWEVENT_MINIMIZED:
                    //  SDL_Log("Window %d minimized", event.window.windowID);
                    //  break;

                case SDL_WINDOWEVENT_MAXIMIZED:
                    SDL_Log("Window %d maximized", event.window.windowID);
                    TheTerm::Instance()->drawTextureScreen();
                    return false;

                case SDL_WINDOWEVENT_RESTORED:
                    SDL_Log("Window %d restored", event.window.windowID);
                    TheTerm::Instance()->drawTextureScreen();
                    return false;

                case SDL_WINDOWEVENT_CLOSE:
                    //TheTerm::Instance()->quit();
                    globalShutdown = true;
                    SDL_Log("Window %d closed", event.window.windowID);
                    return false;

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
            }
        }
    }
    // No inputs of Events.
    return false;
}
