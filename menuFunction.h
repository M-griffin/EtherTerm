#ifndef MenuFunction_H
#define MenuFunction_H

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include <string>

typedef struct MenuRec
{
    std::string MenuName;     // menu name
    std::string Directive;    // normal menu text file
    std::string MenuPrompt;   // menu prompt
    std::string Acs;          // access requirements
    std::string Password;     // password required

    uint16_t  ForceInput;     // 0 user defaults, 1 Hotkeys 1 ENTER
    uint16_t  ForceHelpLevel; // 0 user defaults, 1 force display, 0 force expert
    bool      Lightbar;       // Lightbar Menu?

} MenuRec;

typedef struct CommandRec
{
    std::string  LDesc;       // command description
    std::string  SDesc;       // command string
    std::string  CKeys;       // command execution keys
    std::string  Acs;         // access requirements
    std::string  CmdKeys;     // command keys
    std::string  MString;     // command data
    bool  SText;              // Is this a Scroll Test Comamnd
    uint16_t STLen;           // Length of Scroll Text
    bool  LBarCmd;            // Is This a Lighbar Cmd
    std::string  HiString;    // Highlighed
    std::string  LoString;    // Unhighlighted
    uint16_t Xcoord;          // Lightbar X coord
    uint16_t Ycoord;          // Lightbar Y coord

} CommandRec;


class MenuFunction
{

protected:
    MenuRec    *menur2;
    CommandRec *cmdr2;

    int  noc;           // Number of Commands in Loaded Menu
    int  System_Alive;  // System Exit

public:

    char MENUPATH[255];

    std::string _curmenu;  // Current Menu
    std::string _premenu;  // Previous Menu
    std::string _gosub;    // GoSub Menu.

    // Menu Lightbar Variables
    short xx;        // Holds X Coord
    short yy;        // Holds Y Coord
    short iNoc;      // Holds Lightbar # of choices
    short sNoc;      // Holds Scroll Text # of choices
    short choice;    // Holds Currect Lightbar #

    short  *execnum;
    short  *execnum2;
    short  *execnum3;

    unsigned long cntEscCmds;

    int c, cc;
    bool  EscHit;   // Is Input key Escaped char, or Normal Key
    char  outBuff[1024];      // Holds Formatted Lightbar Data
    std::string output;       // Buffer for writing all lightbars at the same time
    short sRand;        // Randmise Text Scrollies
    pthread_t ahThread;
    bool      tScroll;
    int       executed;       // Test's for hot keys commands excuted, if non pass through loop
    // This is so scrolling isn't killed with invalid menu input!
    int exe;

    uint32_t CURRENT_AREA;    // Lightbar Starting Position for Interfaces.
    // This needs access from ie Title Scan Class.

    MenuFunction();
    ~MenuFunction();

    // Below here are Menu Processing Functions
    bool _loadnew;

    // New Menu Text File Parsing
    void chkparse(std::string &temp);

    int  mnuparse(std::string cfgdata);
    int  menu_read(std::string MenuName);

    void cmdparse(std::string cfgdata, int idx);
    int  cmdexist(std::string MenuName, int idx);
    int  cnt_cmds(std::string MenuName);
    int  cmds_read(std::string MenuName, int idx);


    // Read in / Load Menu and Commands
    void insert_cmdr();
    void menu_reset();
    int  menu_exists();
    void menu_readin();
    void menu_cmds(char *inPut);

    void menu_clear();

    void display_menu(bool forcelevel= false);
    void menu_bars(char *inPut);
    void menu_proc(char *mString, uint32_t area=0);

    // Menu Command Processing
    void menu_docmd(CommandRec *cmdr);

    // Menu System IO
    // ----------------------------------------------------------------------
    // reset global variables when making new conenctions or changeing states.

    static void rightSpacing(char *str, int space);
    static void leftSpacing(char *str, int space);
    static void mask(char *str);
    static void inputfield(char *text, int &len);


    static int  getkey();
    static void getline(char *line,        // Returns Input into Line
                 int   length,      // Max Input Length of String
                 char *leadoff,     // Data to Display in Default String {Optional}
                 int   hid,         // If input is Echomail as hidden    {Optional}
                 char *chlist);     // Valid Input Char List             {Optional}

    static void ansi_fg(char *data, int fg);
    static void ansi_bg(char *data, int bg);
    static void pipe2ansi(char* szString, int buffer=true);
    static void ansiPrintf(std::string fileName);
};

# endif
