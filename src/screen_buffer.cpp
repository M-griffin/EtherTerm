#include "screen_buffer.hpp"
#include "renderer.hpp"

#include <iostream>

ScreenBuffer::ScreenBuffer(int term_width, int term_height)
    : m_position(0)
    , m_x_position(1)
    , m_y_position(1)
    , m_characters_per_line(80)
    , m_term_height(term_height)
    , m_term_width(term_width)
{
    // Reserve Pixel Screen Space for each visable character.
    m_screenBuffer.reserve(m_term_height * m_term_width);
    m_screenBuffer.resize(m_term_height * m_term_width);
}

ScreenBuffer::~ScreenBuffer()
{
    // Clear
    std::vector<ScreenPixel>().swap(m_screenBuffer);
    std::cout << "ScreenBuffer Released" << std::endl;
}


/**
 * @brief handle screen buffer, Keeps track of all screen data
 * @param mySequence
 * @param fg_color
 * @param bg_color
 */
void ScreenBuffer::setScreenBuffer(unsigned char mySequence,
                                   SDL_Color fg_color,
                                   SDL_Color bg_color)
{
    // Keep track of the longest line in buffer for Centering screen.
    // NOT IN USE CURRENTLY
    //if (x_position > max_x_position)
    //    max_x_position = x_position;

    std::string sequence;
    sequence = (signed)mySequence;
    m_sequenceBuffer.m_characterSequence = sequence;
    sequence.erase();

    m_sequenceBuffer.m_foreground = fg_color;
    m_sequenceBuffer.m_background = bg_color;

    // Setup current position in the screen buffer. 1 based for 0 based.
    m_position = ((m_y_position-1) * m_characters_per_line) + (m_x_position-1);

    // Add Sequence to Screen Buffer
    try
    {
        if((unsigned)m_position < m_screenBuffer.size())
            m_screenBuffer.at(m_position) = m_sequenceBuffer;
        else
        {
            std::cout << "Xposition: " << m_x_position-1 << std::endl;
        }
    }
    catch(std::exception &e)
    {
        std::cout << "Exception setScreenBuffer: " << e.what() << std::endl;
        std::cout << "Server sent data that exceeds screen dimensions." << std::endl;
    }
    // Clear for next sequences.
    m_sequenceBuffer.m_characterSequence.erase();
}

/**
 * @brief Scrolls the Screen Buffer Up a line to match the internal SDL_Surface
 */
void ScreenBuffer::scrollScreenBuffer()
{
    //*** IMPORTANT (WIP), must add check for region scrolling only!
    //TheTerminal::Instance()->scrollRegionActive &&
    //                 y_position > TheTerminal::Instance()->bottomMargin))

    // Theory, Erase Line at Top margin, then add a new line bottom margin
    // To move it back down.  That way only the middle is scrolled up.

    // This remove the top line to scroll the screen up
    // And follow the SDL Surface!  later on add history for scroll back.
    try
    {
        m_screenBuffer.erase(
            m_screenBuffer.begin(), m_screenBuffer.begin() + m_characters_per_line
        );
    }
    catch(std::exception &e)
    {
        std::cout << "Exception scrollScreenBuffer: " << e.what() << std::endl;
    }
    // Readd The last Line back to the buffer.
    m_screenBuffer.resize(m_term_height * m_term_width);
}

/**
 * @brief Clear Range of Screen Buffer for Erase Sequences.
 * @param start
 * @param end
 */
void ScreenBuffer::clearScreenBufferRange(int start, int end)
{
    int startPosition = ((m_y_position-1) * m_characters_per_line) + (start);
    int endPosition = startPosition + (end - start);

    //std::cout << "start " << start << " end " << end
    // << std::endl;
    //std::cout << "startPosition " << startPosition << " endPosition " << endPosition
    // << std::endl;

    // Clear out entire line.
    for(int i = startPosition; i < endPosition; i++)
    {
        try
        {
            m_screenBuffer[i].m_characterSequence.erase();
        }
        catch(std::exception &e)
        {
            std::cout << "Exception clearScreenBufferRange: "
                      << e.what() << std::endl;
        }
    }
    // Debugging
    //getScreenBufferText();
}

/**
 * @brief Clears all data in the buffer.
 */
void ScreenBuffer::clearScreenBuffer()
{
    // Allocate the Size
    m_screenBuffer.clear();
    m_screenBuffer.resize(m_term_height * m_term_width);
}

/**
 * @brief Testing, Prints screen butter to console.
 */
void ScreenBuffer::getScreenBufferText()
{
    for(auto &it : m_screenBuffer)
    {
        if(it.m_characterSequence != "")
            std::cout << it.m_characterSequence << std::flush;
        else
            std::cout << " " << std::flush;
    }
}

/**
 * @brief Gets Coordinates from the screen already translated to
 * Screen Buffer Positions, Now we pull the position and throw the
 * Text data into the Clipboard.
 * @param startx
 * @param starty
 * @param numChar
 * @param numRows
 */
void ScreenBuffer::bufferToClipboard(int startx, int starty, int numChar, int numRows)
{
    std::string textBuffer = "";
    int startPosition = (starty * m_characters_per_line) + startx;
    int endPosition   = startPosition + numChar;

    // Loop the Number of Rows to Grab
    for(int i = 0; i < numRows; i++)
    {
        // Grab each line per Row.
        for(int it = startPosition; it < endPosition; it++)
        {
            try
            {
                if(m_screenBuffer[it].m_characterSequence != "")
                {
                    textBuffer += m_screenBuffer[it].m_characterSequence;
                }
                else
                {
                    textBuffer += " ";
                }
            }
            catch(std::exception &e)
            {
                std::cout << "Exception bufferToClipboard: " << e.what() << std::endl;
            }
        }
        // Add Newline at the end of each row.
        textBuffer += "\r\n";

        // Reset start/end position to next Row.
        startPosition += m_term_width;
        endPosition   += m_term_width;
    }
    // Copy Resulting text to the Clipboard.
    SDL_SetClipboardText(textBuffer.c_str());
}
