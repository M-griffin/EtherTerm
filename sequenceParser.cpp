
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "sequenceDecoder.hpp"
#include "socketHandler.hpp"
#include "inputHandler.hpp"
#include "sequenceParser.hpp"
#include "terminal.hpp"

#include <iostream>
#include <sstream>

SequenceParser* SequenceParser::globalInstance = nullptr;

SequenceParser::SequenceParser() :
    preceedingSequence('\0'),
    max_x_position(0),
    saved_cursor_x(1),
    saved_cursor_y(1),
    saved_attribute(0),
    saved_prev_attr(0),
    color_attribute(0),
    prev_color_attribute(0),
    line_wrapped(false),
    cleared_the_screen(false),
    isCursorShown(true)
{
    // Set Default Region to Off!
    TheTerminal::Instance()->setScrollRegion(0, 0, TERM_HEIGHT);
}

SequenceParser::~SequenceParser()
{
    std::cout << "SequenceParser Released" << std::endl;
    try
    {
        std::vector<myScreen>().swap(screenBuffer); // Clear
    }
    catch(std::exception &e)
    {
        std::cout << "Exception std::vector<myScreen>().swap(screenBuffer): "
                  << e.what() << std::endl;
    }
}

/*
 * Reset AnsiParser Specific Class Attributes
 */
void SequenceParser::reset()
{
    x_position = 1;
    y_position = 1;
    preceedingSequence = '\0';
    saved_cursor_x = 1;
    saved_cursor_y = 1;
    saved_attribute = 1;
    saved_prev_attr = 1;
    color_attribute = 0;
    prev_color_attribute = 0;
    cleared_the_screen = false;
    line_wrapped = false;
    std::vector<int>().swap(parameters);
    //isCursorShown = true; // States will override this.

    // Reset Default Colors.
    TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->GREY;
    TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->BLACK;

    // Turn off Scrolling Region
    TheTerminal::Instance()->setScrollRegion(0, 0, TERM_HEIGHT);
}

/*
 * Handles parsing Text and formatting to the screen buffer
 */
void SequenceParser::textInput(const std::string &buffer)
{
    // Char Sequence for Parsing.
    unsigned char sequence = 0;

    // Loop text and parse screen formatting.
    unsigned char nextSequence;
    for(std::string::size_type i = 0; i < buffer.size(); i++)
    {
        // Grab the Sequence translate to unsigned char.
        sequence = buffer[i];
        nextSequence = '\0';

        // Save a copy in case we need to repeat it later.
        preceedingSequence = sequence;

        // Grab the next character in sequence to test for
        // CRLF combinations.
        try
        {
            if(i+1 < buffer.size())
                nextSequence = buffer[i+1];
        }
        catch(std::exception &e)
        {
            std::cout << "Exception AnsiParser::textInput: "
                      << e.what() << std::endl;
        }

        // Debugging!
        //std::cout << "sequence: " << sequence << " "
        //  << static_cast<int>(sequence) << std::endl;

        // Back Space
        if(sequence == '\b')
        {
            //std::cout << "backspace: " << std::endl;
            if(x_position > 1)
                --x_position;
            continue;
        }

        // Handle New Line in ANSI Files properly.
        if(sequence == '\r' && nextSequence == '\n')
        {
            // std::cout << std::endl << "CRLF xpos: " << x_position << std::endl;
            // Stupid fix for expected behavior.  If were on col 81
            // And we get a newline, then were suppose to wrap to next line
            // But also move down a second line!
            if(x_position == 81)
            {
                ++y_position;
            }

            ++i; // Increment past \n (2) char combo.
            x_position = 1;
            ++y_position;

            // Add back for ?7h handling on different TERM WIDTHS
            if(!line_wrapped)
            {
                //screen_buff.color_sequence += "\x1b[40m";
                //screenbuffer('\r');
            }
            // Check here if we need to scroll the screen up 1 row.
            if(y_position > NUM_LINES ||
                    (TheTerminal::Instance()->m_scrollRegionActive &&
                     y_position > TheTerminal::Instance()->m_bottomMargin))
            {
                // If we cleared the screen and hit bottom row, then
                // The very first time we want to spit out the entire screen
                // Since nothing has been drawn yet before we scroll the screen up
                // Row by Row.
                if(cleared_the_screen)
                {
                    TheTerminal::Instance()->renderScreen();       // Surface to Texture
                    TheTerminal::Instance()->drawTextureScreen();  // Draw Texture to Screen
                    TheTerminal::Instance()->scrollScreenUp();     // Scroll the surface up
                    scrollScreenBuffer();
                    if(!TheTerminal::Instance()->m_scrollRegionActive)
                        y_position = NUM_LINES;
                    else
                        y_position = TheTerminal::Instance()->m_bottomMargin;
                    cleared_the_screen = false;
                }
                else
                {
                    TheTerminal::Instance()->renderBottomScreen();   // Surface to Texture of Bottom Row.
                    TheTerminal::Instance()->drawTextureScreen();    // Texture to Screen
                    TheTerminal::Instance()->scrollScreenUp();       // Scroll up for next line.
                    scrollScreenBuffer();
                    if(!TheTerminal::Instance()->m_scrollRegionActive)
                        y_position = NUM_LINES;
                    else
                        y_position = TheTerminal::Instance()->m_bottomMargin;
                }
            }
            //printf("\r\n xpos %i, ypos %i \r\n",x_position, y_position);
            continue;
        }
        else if(sequence == '\r')
        {
            // std::cout << std::endl << "CR xpos: " << x_position << std::endl;
            // Stupid fix for expected behavior.  If were on col 81
            // And we get a newline, then were suppose to wrap to next line
            // But also move down a second line!
            if(x_position == 81)
            {
                ++y_position;
            }

            x_position = 1;
            // Add back for ?7h handling on different TERM WIDTHS
            if(!line_wrapped)
            {
                //screen_buff.color_sequence += "\x1b[40m";
                //screenbuffer('\r');
            }
            continue;
        }
        else if(sequence == '\n')
        {
            // Stupid fix for expected behavior.  If were on col 81
            // And we get a newline, then were suppose to wrap to next line
            // But also move down a second line!
            if(x_position == 81)
            {
                ++y_position;
            }

            // Set position 0, cause next check increments to 1.
            x_position = 1;
            ++y_position;

            // Add back for ?7h handling on different TERM WIDTHS
            if(!line_wrapped)
            {
                //screen_buff.color_sequence += "\x1b[40m";
                //screenbuffer('\r');
            }
            // Check here if we need to scroll the screen.
            if(y_position > NUM_LINES ||
                    (TheTerminal::Instance()->m_scrollRegionActive &&
                     y_position > TheTerminal::Instance()->m_bottomMargin))
            {
                // If we cleared the screen and hit bottom row, then
                // The very first time we want to spit out the entire screen
                // Since nothing has been drawn yet before we scroll the screen.
                if(cleared_the_screen)
                {
                    TheTerminal::Instance()->renderScreen();       // Surface to Texture
                    TheTerminal::Instance()->drawTextureScreen();  // Draw Texture to Screen
                    TheTerminal::Instance()->scrollScreenUp();     // Scroll the surface up
                    scrollScreenBuffer();
                    if(!TheTerminal::Instance()->m_scrollRegionActive)
                        y_position = NUM_LINES;
                    else
                        y_position = TheTerminal::Instance()->m_bottomMargin;
                    cleared_the_screen = false;
                }
                else
                {
                    // Else we want to just append to the last line
                    // Move the last line to the Texture, then
                    // Re-display the screen.
                    TheTerminal::Instance()->renderBottomScreen();   // Surface to Texture of Bottom Row.
                    TheTerminal::Instance()->drawTextureScreen();    // Texture to Screen
                    TheTerminal::Instance()->scrollScreenUp();       // Scroll up for next line.
                    scrollScreenBuffer();
                    if(!TheTerminal::Instance()->m_scrollRegionActive)
                        y_position = NUM_LINES;
                    else
                        y_position = TheTerminal::Instance()->m_bottomMargin;
                }
            }
            continue;
        }

        // Check for X_Position past columns in current line
        if(x_position > characters_per_line)
        {
            // Move to next line
            // If were in scroll region,
            // Then we'll test specifically to scroll the region only.
            if(!TheTerminal::Instance()->m_scrollRegionActive)
            {
                x_position = 1;
                ++y_position;
            }
        }
        // A little more tricky now if were pushing text pass the bottom margin
        //from the scroll region or just x/y coordinates text like status bar
        //outside scroll region we don't want to scroll in this case.

        // Check if y_position greater then screen size
        // if we need to scroll the screen up 1 line.
        // This last check is for normal text being pushed to the screen,
        // NO CR/LR so check if we gone past the bottom margin of the screen.
        if(y_position > NUM_LINES || (TheTerminal::Instance()->m_scrollRegionActive &&
                                      y_position >= TheTerminal::Instance()->m_bottomMargin &&
                                      x_position > characters_per_line))
        {
            // If we cleared the screen and hit bottom row, then
            // The very first time we want to spit out the entire screen
            if(cleared_the_screen)
            {
                // test if scrolling region is active and were drawing in it.
                if(TheTerminal::Instance()->m_scrollRegionActive &&
                        y_position >= TheTerminal::Instance()->m_topMargin &&
                        y_position <= TheTerminal::Instance()->m_bottomMargin)
                {
                    TheTerminal::Instance()->renderScreen();       // Surface to Texture
                    TheTerminal::Instance()->drawTextureScreen();  // Draw Texture to Screen
                    TheTerminal::Instance()->scrollScreenUp();     // Scroll the surface up
                    scrollScreenBuffer();
                    y_position = TheTerminal::Instance()->m_bottomMargin;
                    cleared_the_screen = false;
                    // Reset to beginning of line.
                    if(x_position > characters_per_line)
                        x_position = 1;
                }
                else if(y_position > NUM_LINES)
                {
                    TheTerminal::Instance()->renderScreen();       // Surface to Texture
                    TheTerminal::Instance()->drawTextureScreen();  // Draw Texture to Screen
                    TheTerminal::Instance()->scrollScreenUp();     // Scroll the surface up
                    scrollScreenBuffer();
                    y_position = NUM_LINES;
                    cleared_the_screen = false;
                }
            }
            else
            {
                // Else we want to just append to the last line
                // Move the last line to the Texture, then
                // Re display the screen.
                // test if scrolling region is active and were drawing in it.
                if(TheTerminal::Instance()->m_scrollRegionActive &&
                        y_position >= TheTerminal::Instance()->m_topMargin &&
                        y_position <= TheTerminal::Instance()->m_bottomMargin)
                {
                    TheTerminal::Instance()->renderBottomScreen();   // Surface to Texture of Bottom Row.
                    TheTerminal::Instance()->drawTextureScreen();    // Texture to Screen
                    TheTerminal::Instance()->scrollScreenUp();       // Scroll up for next line.
                    scrollScreenBuffer();
                    // Reset to beginning of line.
                    if(x_position > characters_per_line)
                        x_position = 1;
                    y_position = TheTerminal::Instance()->m_bottomMargin;
                }
                else if(y_position > NUM_LINES)
                {
                    TheTerminal::Instance()->renderBottomScreen();   // Surface to Texture of Bottom Row.
                    TheTerminal::Instance()->drawTextureScreen();    // Texture to Screen
                    TheTerminal::Instance()->scrollScreenUp();       // Scroll up for next line.
                    scrollScreenBuffer();
                    y_position = NUM_LINES;
                }
            }
        }
        //printf(" \r\n XY: %i,%i ",x_position, y_position);
        // Render Char Screen Position 0 Based from 1 Based.
        TheTerminal::Instance()->drawChar(x_position-1, y_position-1, sequence);        // Char to Surface

        // Add to Screen Buffer, Right now were testing unsigned characters,
        // This will be updated to std::string[0] for ASCII characters
        // with extra checking for Unicode soon!
        std::string tempSequence;   // Just temp for now.
        tempSequence = (signed)sequence;
        setScreenBuffer(tempSequence);
        tempSequence.erase();

        // After drawing character send Cursor to Next Position forward.
        ++x_position;
    }
}

/*
 * Handle Screen Position and Display Erase
 */
void SequenceParser::sequenceCursorAndDisplay()
{
    // Switch on Sequence Terminator
    switch(parameters[0])
    {
            // WIP
        case DELETE_CHARACTER:  // ESC[P
            // Deletes the character at the current position by shifting all characters
            // from the current column + p1 left to the current column.  Opened blanks
            // at the end of the line are filled with the current attribute.
            if(parameters.size() == 1)  // Erase current position, move left
            {
                TheTerminal::Instance()->renderDeleteCharScreen(
                    x_position -1, y_position -1, 1);
                break;
            }
            if(parameters.size() == 2)  // Erase current position, move left x
            {
                TheTerminal::Instance()->renderDeleteCharScreen(
                    x_position -1, y_position -1, parameters[1]);
                break;
            }
            break;

        case REPEAT_CHARACTER: // ESC[b
            // Repeat the preceding graphic character P s times (REP).
            if(parameters.size() == 1)  // Repeat Once.
            {
                std::string stringBuilder(1, (char)preceedingSequence);
                textInput(stringBuilder);
                break;
            }
            if(parameters.size() == 2)
            {
                std::string stringBuilder(parameters[1], (char)preceedingSequence);
                textInput(stringBuilder);
                break;
            }
            break;

        case LINE_POS_ABSOLUTE: // ESC[d
            // Line Position Absolute [row] (default = [1,column]) (VPA).
            if(parameters.size() == 1)  // Repeat Once.
            {
                x_position = 1;
                y_position = 1;
                break;
            }
            if(parameters.size() == 2)
            {
                x_position = 1;
                y_position = parameters[1];
                break;
            }
            break;

        case CURSOR_POSITION:
        case CURSOR_POSITION_ALT:
            //printf("\r\n CURSOR_POSITION: p = %i,  param[0] = %i param[1] = %i \r\n", p,  param[0], param[1]);
            if(parameters.size() == 1)
            {
                x_position = 1;
                y_position = 1;
            }
            else if(parameters.size() == 2)
            {
                y_position = parameters[1];
                x_position = 1;
                if(y_position > NUM_LINES)
                    y_position = NUM_LINES;
                else if(y_position < 1)
                    y_position = 1;
            }
            // first_param_implied ESC[;1h no digit on left of ; separator.
            // Empty is sent as -1
            else if(parameters.size() == 3 && parameters[1] == -1)
            {
                x_position = parameters[2];
                if(x_position > characters_per_line)
                    x_position = characters_per_line;
            }
            else
            {
                y_position = parameters[1];
                x_position = parameters[2];

                if(y_position > NUM_LINES)
                    y_position = NUM_LINES;
                else if(y_position < 1)
                    y_position = 1;

                if(x_position > characters_per_line)
                    x_position = characters_per_line;
                else if(x_position < 1)
                    x_position = 1;
            }
            break;

        case SCROLL_REGION:
            if(parameters.size() == 1)
            {
                // Reset to Full Screen.
                TheTerminal::Instance()->setScrollRegion(0, 0, TERM_HEIGHT);
            }
            if(parameters.size() == 3)
            {
                // Enable scrolling from row1 to row2
                TheTerminal::Instance()->setScrollRegion(parameters[1], parameters[2], TERM_HEIGHT);
                // Move the cursor to the home position
                x_position = 1;
                y_position = 1;
            }
            break;

        case CURSOR_PREV_LIVE:
        case CURSOR_UP:
            if(parameters.size() == 1)
            {
                if(y_position > 1)
                    --y_position;
            }
            else
            {
                y_position -= parameters[1];
                if(y_position < 1)
                    y_position = 1;
            }
            break;

        case CURSOR_NEXT_LINE:
        case CURSOR_DOWN:
            if(parameters.size() == 1)
            {
                if(y_position < NUM_LINES)
                    ++y_position;
            }
            else
            {
                y_position += parameters[1];
                if(y_position > NUM_LINES)
                    y_position = NUM_LINES;
            }
            break;

        case CURSOR_FORWARD:
            if(parameters.size() == 1)
            {
                //if (x_position < characters_per_line)
                ++x_position;
                if(x_position > characters_per_line)
                {
                    x_position -= characters_per_line;
                    ++y_position;
                }
            }
            else
            {
                x_position += parameters[1];
                // Some programs push ESC[C past the end
                // of the line and expect us to loop and move
                // Down Automatically.
                if(x_position > characters_per_line)
                {
                    x_position -= characters_per_line;
                    ++y_position;
                }
            }
            break;

        case CURSOR_BACKWARD:
            if(parameters.size() == 1)
            {
                if(x_position > 1)
                {
                    --x_position;
                }
            }
            else if(parameters.size() == 2)
            {
                x_position -= parameters[1];
                if(x_position < 1)
                    x_position = 1;
            }
            break;

        case SAVE_CURSOR_POS:
            saved_cursor_x  = x_position;
            saved_cursor_y  = y_position;
            saved_attribute = color_attribute;
            saved_prev_attr = prev_color_attribute;
            savedForegroundColor = TheTerminal::Instance()->m_currentFGColor;
            savedBackgroundColor = TheTerminal::Instance()->m_currentBGColor;
            break;

        case RESTORE_CURSOR_POS:
            x_position = saved_cursor_x;
            y_position = saved_cursor_y;
            color_attribute = saved_attribute;
            prev_color_attribute = saved_prev_attr;
            TheTerminal::Instance()->m_currentFGColor = savedForegroundColor;
            TheTerminal::Instance()->m_currentBGColor = savedBackgroundColor;
            break;

        case CURSOR_X_POSITION: // XTERM
            if(parameters.size() == 1)
                x_position = 1;
            else
            {
                x_position = parameters[1];
                if(x_position < 1)
                    x_position = 1;
                else if(x_position > characters_per_line)
                    x_position = characters_per_line;
            }
            break;

        case ERASE_DISPLAY:
            // Erase Current Line and Below. ESC[J
            if(parameters.size() == 1)
            {
                TheTerminal::Instance()->renderClearLineBelowScreen(
                    y_position-1, x_position-1);
                break;
            }
            // Erase Current Line and Below. ESC[0J
            if(parameters.size() == 2 && parameters[1] == 0)
            {
                TheTerminal::Instance()->renderClearLineBelowScreen(
                    y_position-1, x_position-1);
                break;
            }
            // Erase Current Line and Above.
            if(parameters.size() == 2 && parameters[1] == 1)
            {
                TheTerminal::Instance()->renderClearLineAboveScreen(
                    y_position-1, x_position-1);
                break;
            }
            // Clear Entire Screen. ESC[2J
            if(parameters.size() == 2 && parameters[1] == 2)
            {
                // If background not 0m, we should probably fill
                // The screen with background color per the spec?
                TheTerminal::Instance()->clearScreenSurface();
                clearScreenBuffer();
                cleared_the_screen = true;
                x_position = 1;
                y_position = 1;
                break;
            }
            break;

        case ERASE_TO_EOL:
            // Handle Rendering from 1 Based to 0 Based so Subtract 1.
            if(parameters.size() == 1)  // Cursor to End of line
            {
                //printf("\r\n EOL xpos %i, ypos %i \r\n", x_position, y_position);
                TheTerminal::Instance()->renderClearLineScreen(y_position-1,
                        x_position-1, characters_per_line); // test removed -1
                clearScreenBufferRange(x_position-1, characters_per_line);
            }
            else if(parameters.size() == 2 && parameters[1] == 1) // Start of Line to Cursor
            {
                TheTerminal::Instance()->renderClearLineScreen(
                    y_position-1, 0, x_position);
                clearScreenBufferRange(0, x_position);
            }
            else if(parameters.size() == 2 && parameters[1] == 2) // Entire Line
            {
                TheTerminal::Instance()->renderClearLineScreen(
                    y_position-1, 0, characters_per_line);
                clearScreenBufferRange(0, characters_per_line);
            }
            break;

        default :
            break;
    }
}

/*
 * Handle Switching Graphics Mode
 * Colors and Cursor modes.
 */
void SequenceParser::sequenceGraphicsModeDisplay()
{
    // Switch on Sequence Terminator
    switch(parameters[0])
    {
        case SET_GRAPHICS_MODE:
            // Rebuild ESC Sequences and Attach Character Position
            // So when we draw out the ANSI we can keep track
            // Of what color everything is.
            // NOTES:
            // Inversion flips colors, however light foreground is translated to dark background.
            // Blinking flips colors, however light foreground is translated to light background
            // Have to double check how iCE colors are supposed to be handled here with blinking.

            //esc_sequence.erase(); // Ignore ESC in color, we get that separately.
            current_color.clear();

            if(parameters.size() == 1)    // Change text attributes / All Attributes off
            {
                //current_color = "\x1b[m";
                prev_color_attribute = 0;
                color_attribute = 0;
                TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->GREY;
                TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->BLACK;
                //screen_buff.color_sequence += current_color;
                //std::cout << "\r\nESC[M\r\n" << std::endl;
                break;
            }
            else
            {
                //current_color += "\x1b[";
                // Current_Color now used to get intensity or Attribute.
                // Set to 1 to skip over Termination Sequence
                for(std::string::size_type i = 1; i < parameters.size(); i++)
                {
                    // Setup Initial Sequence
                    // if (i != 0)
                    //current_color += ";";
                    switch(parameters[i])
                    {
                        case -1: // Implied or Empty Reset = ESC[m
                        case 0:  // All Attributes off
                            //current_color += "0";
                            prev_color_attribute = 0;
                            color_attribute = 0;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            ////printf("\r\nAll Attributes off: %s:%i", esc_sequence.c_str(), color_attribute);
                            // Set default till it's overridden
                            TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->GREY;
                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->BLACK;
                            break;

                        case 1: // BOLD_ON (increase intensity)
                            prev_color_attribute = 1;
                            color_attribute = 1;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            // If current color is dark, flip it to light.
                            if(TheTerminal::Instance()->compareSDL_Colors(TheTerminal::Instance()->m_currentFGColor,TheTerminal::Instance()->BLACK))
                                TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->DARK_GREY;
                            else if(TheTerminal::Instance()->compareSDL_Colors(TheTerminal::Instance()->m_currentFGColor,TheTerminal::Instance()->RED))
                                TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->LIGHT_RED;
                            else if(TheTerminal::Instance()->compareSDL_Colors(TheTerminal::Instance()->m_currentFGColor,TheTerminal::Instance()->GREEN))
                                TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->LIGHT_GREEN;
                            else if(TheTerminal::Instance()->compareSDL_Colors(TheTerminal::Instance()->m_currentFGColor,TheTerminal::Instance()->BROWN))
                                TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->YELLOW;
                            else if(TheTerminal::Instance()->compareSDL_Colors(TheTerminal::Instance()->m_currentFGColor,TheTerminal::Instance()->BLUE))
                                TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->LIGHT_BLUE;
                            else if(TheTerminal::Instance()->compareSDL_Colors(TheTerminal::Instance()->m_currentFGColor,TheTerminal::Instance()->MAGENTA))
                                TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->LIGHT_MAGENTA;
                            else if(TheTerminal::Instance()->compareSDL_Colors(TheTerminal::Instance()->m_currentFGColor,TheTerminal::Instance()->CYAN))
                                TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->LIGHT_CYAN;
                            else if(TheTerminal::Instance()->compareSDL_Colors(TheTerminal::Instance()->m_currentFGColor,TheTerminal::Instance()->GREY))
                                TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->WHITE;
                            break;

                        case 2: // FAINT (decreased intensity) (Not widely used).
                            color_attribute = 2;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            break;

                        case 3: // ITALICS ON
                            color_attribute = 3;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            break;

                        case 4:    // UNDERSCORE
                            color_attribute = 4;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            break;

                        case 5: // BLINK_ON
                            color_attribute = 5;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            // Flip Colors for Ice Colors.
                            SDL_Color tempColor;
                            tempColor = TheTerminal::Instance()->m_currentFGColor;
                            TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->m_currentBGColor;
                            TheTerminal::Instance()->m_currentBGColor = tempColor;
                            break;

                        case 6: // BLINK RAPID MS-DOS ANSI.SYS; 150 per minute or more; not widely supported
                            color_attribute = 6;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            break;

                        case 7: // REVERSE_VIDEO_ON
                            color_attribute = 7;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            tempColor = TheTerminal::Instance()->m_currentFGColor;
                            TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->m_currentBGColor;
                            TheTerminal::Instance()->m_currentBGColor = tempColor;
                            break;

                        case 8: // CONCEALED_ON
                            current_color+= "8";
                            break;

                        case 9: // STRICKTHROUGH
                            current_color += "9";
                            break;

                        case 22: // BOLD OFF
                            //current_color += "22";
                            prev_color_attribute = 0;
                            color_attribute = 0;
                            //std::cout << "Color_Attribute: BOLD OFF " << color_attribute << endl;
                            break;

                        case 23: // ITALICS OFF
                            current_color += "23";
                            //std::cout << "Color_Attribute: ITALICS OFF " << color_attribute << endl;
                            break;

                        case 24: // UNDERLINE OFF
                            current_color += "24";
                            //std::cout << "Color_Attribute: UNDERLINE OFF " << color_attribute << endl;
                            break;

                        case 27: // INVERSE OFF
                            //current_color += "27";
                            color_attribute = 0;
                            //std::cout << "Color_Attribute: INVERSE OFF " << color_attribute << endl;
                            tempColor = TheTerminal::Instance()->m_currentFGColor;
                            TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->m_currentBGColor;
                            TheTerminal::Instance()->m_currentBGColor = tempColor;
                            break;

                        case 28: // CONCEALED_OFF
                            current_color += "28";
                            break;

                        case 29: // STRIKETHROUGH OFF
                            current_color += "29";
                            break;

                            // Foreground Colors
                        case 30: // FG_BLACK
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->BLACK;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->BLACK;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->DARK_GREY;
                                            break;
                                        default:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->BLACK;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->BLACK;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->DARK_GREY;
                                            break;
                                        default:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->BLACK;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->DARK_GREY;
                                    break;
                            }
                            break;

                        case 31: // FG_RED
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->RED;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->RED;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->LIGHT_RED;
                                            break;
                                        default:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->RED;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->RED;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->LIGHT_RED;
                                            break;
                                        default:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->RED;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->LIGHT_RED;
                                    break;
                            }
                            break;

                        case 32: // FG_GREEN
                            //current_color += "32";
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->GREEN;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->GREEN;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->LIGHT_GREEN;
                                            break;
                                        default:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->GREEN;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->GREEN;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->LIGHT_GREEN;
                                            break;
                                        default:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->GREEN;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->LIGHT_GREEN;
                                    break;
                            }
                            break;

                        case 33: // FG_YELLOW
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->BROWN;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->BROWN;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->YELLOW;
                                            break;
                                        default:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->BROWN;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->BROWN;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->YELLOW;
                                            break;
                                        default:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->BROWN;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->YELLOW;
                                    break;
                            }
                            break;

                        case 34: // FG_BLUE
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->BLUE;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->BLUE;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->LIGHT_BLUE;
                                            break;
                                        default:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->BLUE;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->BLUE;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->LIGHT_BLUE;
                                            break;
                                        default:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->BLUE;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->LIGHT_BLUE;
                                    break;
                            }
                            break;

                        case 35: // FG_MAGENTA
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->MAGENTA;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->MAGENTA;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->LIGHT_MAGENTA;
                                            break;
                                        default:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->MAGENTA;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->MAGENTA;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->LIGHT_MAGENTA;
                                            break;
                                        default:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->MAGENTA;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->LIGHT_MAGENTA;
                                    break;
                            }
                            break;

                        case 36: // FG_CYAN
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->CYAN;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->CYAN;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->LIGHT_CYAN;
                                            break;
                                        default:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->CYAN;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->CYAN;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->LIGHT_CYAN;
                                            break;
                                        default:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->CYAN;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->LIGHT_CYAN;
                                    break;
                            }
                            break;

                        case 37: // FG_WHITE
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->GREY;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->GREY;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->WHITE;
                                            break;
                                        default:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->GREY;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->GREY;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->WHITE;
                                            break;
                                        default:
                                            TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->GREY;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->WHITE;
                                    break;
                            }
                            break;

                            // Background Colors
                        case 40: // BG_BLACK
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->BLACK;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->BLACK; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->DARKGREY; break;
                                        default:
                                            TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->DARK_GREY;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->BLACK; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->DARKGREY; break;
                                        default:
                                            TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->BLACK;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->BLACK;
                                    break;
                            }
                            break;

                        case 41: // BG_RED
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->RED;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->RED; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->LIGHTRED; break;
                                        default:
                                            TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->LIGHT_RED;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->RED; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->LIGHTRED; break;
                                        default:
                                            TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->RED;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->RED;
                                    break;
                            }
                            break;

                        case 42: // BG_GREEN
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->GREEN;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->GREEN; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->LIGHTGREEN; break;
                                        default:
                                            TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->LIGHT_GREEN;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->GREEN; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->LIGHTGREEN; break;
                                        default:
                                            TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->GREEN;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->GREEN;
                                    break;
                            }
                            break;

                        case 43: // BG_YELLOW
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->BROWN;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->BROWN; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->YELLOW; break;
                                        default:
                                            TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->YELLOW;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->BROWN; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->YELLOW; break;
                                        default:
                                            TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->BROWN;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->BROWN;
                                    break;
                            }
                            break;

                        case 44: // BG_BLUE
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->BLUE;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->BLUE; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->LIGHTBLUE; break;
                                        default:
                                            TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->LIGHT_BLUE;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->BLUE; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->LIGHTBLUE; break;
                                        default:
                                            TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->BLUE;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->BLUE;
                                    break;
                            }
                            break;

                        case 45: // BG_MAGENTA
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->MAGENTA;
                                    break;

                                case BLINK_ON:
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->MAGENTA; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->LIGHTMAGENTA; break;
                                        default:
                                            TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->LIGHT_MAGENTA;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->MAGENTA; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->LIGHTMAGENTA; break;
                                        default:
                                            TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->MAGENTA;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->MAGENTA;
                                    break;
                            }
                            break;

                        case 46: // BG_CYAN
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->CYAN;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->CYAN; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->LIGHTCYAN; break;
                                        default:
                                            TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->LIGHT_CYAN;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->CYAN; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->LIGHTCYAN; break;
                                        default:
                                            TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->CYAN;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->CYAN;
                                    break;
                            }
                            break;

                        case 47: // BG_WHITE
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->GREY;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->GREY; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->WHITE; break;
                                        default:
                                            TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->WHITE;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->GREY; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->WHITE; break;
                                        default:
                                            TheTerminal::Instance()->m_currentFGColor = TheTerminal::Instance()->GREY;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->m_currentBGColor = TheTerminal::Instance()->GREY;
                                    break;
                            }
                            break;

                        default :
                            //current_color += static_cast<ostringstream*>( &(ostringstream() << param[i]) )->str();
                            //printf("\r\nSET_GRAPHICS_MODE: %s", (char *)esc_sequence.c_str());
                            break;
                    } // End Switch
                } // End For
                //current_color += "m";
                //screen_buff.color_sequence += current_color;
            } // End Else
            break;

        default:
            break;
    }
}

/*
 * Handle Mode Resets and Responses to Server
 */
void SequenceParser::sequenceResetAndResponses()
{
    switch(parameters[0])
    {
        case SET_MODE: // [?h
            if(parameters.size() == 3 && parameters[1] == '?')
            {
                switch(parameters[2])
                {
                    case 7:
                        // Wraparound Mode (DECAWM).
                        //std::cout << "Wraparound Mode" << std::endl;
                        line_wrapped = true;
                        break;
                    case 25:
                        // Show Cursor (DECTCEM).
                        isCursorShown = true;
                        std::cout << "Cursor Shown" << std::endl;
                        break;
                    default:
                        break;
                }
            }
            break;

        case RESET_MODE: // [?l
            if(parameters.size() == 3 && parameters[1] == '?')
            {
                switch(parameters[2])
                {
                    case 7:
                        // Wraparound Mode disabled (DECAWM).
                        line_wrapped = false;
                        //std::cout << "Wraparound Mode Disabled" << std::endl;
                        break;
                    case 25:
                        // Hide Cursor (DECTCEM).
                        isCursorShown = false;
                        std::cout << "Cursor hidden" << std::endl;
                        break;
                    default:
                        break;
                }
            }
            break;

        case SET_KEYBOARD_STRINGS:
            //outf << "SET_KEYBOARD_STRINGS" << esc_sequence << endl;
            // Ingored!
            break;

            // Response back to Server with x/y position sequence.
        case ANSI_DETECTION:
            if(parameters.size() > 0 && parameters[1] == 6)
            {
                int current_y_position = 0;
                // Make sure we don't respond with size larger then our
                // Current Screen, Some BBS's use this response to determine
                // When to send a pause prompt.
                if(y_position > NUM_LINES)
                    current_y_position = NUM_LINES;
                else
                    current_y_position = y_position;

                std::string currentPosition;    // for ESC[6n reply
                std::stringstream streamBuffer;
                streamBuffer << "\x1b[" << current_y_position << ";" << x_position << "R";
                currentPosition = streamBuffer.str();
                if(TheSocketHandler::Instance()->isActive())
                {
                    TheSocketHandler::Instance()->send(
                        (unsigned char *)currentPosition.c_str(),
                        currentPosition.size()+1);
                }
            }
            break;

        default:
            // Rest are ignored.
            break;
    } // End of Switch(c) Case Statements
}

/*
 * Handles parsing Control Sequences and formatting the screen buffer
 * Broken up into (3) Functions to keep it simple and easier to track
 * and maintain. Source Data comes from sequenceParser.cpp
 */
void SequenceParser::sequenceInput(std::vector<int> &sequenceParameters)
{
    // Grab Parameter Control Sequence.
    try
    {
        parameters.swap(sequenceParameters);
    }
    catch(std::exception &e)
    {
        std::cout << "Exception sequenceInput parameters.swap(sequenceParameters): "
                  << e.what() << std::endl;
    }

    // First Handle Any Cursor Movement and Re-display Attributes
    switch(parameters[0])
    {
        case DELETE_CHARACTER:
        case LINE_POS_ABSOLUTE:
        case REPEAT_CHARACTER:
        case CURSOR_POSITION:
        case CURSOR_POSITION_ALT:
        case SCROLL_REGION:
        case CURSOR_PREV_LIVE:
        case CURSOR_UP:
        case CURSOR_NEXT_LINE:
        case CURSOR_DOWN:
        case CURSOR_FORWARD:
        case CURSOR_BACKWARD:
        case SAVE_CURSOR_POS:
        case RESTORE_CURSOR_POS:
        case CURSOR_X_POSITION:
        case ERASE_DISPLAY:
        case ERASE_TO_EOL:
            sequenceCursorAndDisplay();
            std::vector<int>().swap(parameters);
            return;
        default :
            break;
    }

    // Next Handle Graphic Mode Changes
    // Switch on Sequence Terminator
    switch(parameters[0])
    {
        case SET_GRAPHICS_MODE:
            sequenceGraphicsModeDisplay();
            std::vector<int>().swap(parameters);
            return;
        default:
            break;
    }

    // Catch Mode Reset and Responses back to Server
    switch(parameters[0])
    {
        case RESET_MODE:
        case SET_MODE:
        case SET_KEYBOARD_STRINGS:
        case ANSI_DETECTION:
            sequenceResetAndResponses();
            std::vector<int>().swap(parameters);
            return;
        default:
            break;
    }

    // Free and Clear!
    std::vector<int>().swap(parameters);
}


