//#include "sequenceDecoder.hpp"
//#include "socketHandler.hpp"
//#include "inputHandler.hpp"

#include "sequence_parser.hpp"
#include "renderer.hpp"
#include "screen_buffer.hpp"
#include "tcp_connection.hpp"
#include "session.hpp"

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <sstream>


SequenceParser::SequenceParser(renderer_ptr renderer, connection_ptr connection)
    : m_renderer(renderer)
    , m_connection(connection)
    , m_screen_buffer(
        m_renderer->m_termWidth,
        m_renderer->m_termHeight
    )
    , preceedingSequence('\0')
    , max_x_position(0)
    , saved_cursor_x(1)
    , saved_cursor_y(1)
    , saved_attribute(0)
    , saved_prev_attr(0)
    , color_attribute(0)
    , prev_color_attribute(0)
    , line_wrapped(false)
    , cleared_the_screen(false)
    , isCursorShown(true)
{
    // Set Default Region to Off!
    m_renderer->setScrollRegion(0, 0, renderer->m_termHeight);
}

SequenceParser::~SequenceParser()
{
    std::cout << "SequenceParser Released" << std::endl;
    std::vector<ScreenPixel>().swap(m_screen_buffer.m_screenBuffer); // Clear
}


/**
 * @brief Sending Socket Replies to the Client
 * @param msg
 */
void deliver(std::string msg)
{
    if(msg.size() == 0)
        return;

    // Send Message via Session Instance.
    boost::bind(&Session::deliver, _1, boost::ref(msg));
}


/*
 * Reset AnsiParser Specific Class Attributes
 */
void SequenceParser::reset()
{
    m_screen_buffer.m_x_position = 1;
    m_screen_buffer.m_y_position = 1;
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
    m_renderer->m_currentFGColor = m_renderer->GREY;
    m_renderer->m_currentBGColor = m_renderer->BLACK;

    // Turn off Scrolling Region
    m_renderer->setScrollRegion(0, 0, m_renderer->m_termHeight);
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
            {
                nextSequence = buffer[i+1];
            }
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
            if(m_screen_buffer.m_x_position > 1)
            {
                --m_screen_buffer.m_x_position;
            }
            continue;
        }

        // Handle New Line in ANSI Files properly.
        if(sequence == '\r' && nextSequence == '\n')
        {
            // std::cout << std::endl << "CRLF xpos: " << m_screen_buffer.m_x_position << std::endl;
            // Stupid fix for expected behavior.  If were on col 81
            // And we get a newline, then were suppose to wrap to next line
            // But also move down a second line!
            if(m_screen_buffer.m_x_position == 81)
            {
                ++m_screen_buffer.m_y_position;
            }

            ++i; // Increment past \n (2) char combo.
            m_screen_buffer.m_x_position = 1;
            ++m_screen_buffer.m_y_position;

            // Add back for ?7h handling on different TERM WIDTHS
            if(!line_wrapped)
            {
                //screen_buff.color_sequence += "\x1b[40m";
                //screenbuffer('\r');
            }


            // Check here if we need to scroll the screen up 1 row.
            if(m_screen_buffer.m_y_position > m_renderer->m_termHeight ||
                    (m_renderer->m_scrollRegionActive &&
                     m_screen_buffer.m_y_position > m_renderer->m_bottomMargin))
            {
                // If we cleared the screen and hit bottom row, then
                // The very first time we want to spit out the entire screen
                // Since nothing has been drawn yet before we scroll the screen up
                // Row by Row.
                if(cleared_the_screen)
                {
                    m_renderer->renderScreen();       // Surface to Texture
                    m_renderer->drawTextureScreen();  // Draw Texture to Screen
                    m_renderer->scrollScreenUp();     // Scroll the surface up
                    m_screen_buffer.scrollScreenBuffer();
                    if(!m_renderer->m_scrollRegionActive)
                    {
                        m_screen_buffer.m_y_position = m_renderer->m_termHeight;
                    }
                    else
                    {
                        m_screen_buffer.m_y_position = m_renderer->m_bottomMargin;
                    }
                    cleared_the_screen = false;
                }
                else
                {
                    m_renderer->renderBottomScreen();   // Surface to Texture of Bottom Row.
                    m_renderer->drawTextureScreen();    // Texture to Screen
                    m_renderer->scrollScreenUp();       // Scroll up for next line.
                    m_screen_buffer.scrollScreenBuffer();
                    if(!m_renderer->m_scrollRegionActive)
                    {
                        m_screen_buffer.m_y_position = m_renderer->m_termHeight;
                    }
                    else
                    {
                        m_screen_buffer.m_y_position = m_renderer->m_bottomMargin;
                    }
                }
            }
            //printf("\r\n xpos %i, ypos %i \r\n",m_screen_buffer.m_x_position, m_screen_buffer.m_y_position);
            continue;
        }
        else if(sequence == '\r')
        {
            // std::cout << std::endl << "CR xpos: " << m_screen_buffer.m_x_position << std::endl;
            // Stupid fix for expected behavior.  If were on col 81
            // And we get a newline, then were suppose to wrap to next line
            // But also move down a second line!
            if(m_screen_buffer.m_x_position == 81)
            {
                ++m_screen_buffer.m_y_position;
            }

            m_screen_buffer.m_x_position = 1;
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
            if(m_screen_buffer.m_x_position == 81)
            {
                ++m_screen_buffer.m_y_position;
            }

            // Reset to Begining of next Line.
            m_screen_buffer.m_x_position = 1;
            ++m_screen_buffer.m_y_position;

            // Add back for ?7h handling on different TERM WIDTHS
            if(!line_wrapped)
            {
                //screen_buff.color_sequence += "\x1b[40m";
                //screenbuffer('\r');
            }
            // Check here if we need to scroll the screen.
            if(m_screen_buffer.m_y_position > m_renderer->m_termHeight ||
                    (m_renderer->m_scrollRegionActive &&
                     m_screen_buffer.m_y_position > m_renderer->m_bottomMargin))
            {
                // If we cleared the screen and hit bottom row, then
                // The very first time we want to spit out the entire screen
                // Since nothing has been drawn yet before we scroll the screen.
                if(cleared_the_screen)
                {
                    m_renderer->renderScreen();       // Surface to Texture
                    m_renderer->drawTextureScreen();  // Draw Texture to Screen
                    m_renderer->scrollScreenUp();     // Scroll the surface up
                    m_screen_buffer.scrollScreenBuffer();
                    if(!m_renderer->m_scrollRegionActive)
                        m_screen_buffer.m_y_position = m_renderer->m_termHeight;
                    else
                        m_screen_buffer.m_y_position = m_renderer->m_bottomMargin;
                    cleared_the_screen = false;
                }
                else
                {
                    // Else we want to just append to the last line
                    // Move the last line to the Texture, then
                    // Re-display the screen.
                    m_renderer->renderBottomScreen();   // Surface to Texture of Bottom Row.
                    m_renderer->drawTextureScreen();    // Texture to Screen
                    m_renderer->scrollScreenUp();       // Scroll up for next line.
                    m_screen_buffer.scrollScreenBuffer();
                    if(!m_renderer->m_scrollRegionActive)
                        m_screen_buffer.m_y_position = m_renderer->m_termHeight;
                    else
                        m_screen_buffer.m_y_position = m_renderer->m_bottomMargin;
                }
            }
            continue;
        }

        // Check for X_Position past columns in current line
        if(m_screen_buffer.m_x_position > m_renderer->m_termWidth)
        {
            // Move to next line
            // If were in scroll region,
            // Then we'll test specifically to scroll the region only.
            if(!m_renderer->m_scrollRegionActive)
            {
                m_screen_buffer.m_x_position = 1;
                ++m_screen_buffer.m_y_position;
            }
        }
        // A little more tricky now if were pushing text pass the bottom margin
        //from the scroll region or just x/y coordinates text like status bar
        //outside scroll region we don't want to scroll in this case.

        // Check if m_screen_buffer.m_y_position greater then screen size
        // if we need to scroll the screen up 1 line.
        // This last check is for normal text being pushed to the screen,
        // NO CR/LR so check if we gone past the bottom margin of the screen.
        if(m_screen_buffer.m_y_position > m_renderer->m_termHeight || (m_renderer->m_scrollRegionActive &&
                                      m_screen_buffer.m_y_position >= m_renderer->m_bottomMargin &&
                                      m_screen_buffer.m_x_position > m_renderer->m_termWidth))
        {
            // If we cleared the screen and hit bottom row, then
            // The very first time we want to spit out the entire screen
            if(cleared_the_screen)
            {
                // test if scrolling region is active and were drawing in it.
                if(m_renderer->m_scrollRegionActive &&
                        m_screen_buffer.m_y_position >= m_renderer->m_topMargin &&
                        m_screen_buffer.m_y_position <= m_renderer->m_bottomMargin)
                {
                    m_renderer->renderScreen();       // Surface to Texture
                    m_renderer->drawTextureScreen();  // Draw Texture to Screen
                    m_renderer->scrollScreenUp();     // Scroll the surface up
                    m_screen_buffer.scrollScreenBuffer();
                    m_screen_buffer.m_y_position = m_renderer->m_bottomMargin;
                    cleared_the_screen = false;
                    // Reset to beginning of line.
                    if(m_screen_buffer.m_x_position > m_renderer->m_termWidth)
                        m_screen_buffer.m_x_position = 1;
                }
                else if(m_screen_buffer.m_y_position > m_renderer->m_termHeight)
                {
                    m_renderer->renderScreen();       // Surface to Texture
                    m_renderer->drawTextureScreen();  // Draw Texture to Screen
                    m_renderer->scrollScreenUp();     // Scroll the surface up
                    m_screen_buffer.scrollScreenBuffer();
                    m_screen_buffer.m_y_position = m_renderer->m_termHeight;
                    cleared_the_screen = false;
                }
            }
            else
            {
                // Else we want to just append to the last line
                // Move the last line to the Texture, then
                // Re display the screen.
                // test if scrolling region is active and were drawing in it.
                if(m_renderer->m_scrollRegionActive &&
                        m_screen_buffer.m_y_position >= m_renderer->m_topMargin &&
                        m_screen_buffer.m_y_position <= m_renderer->m_bottomMargin)
                {
                    m_renderer->renderBottomScreen();   // Surface to Texture of Bottom Row.
                    m_renderer->drawTextureScreen();    // Texture to Screen
                    m_renderer->scrollScreenUp();       // Scroll up for next line.
                    m_screen_buffer.scrollScreenBuffer();
                    // Reset to beginning of line.
                    if(m_screen_buffer.m_x_position > m_renderer->m_termWidth)
                        m_screen_buffer.m_x_position = 1;
                    m_screen_buffer.m_y_position = m_renderer->m_bottomMargin;
                }
                else if(m_screen_buffer.m_y_position > m_renderer->m_termHeight)
                {
                    m_renderer->renderBottomScreen();   // Surface to Texture of Bottom Row.
                    m_renderer->drawTextureScreen();    // Texture to Screen
                    m_renderer->scrollScreenUp();       // Scroll up for next line.
                    m_screen_buffer.scrollScreenBuffer();
                    m_screen_buffer.m_y_position = m_renderer->m_termHeight;
                }
            }
        }
        //printf(" \r\n XY: %i,%i ",m_screen_buffer.m_x_position, m_screen_buffer.m_y_position);
        // Render Char Screen Position 0 Based from 1 Based.
        m_renderer->drawChar(
            m_screen_buffer.m_x_position-1,
            m_screen_buffer.m_y_position-1,
            sequence
        );

        // Add to Screen Buffer, Right now were testing unsigned characters,
        // This will be updated to std::string[0] for ASCII characters
        // with extra checking for Unicode soon!
        // tempSequence = (signed)sequence;
        // setScreenBuffer(tempSequence);
        // tempSequence.erase();
        m_screen_buffer.setScreenBuffer(
            sequence,
            m_renderer->m_currentFGColor,
            m_renderer->m_currentBGColor
        );

        // After drawing character send Cursor to Next Position forward.
        ++m_screen_buffer.m_x_position;
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
                m_renderer->renderDeleteCharScreen(
                    m_screen_buffer.m_x_position -1,
                    m_screen_buffer.m_y_position -1,
                    1
                );
                break;
            }
            if(parameters.size() == 2)  // Erase current position, move left x
            {
                m_renderer->renderDeleteCharScreen(
                    m_screen_buffer.m_x_position -1,
                    m_screen_buffer.m_y_position -1,
                    parameters[1]
                );
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
                m_screen_buffer.m_x_position = 1;
                m_screen_buffer.m_y_position = 1;
                break;
            }
            if(parameters.size() == 2)
            {
                m_screen_buffer.m_x_position = 1;
                m_screen_buffer.m_y_position = parameters[1];
                break;
            }
            break;

        case CURSOR_POSITION:
        case CURSOR_POSITION_ALT:
            //printf("\r\n CURSOR_POSITION: p = %i,  param[0] = %i param[1] = %i \r\n", p,  param[0], param[1]);
            if(parameters.size() == 1)
            {
                m_screen_buffer.m_x_position = 1;
                m_screen_buffer.m_y_position = 1;
            }
            else if(parameters.size() == 2)
            {
                m_screen_buffer.m_y_position = parameters[1];
                m_screen_buffer.m_x_position = 1;
                if(m_screen_buffer.m_y_position > m_renderer->m_termHeight)
                {
                    m_screen_buffer.m_y_position = m_renderer->m_termHeight;
                }
                else if(m_screen_buffer.m_y_position < 1)
                {
                    m_screen_buffer.m_y_position = 1;
                }
            }
            // first_param_implied ESC[;1h no digit on left of ; separator.
            // Empty is sent as -1
            else if(parameters.size() == 3 && parameters[1] == -1)
            {
                m_screen_buffer.m_x_position = parameters[2];
                if(m_screen_buffer.m_x_position > m_renderer->m_termWidth)
                {
                    m_screen_buffer.m_x_position = m_renderer->m_termWidth;
                }
            }
            else
            {
                m_screen_buffer.m_y_position = parameters[1];
                m_screen_buffer.m_x_position = parameters[2];

                if(m_screen_buffer.m_y_position > m_renderer->m_termHeight)
                {
                    m_screen_buffer.m_y_position = m_renderer->m_termHeight;
                }
                else if(m_screen_buffer.m_y_position < 1)
                {
                    m_screen_buffer.m_y_position = 1;
                }

                if(m_screen_buffer.m_x_position > m_renderer->m_termWidth)
                {
                    m_screen_buffer.m_x_position = m_renderer->m_termWidth;
                }
                else if(m_screen_buffer.m_x_position < 1)
                {
                    m_screen_buffer.m_x_position = 1;
                }
            }
            break;

        case SCROLL_REGION:
            if(parameters.size() == 1)
            {
                // Reset to Full Screen.
                m_renderer->setScrollRegion(0, 0, m_renderer->m_termHeight);
            }
            if(parameters.size() == 3)
            {
                // Enable scrolling from row1 to row2
                // Move the cursor to the home position
                m_renderer->setScrollRegion(parameters[1], parameters[2], m_renderer->m_termHeight);
                m_screen_buffer.m_x_position = 1;
                m_screen_buffer.m_y_position = 1;
            }
            break;

        case CURSOR_PREV_LIVE:
        case CURSOR_UP:
            if(parameters.size() == 1)
            {
                if(m_screen_buffer.m_y_position > 1)
                {
                    --m_screen_buffer.m_y_position;
                }
            }
            else
            {
                m_screen_buffer.m_y_position -= parameters[1];
                if(m_screen_buffer.m_y_position < 1)
                {
                    m_screen_buffer.m_y_position = 1;
                }
            }
            break;

        case CURSOR_NEXT_LINE:
        case CURSOR_DOWN:
            if(parameters.size() == 1)
            {
                if(m_screen_buffer.m_y_position < m_renderer->m_termHeight)
                {
                    ++m_screen_buffer.m_y_position;
                }
            }
            else
            {
                m_screen_buffer.m_y_position += parameters[1];
                if(m_screen_buffer.m_y_position > m_renderer->m_termHeight)
                {
                    m_screen_buffer.m_y_position = m_renderer->m_termHeight;
                }
            }
            break;

        case CURSOR_FORWARD:
            if(parameters.size() == 1)
            {
                //if (m_screen_buffer.m_x_position < characters_per_line)
                ++m_screen_buffer.m_x_position;
                if(m_screen_buffer.m_x_position > m_renderer->m_termWidth)
                {
                    m_screen_buffer.m_x_position -= m_renderer->m_termWidth;
                    ++m_screen_buffer.m_y_position;
                }
            }
            else
            {
                m_screen_buffer.m_x_position += parameters[1];
                // Some programs push ESC[C past the end
                // of the line and expect us to loop and move
                // Down Automatically.
                if(m_screen_buffer.m_x_position > m_renderer->m_termWidth)
                {
                    m_screen_buffer.m_x_position -= m_renderer->m_termWidth;
                    ++m_screen_buffer.m_y_position;
                }
            }
            break;

        case CURSOR_BACKWARD:
            if(parameters.size() == 1)
            {
                if(m_screen_buffer.m_x_position > 1)
                {
                    --m_screen_buffer.m_x_position;
                }
            }
            else if(parameters.size() == 2)
            {
                m_screen_buffer.m_x_position -= parameters[1];
                if(m_screen_buffer.m_x_position < 1)
                {
                    m_screen_buffer.m_x_position = 1;
                }
            }
            break;

        case SAVE_CURSOR_POS:
            saved_cursor_x  = m_screen_buffer.m_x_position;
            saved_cursor_y  = m_screen_buffer.m_y_position;
            saved_attribute = color_attribute;
            saved_prev_attr = prev_color_attribute;
            savedForegroundColor = m_renderer->m_currentFGColor;
            savedBackgroundColor = m_renderer->m_currentBGColor;
            break;

        case RESTORE_CURSOR_POS:
            m_screen_buffer.m_x_position = saved_cursor_x;
            m_screen_buffer.m_y_position = saved_cursor_y;
            color_attribute = saved_attribute;
            prev_color_attribute = saved_prev_attr;
            m_renderer->m_currentFGColor = savedForegroundColor;
            m_renderer->m_currentBGColor = savedBackgroundColor;
            break;

        case CURSOR_X_POSITION: // XTERM
            if(parameters.size() == 1)
                m_screen_buffer.m_x_position = 1;
            else
            {
                m_screen_buffer.m_x_position = parameters[1];
                if(m_screen_buffer.m_x_position < 1)
                {
                    m_screen_buffer.m_x_position = 1;
                }
                else if(m_screen_buffer.m_x_position > m_renderer->m_termWidth)
                {
                    m_screen_buffer.m_x_position = m_renderer->m_termWidth;
                }
            }
            break;

        case ERASE_DISPLAY:
            // Erase Current Line and Below. ESC[J
            if(parameters.size() == 1)
            {
                m_renderer->renderClearLineBelowScreen(
                    m_screen_buffer.m_y_position-1,
                    m_screen_buffer.m_x_position-1
                );
                break;
            }
            // Erase Current Line and Below. ESC[0J
            if(parameters.size() == 2 && parameters[1] == 0)
            {
                m_renderer->renderClearLineBelowScreen(
                    m_screen_buffer.m_y_position-1,
                    m_screen_buffer.m_x_position-1
                );
                break;
            }
            // Erase Current Line and Above.
            if(parameters.size() == 2 && parameters[1] == 1)
            {
                m_renderer->renderClearLineAboveScreen(
                    m_screen_buffer.m_y_position-1,
                    m_screen_buffer.m_x_position-1
                );
                break;
            }
            // Clear Entire Screen. ESC[2J
            if(parameters.size() == 2 && parameters[1] == 2)
            {
                // If background not 0m, we should probably fill
                // The screen with background color per the spec?
                m_renderer->clearScreenSurface();
                m_screen_buffer.clearScreenBuffer();
                cleared_the_screen = true;
                m_screen_buffer.m_x_position = 1;
                m_screen_buffer.m_y_position = 1;
                break;
            }
            break;

        case ERASE_TO_EOL:
            // Handle Rendering from 1 Based to 0 Based so Subtract 1.
            if(parameters.size() == 1)  // Cursor to End of line
            {
                //printf("\r\n EOL xpos %i, ypos %i \r\n",
                //    m_screen_buffer.m_x_position,
                //    m_screen_buffer.m_y_position
                //);

                m_renderer->renderClearLineScreen(
                    m_screen_buffer.m_y_position-1,
                    m_screen_buffer.m_x_position-1,
                    m_renderer->m_termWidth
                );

                m_screen_buffer.clearScreenBufferRange(
                    m_screen_buffer.m_x_position-1,
                    m_renderer->m_termWidth
                );
            }
            else if(parameters.size() == 2 && parameters[1] == 1) // Start of Line to Cursor
            {
                m_renderer->renderClearLineScreen(
                    m_screen_buffer.m_y_position-1,
                    0,
                    m_screen_buffer.m_x_position
                );

                m_screen_buffer.clearScreenBufferRange(0, m_screen_buffer.m_x_position);
            }
            else if(parameters.size() == 2 && parameters[1] == 2) // Entire Line
            {
                m_renderer->renderClearLineScreen(
                    m_screen_buffer.m_y_position-1,
                    0,
                    m_renderer->m_termWidth
                );

                m_screen_buffer.clearScreenBufferRange(0, m_renderer->m_termWidth);
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
                m_renderer->m_currentFGColor = m_renderer->GREY;
                m_renderer->m_currentBGColor = m_renderer->BLACK;
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
                            m_renderer->m_currentFGColor = m_renderer->GREY;
                            m_renderer->m_currentBGColor = m_renderer->BLACK;
                            break;

                        case 1: // BOLD_ON (increase intensity)
                            prev_color_attribute = 1;
                            color_attribute = 1;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            // If current color is dark, flip it to light.
                            if(m_renderer->compareSDL_Colors(m_renderer->m_currentFGColor, m_renderer->BLACK))
                                m_renderer->m_currentFGColor = m_renderer->DARK_GREY;
                            else if(m_renderer->compareSDL_Colors(m_renderer->m_currentFGColor, m_renderer->RED))
                                m_renderer->m_currentFGColor = m_renderer->LIGHT_RED;
                            else if(m_renderer->compareSDL_Colors(m_renderer->m_currentFGColor, m_renderer->GREEN))
                                m_renderer->m_currentFGColor = m_renderer->LIGHT_GREEN;
                            else if(m_renderer->compareSDL_Colors(m_renderer->m_currentFGColor, m_renderer->BROWN))
                                m_renderer->m_currentFGColor = m_renderer->YELLOW;
                            else if(m_renderer->compareSDL_Colors(m_renderer->m_currentFGColor, m_renderer->BLUE))
                                m_renderer->m_currentFGColor = m_renderer->LIGHT_BLUE;
                            else if(m_renderer->compareSDL_Colors(m_renderer->m_currentFGColor, m_renderer->MAGENTA))
                                m_renderer->m_currentFGColor = m_renderer->LIGHT_MAGENTA;
                            else if(m_renderer->compareSDL_Colors(m_renderer->m_currentFGColor, m_renderer->CYAN))
                                m_renderer->m_currentFGColor = m_renderer->LIGHT_CYAN;
                            else if(m_renderer->compareSDL_Colors(m_renderer->m_currentFGColor, m_renderer->GREY))
                                m_renderer->m_currentFGColor = m_renderer->WHITE;
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
                            tempColor = m_renderer->m_currentFGColor;
                            m_renderer->m_currentFGColor = m_renderer->m_currentBGColor;
                            m_renderer->m_currentBGColor = tempColor;
                            break;

                        case 6: // BLINK RAPID MS-DOS ANSI.SYS; 150 per minute or more; not widely supported
                            color_attribute = 6;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            break;

                        case 7: // REVERSE_VIDEO_ON
                            color_attribute = 7;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            tempColor = m_renderer->m_currentFGColor;
                            m_renderer->m_currentFGColor = m_renderer->m_currentBGColor;
                            m_renderer->m_currentBGColor = tempColor;
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
                            tempColor = m_renderer->m_currentFGColor;
                            m_renderer->m_currentFGColor = m_renderer->m_currentBGColor;
                            m_renderer->m_currentBGColor = tempColor;
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
                                    m_renderer->m_currentFGColor = m_renderer->BLACK;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            m_renderer->m_currentBGColor = m_renderer->BLACK;
                                            break;
                                        case 1:
                                            m_renderer->m_currentBGColor = m_renderer->DARK_GREY;
                                            break;
                                        default:
                                            m_renderer->m_currentBGColor = m_renderer->BLACK;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            m_renderer->m_currentBGColor = m_renderer->BLACK;
                                            break;
                                        case 1:
                                            m_renderer->m_currentBGColor = m_renderer->DARK_GREY;
                                            break;
                                        default:
                                            m_renderer->m_currentBGColor = m_renderer->BLACK;
                                            break;
                                    }
                                    break;

                                default:
                                    m_renderer->m_currentFGColor = m_renderer->DARK_GREY;
                                    break;
                            }
                            break;

                        case 31: // FG_RED
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    m_renderer->m_currentFGColor = m_renderer->RED;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            m_renderer->m_currentBGColor = m_renderer->RED;
                                            break;
                                        case 1:
                                            m_renderer->m_currentBGColor = m_renderer->LIGHT_RED;
                                            break;
                                        default:
                                            m_renderer->m_currentBGColor = m_renderer->RED;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            m_renderer->m_currentBGColor = m_renderer->RED;
                                            break;
                                        case 1:
                                            m_renderer->m_currentBGColor = m_renderer->LIGHT_RED;
                                            break;
                                        default:
                                            m_renderer->m_currentBGColor = m_renderer->RED;
                                            break;
                                    }
                                    break;

                                default:
                                    m_renderer->m_currentFGColor = m_renderer->LIGHT_RED;
                                    break;
                            }
                            break;

                        case 32: // FG_GREEN
                            //current_color += "32";
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    m_renderer->m_currentFGColor = m_renderer->GREEN;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            m_renderer->m_currentBGColor = m_renderer->GREEN;
                                            break;
                                        case 1:
                                            m_renderer->m_currentBGColor = m_renderer->LIGHT_GREEN;
                                            break;
                                        default:
                                            m_renderer->m_currentBGColor = m_renderer->GREEN;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            m_renderer->m_currentBGColor = m_renderer->GREEN;
                                            break;
                                        case 1:
                                            m_renderer->m_currentBGColor = m_renderer->LIGHT_GREEN;
                                            break;
                                        default:
                                            m_renderer->m_currentBGColor = m_renderer->GREEN;
                                            break;
                                    }
                                    break;

                                default:
                                    m_renderer->m_currentFGColor = m_renderer->LIGHT_GREEN;
                                    break;
                            }
                            break;

                        case 33: // FG_YELLOW
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    m_renderer->m_currentFGColor = m_renderer->BROWN;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            m_renderer->m_currentBGColor = m_renderer->BROWN;
                                            break;
                                        case 1:
                                            m_renderer->m_currentBGColor = m_renderer->YELLOW;
                                            break;
                                        default:
                                            m_renderer->m_currentBGColor = m_renderer->BROWN;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            m_renderer->m_currentBGColor = m_renderer->BROWN;
                                            break;
                                        case 1:
                                            m_renderer->m_currentBGColor = m_renderer->YELLOW;
                                            break;
                                        default:
                                            m_renderer->m_currentBGColor = m_renderer->BROWN;
                                            break;
                                    }
                                    break;

                                default:
                                    m_renderer->m_currentFGColor = m_renderer->YELLOW;
                                    break;
                            }
                            break;

                        case 34: // FG_BLUE
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    m_renderer->m_currentFGColor = m_renderer->BLUE;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            m_renderer->m_currentBGColor = m_renderer->BLUE;
                                            break;
                                        case 1:
                                            m_renderer->m_currentBGColor = m_renderer->LIGHT_BLUE;
                                            break;
                                        default:
                                            m_renderer->m_currentBGColor = m_renderer->BLUE;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            m_renderer->m_currentBGColor = m_renderer->BLUE;
                                            break;
                                        case 1:
                                            m_renderer->m_currentBGColor = m_renderer->LIGHT_BLUE;
                                            break;
                                        default:
                                            m_renderer->m_currentBGColor = m_renderer->BLUE;
                                            break;
                                    }
                                    break;

                                default:
                                    m_renderer->m_currentFGColor = m_renderer->LIGHT_BLUE;
                                    break;
                            }
                            break;

                        case 35: // FG_MAGENTA
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    m_renderer->m_currentFGColor = m_renderer->MAGENTA;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            m_renderer->m_currentBGColor = m_renderer->MAGENTA;
                                            break;
                                        case 1:
                                            m_renderer->m_currentBGColor = m_renderer->LIGHT_MAGENTA;
                                            break;
                                        default:
                                            m_renderer->m_currentBGColor = m_renderer->MAGENTA;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            m_renderer->m_currentBGColor = m_renderer->MAGENTA;
                                            break;
                                        case 1:
                                            m_renderer->m_currentBGColor = m_renderer->LIGHT_MAGENTA;
                                            break;
                                        default:
                                            m_renderer->m_currentBGColor = m_renderer->MAGENTA;
                                            break;
                                    }
                                    break;

                                default:
                                    m_renderer->m_currentFGColor = m_renderer->LIGHT_MAGENTA;
                                    break;
                            }
                            break;

                        case 36: // FG_CYAN
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    m_renderer->m_currentFGColor = m_renderer->CYAN;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            m_renderer->m_currentBGColor = m_renderer->CYAN;
                                            break;
                                        case 1:
                                            m_renderer->m_currentBGColor = m_renderer->LIGHT_CYAN;
                                            break;
                                        default:
                                            m_renderer->m_currentBGColor = m_renderer->CYAN;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            m_renderer->m_currentBGColor = m_renderer->CYAN;
                                            break;
                                        case 1:
                                            m_renderer->m_currentBGColor = m_renderer->LIGHT_CYAN;
                                            break;
                                        default:
                                            m_renderer->m_currentBGColor = m_renderer->CYAN;
                                            break;
                                    }
                                    break;

                                default:
                                    m_renderer->m_currentFGColor = m_renderer->LIGHT_CYAN;
                                    break;
                            }
                            break;

                        case 37: // FG_WHITE
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    m_renderer->m_currentFGColor = m_renderer->GREY;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            m_renderer->m_currentBGColor = m_renderer->GREY;
                                            break;
                                        case 1:
                                            m_renderer->m_currentBGColor = m_renderer->WHITE;
                                            break;
                                        default:
                                            m_renderer->m_currentBGColor = m_renderer->GREY;
                                            break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:
                                            m_renderer->m_currentBGColor = m_renderer->GREY;
                                            break;
                                        case 1:
                                            m_renderer->m_currentBGColor = m_renderer->WHITE;
                                            break;
                                        default:
                                            m_renderer->m_currentBGColor = m_renderer->GREY;
                                            break;
                                    }
                                    break;

                                default:
                                    m_renderer->m_currentFGColor = m_renderer->WHITE;
                                    break;
                            }
                            break;

                            // Background Colors
                        case 40: // BG_BLACK
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    m_renderer->m_currentBGColor = m_renderer->BLACK;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->BLACK; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->DARKGREY; break;
                                        default:
                                            m_renderer->m_currentFGColor = m_renderer->DARK_GREY;
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
                                            m_renderer->m_currentFGColor = m_renderer->BLACK;
                                            break;
                                    }
                                    break;

                                default:
                                    m_renderer->m_currentBGColor = m_renderer->BLACK;
                                    break;
                            }
                            break;

                        case 41: // BG_RED
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    m_renderer->m_currentBGColor = m_renderer->RED;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->RED; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->LIGHTRED; break;
                                        default:
                                            m_renderer->m_currentFGColor = m_renderer->LIGHT_RED;
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
                                            m_renderer->m_currentFGColor = m_renderer->RED;
                                            break;
                                    }
                                    break;

                                default:
                                    m_renderer->m_currentBGColor = m_renderer->RED;
                                    break;
                            }
                            break;

                        case 42: // BG_GREEN
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    m_renderer->m_currentBGColor = m_renderer->GREEN;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->GREEN; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->LIGHTGREEN; break;
                                        default:
                                            m_renderer->m_currentFGColor = m_renderer->LIGHT_GREEN;
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
                                            m_renderer->m_currentFGColor = m_renderer->GREEN;
                                            break;
                                    }
                                    break;

                                default:
                                    m_renderer->m_currentBGColor = m_renderer->GREEN;
                                    break;
                            }
                            break;

                        case 43: // BG_YELLOW
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    m_renderer->m_currentBGColor = m_renderer->BROWN;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->BROWN; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->YELLOW; break;
                                        default:
                                            m_renderer->m_currentFGColor = m_renderer->YELLOW;
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
                                            m_renderer->m_currentFGColor = m_renderer->BROWN;
                                            break;
                                    }
                                    break;

                                default:
                                    m_renderer->m_currentBGColor = m_renderer->BROWN;
                                    break;
                            }
                            break;

                        case 44: // BG_BLUE
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    m_renderer->m_currentBGColor = m_renderer->BLUE;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->BLUE; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->LIGHTBLUE; break;
                                        default:
                                            m_renderer->m_currentFGColor = m_renderer->LIGHT_BLUE;
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
                                            m_renderer->m_currentFGColor = m_renderer->BLUE;
                                            break;
                                    }
                                    break;

                                default:
                                    m_renderer->m_currentBGColor = m_renderer->BLUE;
                                    break;
                            }
                            break;

                        case 45: // BG_MAGENTA
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    m_renderer->m_currentBGColor = m_renderer->MAGENTA;
                                    break;

                                case BLINK_ON:
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->MAGENTA; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->LIGHTMAGENTA; break;
                                        default:
                                            m_renderer->m_currentFGColor = m_renderer->LIGHT_MAGENTA;
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
                                            m_renderer->m_currentFGColor = m_renderer->MAGENTA;
                                            break;
                                    }
                                    break;

                                default:
                                    m_renderer->m_currentBGColor = m_renderer->MAGENTA;
                                    break;
                            }
                            break;

                        case 46: // BG_CYAN
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    m_renderer->m_currentBGColor = m_renderer->CYAN;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->CYAN; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->LIGHTCYAN; break;
                                        default:
                                            m_renderer->m_currentFGColor = m_renderer->LIGHT_CYAN;
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
                                            m_renderer->m_currentFGColor = m_renderer->CYAN;
                                            break;
                                    }
                                    break;

                                default:
                                    m_renderer->m_currentBGColor = m_renderer->CYAN;
                                    break;
                            }
                            break;

                        case 47: // BG_WHITE
                            switch(color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    m_renderer->m_currentBGColor = m_renderer->GREY;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                            //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->GREY; break;
                                            //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->WHITE; break;
                                        default:
                                            m_renderer->m_currentFGColor = m_renderer->WHITE;
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
                                            m_renderer->m_currentFGColor = m_renderer->GREY;
                                            break;
                                    }
                                    break;

                                default:
                                    m_renderer->m_currentBGColor = m_renderer->GREY;
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
                if(m_screen_buffer.m_y_position > m_renderer->m_termHeight)
                {
                    current_y_position = m_renderer->m_termHeight;
                }
                else
                {
                    current_y_position = m_screen_buffer.m_y_position;
                }

                std::string currentPosition;    // for ESC[6n reply
                std::stringstream streamBuffer;
                streamBuffer << "\x1b[" << current_y_position << ";" << m_screen_buffer.m_x_position << "R";
                currentPosition = streamBuffer.str();

                // Use the Socket Conenction to Reply the current x/y position
                if (m_connection->is_open())
                {
                    // Make Async Write to the Socket using Session Callback.
                    deliver(currentPosition);
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
        //parameters.swap(sequenceParameters);
        parameters = std::move(sequenceParameters);
    }
    catch(std::exception &e)
    {
        std::cout << "Exception sequenceInput parameters.swap(sequenceParameters): "
                  << e.what() << std::endl;
    }


    // Handle Character set conversions
    switch(parameters[0])
    {
            // Catch Character Set Switching that is not supported
            // just yet, pass through these sequences.
        case '%':
        case '(':
        case ')':
            std::cout << "CharSet Change: "
                      << parameters[0] << std::endl;
            return;
        default:
            break;
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
