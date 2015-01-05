
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "sequenceParser.h"
#include "socketHandler.h"
#include "inputHandler.h"
#include "ansiParser.h"
#include "terminal.h"

#include <iostream>
#include <sstream>

using namespace std;

/*
typedef struct myscreen
{
    std::string esc_sequence;
    std::string color_sequence;
    unsigned char c;
    int x_pos;
    int y_pos;

} myscreen;

myscreen screen_buff;
std::vector < myscreen > ansi_screen_buffer;
*/


/**
 * handle screen buffer
 * This function Insert Char a string position,
 * If position doesn't exist yet, it allocate
 * or Pad until we read the position.
 *
void screenbuffer(unsigned char c)
{

    // Keep track of the lonest line in buffer for Centering screen.
    if (x_position > max_x_position)
        max_x_position = x_position;

    screen_buff.c = c;
    screen_buff.x_pos = x_position;
    screen_buff.y_pos = y_position;

     // Move Cursor to next position after character insert.
    if (x_position > characters_per_line)
    {
        // Move to next line
        x_position = 1;
        ++y_position;
    }
    else
    {
        ++x_position;
    }

    ansi_screen_buffer.push_back(screen_buff);

    // Clear for next sequences.
    screen_buff.c = '\0';
    screen_buff.x_pos = 1;
    screen_buff.y_pos = 1;
    screen_buff.color_sequence.erase();
    screen_buff.esc_sequence.erase();

}
*/

AnsiParser* AnsiParser::globalInstance = 0;

AnsiParser::AnsiParser() :
    x_position(1),
    y_position(1),
    max_x_position(0),
    characters_per_line(80),
    position(0),
    saved_cursor_x(1),
    saved_cursor_y(1),
    color_attribute(0),
    prev_color_attribute(0),
    line_wrapped(false),
    cleared_the_screen(false)
{ }

/*
 * Reset AnsiParser Sepcific Class Attributes
 */
void AnsiParser::reset()
{
    saved_cursor_x = 1;
    saved_cursor_y = 1;
    color_attribute = 0;
    prev_color_attribute = 0;
    cleared_the_screen = false;
    line_wrapped = false;

    // Reset Default Colors.
    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->grey;
    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->black;
}

/*
 * Handles parsing Text and formatting to the screen buffer
 */
void AnsiParser::textInput(std::string buffer)
{
    //std::cout << "TextInput:" << buffer << std::endl;
    // Reset Each run.
    position                   = 0;
    max_x_position             = 0;
    characters_per_line        = 80;

    // Char Sequence for Parsing.
    unsigned char sequence     = 0;

    // Toggle on the Renderer.
    TheTerm::Instance()->setRenderReady(true);

    // Loop text and parse screen formatting.
    std::string::iterator it;
    for(it = buffer.begin(); it != buffer.end(); ++it)
    {
        // Grab the Sequence translate to unsigned char.
        sequence = *it;

        // Back Spoce
        if(sequence == '\b')
        {
            if(x_position > 1)
                --x_position;
            continue;
        }
        // Handle New Line in ANSI Files properly.
        if(sequence == '\r' && (*it+1) == '\n')
        {
            //printf("CRLF");
            // Char Buffer, move to next position.
            ++*it; // Incriment past \n (2) char combo.
            x_position = 1;
            ++y_position;

            // Doesn't seem to be needed anymore
            if(!line_wrapped)
            {
                //screen_buff.color_sequence += "\x1b[40m";
                //screenbuffer('\r');
            }
            // Check here if we need to scroll the screen up 1 row.
            if(y_position > NUM_LINES ||
                    (TheTerm::Instance()->scrollRegionActive &&
                     y_position > TheTerm::Instance()->botMargin))
            {
                // If we cleared the screen and hit bottom row, then
                // The very first time we want to spit out the entire screen
                // Since nothing has been drawn yet before we scroll the screen up
                // Row by Row.
                if(cleared_the_screen)
                {
                    TheTerm::Instance()->renderScreen();       // Surface to Texture
                    TheTerm::Instance()->drawTextureScreen();  // Draw Texture to Screen
                    TheTerm::Instance()->scrollScreenUp();     // Scroll the surface up
                    if(!TheTerm::Instance()->scrollRegionActive)
                        y_position = NUM_LINES;
                    else
                        y_position = TheTerm::Instance()->botMargin;
                    cleared_the_screen = false;
                }
                else
                {
                    TheTerm::Instance()->renderBottomScreen();    // Surface to Texture of Bottom Row.
                    TheTerm::Instance()->drawTextureScreen();    // Testure to Screen
                    TheTerm::Instance()->scrollScreenUp();       // Scroll up for next line.
                    if(!TheTerm::Instance()->scrollRegionActive)
                        y_position = NUM_LINES;
                    else
                        y_position = TheTerm::Instance()->botMargin;
                }
            }
            //printf("\r\n xpos %i, ypos %i \r\n",x_position, y_position);
            continue;
        }
        if(sequence == '\r')
        {
            //printf("CR");
            x_position = 1;
            // Doesn't seem to be needed anymore
            if(!line_wrapped)
            {
                //screen_buff.color_sequence += "\x1b[40m";
                //screenbuffer('\r');
            }
            continue;
        }
        else if(sequence == '\n')
        {
            //printf("LF");
            // Set position 0, casue next check incriments to 1.
            x_position = 1;
            ++y_position;

            // Doesn't seem to be needed anymore
            if(!line_wrapped)
            {
                //screen_buff.color_sequence += "\x1b[40m";
                //screenbuffer('\r');
            }
            // Check here if we need to scroll the screen.
            if(y_position > NUM_LINES ||
                    (TheTerm::Instance()->scrollRegionActive &&
                     y_position > TheTerm::Instance()->botMargin))
            {
                // If we cleared the screen and hit bottom row, then
                // The very first time we want to spit out the entire screen
                // Since nothing has been drawn yet before we scroll thes screen.
                if(cleared_the_screen)
                {
                    TheTerm::Instance()->renderScreen();          // Surface to Texture
                    TheTerm::Instance()->drawTextureScreen();  // Draw Texture to Screen
                    TheTerm::Instance()->scrollScreenUp();     // Scroll the surface up
                    if(!TheTerm::Instance()->scrollRegionActive)
                        y_position = NUM_LINES;
                    else
                        y_position = TheTerm::Instance()->botMargin;
                    cleared_the_screen = false;
                }
                else
                {
                    // Else we want to just append to the last line
                    // Move the last line to the Texture, then
                    // Redisplay the scrren.
                    TheTerm::Instance()->renderBottomScreen();    // Surface to Texture of Bottom Row.
                    TheTerm::Instance()->drawTextureScreen();    // Testure to Screen
                    TheTerm::Instance()->scrollScreenUp();       // Scroll up for next line.
                    if(!TheTerm::Instance()->scrollRegionActive)
                        y_position = NUM_LINES;
                    else
                        y_position = TheTerm::Instance()->botMargin;
                }
            }
            continue;
        }
        // Check for X_Position past colums in current line
        if(x_position > characters_per_line)
        {
            // Move to next line
            // If were in scroll region,
            // Then we'll test specifically to scroll the region only.
            if(!TheTerm::Instance()->scrollRegionActive)
            {
                x_position = 1;
                ++y_position;
            }
        }
        // A little more tricky now if were pushing text pass the bottom margin
        //from the scroll region or just x/y coording text like status bar
        //outside scroll region we don't want to scroll in this case.

        // Check if y_position greater then screen size
        // if we need to scroll the screen up 1 line.
        // This last check is for normal text being pushed to the screen,
        // NO CR/LR so check if we gone past the bottom margin of the screen.
        if(y_position > NUM_LINES || (TheTerm::Instance()->scrollRegionActive &&
                                      y_position >= TheTerm::Instance()->botMargin &&
                                      x_position > characters_per_line))
        {
            // If we cleared the screen and hit bottom row, then
            // The very first time we want to spit out the entire screen
            if(cleared_the_screen)
            {
                // test if scrollign region is active and were drawing in it.
                if(TheTerm::Instance()->scrollRegionActive &&
                        y_position >= TheTerm::Instance()->topMargin &&
                        y_position <= TheTerm::Instance()->botMargin)
                {
                    TheTerm::Instance()->renderScreen();       // Surface to Texture
                    TheTerm::Instance()->drawTextureScreen();  // Draw Texture to Screen
                    TheTerm::Instance()->scrollScreenUp();     // Scroll the surface up
                    y_position = TheTerm::Instance()->botMargin;
                    cleared_the_screen = false;
                    // Reset to begining of line.
                    if(x_position > characters_per_line)
                        x_position = 1;
                }
                else if(y_position > NUM_LINES)
                {
                    TheTerm::Instance()->renderScreen();          // Surface to Texture
                    TheTerm::Instance()->drawTextureScreen();  // Draw Texture to Screen
                    TheTerm::Instance()->scrollScreenUp();     // Scroll the surface up
                    y_position = NUM_LINES;
                    cleared_the_screen = false;
                }
            }
            else
            {
                // Else we want to just append to the last line
                // Move the last line to the Texture, then
                // Redisplay the scrren.
                // test if scrollign region is active and were drawing in it.
                if(TheTerm::Instance()->scrollRegionActive &&
                        y_position >= TheTerm::Instance()->topMargin &&
                        y_position <= TheTerm::Instance()->botMargin)
                {
                    TheTerm::Instance()->renderBottomScreen();    // Surface to Texture of Bottom Row.
                    TheTerm::Instance()->drawTextureScreen();    // Testure to Screen
                    TheTerm::Instance()->scrollScreenUp();       // Scroll up for next line.
                    // Reset to begining of line.
                    if(x_position > characters_per_line)
                        x_position = 1;
                    y_position = TheTerm::Instance()->botMargin;
                }
                else if(y_position > NUM_LINES)
                {
                    TheTerm::Instance()->renderBottomScreen();    // Surface to Texture of Bottom Row.
                    TheTerm::Instance()->drawTextureScreen();    // Testure to Screen
                    TheTerm::Instance()->scrollScreenUp();       // Scroll up for next line.
                    y_position = NUM_LINES;
                }
            }
        }
        //printf(" \r\n XY: %i,%i ",x_position, y_position);
        // Render Char Screen Position 0 Based from 1 Based.
        TheTerm::Instance()->drawChar(x_position-1, y_position-1, sequence);        // Char to Surface
        //TheTerm::Instance()->RenderCharScreen(x_position-1, y_position-1);   // Surface to Texture
        // After draining Character send Cursor to Next Position forward.
        ++x_position;
    }
}

/*
 * Handle Screen Position and Display Erase
 */
void AnsiParser::sequenceCursorAndDisplay()
{
    // Switch on Sequence Terminator
    switch(parameters[0])
    {
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
            // first_param_implied ESC[;1h no digit on left of ; seperator.
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
            //screen_buff.esc_sequence += esc_sequence;
            break;

        case SCROLL_REGION:
            if(parameters.size() == 1)
            {
                // Reset to Full Screen.
                TheTerm::Instance()->setScrollRegion(0, 0);
            }
            if(parameters.size() == 3)
            {
                // Enable scrolling from row1 to row2
                TheTerm::Instance()->setScrollRegion(parameters[1], parameters[2]);
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
            //screen_buff.esc_sequence += esc_sequence;
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
            //screen_buff.esc_sequence += esc_sequence;
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
            //screen_buff.esc_sequence += esc_sequence;
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
            //screen_buff.esc_sequence += esc_sequence;
            break;

        case SAVE_CURSOR_POS:
            saved_cursor_x = x_position;
            saved_cursor_y = y_position;
            //screen_buff.esc_sequence += esc_sequence;
            break;

        case RESTORE_CURSOR_POS:
            x_position = saved_cursor_x;
            y_position = saved_cursor_y;
            //screen_buff.esc_sequence += esc_sequence;
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
            //printf("\r\n\r\n CLEARTHESCREEN!! param[0] %i, %c \r\n\r\n", param[0],param[0]);
            // Clearn Screen ESC[J  // Lasy Servers use ESC[J!
            if(parameters.size() == 1)
            {
                TheTerm::Instance()->clearScreenSurface();
                cleared_the_screen = true;
                // Home Cursor, some bbs systems don't do this and makes a mess!!
                x_position = 1;
                y_position = 1;
            }
            // Clearn Screen ESC[2J
            if(parameters.size() == 2 && parameters[1] == 2)
            {
                //printf("\r\n\r\n CLEARTHESCREEN!! \r\n\r\n");
                TheTerm::Instance()->clearScreenSurface();
                cleared_the_screen = true;
                // Home Cursor, some bbs systems don't do this and makes a mess!!
                x_position = 1;
                y_position = 1;
                //screen_buff.esc_sequence += esc_sequence;
            }
            break;

        case ERASE_TO_EOL:
            //screen_buff.esc_sequence += esc_sequence;
            // Handle Rendering from 1 Bases to 0 Based so Subtract 1.
            if(parameters.size() == 1)  // Cursor to End of line
            {
                //printf("\r\n EOL xpos %i, ypos %i \r\n", x_position, y_position);
                TheTerm::Instance()->renderClearLineScreen(y_position-1,
                        x_position-1, characters_per_line); // test removeed -1
            }
            else if(parameters.size() == 2 && parameters[1] == 1) // Start of Line to Cursor
                TheTerm::Instance()->renderClearLineScreen(
                    y_position-1, 0, x_position);
            else if(parameters.size() == 2 && parameters[1] == 2) // Entire Line
                TheTerm::Instance()->renderClearLineScreen(
                    y_position-1, 0, characters_per_line);
            break;

        default :
            break;
    }
}

/*
 * Handle Switching Graphics Mode
 * Colors and Cursor modes.
 */
void AnsiParser::sequenceGraphicsModeDisplay()
{
    // Switch on Sequence Terminator
    switch(parameters[0])
    {
        case SET_GRAPHICS_MODE:
            // Rebuild ESC Sequqnces and Attach Character Position
            // So when we draw out the ansi we can keep track
            // Of what color everything is.
            //esc_sequence.erase(); // Ignore ESC in color, we get that sepeartely.
            current_color.clear();
            //position = ((y_position-1) * characters_per_line) + (x_position-1);

            if(parameters.size() == 1)    // Change text attributes / All Attributes off
            {
                //current_color = "\x1b[m";
                prev_color_attribute = 0;
                color_attribute = 0;
                TheTerm::Instance()->currentFGColor = TheTerm::Instance()->grey;
                TheTerm::Instance()->currentBGColor = TheTerm::Instance()->black;
                //screen_buff.color_sequence += current_color;
                //std::cout << "\r\nESC[M\r\n" << std::endl;
                break;
            }
            else
            {
                //current_color += "\x1b[";
                // Current_Color now used to get intenestity or Attribute.
                // Set to 1 to skip over Termination Sequence
                for(std::string::size_type i = 1; i < parameters.size(); i++)
                {
                    // Setup Iniqal Sequence
                    // if (i != 0)
                    //current_color += ";";
                    switch(parameters[i])
                    {
                        case -1: // Implied or Empty Reset
                        case 0:  // All Attributes off
                            //current_color += "0";
                            prev_color_attribute = 0;
                            color_attribute = 0;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            ////printf("\r\nAll Attributes off: %s:%i", esc_sequence.c_str(), color_attribute);
                            // Set default till it's overwridden
                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->grey;
                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->black;
                            break;

                        case 1: // BOLD_ON (increase intensity)
                            prev_color_attribute = 1;
                            color_attribute = 1;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            // If current color is dark, flip it to light.
                            if(TheTerm::Instance()->compareSDL_Colors(TheTerm::Instance()->currentFGColor,TheTerm::Instance()->black))
                                TheTerm::Instance()->currentFGColor = TheTerm::Instance()->darkGrey;
                            else if(TheTerm::Instance()->compareSDL_Colors(TheTerm::Instance()->currentFGColor,TheTerm::Instance()->red))
                                TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightRed;
                            else if(TheTerm::Instance()->compareSDL_Colors(TheTerm::Instance()->currentFGColor,TheTerm::Instance()->green))
                                TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightGreen;
                            else if(TheTerm::Instance()->compareSDL_Colors(TheTerm::Instance()->currentFGColor,TheTerm::Instance()->brown))
                                TheTerm::Instance()->currentFGColor = TheTerm::Instance()->yellow;
                            else if(TheTerm::Instance()->compareSDL_Colors(TheTerm::Instance()->currentFGColor,TheTerm::Instance()->blue))
                                TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightBlue;
                            else if(TheTerm::Instance()->compareSDL_Colors(TheTerm::Instance()->currentFGColor,TheTerm::Instance()->magenta))
                                TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightMagenta;
                            else if(TheTerm::Instance()->compareSDL_Colors(TheTerm::Instance()->currentFGColor,TheTerm::Instance()->cyan))
                                TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightCyan;
                            else if(TheTerm::Instance()->compareSDL_Colors(TheTerm::Instance()->currentFGColor,TheTerm::Instance()->grey))
                                TheTerm::Instance()->currentFGColor = TheTerm::Instance()->white;
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
                            tempColor = TheTerm::Instance()->currentFGColor;
                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->currentBGColor;
                            TheTerm::Instance()->currentBGColor = tempColor;
                            break;

                        case 6: // BLINK RAPID MS-DOS ANSI.SYS; 150 per minute or more; not widely supported
                            color_attribute = 6;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            break;

                        case 7: // REVERSE_VIDEO_ON
                            color_attribute = 7;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            tempColor = TheTerm::Instance()->currentFGColor;
                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->currentBGColor;
                            TheTerm::Instance()->currentBGColor = tempColor;
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
                            tempColor = TheTerm::Instance()->currentFGColor;
                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->currentBGColor;
                            TheTerm::Instance()->currentBGColor = tempColor;
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
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->black;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->black;
                                            break;
                                        case 1:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->darkGrey;
                                            break;
                                        default:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->black;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->black;
                                            break;
                                        case 1:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->darkGrey;
                                            break;
                                        default:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->black;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->darkGrey;
                                    break;
                            }
                            break;

                        case 31: // FG_RED
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->red;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->red;
                                            break;
                                        case 1:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->lightRed;
                                            break;
                                        default:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->red;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->red;
                                            break;
                                        case 1:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->lightRed;
                                            break;
                                        default:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->red;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightRed;
                                    break;
                            }
                            break;

                        case 32: // FG_GREEN
                            //current_color += "32";
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->green;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->green;
                                            break;
                                        case 1:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->lightGreen;
                                            break;
                                        default:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->green;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->green;
                                            break;
                                        case 1:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->lightGreen;
                                            break;
                                        default:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->green;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightGreen;
                                    break;
                            }
                            break;

                        case 33: // FG_YELLOW
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->brown;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->brown;
                                            break;
                                        case 1:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->yellow;
                                            break;
                                        default:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->brown;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->brown;
                                            break;
                                        case 1:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->yellow;
                                            break;
                                        default:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->brown;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->yellow;
                                    break;
                            }
                            break;

                        case 34: // FG_BLUE
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->blue;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->blue;
                                            break;
                                        case 1:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->lightBlue;
                                            break;
                                        default:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->blue;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->blue;
                                            break;
                                        case 1:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->lightBlue;
                                            break;
                                        default:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->blue;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightBlue;
                                    break;
                            }
                            break;

                        case 35: // FG_MAGENTA
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->magenta;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->magenta;
                                            break;
                                        case 1:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->lightMagenta;
                                            break;
                                        default:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->magenta;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->magenta;
                                            break;
                                        case 1:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->lightMagenta;
                                            break;
                                        default:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->magenta;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightMagenta;
                                    break;
                            }
                            break;

                        case 36: // FG_CYAN
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->cyan;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->cyan;
                                            break;
                                        case 1:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->lightCyan;
                                            break;
                                        default:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->cyan;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->cyan;
                                            break;
                                        case 1:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->lightCyan;
                                            break;
                                        default:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->cyan;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightCyan;
                                    break;
                            }
                            break;

                        case 37: // FG_WHITE
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->grey;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->grey;
                                            break;
                                        case 1:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->white;
                                            break;
                                        default:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->grey;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->grey;
                                            break;
                                        case 1:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->white;
                                            break;
                                        default:
                                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->grey;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->white;
                                    break;
                            }
                            break;

                            // Background Colors
                        case 40: // BG_BLACK
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->black;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->black; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->darkGrey; break;
                                        default:
                                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->darkGrey;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->black; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->darkGrey; break;
                                        default:
                                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->black;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->black;
                                    break;
                            }
                            break;

                        case 41: // BG_RED
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->red;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->red; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightRed; break;
                                        default:
                                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightRed;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->red; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightRed; break;
                                        default:
                                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->red;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->red;
                                    break;
                            }
                            break;

                        case 42: // BG_GREEN
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->green;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->green; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightGreen; break;
                                        default:
                                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightGreen;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->green; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightGreen; break;
                                        default:
                                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->green;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->green;
                                    break;
                            }
                            break;

                        case 43: // BG_YELLOW
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->brown;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->brown; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->yellow; break;
                                        default:
                                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->yellow;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->brown; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->yellow; break;
                                        default:
                                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->brown;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->brown;
                                    break;
                            }
                            break;

                        case 44: // BG_BLUE
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->blue;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->blue; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightBlue; break;
                                        default:
                                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightBlue;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->blue; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightBlue; break;
                                        default:
                                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->blue;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->blue;
                                    break;
                            }
                            break;

                        case 45: // BG_MAGENTA
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->magenta;
                                    break;

                                case BLINK_ON:
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->magenta; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightMagenta; break;
                                        default:
                                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightMagenta;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->magenta; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightMagenta; break;
                                        default:
                                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->magenta;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->magenta;
                                    break;
                            }
                            break;

                        case 46: // BG_CYAN
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->cyan;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->cyan; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightCyan; break;
                                        default:
                                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightCyan;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->cyan; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightCyan; break;
                                        default:
                                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->cyan;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->cyan;
                                    break;
                            }
                            break;

                        case 47: // BG_WHITE
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->grey;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->grey; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->white; break;
                                        default:
                                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->white;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->grey; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->white; break;
                                        default:
                                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->grey;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->grey;
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
void AnsiParser::sequenceResetAndResponses()
{
    switch(parameters[0])
    {
        case RESET_MODE:
            //screen_buff.esc_sequence += esc_sequence;
            if(parameters[1] == 7)
                line_wrapped = false;
            break;

        case SET_MODE:
            if(parameters.size() > 0 && parameters[1] == 7)
            {
                //line_wrapped = true;
            }
            break;

        case SET_KEYBOARD_STRINGS:
            //outf << "SET_KEYBOARD_STRINGS" << esc_sequence << endl;
            // Ingored!
            break;

            // Respons back to Server with x/y position sequence.
        case ANSI_DETECTION:
            if(parameters.size() > 0 && parameters[1] == 6)
            {
                int current_y_position = 0;
                // Make sure we don't respond with size larger then our
                // Current Screen, Some BBS's use this respose to determine
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
                //screen_buff.esc_sequence += esc_sequence;
            }
            break;

        default:
            // Rest are ignored.
            break;
    } // End of Switch(c) Case Statements
}

/*
 * Handles parsing Control Sequences and formatting the screen buffer
 */
void AnsiParser::sequenceInput(std::vector<int> sequenceParameters)
{
    // Reset Each run.
    position                   = 0;
    characters_per_line        = 80;
    max_x_position             = 0;

    // Grab Parameterized Control Sequence.
    parameters.swap(sequenceParameters);

    // First Handle Any Cursor Movement and Redisplay Attributes
    switch(parameters[0])
    {
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
            break;
        default :
            break;
    }

    // Next Handle Graphic Mode Changes
    // Switch on Sequence Terminator
    switch(parameters[0])
    {
        case SET_GRAPHICS_MODE:
            sequenceGraphicsModeDisplay();
            break;
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
            break;
        default:
            break;
    }
}
