#ifndef MenuFunction_H
#define MenuFunction_H

#include "menu_io.hpp"
#include "sequence_decoder.hpp"
#include "common_methods.hpp"

#include <string>
#include <vector>
#include <map>


// Forward Declerations
class CommonIO;
typedef std::shared_ptr<CommonIO> common_io_ptr;

class Menu;
typedef std::shared_ptr<Menu> menu_ptr;

class MenuOption;


/**
 * @class MenuFunction
 * @author Michael Griffin
 * @date 12/20/2015, started refactor 12/17/2020
 * @file menu_function.hpp
 * @brief Handle Internal Menu Commands and Lightbar processing.
 */
class MenuFunction
    : BaseCommon
{
public:

    // This needs access from ie Title Scan Class.
    MenuFunction(sequence_decoder_ptr &decoder, const std::string &program_path, common_io_ptr &comon_io);
    ~MenuFunction();

    std::string          m_program_path;
    sequence_decoder_ptr m_sequence_decoder;
    menu_ptr             m_menu_info;          // Menu Info
    MenuIO               m_menu_io;

    unsigned int  m_commands_executed;        // Test's for hot keys commands executed, if non pass through loop
    unsigned int  m_first_commands_executed;  // If we executed all FIRSTCMD and nothing is left then return
    unsigned int  m_starting_position;        // Light-bar Starting Position for Interfaces.

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

    std::string m_menu_path;

    std::string m_curent_menu;       // Current Menu
    std::string m_previous_menu;     // Previous Menu
    std::string m_gosub_menu;        // GoSub Menu.
    std::string m_output;            // Buffer for writing all light-bars at the same time

    std::vector<int>                  m_command_index;
    std::map<std::string, MenuOption> m_command_index_function;

    // Below here are Menu Processing Functions
    bool m_isLoadNewMenu;

    std::string                m_esc_cmd_keys_string;


    // Read in / Load Menu and Commands
    void menuReset();
    void menuStart(const std::string &curentMenu);
    void menuClearObjects();

    std::string checkForEscapeCommandSequence(std::string value);

    void menuLightBars(char *returnParameters,
                       const std::string &inputSequence);

    void menuProcess(char *returnParameters,
                     const std::string &inputSequence,
                     int area=0);

    // Menu Command Processing
    void menuDoCommands(MenuOption &option);

};

# endif
