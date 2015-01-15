
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


// Screen Buffer Vector Constructors
AnsiParser::myScreen::myScreen()
{
    characterSequence = "";
}

AnsiParser::myScreen::myScreen(std::string sequence, SDL_Color fg, SDL_Color bg)
: characterSequence(sequence), foreground(fg), background(bg) { }


/**
 * handle screen buffer, Keeps track of all data
 * Poltted through the Ansi Parser so we can pull
 * Text and or redraw screens at anytime.
 */    
void AnsiParser::setScreenBuffer(std::string mySequence)
{
    // Keep track of the lonest line in buffer for Centering screen.
    // NOT IN USE CURRENTLY
    //if (x_position > max_x_position)
    //    max_x_position = x_position;

    sequenceBuffer.characterSequence = mySequence;
    sequenceBuffer.foreground = TheTerminal::Instance()->currentFGColor;
    sequenceBuffer.background = TheTerminal::Instance()->currentBGColor;

    // Setup current position in the screen buffer. 1 based for 0 based.
    position = ((y_position-1) * characters_per_line) + (x_position-1);

    // Add Sequence to Screen Buffer
    screenBuffer.at(position) = sequenceBuffer;

    // Clear for next sequences.
    sequenceBuffer.characterSequence.erase();
}

void AnsiParser::scrollScreenBuffer()
{
    // This remove the top line to scroll the screen up
    // And follow the SDL Surface!  lateron add history for scroll back.
    screenBuffer.erase(
        screenBuffer.begin(), screenBuffer.begin() + characters_per_line);

    // Readd The last Line back to the buffer.
    screenBuffer.resize( TERM_HEIGHT * TERM_WIDTH );
}

void AnsiParser::clearScreenBuffer()
{
    // Allocate the Size
    screenBuffer.clear();
    screenBuffer.resize( TERM_HEIGHT * TERM_WIDTH );
}

// Debug to console.
void AnsiParser::getScreenBufferText()
{
    //std::cout << "* getScreenBufferText" << std::endl; // Start Fresh line.
    for (auto &it : screenBuffer)
    {
        if (it.characterSequence != "")
            std::cout << it.characterSequence << std::flush;
        else
            std::cout << " " << std::flush;
    }
}

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
{
    //Set the default size of the vector screen buffer
    //Thne Fill each element with defaults
    screenBuffer.reserve( TERM_HEIGHT * TERM_WIDTH );
    screenBuffer.resize( TERM_HEIGHT * TERM_WIDTH );
}

AnsiParser::~AnsiParser()
{
    std::cout << "AnsiParser Released" << std::endl;
    std::vector<myScreen>().swap(screenBuffer); // Clear
}

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
    TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->grey;
    TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->black;
}

/*
 * Handles parsing Text and formatting to the screen buffer
 */
void AnsiParser::textInput(std::string buffer)
{
    //std::cout << "TextInput:" << buffer << std::endl;

    // Char Sequence for Parsing.
    unsigned char sequence     = 0;

    // Toggle on the Renderer.
    TheTerminal::Instance()->setRenderReady(true);

    // Loop text and parse screen formatting.
    unsigned char nextSequence;
    for(std::string::size_type i = 0; i < buffer.size(); i++)
    {
        // Grab the Sequence translate to unsigned char.
        sequence = buffer[i];
        nextSequence = '\0';

        // Grab the next character in sequence to test for
        // CRLF combinations.
        if (i+1 < buffer.size())
            nextSequence = buffer[i+1];

        // Back Space
        if(sequence == '\b')
        {
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
            if (x_position == 81)
            {
                ++y_position;
            }

            ++i; // Incriment past \n (2) char combo.
            x_position = 1;
            ++y_position;

            // Add back for ?7h handeling on different TERM WIDTHS
            if(!line_wrapped)
            {
                //screen_buff.color_sequence += "\x1b[40m";
                //screenbuffer('\r');
            }
            // Check here if we need to scroll the screen up 1 row.
            if(y_position > NUM_LINES ||
                    (TheTerminal::Instance()->scrollRegionActive &&
                     y_position > TheTerminal::Instance()->bottomMargin))
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
                    if(!TheTerminal::Instance()->scrollRegionActive)
                        y_position = NUM_LINES;
                    else
                        y_position = TheTerminal::Instance()->bottomMargin;
                    cleared_the_screen = false;
                }
                else
                {
                    TheTerminal::Instance()->renderBottomScreen();   // Surface to Texture of Bottom Row.
                    TheTerminal::Instance()->drawTextureScreen();    // Testure to Screen
                    TheTerminal::Instance()->scrollScreenUp();       // Scroll up for next line.
                    if(!TheTerminal::Instance()->scrollRegionActive)
                        y_position = NUM_LINES;
                    else
                        y_position = TheTerminal::Instance()->bottomMargin;
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
            if (x_position == 81)
            {
                ++y_position;
            }

            x_position = 1;
            // Add back for ?7h handeling on different TERM WIDTHS
            if(!line_wrapped)
            {
                //screen_buff.color_sequence += "\x1b[40m";
                //screenbuffer('\r');
            }
            continue;
        }
        else if(sequence == '\n')
        {
            // std::cout << std::endl << "LF xpos: " << x_position << std::endl;
            // Stupid fix for expected behavior.  If were on col 81
            // And we get a newline, then were suppose to wrap to next line
            // But also move down a second line!
            if (x_position == 81)
            {
                ++y_position;
            }

            //printf("LF");
            // Set position 0, casue next check incriments to 1.
            x_position = 1;
            ++y_position;

            // Add back for ?7h handeling on different TERM WIDTHS
            if(!line_wrapped)
            {
                //screen_buff.color_sequence += "\x1b[40m";
                //screenbuffer('\r');
            }
            // Check here if we need to scroll the screen.
            if(y_position > NUM_LINES ||
                    (TheTerminal::Instance()->scrollRegionActive &&
                     y_position > TheTerminal::Instance()->bottomMargin))
            {
                // If we cleared the screen and hit bottom row, then
                // The very first time we want to spit out the entire screen
                // Since nothing has been drawn yet before we scroll thes screen.
                if(cleared_the_screen)
                {
                    TheTerminal::Instance()->renderScreen();       // Surface to Texture
                    TheTerminal::Instance()->drawTextureScreen();  // Draw Texture to Screen
                    TheTerminal::Instance()->scrollScreenUp();     // Scroll the surface up
                    if(!TheTerminal::Instance()->scrollRegionActive)
                        y_position = NUM_LINES;
                    else
                        y_position = TheTerminal::Instance()->bottomMargin;
                    cleared_the_screen = false;
                }
                else
                {
                    // Else we want to just append to the last line
                    // Move the last line to the Texture, then
                    // Redisplay the scrren.
                    TheTerminal::Instance()->renderBottomScreen();   // Surface to Texture of Bottom Row.
                    TheTerminal::Instance()->drawTextureScreen();    // Testure to Screen
                    TheTerminal::Instance()->scrollScreenUp();       // Scroll up for next line.
                    if(!TheTerminal::Instance()->scrollRegionActive)
                        y_position = NUM_LINES;
                    else
                        y_position = TheTerminal::Instance()->bottomMargin;
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
            if(!TheTerminal::Instance()->scrollRegionActive)
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
        if(y_position > NUM_LINES || (TheTerminal::Instance()->scrollRegionActive &&
                                      y_position >= TheTerminal::Instance()->bottomMargin &&
                                      x_position > characters_per_line))
        {
            // If we cleared the screen and hit bottom row, then
            // The very first time we want to spit out the entire screen
            if(cleared_the_screen)
            {
                // test if scrolling region is active and were drawing in it.
                if(TheTerminal::Instance()->scrollRegionActive &&
                        y_position >= TheTerminal::Instance()->topMargin &&
                        y_position <= TheTerminal::Instance()->bottomMargin)
                {
                    TheTerminal::Instance()->renderScreen();       // Surface to Texture
                    TheTerminal::Instance()->drawTextureScreen();  // Draw Texture to Screen
                    TheTerminal::Instance()->scrollScreenUp();     // Scroll the surface up
                    y_position = TheTerminal::Instance()->bottomMargin;
                    cleared_the_screen = false;
                    // Reset to begining of line.
                    if(x_position > characters_per_line)
                        x_position = 1;
                }
                else if(y_position > NUM_LINES)
                {
                    TheTerminal::Instance()->renderScreen();       // Surface to Texture
                    TheTerminal::Instance()->drawTextureScreen();  // Draw Texture to Screen
                    TheTerminal::Instance()->scrollScreenUp();     // Scroll the surface up
                    y_position = NUM_LINES;
                    cleared_the_screen = false;
                }
            }
            else
            {
                // Else we want to just append to the last line
                // Move the last line to the Texture, then
                // Redisplay the scrren.
                // test if scrolling region is active and were drawing in it.
                if(TheTerminal::Instance()->scrollRegionActive &&
                        y_position >= TheTerminal::Instance()->topMargin &&
                        y_position <= TheTerminal::Instance()->bottomMargin)
                {
                    TheTerminal::Instance()->renderBottomScreen();   // Surface to Texture of Bottom Row.
                    TheTerminal::Instance()->drawTextureScreen();    // Testure to Screen
                    TheTerminal::Instance()->scrollScreenUp();       // Scroll up for next line.
                    // Reset to begining of line.
                    if(x_position > characters_per_line)
                        x_position = 1;
                    y_position = TheTerminal::Instance()->bottomMargin;
                }
                else if(y_position > NUM_LINES)
                {
                    TheTerminal::Instance()->renderBottomScreen();   // Surface to Texture of Bottom Row.
                    TheTerminal::Instance()->drawTextureScreen();    // Testure to Screen
                    TheTerminal::Instance()->scrollScreenUp();       // Scroll up for next line.
                    y_position = NUM_LINES;
                }
            }
        }
        //printf(" \r\n XY: %i,%i ",x_position, y_position);
        // Render Char Screen Position 0 Based from 1 Based.
        TheTerminal::Instance()->drawChar(x_position-1, y_position-1, sequence);        // Char to Surface

        // Add to Screen Buffer, Right now were testing unsigned characters,
        // This will be updated to std::string[0] for ascii characers
        // with extra checking for unicode soon!
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
                TheTerminal::Instance()->setScrollRegion(0, 0);
            }
            if(parameters.size() == 3)
            {
                // Enable scrolling from row1 to row2
                TheTerminal::Instance()->setScrollRegion(parameters[1], parameters[2]);
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
            // Clearn Screen ESC[J  // Lazy Servers use ESC[J!
            if(parameters.size() == 1)
            {
                TheTerminal::Instance()->clearScreenSurface();
                cleared_the_screen = true;
                // Home Cursor, some bbs systems don't do this and makes a mess!!
                x_position = 1;
                y_position = 1;
            }
            // Clearn Screen ESC[2J
            if(parameters.size() == 2 && parameters[1] == 2)
            {
                //printf("\r\n\r\n CLEARTHESCREEN!! \r\n\r\n");
                TheTerminal::Instance()->clearScreenSurface();
                cleared_the_screen = true;
                // Home Cursor, some bbs systems don't do this and makes a mess!!
                x_position = 1;
                y_position = 1;
                //screen_buff.esc_sequence += esc_sequence;
            }
            break;

        case ERASE_TO_EOL:
            //screen_buff.esc_sequence += esc_sequence;
            // Handle Rendering from 1 Based to 0 Based so Subtract 1.
            if(parameters.size() == 1)  // Cursor to End of line
            {
                //printf("\r\n EOL xpos %i, ypos %i \r\n", x_position, y_position);
                TheTerminal::Instance()->renderClearLineScreen(y_position-1,
                        x_position-1, characters_per_line); // test removeed -1
            }
            else if(parameters.size() == 2 && parameters[1] == 1) // Start of Line to Cursor
                TheTerminal::Instance()->renderClearLineScreen(
                    y_position-1, 0, x_position);
            else if(parameters.size() == 2 && parameters[1] == 2) // Entire Line
                TheTerminal::Instance()->renderClearLineScreen(
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
            // NOTES:
            // Inversing flips colors, however light foreground is translated to dark background.
            // Blinking flips colors, however light foreground is translated to light background
            // Have to double check how iCE colors are supposed to be handeled here with blinking.
            
            //esc_sequence.erase(); // Ignore ESC in color, we get that sepeartely.
            current_color.clear();

            if(parameters.size() == 1)    // Change text attributes / All Attributes off
            {
                //current_color = "\x1b[m";
                prev_color_attribute = 0;
                color_attribute = 0;
                TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->grey;
                TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->black;
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
                        case -1: // Implied or Empty Reset = ESC[m
                        case 0:  // All Attributes off
                            //current_color += "0";
                            prev_color_attribute = 0;
                            color_attribute = 0;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            ////printf("\r\nAll Attributes off: %s:%i", esc_sequence.c_str(), color_attribute);
                            // Set default till it's overwridden
                            TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->grey;
                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->black;
                            break;

                        case 1: // BOLD_ON (increase intensity)
                            prev_color_attribute = 1;
                            color_attribute = 1;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            // If current color is dark, flip it to light.
                            if(TheTerminal::Instance()->compareSDL_Colors(TheTerminal::Instance()->currentFGColor,TheTerminal::Instance()->black))
                                TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->darkGrey;
                            else if(TheTerminal::Instance()->compareSDL_Colors(TheTerminal::Instance()->currentFGColor,TheTerminal::Instance()->red))
                                TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->lightRed;
                            else if(TheTerminal::Instance()->compareSDL_Colors(TheTerminal::Instance()->currentFGColor,TheTerminal::Instance()->green))
                                TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->lightGreen;
                            else if(TheTerminal::Instance()->compareSDL_Colors(TheTerminal::Instance()->currentFGColor,TheTerminal::Instance()->brown))
                                TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->yellow;
                            else if(TheTerminal::Instance()->compareSDL_Colors(TheTerminal::Instance()->currentFGColor,TheTerminal::Instance()->blue))
                                TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->lightBlue;
                            else if(TheTerminal::Instance()->compareSDL_Colors(TheTerminal::Instance()->currentFGColor,TheTerminal::Instance()->magenta))
                                TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->lightMagenta;
                            else if(TheTerminal::Instance()->compareSDL_Colors(TheTerminal::Instance()->currentFGColor,TheTerminal::Instance()->cyan))
                                TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->lightCyan;
                            else if(TheTerminal::Instance()->compareSDL_Colors(TheTerminal::Instance()->currentFGColor,TheTerminal::Instance()->grey))
                                TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->white;
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
                            tempColor = TheTerminal::Instance()->currentFGColor;
                            TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->currentBGColor;
                            TheTerminal::Instance()->currentBGColor = tempColor;
                            break;

                        case 6: // BLINK RAPID MS-DOS ANSI.SYS; 150 per minute or more; not widely supported
                            color_attribute = 6;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            break;

                        case 7: // REVERSE_VIDEO_ON
                            color_attribute = 7;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            tempColor = TheTerminal::Instance()->currentFGColor;
                            TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->currentBGColor;
                            TheTerminal::Instance()->currentBGColor = tempColor;
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
                            tempColor = TheTerminal::Instance()->currentFGColor;
                            TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->currentBGColor;
                            TheTerminal::Instance()->currentBGColor = tempColor;
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
                                    TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->black;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->black;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->darkGrey;
                                            break;
                                        default:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->black;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->black;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->darkGrey;
                                            break;
                                        default:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->black;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->darkGrey;
                                    break;
                            }
                            break;

                        case 31: // FG_RED
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->red;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->red;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->lightRed;
                                            break;
                                        default:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->red;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->red;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->lightRed;
                                            break;
                                        default:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->red;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->lightRed;
                                    break;
                            }
                            break;

                        case 32: // FG_GREEN
                            //current_color += "32";
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->green;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->green;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->lightGreen;
                                            break;
                                        default:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->green;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->green;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->lightGreen;
                                            break;
                                        default:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->green;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->lightGreen;
                                    break;
                            }
                            break;

                        case 33: // FG_YELLOW
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->brown;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->brown;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->yellow;
                                            break;
                                        default:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->brown;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->brown;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->yellow;
                                            break;
                                        default:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->brown;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->yellow;
                                    break;
                            }
                            break;

                        case 34: // FG_BLUE
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->blue;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->blue;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->lightBlue;
                                            break;
                                        default:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->blue;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->blue;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->lightBlue;
                                            break;
                                        default:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->blue;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->lightBlue;
                                    break;
                            }
                            break;

                        case 35: // FG_MAGENTA
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->magenta;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->magenta;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->lightMagenta;
                                            break;
                                        default:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->magenta;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->magenta;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->lightMagenta;
                                            break;
                                        default:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->magenta;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->lightMagenta;
                                    break;
                            }
                            break;

                        case 36: // FG_CYAN
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->cyan;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->cyan;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->lightCyan;
                                            break;
                                        default:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->cyan;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->cyan;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->lightCyan;
                                            break;
                                        default:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->cyan;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->lightCyan;
                                    break;
                            }
                            break;

                        case 37: // FG_WHITE
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->grey;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->grey;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->white;
                                            break;
                                        default:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->grey;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->grey;
                                            break;
                                        case 1:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->white;
                                            break;
                                        default:
                                            TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->grey;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->white;
                                    break;
                            }
                            break;

                            // Background Colors
                        case 40: // BG_BLACK
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->black;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->black; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->darkGrey; break;
                                        default:
                                            TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->darkGrey;
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
                                            TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->black;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->black;
                                    break;
                            }
                            break;

                        case 41: // BG_RED
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->red;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->red; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightRed; break;
                                        default:
                                            TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->lightRed;
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
                                            TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->red;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->red;
                                    break;
                            }
                            break;

                        case 42: // BG_GREEN
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->green;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->green; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightGreen; break;
                                        default:
                                            TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->lightGreen;
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
                                            TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->green;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->green;
                                    break;
                            }
                            break;

                        case 43: // BG_YELLOW
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->brown;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->brown; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->yellow; break;
                                        default:
                                            TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->yellow;
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
                                            TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->brown;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->brown;
                                    break;
                            }
                            break;

                        case 44: // BG_BLUE
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->blue;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->blue; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightBlue; break;
                                        default:
                                            TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->lightBlue;
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
                                            TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->blue;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->blue;
                                    break;
                            }
                            break;

                        case 45: // BG_MAGENTA
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->magenta;
                                    break;

                                case BLINK_ON:
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->magenta; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightMagenta; break;
                                        default:
                                            TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->lightMagenta;
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
                                            TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->magenta;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->magenta;
                                    break;
                            }
                            break;

                        case 46: // BG_CYAN
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->cyan;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->cyan; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightCyan; break;
                                        default:
                                            TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->lightCyan;
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
                                            TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->cyan;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->cyan;
                                    break;
                            }
                            break;

                        case 47: // BG_WHITE
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->grey;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->grey; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->white; break;
                                        default:
                                            TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->white;
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
                                            TheTerminal::Instance()->currentFGColor = TheTerminal::Instance()->grey;
                                            break;
                                    }
                                    break;

                                default:
                                    TheTerminal::Instance()->currentBGColor = TheTerminal::Instance()->grey;
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
 * Broken up into (3) Functions to keep it simple and easier to track
 * and maintain. Source Data comes from sequenceParser.cpp
 */
void AnsiParser::sequenceInput(std::vector<int> sequenceParameters)
{
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
            return;
        default:
            break;
    }
}
