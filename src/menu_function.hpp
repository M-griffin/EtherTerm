#ifndef MenuFunction_H
#define MenuFunction_H

#include "menu_io.hpp"
#include "sequence_decoder.hpp"

#include <string>
#include <vector>
#include <map>

/**
 * @class MenuRecord
 * @author Michael Griffin
 * @date 12/20/2015
 * @file menu_function.hpp
 * @brief Struct for Menu Records
 */
typedef struct MenuRecord
{
    std::string menu_mame;     // menu name
    std::string directive;     // normal menu text file
    std::string menu_prompt;   // menu prompt
    bool        is_lightbar;   // Light-bar Menu?

} MenuRecord;

/**
 * @class CommandRecord
 * @author Michael Griffin
 * @date 12/20/2015
 * @file menu_function.hpp
 * @brief Struct for Menu Command Records
 */
typedef struct CommandRecord
{
    std::string  description;      // command description
    std::string  short_desc;       // command string
    std::string  control_key;      // command execution keys
    std::string  command;          // command key/hot key.
    std::string  method_string;    // command data
    bool         is_lightbar;      // Is This a Light-bar Cmd
    std::string  active_string;    // Highlighted
    std::string  inactive_string;  // Un-highlighted
    uint16_t     x_coord;          // Light-bar X coordinate
    uint16_t     y_coord;          // Light-bar Y coordinate

} CommandRecord;

/**
 * @class MenuFunction
 * @author Michael Griffin
 * @date 12/20/2015
 * @file menu_function.hpp
 * @brief Handle Internal Menu Commands and Lightbar processing.
 */
class MenuFunction
{
public:

    // This needs access from ie Title Scan Class.
    MenuFunction(sequence_decoder_ptr &decoder, const std::string &program_path);
    ~MenuFunction();

    std::string          m_program_path;
    sequence_decoder_ptr m_sequence_decoder;
    MenuIO               m_menu_io;

    int  m_num_commands;             // Number of Commands in Loaded Menu
    int  m_commands_executed;        // Test's for hot keys commands executed, if non pass through loop
    int  m_first_commands_executed;  // If we executed all FIRSTCMD and nothing is left then return
    int  m_starting_position;        // Light-bar Starting Position for Interfaces.

    // Menu Light-bar Variables
    short m_x_position;              // Holds X coordinate
    short m_y_position;              // Holds Y coordinate
    short m_num_lightbar_commands;   // Holds Light-bar # of choices
    short m_choice;                  // Holds Current Light-bar #
    short m_num_esc_commands;

    int  m_sequence;
    int  m_second_sequence;
    bool m_is_escape_sequence;       // Is Input key Escaped char, or Normal Key
    bool m_is_system_active;         // System Exit

    char m_menu_path[255];
    char m_output_buffer[1024];      // Holds Formatted Light-bar Data

    std::string m_curent_menu;       // Current Menu
    std::string m_previous_menu;     // Previous Menu
    std::string m_gosub_menu;        // GoSub Menu.
    std::string m_output;            // Buffer for writing all light-bars at the same time


    MenuRecord                 m_menu_record;
    std::vector<CommandRecord> m_command_record;
    std::vector<int>           m_command_index;
    std::map<std::string, int> m_command_index_function;

    // Below here are Menu Processing Functions
    bool m_isLoadNewMenu;

    std::string getProgramPath();
    std::string getDirectoryPath();

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

