#ifndef MenuFunction_H
#define MenuFunction_H

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "menuIO.hpp"

#include <string>
#include <vector>

typedef struct MenuRecord
{
    std::string MenuName;     // menu name
    std::string Directive;    // normal menu text file
    std::string MenuPrompt;   // menu prompt
    bool        Lightbar;     // Light-bar Menu?

} MenuRecord;

typedef struct CommandRecord
{
    std::string  LDesc;       // command description
    std::string  SDesc;       // command string
    std::string  CKeys;       // command execution keys
    std::string  CmdKeys;     // command keys
    std::string  MString;     // command data
    bool         LBarCmd;     // Is This a Light-bar Cmd
    std::string  HiString;    // Highlighted
    std::string  LoString;    // Un-highlighted
    uint16_t     Xcoord;      // Light-bar X coordinate
    uint16_t     Ycoord;      // Light-bar Y coordinate

} CommandRecord;


class MenuFunction :
    public MenuIO
{
protected:

    int  m_numCommands;            // Number of Commands in Loaded Menu
    int  m_commandsExecuted;       // Test's for hot keys commands executed, if non pass through loop
    int  m_firstCommandsExecuted;  // If we executed all FIRSTCMD and nothing is left then return
    int  m_startingPosition;       // Light-bar Starting Position for Interfaces.

    // Menu Light-bar Variables
    short m_xPosition;               // Holds X coordinate
    short m_yPosition;               // Holds Y coordinate
    short m_numLightbarCommand;      // Holds Light-bar # of choices
    short m_choice;                  // Holds Current Light-bar #
    short m_cntEscCmds;

    int  m_sequence;
    int  m_secondSequence;
    bool m_isEscapeSequence;       // Is Input key Escaped char, or Normal Key
    bool m_isSystemActive;         // System Exit

    char MENU_PATH[255];
    char m_outBuff[1024];          // Holds Formatted Light-bar Data

    std::string m_curentMenu;    // Current Menu
    std::string m_previousMenu;  // Previous Menu
    std::string m_gosubMenu;     // GoSub Menu.
    std::string m_output;        // Buffer for writing all light-bars at the same time


    MenuRecord                 m_menuRecord;
    std::vector<CommandRecord> m_commandRecord;
    std::vector<int>           m_commandIndex;
    std::vector<int>           m_commandIndexLightbar;

public:   

    // Below here are Menu Processing Functions
    bool m_isLoadNewMenu;

    // This needs access from ie Title Scan Class.
    MenuFunction();
    ~MenuFunction();

    std::string setupMenuPath();
    void dataParseHelper(std::string &temp);

    // Command Data Functions
    void commandsParse(std::string cfgdata,
                       int idx,
                       CommandRecord *cmdRecord);
    int  commandsExist(std::string MenuName, int idx);
    int  commandsCount(std::string MenuName);
    int  commandsReadData(std::string MenuName, int idx);

    // Menu Data Functions
    int  menuParseData(std::string cfgdata);
    int  menuReadData(std::string MenuName);

    // Read in / Load Menu and Commands
    void menuReadCommands();
    void menuReload();
    int  menuExists();
    void menuStart(std::string curentMenu);
    void menuClearObjects();

    void menuLightBars(char *returnParameters,
                       const std::string &inputSequence);

    void menuProcess(char *returnParameters,
                     const std::string &inputSequence,
                     int area=0);

    // Menu Command Processing
    void menuDoCommands(CommandRecord *cmdr);

};

# endif
