
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "screenBuffer.hpp"
#include "renderer.hpp"

#include <iostream>

ScreenBuffer::ScreenBuffer() :
    position(0),
    x_position(1),
    y_position(1),
    characters_per_line(80)
{
    //Set the default size of the vector screen buffer
    //Then Fill each element with defaults
    screenBuffer.reserve(TERM_HEIGHT * TERM_WIDTH);
    screenBuffer.resize(TERM_HEIGHT * TERM_WIDTH);
}

ScreenBuffer::~ScreenBuffer()
{
    // Clear
    std::vector<myScreen>().swap(screenBuffer);
    std::cout << "ScreenBuffer Released" << std::endl;
}

// Screen Buffer Vector Constructors
ScreenBuffer::myScreen::myScreen() :
    foreground({0, 0, 0, 0}),
    background({0, 0, 0, 0})
{ }

ScreenBuffer::myScreen::myScreen(
    std::string sequence,
    SDL_Color fg, SDL_Color bg) :
        characterSequence(sequence),
        foreground(fg),
        background(bg)
{ }

/**
 * handle screen buffer, Keeps track of all data
 * Plotted through the ANSI Parser so we can pull
 * Text and or redraw screens at anytime.
 */
void ScreenBuffer::setScreenBuffer(unsigned char mySequence)
{
    // Keep track of the longest line in buffer for Centering screen.
    // NOT IN USE CURRENTLY
    //if (x_position > max_x_position)
    //    max_x_position = x_position;

    std::string sequence;
    sequence = (signed)mySequence;
    sequenceBuffer.characterSequence = sequence;
    sequence.erase();

    sequenceBuffer.foreground = TheRenderer::Instance()->m_currentFGColor;
    sequenceBuffer.background = TheRenderer::Instance()->m_currentBGColor;

    // Setup current position in the screen buffer. 1 based for 0 based.
    position = ((y_position-1) * characters_per_line) + (x_position-1);

    // Add Sequence to Screen Buffer
    try
    {
        if((unsigned)position < screenBuffer.size())
            screenBuffer.at(position) = sequenceBuffer;
        else
        {
            std::cout << "Xposition: " << x_position-1 << std::endl;
        }
    }
    catch(std::exception &e)
    {
        std::cout << "Exception setScreenBuffer: " << e.what() << std::endl;
        std::cout << "Server sent data that exceeds screen dimensions." << std::endl;
    }
    // Clear for next sequences.
    sequenceBuffer.characterSequence.erase();
}

/*
 * Moves the Screen Buffer Up a line to match the internal SDL_Surface
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
        screenBuffer.erase(
            screenBuffer.begin(), screenBuffer.begin() + characters_per_line);
    }
    catch(std::exception &e)
    {
        std::cout << "Exception scrollScreenBuffer: " << e.what() << std::endl;
    }
    // Readd The last Line back to the buffer.
    screenBuffer.resize(TERM_HEIGHT * TERM_WIDTH);
}

/*
 * Clear Range of Screen Buffer for Erase Sequences.
 */
void ScreenBuffer::clearScreenBufferRange(int start, int end)
{
    int startPosition = ((y_position-1) * characters_per_line) + (start);
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
            screenBuffer[i].characterSequence.erase();
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
/*
 * When the Screen/Surface is cleared, we also clear the buffer
 */
void ScreenBuffer::clearScreenBuffer()
{
    // Allocate the Size
    screenBuffer.clear();
    screenBuffer.resize(TERM_HEIGHT * TERM_WIDTH);
}

// Debug to console.
void ScreenBuffer::getScreenBufferText()
{
    //std::cout << "* getScreenBufferText" << std::endl; // Start Fresh line.
    for(auto &it : screenBuffer)
    {
        if(it.characterSequence != "")
            std::cout << it.characterSequence << std::flush;
        else
            std::cout << " " << std::flush;
    }
}

/*
 * Gets Coordinates from the screen already translated to
 * Screen Buffer Positions, Now we pull the position and throw the
 * Text data into the Clipboard.
 */
void ScreenBuffer::bufferToClipboard(int startx, int starty, int numChar, int numRows)
{
    std::string textBuffer = "";
    int startPosition = (starty * characters_per_line) + startx;
    int endPosition   = startPosition + numChar;

    // Loop the Number of Rows to Grab
    for(int ot = 0; ot < numRows; ot++)
    {
        // Grab each line per Row.
        for(int it = startPosition; it < endPosition; it++)
        {
            try
            {
                if(screenBuffer[it].characterSequence != "")
                {
                    textBuffer += screenBuffer[it].characterSequence;
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
        startPosition += characters_per_line;
        endPosition   += characters_per_line;
    }
    // Copy Resulting text to the Clipboard.
    SDL_SetClipboardText(textBuffer.c_str());
}
