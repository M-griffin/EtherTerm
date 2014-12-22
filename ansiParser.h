#ifndef __ANSI_H    /*  An extra safeguard to prevent this header from  */
#define __ANSI_H    /*  being included twice in the same source file    */

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include <stdint.h>
#include <string>

class AnsiParser
{
public:

    static AnsiParser* Instance()
    {
        if(globalInstance == 0)
        {
            globalInstance = new AnsiParser();
        }

        return globalInstance;
    }

    int  x_position;
    int  y_position;
    int  max_x_position;
    int  characters_per_line;
    int  position;
    int  has_scrolled;

    const  char ESC   = '\x1b';
    const  int  TERM_HEIGHT = 25;
    const  int  TERM_WIDTH  = 80;

    #define NUM_LINES   25       // change to 50 for 80x50 ANSI's
    #define FALSE 0
    #define TRUE 1
    #define MAX_PARAMS  10

    // ANSI escape sequences
    #define CURSOR_POSITION       'H'
    #define CURSOR_POSITION_ALT   'f'   // equivalent to 'H'
    #define CURSOR_UP             'A'
    #define CURSOR_DOWN           'B'
    #define CURSOR_FORWARD        'C'
    #define CURSOR_BACKWARD       'D'

    #define CURSOR_X_POSITION     'G'   // XTERM
    #define CURSOR_NEXT_LINE      'E'   // XTERM
    #define CURSOR_PREV_LIVE      'F'   // XTERM

    #define ERASE_DISPLAY         'J'   // 2J
    #define ERASE_TO_EOL          'K'

    #define SAVE_CURSOR_POS       's'
    #define RESTORE_CURSOR_POS    'u'
    #define SET_GRAPHICS_MODE     'm'
    #define SET_MODE              'h'   // Line Wraparound ?7h
    #define RESET_MODE            'l'
    #define SET_KEYBOARD_STRINGS  'p'
    #define ANSI_DETECTION        'n'   // ESC[6n
    #define SCROLL_REGION          'r'   // ESC[#;#r

    // Text attributes
    #define ALL_ATTRIBUTES_OFF 0
    #define BOLD_ON            1
    #define UNDERSCORE         4
    #define BLINK_ON           5
    #define REVERSE_VIDEO_ON   7
    #define CONCEALED_ON       8

    // Foreground colors
    #define FG_BLACK    30
    #define FG_RED      31
    #define FG_GREEN    32
    #define FG_YELLOW   33
    #define FG_BLUE     34
    #define FG_MAGENTA  35
    #define FG_CYAN     36
    #define FG_WHITE    37

    // Background colors

    #define BG_BLACK    40
    #define BG_RED      41
    #define BG_GREEN    42
    #define BG_YELLOW   43
    #define BG_BLUE     44
    #define BG_MAGENTA  45
    #define BG_CYAN     46
    #define BG_WHITE    47

    // reset global variables when making new conenctions or changeing states.
    char INPUT_COLOR[255];
    void rightSpacing(char *str, int space);
    void leftSpacing(char *str, int space);
    void mask(char *str);
    void inputfield(char *text, int &len);


    int  getkey();
    void getline(char *line,        // Returns Input into Line
                 int   length,      // Max Input Length of String
                 char *leadoff,     // Data to Display in Default String {Optional}
                 int   hid,         // If input is Echomail as hidden    {Optional}
                 char *chlist);     // Valid Input Char List             {Optional}

    void ansi_fg(char *data, int fg);
    void ansi_bg(char *data, int bg);
    void pipe2ansi(char* szString, int buffer=true);

    void reset();
    void ansiPrintf(std::string fileName);
    void ansiparse(unsigned char *buff, int center_screen);

private:

    int saved_cursor_x;
    int saved_cursor_y;

    int color_attribute;
    int prev_color_attribute;
    int cleared_the_screen;

    AnsiParser();
    ~AnsiParser();

    AnsiParser(const AnsiParser&);
    AnsiParser& operator=(const AnsiParser&);

    // singleton
    static AnsiParser* globalInstance;
};

typedef AnsiParser TheAnsiParser;

#endif
