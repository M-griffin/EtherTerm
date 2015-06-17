#ifndef SCREENBUFFER_HPP
#define SCREENBUFFER_HPP

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#ifdef TARGET_OS_MAC
#include <SDL2/SDL.h>
#elif _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <string>
#include <vector>

class ScreenBuffer
{
public:
    ScreenBuffer();
    ~ScreenBuffer();

    int position;
    int x_position;
    int y_position;
    int characters_per_line;

    enum {
        TERM_HEIGHT = 25,
        TERM_WIDTH  = 80
    };

    // Holds Screen Buffer Structure
    // We want each character as a string for multi-byte UTF8 characters.
    struct myScreen
    {
        myScreen();
        myScreen(std::string sequence, SDL_Color fg, SDL_Color bg);
        std::string characterSequence;
        SDL_Color foreground;
        SDL_Color background;
    };

    // Screen Array.
    myScreen sequenceBuffer;
    std::vector<myScreen> screenBuffer;

    // Function for populating the Screen Buffer
    void setScreenBuffer(std::string mySequence);
    void scrollScreenBuffer();
    void clearScreenBufferRange(int start, int end);
    void clearScreenBuffer();
    void getScreenBufferText();
    void bufferToClipboard(int startx, int starty, int numChar, int numRows);

};

#endif // SCREENBUFFER_HPP
