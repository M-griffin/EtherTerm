#ifdef TARGET_OS_MAC
#include <SDL2/SDL.h>
#elif _WIN32
#include <SDL2/SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "menu_io.hpp"
#include "sequence_decoder.hpp"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>

MenuIO::MenuIO(sequence_decoder_ptr decoder, std::string program_path)
    : m_sequence_decoder(decoder)
    , m_program_path(program_path)

{
    std::cout << "MenuIO Created!" << std::endl;
}

MenuIO::~MenuIO()
{
    std::cout << "~MenuIO" << std::endl;
}


/**
 * @brief Right string padding
 * @param str
 * @param space
 */
void MenuIO::rightSpacing(std::string &str, int space)   // Pad Right
{
    if(space == 0) return;

    int s = str.size();
    // if Line > Space, Erase to Make it match!
    if(s >= space)
    {
        str.erase(space, s - space);
        return;
    }
    for(int i = 0; i < (space-s); i++)
        str += ' ';
}

/**
 * @brief Left String Padding.
 * @param str
 * @param space
 */
void MenuIO::leftSpacing(std::string &str, int space)   // Pad Left
{
    if(space == 0) return;

    int s = str.size();
    // if Line > Space, Erase to Make it match!
    if(s > space)
    {
        // Truncate to the last space digits.
        str.erase(0, s - space);
        return;
    }
    for(int i = 0; i < (space-s); i++)
        str.insert(0, 1, ' ');
}

/**
 * @brief Setup Text Input Fields
 * @param text
 * @param len
 */
void MenuIO::inputField(std::string &text, int &len)
{
    std::string repeat;
    char formatted[1024]= {0};

    char sTmp[3]  = {0};
    char sTmp2[3] = {0};

    // Parse for Input String Modifiers
    std::string::size_type tempLength;
    std::string::size_type position;
    std::string::size_type stringSize;

    char INPUT_COLOR[255]= {0};
    bool isColorOverRide = false; //found input color

    stringSize = text.size()-1;
    if(len == 0)
    {
        return;
    }

    // Override Input Length for ANSI
    position = text.find("|IN", 0);
    if(position != std::string::npos)
    {
        // Make sure we don't go past the bounds
        if(position+4 <= stringSize)
        {
            // (Unit Test Notes)
            // Need to Test if idDigit!  And only if, both are
            // Then we cut these out and erase!,  Otherwise
            // We only remove the |IN pipe sequence.
            if(isdigit(text[position+3]) && isdigit(text[position+4]))
            {
                sTmp[0] = text[position+3];
                sTmp[1] = text[position+4];
                text.erase(position, 5);
                tempLength = atoi(sTmp);
                if((signed)tempLength < len)
                    len = tempLength; // Set new Length
            }
            else
            {
                text.erase(position, 3);
            }
        }
    }

    // Override Foreground/Background Input Field Colors
    position = text.find("|FB",0);
    if(position != std::string::npos)
    {
        // (Unit Test Notes)
        // Need to Test if isDigit!  And only if, both are
        // Then we cut these out and erase!,  Otherwise
        // We only remove the |FB pipe sequence.

        memset(&sTmp, 0, 3);
        memset(&sTmp2, 0, 3);
        // Make sure we don't go past the bounds
        if(position+6 <= stringSize)
        {
            if(isdigit(text[position+3]) && isdigit(text[position+4]) &&
                    isdigit(text[position+5]) && isdigit(text[position+6]))
            {
                sTmp[0]  = text[position+3]; // Foreground 00-15
                sTmp[1]  = text[position+4];
                sTmp2[0] = text[position+5]; // Background 16-23
                sTmp2[1] = text[position+6];
                text.erase(position, 7);
                sprintf(INPUT_COLOR, "|%s|%s", sTmp, sTmp2);
                isColorOverRide = true;
            }
            else
            {
                text.erase(position, 3);
            }
        }
    }

    // Pad len amount of spaces.
    if(len > 0)
    {
        repeat.insert(0, len, ' ');
    }

    // Set Default Input Color if none was passed.
    if(!isColorOverRide)
    {
        sprintf(INPUT_COLOR,"|00|19");
    }

    // Format Input Field
    sprintf(formatted, "%s|07|16[%s%s|07|16]%s\x1b[%iD",
            (char *)text.c_str(), // Field Name
            INPUT_COLOR,          // Field Fg,Bg Color
            repeat.c_str(),       // Padding length of Field
            INPUT_COLOR,          // Reset Input
            len+1);               // Move back to starting position of field.

    text = formatted;
}


/*
 * Get Single Key Input (Blocking)
 *
int MenuIO::getKey()
{
    std::string inputSequence;
    while(!TheInputHandler::Instance()->isGlobalShutdown())
    {
        // Catch updates when in the menu system.
        TheSequenceManager::Instance()->update();

        if(TheInputHandler::Instance()->update())
        {
            if(TheInputHandler::Instance()->getInputSequence(inputSequence))
                break;
        }
        SDL_Delay(10);
    }

    // If Global Exit, return right away.
    if(TheInputHandler::Instance()->isGlobalShutdown())
    {
        return EOF;
    }
    return inputSequence[0];
}*/


/*
 * Get Single Key Input (Non-Blocking)
 *
 * Not Used At this time.
 *
int MenuIO::checkKey()
{
    std::string inputSequence;
    if(TheInputHandler::Instance()->isGlobalShutdown())
    {
        return EOF;
    }
    if(TheInputHandler::Instance()->update())
    {
        inputSequence = TheInputHandler::Instance()->getInputSequence();
    }
    else
    {
        SDL_Delay(10);
        return 0;
    }
    return inputSequence[0];
}
*/

/*
 * Get Input up to <ENTER>
 */
/*
void MenuIO::getLine(char *line,     // Returns Input into Line
                    int   length,   // Max Input Length of String
                    char *leadoff,  // Data to Display in Default String {Optional}
                    int   hid)      // If input is Echoed as hidden      {Optional}
{
   int sequence = 0;
   int secondSequence = 0;
   int i   = 0;
   int Col = 0;
   int isEscapeSequence = 0;
   int  cursorBlink = 0;
   bool startBlinking = false;

   std::string output;
   std::string input;
   std::string inputSequence;

   // Cursor Blinking.
   time_t ttime, ttime2;
   ttime = SDL_GetTicks();
#define DEL 0x7f

   // If were starting Off Input with a String already in buffer!  display it!
   if(leadoff != 0)
   {
       input = leadoff;
       i = input.size();
       Col = i;
       //TheSequenceManager::Instance()->decode(input);
       m_sequence_decoder->decodeEscSequenceData(input);
   }

   while(!TheInputHandler::Instance()->isGlobalShutdown())
   {
       // Catch Screen updates when in the menu system.
       TheSequenceManager::Instance()->update();

       if(TheInputHandler::Instance()->update() &&
               !TheInputHandler::Instance()->isGlobalShutdown())
       {
           // We got data, turn off the cursor!
           ttime = SDL_GetTicks();
           startBlinking = false;
           cursorBlink = 0;
           // Get the Sequence.
           if(TheInputHandler::Instance()->getInputSequence(inputSequence))
           {
               // Check for Abort, single ESC character.
               if(inputSequence == "\x1b" && inputSequence.size() == 1)
               {
                   isEscapeSequence = false;
                   strcpy(line,"\x1b");
                   return;
               }
           }
       }
       else
       {
           if(TheSequenceParser::Instance()->isCursorActive() &&
                   !TheInputHandler::Instance()->isGlobalShutdown())
           {
               startBlinking = true;
               // Setup Timer for Blinking Cursor
               // Initial State = On, then Switch to off in next loop.
               if(cursorBlink % 2 == 0)
               {
                   ttime2 = SDL_GetTicks();
                   if(startBlinking && (ttime2 - ttime) > 400)
                   {
                       TheRenderer::Instance()->renderCursorOffScreen();
                       TheRenderer::Instance()->drawTextureScreen();
                       --cursorBlink;
                       ttime = SDL_GetTicks();
                   }
               }
               else
               {
                   ttime2 = SDL_GetTicks();
                   if(startBlinking && (ttime2 - ttime) > 400)
                   {
                       TheRenderer::Instance()->renderCursorOnScreen();
                       TheRenderer::Instance()->drawTextureScreen();
                       ++cursorBlink;
                       ttime = SDL_GetTicks();
                   }
               }
           }
           SDL_Delay(10);
           continue;
       }

       // Catch any shutdown here before doing anymore.
       if(TheInputHandler::Instance()->isGlobalShutdown())
       {
           return;
       }
       sequence = inputSequence[0];
       if(sequence == '\r')
       {
           sequence = '\n';
       }

       // Escape in this case, ignore, later add movement in string
       if((int)sequence == 27)
       {
           if(inputSequence.size() >= 3)
               secondSequence = inputSequence[2];
           isEscapeSequence = true;
       }
       else
       {
           isEscapeSequence = false;
       }

       // Catch all Escaped Keys for Cursor Movement
       if(isEscapeSequence)
       {
           switch(secondSequence)
           {
               case '3' : // Delete
                   if(i != 0 || Col != 0)
                   {
                       sequenceToAnsi("\x1b[D \x1b[D");
                       input.erase(Col-1,1);
                       --i;
                       --Col;
                   }
                   break;

               default :
                   break;
           }
       }
       else if((int)sequence == 25)
       {
           // CTRL Y - Clear Line
           input.erase();
           i = Col;
           for(; i != 0; i--)
           {
               sequenceToAnsi("\x1b[D \x1b[D");
           }
           i = 0;
           Col = i;
       }

       // delete 127
       // Do destructive backspace
       // on VT100 Terms 127 DEL == BS!
       // Since we have no DELETE in this, delete on 1 liens will works like BS.
       else if((int)sequence == 0x08 ||
               (int)sequence == 207  ||
               (int)sequence == 0x7f)
       {
           if(i != 0 || Col != 0)
           {
               sequenceToAnsi("\x1b[D \x1b[D");
               input.erase(Col-1,1);
               --i;
               --Col;
           }
       }
       // Normal Key Input, Letters & numbers
       else if((int)sequence > 31 && (int)sequence < 126)
       {
           if(i != length-1)
           {
               if(hid)
               {
                   sequenceToAnsi("*");
               }
               else
               {
                   output = sequence;
                   sequenceToAnsi(output);
               }
               input += sequence;
               ++i;
               ++Col;
           }
       }
       else if(sequence == '\r' || sequence == '\n')
       {
           strncpy(line, (char *)input.c_str(), length);
           break;
       }
   }

   // If Global Exit, return right away.
   if(TheInputHandler::Instance()->isGlobalShutdown())
   {
       return;
   }
}
*/

/**
 * @brief Foreground ESC Sequence Translation
 * @param data
 * @param fg
 */
void MenuIO::foregroundColorSequence(char *data, int fg)
{
    switch(fg)
    {
        case 0:
            strcat(data, "x[0;30m");
            break;
        case 1:
            strcat(data, "x[0;34m");
            break;
        case 2:
            strcat(data, "x[0;32m");
            break;
        case 3:
            strcat(data, "x[0;36m");
            break;
        case 4:
            strcat(data, "x[0;31m");
            break;
        case 5:
            strcat(data, "x[0;35m");
            break;
        case 6:
            strcat(data, "x[0;33m");
            break;
        case 7:
            strcat(data, "x[0;37m");
            break;
        case 8:
            strcat(data, "x[1;30m");
            break;
        case 9:
            strcat(data, "x[1;34m");
            break;
        case 10:
            strcat(data, "x[1;32m");
            break;
        case 11:
            strcat(data, "x[1;36m");
            break;
        case 12:
            strcat(data, "x[1;31m");
            break;
        case 13:
            strcat(data, "x[1;35m");
            break;
        case 14:
            strcat(data, "x[1;33m");
            break;
        case 15:
            strcat(data, "x[1;37m");
            break;
        default :
            break;
    }
    data[0] = '\x1b';
}

/**
 * @brief Background ESC Sequence Translation
 * @param data
 * @param bg
 */
void MenuIO::backgroundColorSequence(char *data, int bg)
{
    switch(bg)
    {
        case 16:
            strcat(data, "x[40m");
            break;
        case 17:
            strcat(data, "x[44m");
            break;
        case 18:
            strcat(data, "x[42m");
            break;
        case 19:
            strcat(data, "x[46m");
            break;
        case 20:
            strcat(data, "x[41m");
            break;
        case 21:
            strcat(data, "x[45m");
            break;
        case 22:
            strcat(data, "x[43m");
            break;
        case 23:
            strcat(data, "x[47m");
            break;
            // Default to none.
        case 24:
            strcat(data, "x[0m");
            break;
        default :
            break;
    }
    data[0] = '\x1b';
}

/**
 * @brief Parses MCI and PIPE Codes to ANSI Sequences
 * @param sequence
 */
void MenuIO::sequenceToAnsi(const std::string &sequence)
{
    std::string::size_type id1 = 0;  // Pipe Position
    char pipe_sequence[3];           // Holds 1, 2nd digit of Pipe
    char pipe_position1[3];          // Hold XY Pos for ANSI Position Codes
    char pipe_position2[3];          // Hold XY Pos for ANSI Position Codes
    char esc_sequence[1024];         // Holds Converted Pipe 2 ANSI

    std::string data_string = sequence;
#define SP 0x20

    // Search for First Pipe
    while(id1 != std::string::npos)
    {
        id1 = data_string.find("|", id1);
        if(id1 != std::string::npos && id1+2 < data_string.size())
        {
            memset(&pipe_sequence,0,sizeof(pipe_sequence));
            memset(&esc_sequence,0,sizeof(esc_sequence));
            pipe_sequence[0] = data_string[id1+1];  // Get First # after Pipe
            pipe_sequence[1] = data_string[id1+2];  // Get Second Number After Pipe
            if(pipe_sequence[0] == '\0' || pipe_sequence[0] == '\r' || pipe_sequence[0] == EOF) break;
            if(pipe_sequence[1] == '\0' || pipe_sequence[1] == '\r' || pipe_sequence[0] == EOF) break;

            //std::cout << "\r\n*** pipe_sequence: " << pipe_sequence << std::endl;
            if(isdigit(pipe_sequence[0]) && isdigit(pipe_sequence[1]))
            {
                std::string temp_sequence = "";
                std::string::size_type type;
                temp_sequence += pipe_sequence[0];
                temp_sequence += pipe_sequence[1];
                int pipe_color_value = 0;

                try
                {
                    pipe_color_value = std::stoi(temp_sequence, &type);

                    if(pipe_color_value >= 0 && pipe_color_value < 16)
                    {
                        foregroundColorSequence(esc_sequence, pipe_color_value);
                    }
                    else if(pipe_color_value >= 16 && pipe_color_value < 24)
                    {
                        backgroundColorSequence(esc_sequence, pipe_color_value);
                    }
                    else
                    {
                        ++id1;
                    }

                    // Replace pipe code with ANSI Sequence
                    if(strcmp(esc_sequence,"") != 0)
                    {
                        data_string.replace(id1, 3, esc_sequence);
                    }

                }
                catch(const std::invalid_argument& ia)
                {
                    std::cout << "Invalid argument: " << ia.what() << '\n';
                    ++id1;
                }
            }
            // Else not a Pipe Color / Parse for Screen Modification
            else if(pipe_sequence[0] == 'C')
            {
                // Carriage Return / New Line
                if(strcmp(pipe_sequence,"CR") == 0)
                {
                    backgroundColorSequence(esc_sequence, 16);  // Clear Background Attribute first
                    strcat(esc_sequence,"\r\n");
                    data_string.replace(id1, 3, esc_sequence);
                    id1 = 0;
                }
                // Clear Screen
                else if(strcmp(pipe_sequence,"CS") == 0)
                {
                    backgroundColorSequence(esc_sequence, 16);
                    // Set Scroll Region, Clear Background, Then Home Cursor.
                    strcat(esc_sequence,"\x1b[2J\x1b[1;1H");
                    data_string.replace(id1, 3, esc_sequence);
                    id1 = 0;
                }
            }
            else
            {
                if(strcmp(pipe_sequence,"XY") == 0 && id1+6 < data_string.size())
                {
                    memset(&pipe_position1, 0, sizeof(pipe_position1));
                    memset(&pipe_position2, 0, sizeof(pipe_position2));
                    // X Pos
                    pipe_position1[0] = data_string[id1+3];
                    pipe_position1[1] = data_string[id1+4];
                    // Y Pos
                    pipe_position2[0] = data_string[id1+5];
                    pipe_position2[1] = data_string[id1+6];

                    // Clear Background Attribute first
                    backgroundColorSequence(esc_sequence, 16);

                    char char_replacement[200] = {0};
                    sprintf(char_replacement,"%s\x1b[%i;%iH", esc_sequence, atoi(pipe_position2), atoi(pipe_position1));
                    data_string.replace(id1, 7, char_replacement);
                }
                else
                {
                    // End of the Line, nothing parsed out so
                    // Skip ahead past current code.
                    ++id1;
                }
            }
        }
        else
        {
            break;
        }
        id1 = data_string.find("|",id1);
    }
    m_sequence_decoder->decodeEscSequenceData(data_string);
}

/**
 * @brief Reads in ANSI text file and pushes to Display
 * @param file_name
 */
void MenuIO::displayMenuAnsi(const std::string &file_name)
{
    std::string path = m_program_path;
#ifdef _WIN32
    path.append("assets\\directory\\");
#else
    path.append("assets/directory/");
#endif
    path.append(file_name);

    std::string buff;
    FILE *fp;
    int sequence = 0;
    if((fp = fopen(path.c_str(), "r+")) ==  nullptr)
    {
        std::cout << "MenuIO displayAnsiFile(): not found: "
                  << std::endl << path << std::endl;
        return;
    }
    do
    {
        sequence = getc(fp);
        if(sequence != EOF)
            buff += sequence;
    }
    while(sequence != EOF);
    fclose(fp);

    // Send to the Sequence Manager.
    m_sequence_decoder->decodeEscSequenceData(buff);
}
