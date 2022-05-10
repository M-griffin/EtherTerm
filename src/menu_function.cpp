#include "menu_function.hpp"
#include "sequence_decoder.hpp"
#include "menu_io.hpp"
#include "static_methods.hpp"
#include "common_io.hpp"
#include "menu.hpp"
#include "menu_dao.hpp"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <unistd.h>
#include <string>
#include <fstream>

MenuFunction::MenuFunction(sequence_decoder_ptr &decoder, const std::string &program_path, common_io_ptr &comon_io)
    : m_program_path(program_path)
    , m_sequence_decoder(decoder)
    , m_menu_info(new Menu())
    , m_menu_io(decoder, program_path, comon_io)
    , m_commands_executed(0)
    , m_first_commands_executed(0)
    , m_starting_position(0)
    , m_x_position(0)
    , m_y_position(0)
    , m_num_lightbar_commands(0)
    , m_choice(0)
    , m_num_esc_commands(0)
    , m_sequence(0)
    , m_second_sequence(0)
    , m_is_escape_sequence(false)
    , m_is_system_active(true)
    , m_isLoadNewMenu(true)
    , m_esc_cmd_keys_string("ESC, LEFT, RIGHT, UP, \
        DOWN, HOME, END, PAGEUP, PAGEDN")
{
    std::cout << "MenuFunction Created!" << std::endl;
}

MenuFunction::~MenuFunction()
{
    std::cout << "~MenuFunction" << std::endl;
    std::vector<int>().swap(m_command_index);
    std::map<std::string, MenuOption>().swap(m_command_index_function);
}


/**
 * @brief Reset or Set Initial Defaults before a menu is read.
 */
void MenuFunction::menuReset()
{
    std::cout << "DEBUG: menuReset()" << std::endl;
    m_x_position            = 1;   // Holds X Coord
    m_y_position            = 1;   // Holds Y Coord
    m_num_lightbar_commands = 0;   // Holds Light-bar # of choices
    m_num_esc_commands      = 0;   // Number of ESC Commands.

    m_sequence              = '\0';
    m_second_sequence       = '\0';   // Hold Input / Light-bar Key
    m_is_escape_sequence    = false;  // Is Input key Escaped char, or Normal Key

    m_output.erase();                 // Buffer for writing all light-bars at the same time
    m_commands_executed = 0;          // Test's for hot keys commands executed, if non pass through loop

    // This is so scrolling isn't killed with invalid menu input!
    m_first_commands_executed = 0;

    // Clear Out the Menu Record
    m_menu_info->directive.erase();
    m_menu_info->is_lightbar = false;
    m_menu_info->menu_name.erase();
    m_menu_info->menu_prompt.erase();
}

/**
 * @brief Function to Read in a Menu set in the class
 */
void MenuFunction::menuStart(const std::string &currentMenu)
{
    std::cout << "DEBUG: menuStart()" << std::endl;

    // Don't reload if menu hasn't changed.
    if(m_previous_menu == currentMenu)
    {
        m_isLoadNewMenu = false;
        return;
    }

    // Menu Reset of Global Class Variables
    menuReset();

    // Setup the current Menu Passed.
    m_curent_menu = currentMenu;
    std::string path = StaticMethods::getDirectoryPath(m_program_path);

    // Reset Gobal Handle to the menu w/ options.
    m_menu_info.reset(new Menu());

    // Call MenuDao to readin .yaml file
    MenuDao mnuDao(m_menu_info, m_curent_menu, path);

    // Verify if file exists, load it.
    // Otherwise we'll need to show an error, lateron Create new.
    if(mnuDao.fileExists())
    {
        // Reset the Smart Pointer on menu load.
        std::cout << " ### loading menu: " << path << " " << m_curent_menu << std::endl;
        mnuDao.loadMenu();
        m_isLoadNewMenu = true;
    }
    else
    {
        std::cout << " ### Error: loading menu: " << path << " " << m_curent_menu << std::endl;
        m_isLoadNewMenu = false;
        return;
    }

    // Read Menu ANSI to String that will hold light-bars, Push to screen same-time
    // Will speed up display and make ghosting not appear as much
    if(m_menu_info->directive != "")
    {
        m_menu_io.displayMenuAnsi(m_menu_info->directive);
    }

    // Reset Vector Lists.
    menuClearObjects();
    m_first_commands_executed = 0;

    // Now loop and scan for first cmd and each time
    for(unsigned int i = 0; i < m_menu_info->menu_options.size(); i++)
    {
        auto &mo = m_menu_info->menu_options[i];

        // If we find a FIRSTCMD, Execute it right away!
        if(mo.control_key == "FIRSTCMD")
        {
            menuDoCommands(mo);  // switch method to options vecto
            ++m_first_commands_executed;

            continue;
        }

        // Get Light-bar Number of Commands
        // Registered as Light bars
        if(mo.is_lightbar)
        {
            m_command_index.push_back(i);
        }

        // Populate Escapted Control Keys Mapping.
        if(stdStringContains(m_esc_cmd_keys_string, mo.control_key))
        {
            m_command_index_function.insert(
                std::make_pair(mo.control_key, mo)
            );
        }

        m_previous_menu = m_curent_menu;

        // Return if we executed all commands as FIRSTCMD!
        if(m_first_commands_executed == m_menu_info->menu_options.size())
        {
            return;
        }
    }

    m_output.clear();

    // Set the Sizes
    m_num_esc_commands = m_command_index_function.size();
    m_num_lightbar_commands = m_command_index.size();

    auto &menu_options = m_menu_info->menu_options;

    if(m_num_lightbar_commands != 0)
    {
        // Throw out a menu prompt if there is one available. - Add MCI Parsing too!
        // Use Same X Row, these got reversed somehow!!! grrrr
        m_x_position = 1;
        m_y_position = menu_options[m_command_index[0]].y_coord;
    }

    // Light-bar menu then draw
    if(m_num_lightbar_commands > 0)
    {
        auto &menu_options = m_menu_info->menu_options;

        // Setup of Remaining Light-bars in Low highlight Form
        for(int rep = 0; rep != m_num_lightbar_commands; rep++)
        {
            if(rep != m_choice)
            {
                m_x_position = menu_options[m_command_index[rep]].x_coord;
                m_y_position = menu_options[m_command_index[rep]].y_coord;
                m_output += stdStringFormat("\x1b[%i;%iH%s",m_y_position,m_x_position,
                                            (char *)menu_options[m_command_index[rep]].inactive_string.c_str());
            }
            else
            {
                m_x_position = menu_options[m_command_index[rep]].x_coord;
                m_y_position = menu_options[m_command_index[rep]].y_coord;
                m_output += stdStringFormat("\x1b[%i;%iH%s",m_y_position,m_x_position,
                                            (char *)menu_options[m_command_index[rep]].active_string.c_str());
            }
        }

        // Moves cursor to end of line during light-bars.
        m_output += stdStringFormat("\x1b[%i;79H",m_y_position);
        m_menu_io.sequenceToAnsi(m_output);
    }
    // Not Light-bars, print out menu ANSI.
    else
    {
        m_menu_io.sequenceToAnsi(m_output);
    }

    //Replace Messages Left...
    m_output.clear();
    m_output = m_menu_info->menu_prompt;

    // If light-bar prompt, reset to beginning of line.
    if(m_num_lightbar_commands > 0)
    {
        m_x_position = 1;
        m_output += stdStringFormat("\x1b[%i;%iH%s",m_y_position,m_x_position,
                                    (char *)m_menu_info->menu_prompt.c_str());
    }

    if(m_output.size() > 1)
    {
        m_menu_io.sequenceToAnsi(m_output);
    }
}

/**
 * @brief Clear Menu When Done.
 */
void MenuFunction::menuClearObjects()
{
    std::cout << "DEBUG: menuClearObjects()" << std::endl;
    std::vector<int>().swap(m_command_index);
    std::map<std::string, MenuOption>().swap(m_command_index_function);
}


/**
 * @brief Check Mapping for ESC Sequence tags
 * @param value
 * @return
 */
std::string MenuFunction::checkForEscapeCommandSequence(std::string value)
{
    std::cout << "DEBUG: checkForEscapeCommandSequence()" << std::endl;
    auto it = m_command_index_function.find(value.c_str());

    if(it != m_command_index_function.end())
    {
        std::cout << it->second.command << std::endl;

        ++m_commands_executed;
        menuDoCommands(it->second);
        return it->second.command.c_str();
    }

    return "\0";
}


/**
 * @brief Used for Light-bars and Hokey Menu's
 * This is also used in Full Screen Interfaces
 * for Light-bars selections and Return
 * Pass-though commands from Menu Light-bar Prompts.
 */
void MenuFunction::menuLightBars(char *returnParameters,
                                 const std::string &inputSequence)
{
    std::cout << "DEBUG: menuLightBars()" << std::endl;
    //std::cout << "Menu Bars Input received: " << inputSequence << std::endl;
    m_sequence = inputSequence[0];

    // Check here for Arrow Key / Escaped Input was Received
    if((int)m_sequence == 27)
    {
        //std::cout << "ESC RECEIVED" << std::endl;
        if(inputSequence.size() > 1)
        {
            m_second_sequence = inputSequence[2];

            // Handle Hardware Input Sequences
            if(m_second_sequence == '0')
            {
                m_second_sequence = inputSequence[3];
            }
        }
        else
            m_second_sequence = '\0';

        m_is_escape_sequence = true;
    }
    else
    {
        m_is_escape_sequence = false;
    }

    m_output.clear();
    m_commands_executed  = 0;

    if(m_is_escape_sequence)
    {
        // Input Key is Escaped Meaning Arrow Keys
        // std::cout << "ESC Hit: " << cc << " cntEscCmds:" << cntEscCmds << std::endl;
        // Run through and check for any Menu commands that overdrive default pass-through!
        if(m_num_esc_commands > 0)
        {
            if(m_second_sequence == '\0')  // Straight ESC Key
            {
                std::string parameters = checkForEscapeCommandSequence(std::string("ESC"));
                strcpy(returnParameters,(const char*)parameters.c_str());
            }
            else if(m_second_sequence == 'A')
            {
                std::string parameters = checkForEscapeCommandSequence(std::string("UP"));
                strcpy(returnParameters,(const char*)parameters.c_str());
            }
            else if(m_second_sequence == 'B')
            {
                std::string parameters = checkForEscapeCommandSequence(std::string("DOWN"));
                strcpy(returnParameters,(const char*)parameters.c_str());
            }
            else if(m_second_sequence == 'C')
            {
                std::string parameters = checkForEscapeCommandSequence(std::string("RIGHT"));
                strcpy(returnParameters,(const char*)parameters.c_str());
            }
            else if(m_second_sequence == 'D')
            {
                std::string parameters = checkForEscapeCommandSequence(std::string("LEFT"));
                strcpy(returnParameters,(const char*)parameters.c_str());
            }
            else if(m_second_sequence == 'V' ||
                    (m_second_sequence == '5' && inputSequence[3] == '~'))
            {
                std::string parameters = checkForEscapeCommandSequence(std::string("PAGEUP"));
                strcpy(returnParameters,(const char*)parameters.c_str());
            }
            else if(m_second_sequence == 'U' ||
                    (m_second_sequence == '6' && inputSequence[3] == '~'))
            {
                std::string parameters = checkForEscapeCommandSequence(std::string("PAGEDN"));
                strcpy(returnParameters,(const char*)parameters.c_str());
            }
            else if(m_second_sequence == 'H')
            {
                std::string parameters = checkForEscapeCommandSequence(std::string("HOME"));
                strcpy(returnParameters,(const char*)parameters.c_str());
            }
            else if(m_second_sequence == 'K')
            {
                std::string parameters = checkForEscapeCommandSequence(std::string("END"));
                strcpy(returnParameters,(const char*)parameters.c_str());
            }

            // Executed == 0, Then Key Pressed was not valid! :)
            // Pass through the ESC then
            if(m_commands_executed == 0)
            {
                returnParameters[0] = m_second_sequence;

                // Translate to Terminal ESC Keys
                if(m_second_sequence == '0')
                    m_second_sequence = inputSequence[3];
            }
            else
            {
                //std::cout << "Executed, now pass-through back to interface the key pressed!" << std::endl;
                return;
            }
        }

        // If true, means we pass A and B up/dn through to system!
        //elog ("Start Light-bar Processing... CMD: %c, cntEscCmds: %ld", cc, cntEscCmds);
        if(m_num_lightbar_commands > 0)
        {
            auto &menu_options = m_menu_info->menu_options;

            ////errlog2("menu_bars() 23.4 - iNoc > 0");
            if(m_second_sequence == 'D' && m_commands_executed == 0)
            {
                m_x_position = menu_options[m_command_index[m_choice]].x_coord;
                m_y_position = menu_options[m_command_index[m_choice]].y_coord;
                m_output += stdStringFormat("\x1b[%i;%iH%s",m_y_position,m_x_position,
                                            (char *)menu_options[m_command_index[m_choice]].inactive_string.c_str());

                if(m_choice == 0) m_choice = m_num_lightbar_commands-1;
                else --m_choice;

                m_x_position = menu_options[m_command_index[m_choice]].x_coord;
                m_y_position = menu_options[m_command_index[m_choice]].y_coord;
                m_output += stdStringFormat("\x1b[%i;%iH%s\x1b[%i;79H",m_y_position,m_x_position,
                                            (char *)menu_options[m_command_index[m_choice]].active_string.c_str(),m_y_position);

                m_menu_io.sequenceToAnsi(m_output);
            }
            else if(m_second_sequence == 'C' && m_commands_executed == 0)
            {
                m_x_position = menu_options[m_command_index[m_choice]].x_coord;
                m_y_position = menu_options[m_command_index[m_choice]].y_coord;
                m_output += stdStringFormat("\x1b[%i;%iH%s",m_y_position,m_x_position,
                                            (char *)menu_options[m_command_index[m_choice]].inactive_string.c_str());

                if(m_choice == m_num_lightbar_commands-1) m_choice = 0;
                else ++m_choice;

                m_x_position = menu_options[m_command_index[m_choice]].x_coord;
                m_y_position = menu_options[m_command_index[m_choice]].y_coord;
                m_output += stdStringFormat("\x1b[%i;%iH%s\x1b[%i;79H",m_y_position,m_x_position,
                                            (char *)menu_options[m_command_index[m_choice]].active_string.c_str(),m_y_position);

                m_menu_io.sequenceToAnsi(m_output);
            }

        } //</iNoc>
        else
        {
            std::cout << "Normal Key Input! ON ESCAPE ???REALLY?? OR JUST FANTASY: " << std::endl;

            // normal Key Input.
            if((int)m_sequence == 10)
            {
                m_commands_executed = 0;

                // Here Loop through and execute stacked Commands
                for(std::string::size_type ckey = 0; ckey != m_menu_info->menu_options.size(); ckey++)
                {
                    auto &mo  = m_menu_info->menu_options[ckey];
                    auto &moc = m_menu_info->menu_options[m_command_index[m_choice]];

                    // ENTER or Existing Lightbar Selection
                    if(mo.control_key == "ENTER" || mo.control_key == moc.control_key)
                    {
                        ++m_commands_executed;
                        menuDoCommands(mo);
                        strcpy(returnParameters,(const char*)mo.command.c_str());
                    }
                }

                if(m_commands_executed > 0)
                {
                    return;
                }
            }
            //  Else go through and run stack commands on hot key pressed.
            else
            {
                m_commands_executed = 0;     // Normal Key Inputted, if Match's

                for(std::string::size_type ckey = 0; ckey != m_menu_info->menu_options.size(); ckey++)
                {
                    auto &mo  = m_menu_info->menu_options[ckey];

                    // Loop and Run Stacked Commands.
                    if(m_sequence == 32)
                    {
                        // Else check if it's a space-bar
                        if(mo.control_key == "SPACE")
                        {
                            ++m_commands_executed;
                            menuDoCommands(mo);
                            strcpy(returnParameters,(const char*)mo.command.c_str());
                        }
                    }
                    // Check any remaining Keys Hot Key Input.
                    else if(mo.control_key[0] == toupper(m_sequence) &&
                            mo.control_key.size() < 2)
                    {
                        ++m_commands_executed;
                        menuDoCommands(mo);
                        strcpy(returnParameters,(const char*)mo.command.c_str());
                    }
                }
            }

            // Executed == 0, Then Key Pressed was not valid!, and no stacked commands to loop :)
            if(m_commands_executed > 0)
            {
                return;
            }
        } // </</iNoc>
    } // </EscHit>
    // Normal Key Input
    else
    {
        // If ENTER, Return Cmd # of Light-bar Executed
        // also catch any stacked keys and execute in order!
        if((int)m_sequence == 13 && m_num_lightbar_commands > 0)
        {
            // Only Execute [ENTER] on Light-bars!
            m_commands_executed = 0;

            // Here Loop through and execute stacked Commands
            for(std::string::size_type ckey = 0; ckey != m_menu_info->menu_options.size(); ckey++)
            {
                auto &mo  = m_menu_info->menu_options[ckey];
                auto &moc  = m_menu_info->menu_options[m_command_index[m_choice]];

                if(mo.control_key == moc.control_key)
                {
                    ++m_commands_executed;
                    menuDoCommands(mo);
                    strcpy(returnParameters,(const char*)mo.command.c_str());
                }
            }

            // Executed Return Pass-through or Normal Key!
            if(m_commands_executed > 0)
            {
                return;
            }
        }
        else if((int)m_sequence == 13 && m_num_lightbar_commands == 0)
        {
            // Check for ENTER CKEY!
            m_commands_executed = 0;

            // Here Loop through and execute stacked Commands
            for(std::string::size_type ckey = 0; ckey != m_menu_info->menu_options.size(); ckey++)
            {
                auto &mo  = m_menu_info->menu_options[ckey];

                if(mo.control_key == "ENTER")
                {
                    ++m_commands_executed;
                    menuDoCommands(mo);
                    strcpy(returnParameters,(const char*)mo.command.c_str());
                }
            }

            if(m_commands_executed > 0)
            {
                return;
            }
        }
        //  Else go through and run stack commands on hot key pressed.
        else
        {
            m_commands_executed = 0;      // Normal Key Inputted, if Match's

            for(std::string::size_type ckey = 0; ckey != m_menu_info->menu_options.size(); ckey++)
            {
                auto &mo  = m_menu_info->menu_options[ckey];

                // Loop and Run Stacked Commands.
                if(m_sequence == 32)
                {
                    // Else check if it's a space-bar
                    if(mo.control_key == "SPACE")
                    {
                        ++m_commands_executed;
                        menuDoCommands(mo);
                        strcpy(returnParameters,(const char*)mo.command.c_str());
                    }
                }
                // Check any remaining Keys Hot Key Input.
                else if(mo.control_key[0] == toupper(m_sequence) && mo.control_key.size() < 2)
                {
                    ++m_commands_executed;
                    menuDoCommands(mo);
                    strcpy(returnParameters,(const char*)mo.command.c_str());
                }
            }

            // Executed == 0, Then Key Pressed was not valid!, and no stacked commands to loop :)
            if(m_commands_executed > 0)
            {
                return;
            }
        }
    } // </EscHit> After Else
}

/**
 * @brief Main Entry Point for a Menu Pass Input in
 * The Menu System will process the new Action
 * Then Return pass-through commands if appropriate
 */
void MenuFunction::menuProcess(char *returnParameters,
                               const std::string &inputSequence,
                               int area)
{
    std::cout << "DEBUG: menuProcess()" << std::endl;
    m_starting_position = area;
    strcpy(returnParameters, "");

    if(m_menu_info->menu_options.size() == 0)
    {
        // No Menu Commands Loaded, just return!
        menuReset();
        return;
    }

    // Startup the Lightbar System.
    menuLightBars(returnParameters, inputSequence);
}

/**
 * @brief Menu System - Run Menu Command
 * Either handle pass-through, or execute a menu change.
 */
void MenuFunction::menuDoCommands(MenuOption &option)
{
    std::cout << "DEBUG: menuDoCommands()" << std::endl;
    unsigned char c1 = option.command[0];
    unsigned char c2 = option.command[1];

    switch(c1)
    {
        // Message Reader Return right away
        case '!' :
            // These are Pass-through Menu Commands.
            break;

        case '-' :
            switch(c2)
            {
                case '^' : // Change Menu
                    m_curent_menu.clear();
                    m_curent_menu = option.method_string;
                    m_isLoadNewMenu = true;
                    m_choice = 0;
                    break;

                case '/' : // Gosub Menu - Remember current Menu, then changes to new
                    m_gosub_menu.clear();
                    m_gosub_menu = m_curent_menu;
                    m_curent_menu.clear();
                    m_curent_menu = option.method_string;
                    m_isLoadNewMenu = true;
                    m_choice = 0;
                    break;

                case '!' : // Return to Original Menu from GoSub Menu.
                    if(m_gosub_menu == "") break;

                    m_curent_menu.clear();
                    m_curent_menu = m_gosub_menu;
                    m_isLoadNewMenu = true;
                    m_choice = 0;
                    break;

                case 'T' : // Display a line of Text
                    m_menu_io.sequenceToAnsi(option.method_string);
                    break;

                default  : // None Found!
                    break;
            }

            break;

        // None Found!
        default  :
            break;
    }
}
