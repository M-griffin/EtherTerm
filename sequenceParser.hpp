#ifndef __ANSI_H    /*  An extra safeguard to prevent this header from  */
#define __ANSI_H    /*  being included twice in the same source file    */

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "screenBuffer.hpp"

#ifdef TARGET_OS_MAC
#include <SDL2/SDL.h>
#elif _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <stdint.h>
#include <string>
#include <vector>

class SequenceParser :
    public ScreenBuffer
{
public:

    static SequenceParser* Instance()
    {
        if(!globalInstance)
        {
            globalInstance = new SequenceParser();
        }
        return globalInstance;
    }

    // Release And Clear the Singleton
    static void ReleaseInstance()
    {
        if(globalInstance)
        {
            delete globalInstance;
            globalInstance = nullptr;
        }
        return;
    }

    const  char ESC   = '\x1b';
    // This needs to be passed from TheTerminal instance eventually
    const  int  TERM_HEIGHT = 25; // Temp
    const  int  TERM_WIDTH  = 80; // Temp

#define NUM_LINES   25       // change to 50 for 80x50 ANSI's
#define MAX_PARAMS  10

    // Control Sequence Terminators
#define CURSOR_UP             'A'
#define CURSOR_DOWN           'B'
#define CURSOR_FORWARD        'C'
#define CURSOR_BACKWARD       'D'
#define CURSOR_NEXT_LINE      'E'   // XTERM
#define CURSOR_PREV_LIVE      'F'   // XTERM
#define CURSOR_X_POSITION     'G'   // XTERM
#define CURSOR_POSITION       'H'

#define ERASE_DISPLAY         'J'   // 2J
#define ERASE_TO_EOL          'K'

#define DELETE_CHARACTER      'P'   // Erase Character(2)

#define REPEAT_CHARACTER      'b'   // Repeat preceding character
#define LINE_POS_ABSOLUTE     'd'   // Line Position Absolute [row] (default = [1,column]) (VPA).
#define CURSOR_POSITION_ALT   'f'   // equivalent to 'H'
#define SET_MODE              'h'   // Line Wraparound ?7h
#define SCROLL_REGION         'r'   // ESC[#;#r
#define SAVE_CURSOR_POS       's'
#define RESTORE_CURSOR_POS    'u'
#define RESET_MODE            'l'
#define SET_GRAPHICS_MODE     'm'
#define ANSI_DETECTION        'n'   // ESC[6n
#define SET_KEYBOARD_STRINGS  'p'

    /* Extra Notes.
    ESC [ 7 ; col h Enables line wrapping at column position. If col (1-based)
     *  is absent, wrap at column 80.
    ESC [ 7 l     Disables line wrapping. Lines wraps at the end of screen buffer.
    ESC [ 25 h     Show text cursor.
    ESC [ 25 l     Hide text cursor.
    */

    // Handles Character Repeats.
    unsigned char preceedingSequence;

    // Text attributes
    enum
    {
        // Attributes
        ALL_ATTRIBUTES_OFF = 0,
        BOLD_ON            = 1,
        UNDERSCORE         = 4,
        BLINK_ON           = 5,
        REVERSE_VIDEO_ON   = 7,
        CONCEALED_ON       = 8,

        // Foreground colors
        FG_BLACK           = 30,
        FG_RED             = 31,
        FG_GREEN           = 32,
        FG_YELLOW          = 33,
        FG_BLUE            = 34,
        FG_MAGENTA         = 35,
        FG_CYAN            = 36,
        FG_WHITE           = 37,

        // Background colors
        BG_BLACK           = 40,
        BG_RED             = 41,
        BG_GREEN           = 42,
        BG_YELLOW          = 43,
        BG_BLUE            = 44,
        BG_MAGENTA         = 45,
        BG_CYAN            = 46,
        BG_WHITE           = 47
    };

    // Reset Static Variables values for the class
    void reset();

    // Parses Straight Text input with no control sequences
    void textInput(const std::string &buffer);

    // Parses Control sequences passed in vector. (sequenceParser)
    void sequenceCursorAndDisplay();
    void sequenceGraphicsModeDisplay();
    void sequenceResetAndResponses();

    // Handles parameterized ESC Sequences.
    void sequenceInput(std::vector<int> &sequenceParameters);

    bool isCursorActive()
    {
        return isCursorShown;
    }
    void setCursorActive(bool active)
    {
        isCursorShown = active;
    }

private:

    std::vector<int> parameters;
    std::string current_color;
    std::string textBuffer;

    int max_x_position;
    int saved_cursor_x;
    int saved_cursor_y;
    int saved_attribute;
    int saved_prev_attr;
    int color_attribute;
    int prev_color_attribute;

    bool line_wrapped;
    bool cleared_the_screen;
    bool isCursorShown;

    SDL_Color savedForegroundColor;
    SDL_Color savedBackgroundColor;

    SequenceParser();
    ~SequenceParser();
    SequenceParser(const SequenceParser&);
    SequenceParser& operator=(const SequenceParser&);

    static SequenceParser* globalInstance;
};

typedef SequenceParser TheSequenceParser;

#endif
