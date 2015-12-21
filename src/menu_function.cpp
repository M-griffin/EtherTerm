#include "menu_function.hpp"
#include "sequence_decoder.hpp"
#include "menu_io.hpp"


#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>   // gcc 4.3
#include <unistd.h>  // gcc 4.7

#include <string>   // string and wstring
#include <fstream>


MenuFunction::MenuFunction(sequence_decoder_ptr decoder, std::string program_path)
    : m_program_path(program_path)
    , m_sequence_decoder(decoder)
    , m_menu_io(decoder, program_path)
    , m_num_commands(0)
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
    , m_menu_record()
    , m_isLoadNewMenu(true)
{
    strcpy(m_menu_path, "");
    strcpy(m_output_buffer, "");
}

MenuFunction::~MenuFunction()
{
    std::vector<CommandRecord>().swap(m_command_record);
    std::vector<int>().swap(m_command_index);
    std::vector<int>().swap(m_command_index_lightbar);
}

/*
 * Setup Menu Path
 */
std::string MenuFunction::setupMenuPath()
{
    std::string path = m_program_path;
#ifdef _WIN32
    path += "assets\\";
#else
    path += "assets/";
#endif
    return path;
}
/*
 * Parse Helper for Menu's / Commands
 */
void MenuFunction::dataParseHelper(std::string &temp)
{
    std::string temp1;
    std::string::size_type st1 = 0;
    std::string::size_type st2 = 0;
    std::string::size_type  ct = 0;

    st1 = temp.find('"', 0);
    st2 = temp.find('"', st1+1);
    if(st1 != std::string::npos &&
            st2 != std::string::npos)
    {
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if(temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        temp = temp1;
    }
    else
        temp.erase();
}

/*
 * Parse Menu.TXT Files
 */
int MenuFunction::menuParseData(std::string cfgdata)
{
    std::string::size_type id1 = 0;

    // Discards any Configure lines with the # Character
    if(cfgdata[0] == '#') return false;

    id1 = cfgdata.find("MenuName ", 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        m_menu_record.menu_mame = cfgdata;
        return false;
    }

    id1 = cfgdata.find("Directive ", 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        m_menu_record.directive = cfgdata;
        return false;
    }

    id1 = cfgdata.find("menu_prompt ", 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        m_menu_record.menu_prompt = cfgdata;
        return false;
    }

    id1 = cfgdata.find("Lightbar ", 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        if(cfgdata == "TRUE")
            m_menu_record.is_lightbar = true;
        else
            m_menu_record.is_lightbar = false;
        return false;
    }
    return false;
}

/*
 * Loop for Reading reading a menu
 */
int MenuFunction::menuReadData(std::string MenuName)
{
    std::string path = setupMenuPath();
    path.append(MenuName);
    path.append(".menu");

    std::cout << "Read Menu: " << path << std::endl;
    std::ifstream iFS;
    iFS.open(path.c_str());
    if(!iFS.is_open())
    {
        std::cout << "Error: Unable to Read Menu: "
                  << path
                  << std::endl;
        return false;
    }

    std::string cfgdata;
    while(std::getline(iFS,cfgdata,'\n'))
    {
        menuParseData(cfgdata);
    }
    iFS.close();
    return true;
}

/*
 * Parse Commands Reads from a Menu File
 */
void MenuFunction::commandsParse(std::string cfgdata,
                                 int idx,
                                 CommandRecord *cmdRecord)
{
    std::string::size_type id1 = 0;
    char sText[200] = {0};

    // Discards any Configure lines with the # Character
    if(cfgdata[0] == '#')
        return;

    sprintf(sText,"LDesc[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        cmdRecord->description = cfgdata;
        return;
    }

    sprintf(sText,"SDesc[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        cmdRecord->short_desc = cfgdata;
        return;
    }

    sprintf(sText,"CKeys[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        cmdRecord->control_key = cfgdata;
        return;
    }

    sprintf(sText,"CmdKeys[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        cmdRecord->command = cfgdata;
        return;
    }

    sprintf(sText,"MString[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        cmdRecord->method_string = cfgdata;
        return;
    }

    sprintf(sText,"HiString[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        cmdRecord->active_string = cfgdata;
        return;
    }

    sprintf(sText,"LoString[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        cmdRecord->inactive_string = cfgdata;
        return;
    }

    sprintf(sText,"Xcoord[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        cmdRecord->x_coord = atoi((char *)cfgdata.c_str());
        return;
    }

    sprintf(sText,"Ycoord[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        cmdRecord->y_coord = atoi((char *)cfgdata.c_str());
        return;
    }

    sprintf(sText,"LBarCmd[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        if(cfgdata == "TRUE")
            cmdRecord->is_lightbar = true;
        else
            cmdRecord->is_lightbar = false;
        return;
    }
}

/*
 * Check if Command Exists (Used for Counting Commands)
 */
int MenuFunction::commandsExist(std::string MenuName, int idx)
{
    //std::cout << "MenuFunction::cmdexist" << std::endl;
    std::string path = setupMenuPath();
    path.append(MenuName);
    path.append(".menu");
    int ret = false;
    char sText[200]= {0};

    // Open file for reading and parsing.
    std::ifstream iFS2;
    iFS2.open(path.c_str());
    if(!iFS2.is_open())
    {
        //errlog((char *)"Couldn't Open Menu Command [cmdexist()]: %s\n", path.c_str());
        return ret;
    }

    // Loop Through and Find the The Command
    sprintf(sText,"[CommandRec%.03d]",idx);
    std::string cfgdata;
    while(std::getline(iFS2,cfgdata,'\n'))
    {
        if(cfgdata.find(sText,0) != std::string::npos)
            ret = true;
    }
    iFS2.close();
    return ret;
}

/*
 * Right now this loops through the menu too many times! haha
 * Make it parse once for the max command rec found, save cpu! :)
 * Files are small, so no rush! :)
 */
int MenuFunction::commandsCount(std::string MenuName)
{
    int  cnt = 0;
    while(commandsExist(MenuName,cnt))
    {
        ++cnt;
    }
    return cnt;
}

/**
 * Loop to Read commands in a Menu File.
 */
int MenuFunction::commandsReadData(std::string MenuName, int idx)
{
    std::string path = setupMenuPath();
    path.append(MenuName);
    path.append(".menu");

    int ret = 0;
    ret = commandsExist(MenuName,idx);
    if(ret < 1) return false;

    // Else Read and Parse it
    std::ifstream iFS;
    iFS.open(path.c_str());
    if(!iFS.is_open())
    {
        //errlog((char *)"Couldn't Open Menu Commands: %s\n", path.c_str());
        return false;
    }

    CommandRecord cmdRecord;
    std::string cfgdata;
    for(;;)
    {
        std::getline(iFS, cfgdata, '\n');
        commandsParse(cfgdata, idx, &cmdRecord);
        if(iFS.eof()) break;
    }
    iFS.close();

    // Add to List of Commands.
    m_command_record.push_back(cmdRecord);
    return true;
}

/**
 * Loads All Commands into Class.
 */
void MenuFunction::menuReadCommands()
{
    int idx = 0;
    while(commandsReadData(m_curent_menu, idx))
    {
        ++idx;
    }
}

/**
 * reset or Set Initial Defaults before a menu is read.
 */
void MenuFunction::menuReload()
{
    m_x_position          = 1;   // Holds X Coord
    m_y_position          = 1;   // Holds Y Coord
    m_num_commands        = 0;   // Number of Commands.
    m_num_lightbar_commands = 0;   // Holds Light-bar # of choices
    m_num_esc_commands         = 0;   // Number of ESC Commands.

    m_sequence  = '\0';
    m_second_sequence = '\0';                 // Hold Input / Light-bar Key
    m_is_escape_sequence = false;              // Is Input key Escaped char, or Normal Key
    memset(&m_output_buffer,0,sizeof(m_output_buffer));  // Holds Formatted Light-bar Data
    m_output.erase();                        // Buffer for writing all light-bars at the same time
    m_commands_executed = 0;                  // Test's for hot keys commands executed, if non pass through loop
    // This is so scrolling isn't killed with invalid menu input!
    m_first_commands_executed = 0;

    // Clear Out the Menu Record
    m_menu_record.directive.erase();
    m_menu_record.is_lightbar = false;
    m_menu_record.menu_mame.erase();
    m_menu_record.menu_prompt.erase();
}

/**
 * Function to Read in a Menu set in the class
 */
void MenuFunction::menuStart(std::string currentMenu)
{
    if(m_previous_menu == currentMenu)
    {
        m_isLoadNewMenu = false;
        return ;
    }

    // Setup the current Menu Passed.
    m_curent_menu = currentMenu;

    std::string path = setupMenuPath();
    path.append(m_curent_menu);
    path.append(".menu");

    FILE *fstr;
    fstr = fopen(path.c_str(),"rb+");
    if(!fstr)
    {
        m_isLoadNewMenu = false;
        return;
    }
    else
    {
        fclose(fstr);
    }

    // Setup the Default Setting and And Clear Allocations.
    menuReload(); // Loading new Menu, Clear!
    if(!menuReadData(m_curent_menu))
    {
        std::cout << "Error: menuStart() - Unable to Read Current Menu "
                  << m_curent_menu
                  << std::endl;
        return;
    }
    m_num_commands = commandsCount(m_curent_menu);

    // Clear And Reload Menu Commands.
    std::vector<CommandRecord>().swap(m_command_record);
    menuReadCommands();
    m_isLoadNewMenu = false;

    if(m_num_commands == 0)
    {
        std::cout << "Error: No menu commands registered" << std::endl;
        return;
    }

    // Read Menu ANSI to String that will hold light-bars, Push to screen same-time
    // Will speed up display and make ghosting not appear as much
    if(m_menu_record.directive != "")
    {
        m_menu_io.displayAnsiFile(m_menu_record.directive);
    }

    // First time load, Map Command Keys to HOTKEYS
    // If menu has changed, then reload commands.
    if(m_previous_menu != m_curent_menu)
    {
        // Reset Vector Lists.
        menuClearObjects();

        m_num_lightbar_commands = 0;
        m_first_commands_executed = 0;
        m_num_esc_commands = 0;
        for(int i = 0; i != m_num_commands; i++)
        {
            // If we find a FIRSTCMD, Execute it right away!
            if(m_command_record[i].control_key == "FIRSTCMD")
            {
                menuDoCommands(&m_command_record[i]);
                ++m_first_commands_executed;
            }

            // Get Light-bar Number of Commands
            // Registered as Light bars
            if(m_command_record[i].is_lightbar)
            {
                m_command_index.push_back(i);
                ++m_num_lightbar_commands;
            }

            // Check for Function Key Commands.
            if(m_command_record[i].control_key == "ESC")
            {
                m_command_index_lightbar.push_back(i);
                ++m_num_esc_commands;
            }
            else if(m_command_record[i].control_key == "LEFT")
            {
                m_command_index_lightbar.push_back(i);
                ++m_num_esc_commands;
            }
            else if(m_command_record[i].control_key == "RIGHT")
            {
                m_command_index_lightbar.push_back(i);
                ++m_num_esc_commands;
            }
            else if(m_command_record[i].control_key == "UP")
            {
                m_command_index_lightbar.push_back(i);
                ++m_num_esc_commands;
            }
            else if(m_command_record[i].control_key == "DOWN")
            {
                m_command_index_lightbar.push_back(i);
                ++m_num_esc_commands;
            }

            else if(m_command_record[i].control_key == "HOME")
            {
                m_command_index_lightbar.push_back(i);
                ++m_num_esc_commands;
            }
            else if(m_command_record[i].control_key == "END")
            {
                m_command_index_lightbar.push_back(i);
                ++m_num_esc_commands;
            }
            else if(m_command_record[i].control_key == "PAGEUP")
            {
                m_command_index_lightbar.push_back(i);
                ++m_num_esc_commands;
            }
            else if(m_command_record[i].control_key == "PAGEDN")
            {
                m_command_index_lightbar.push_back(i);
                ++m_num_esc_commands;
            }
        }
        // Return if we executed all commands as FIRSTCMD!
        if(m_first_commands_executed == m_num_commands)
        {
            return;
        }
        m_previous_menu = m_curent_menu;
    }
    m_output.clear();

    if(m_num_lightbar_commands != 0)
    {
        // Throw out a menu prompt if there is one available. - Add MCI Parsing too!
        // Use Same X Row, these got reversed somehow!!! grrrr
        m_x_position = 1;
        m_y_position = m_command_record[m_command_index[0]].y_coord;
    }

    // Light-bar menu then draw
    if(m_num_lightbar_commands > 0)
    {
        // Setup of Remaining Light-bars in Low highlight Form
        for(int rep = 0; rep != m_num_lightbar_commands; rep++)
        {
            if(rep != m_choice)
            {
                m_x_position = m_command_record[m_command_index[rep]].x_coord;
                m_y_position = m_command_record[m_command_index[rep]].y_coord;
                sprintf(m_output_buffer,"\x1b[%i;%iH%s",m_y_position,m_x_position,
                        (char *)m_command_record[m_command_index[rep]].inactive_string.c_str());
                m_output += m_output_buffer;
            }
            else
            {
                m_x_position = m_command_record[m_command_index[rep]].x_coord;
                m_y_position = m_command_record[m_command_index[rep]].y_coord;
                sprintf(m_output_buffer,"\x1b[%i;%iH%s",m_y_position,m_x_position,
                        (char *)m_command_record[m_command_index[rep]].active_string.c_str());
                m_output += m_output_buffer;
            }
        }
        // Moves cursor to end of line during light-bars.
        sprintf(m_output_buffer,"\x1b[%i;79H",m_y_position);
        m_output += m_output_buffer;
        m_menu_io.sequenceToAnsi(m_output);
    }
    // Not Light-bars, print out menu ANSI.
    else
    {
        m_menu_io.sequenceToAnsi(m_output);
    }
    //Replace Messages Left...
    m_output.clear();
    m_output = m_menu_record.menu_prompt;

    // If light-bar prompt, reset to beginning of line.
    if(m_num_lightbar_commands > 0)
    {
        m_x_position = 1;
        sprintf(m_output_buffer,"\x1b[%i;%iH%s",m_y_position,m_x_position,
                (char *)m_menu_record.menu_prompt.c_str());
        m_output = m_output_buffer;
    }
    if(m_output.size() > 1)
    {
        m_menu_io.sequenceToAnsi(m_output);
    }
}

/**
 * Clear Menu When Done.
 */
void MenuFunction::menuClearObjects()
{
    std::vector<int>().swap(m_command_index);
    std::vector<int>().swap(m_command_index_lightbar);
}

/**
 * Used for Light-bars and Hokey Menu's
 * This is also used in Full Screen Interfaces
 * for Light-bars selections and Return
 * Pass-though commands from Menu Light-bar Prompts.
 */
void MenuFunction::menuLightBars(char *returnParameters,
                                 const std::string &inputSequence)
{
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

    ////errlog2("menu_bars() 23 .0 ");
    m_commands_executed  = 0;
    if(m_is_escape_sequence)
    {
        // Input Key is Escaped Meaning Arrow Keys
        // std::cout << "ESC Hit: " << cc << " cntEscCmds:" << cntEscCmds << std::endl;
        // Run through and check for any Menu commands that overdrive default pass-through!
        if(m_num_esc_commands > 0)
        {
            // ESC Commands in Menu might override light-bars.
            for(int ckey = 0; ckey != (signed)m_num_esc_commands; ckey++)
            {
                if(m_second_sequence == '\0')  // Straight ESC Key
                {
                    if(m_command_record[m_command_index_lightbar[ckey]].control_key == "ESC")
                    {
                        ++m_commands_executed;
                        menuDoCommands(&m_command_record[m_command_index_lightbar[ckey]]);
                        strcpy(returnParameters,(const char*)m_command_record[m_command_index_lightbar[ckey]].command.c_str());
                    }
                }
                else if(m_second_sequence == 'A')
                {
                    if(m_command_record[m_command_index_lightbar[ckey]].control_key == "UP")
                    {
                        ++m_commands_executed;
                        menuDoCommands(&m_command_record[m_command_index_lightbar[ckey]]);
                        strcpy(returnParameters,(const char*)m_command_record[m_command_index_lightbar[ckey]].command.c_str());
                    }
                }
                else if(m_second_sequence == 'B')
                {
                    if(m_command_record[m_command_index_lightbar[ckey]].control_key == "DOWN")
                    {
                        ++m_commands_executed;
                        menuDoCommands(&m_command_record[m_command_index_lightbar[ckey]]);
                        strcpy(returnParameters,(const char*)m_command_record[m_command_index_lightbar[ckey]].command.c_str());
                    }
                }
                else if(m_second_sequence == 'C')
                {
                    if(m_command_record[m_command_index_lightbar[ckey]].control_key == "RIGHT")
                    {
                        ++m_commands_executed;
                        menuDoCommands(&m_command_record[m_command_index_lightbar[ckey]]);
                        strcpy(returnParameters,(const char*)m_command_record[m_command_index_lightbar[ckey]].command.c_str());
                    }
                }
                else if(m_second_sequence == 'D')
                {
                    if(m_command_record[m_command_index_lightbar[ckey]].control_key == "LEFT")
                    {
                        ++m_commands_executed;
                        menuDoCommands(&m_command_record[m_command_index_lightbar[ckey]]);
                        strcpy(returnParameters,(const char*)m_command_record[m_command_index_lightbar[ckey]].command.c_str());
                    }
                }
                else if(m_second_sequence == 'V' ||
                        (m_second_sequence == '5' && inputSequence[3] == '~'))
                {
                    if(m_command_record[m_command_index_lightbar[ckey]].control_key == "PAGEUP")
                    {
                        ++m_commands_executed;
                        menuDoCommands(&m_command_record[m_command_index_lightbar[ckey]]);
                        strcpy(returnParameters,(const char*)m_command_record[m_command_index_lightbar[ckey]].command.c_str());
                    }
                }
                else if(m_second_sequence == 'U' ||
                        (m_second_sequence == '6' && inputSequence[3] == '~'))
                {
                    if(m_command_record[m_command_index_lightbar[ckey]].control_key == "PAGEDN")
                    {
                        ++m_commands_executed;
                        menuDoCommands(&m_command_record[m_command_index_lightbar[ckey]]);
                        strcpy(returnParameters,(const char*)m_command_record[m_command_index_lightbar[ckey]].command.c_str());
                    }
                }
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
            ////errlog2("menu_bars() 23.4 - iNoc > 0");
            if(m_second_sequence == 'D' && m_commands_executed == 0)
            {
                m_x_position = m_command_record[m_command_index[m_choice]].x_coord;
                m_y_position = m_command_record[m_command_index[m_choice]].y_coord;
                sprintf(m_output_buffer,"\x1b[%i;%iH%s",m_y_position,m_x_position,
                        (char *)m_command_record[m_command_index[m_choice]].inactive_string.c_str());
                m_output += m_output_buffer;
                if(m_choice == 0) m_choice = m_num_lightbar_commands-1;
                else --m_choice;
                m_x_position = m_command_record[m_command_index[m_choice]].x_coord;
                m_y_position = m_command_record[m_command_index[m_choice]].y_coord;
                sprintf(m_output_buffer,"\x1b[%i;%iH%s\x1b[%i;79H",m_y_position,m_x_position,
                        (char *)m_command_record[m_command_index[m_choice]].active_string.c_str(),m_y_position);
                m_output += m_output_buffer;
                m_menu_io.sequenceToAnsi(m_output);
            }
            else if(m_second_sequence == 'C' && m_commands_executed == 0)
            {
                m_x_position = m_command_record[m_command_index[m_choice]].x_coord;
                m_y_position = m_command_record[m_command_index[m_choice]].y_coord;
                sprintf(m_output_buffer,"\x1b[%i;%iH%s",m_y_position,m_x_position,
                        (char *)m_command_record[m_command_index[m_choice]].inactive_string.c_str());
                m_output += m_output_buffer;
                if(m_choice == m_num_lightbar_commands-1) m_choice = 0;
                else ++m_choice;
                m_x_position = m_command_record[m_command_index[m_choice]].x_coord;
                m_y_position = m_command_record[m_command_index[m_choice]].y_coord;
                sprintf(m_output_buffer,"\x1b[%i;%iH%s\x1b[%i;79H",m_y_position,m_x_position,
                        (char *)m_command_record[m_command_index[m_choice]].active_string.c_str(),m_y_position);
                m_output += m_output_buffer;
                m_menu_io.sequenceToAnsi(m_output);
            }

        } //</iNoc>
        else
        {
            //std::cout << "Normal Key Input!: " << (int)c << std::endl;
            // normal Key Input.
            if((int)m_sequence == 10)
            {
                m_commands_executed = 0;
                // Here Loop through and execute stacked Commands
                for(int ckey = 0; ckey != m_num_commands; ckey++)
                {
                    if(m_command_record[ckey].control_key == "ENTER")
                    {
                        ++m_commands_executed;
                        menuDoCommands(&m_command_record[ckey]);
                        strcpy(returnParameters,(const char*)m_command_record[ckey].command.c_str());
                    }
                    else if(m_command_record[ckey].control_key == m_command_record[m_command_index[m_choice]].control_key)
                    {
                        ++m_commands_executed;
                        menuDoCommands(&m_command_record[ckey]);
                        strcpy(returnParameters,(const char*)m_command_record[ckey].command.c_str());
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
                for(int ckey = 0; ckey != m_num_commands; ckey++)
                {
                    // Loop and Run Stacked Commands.
                    if(m_sequence == 32)
                    {
                        // Else check if it's a space-bar
                        if(m_command_record[ckey].control_key == "SPACE")
                        {
                            ++m_commands_executed;
                            menuDoCommands(&m_command_record[ckey]);
                            strcpy(returnParameters,(const char*)m_command_record[ckey].command.c_str());
                        }
                    }
                    // Check any remaining Keys Hot Key Input.
                    else if(m_command_record[ckey].control_key[0] == toupper(m_sequence) &&
                            m_command_record[ckey].control_key.size() < 2)
                    {
                        ++m_commands_executed;
                        menuDoCommands(&m_command_record[ckey]);
                        strcpy(returnParameters,(const char*)m_command_record[ckey].command.c_str());
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
        // If Enter, Return Cmd # of Light-bar Executed
        // also catch any stacked keys and execute in order!
        if((int)m_sequence == 13 && m_num_lightbar_commands > 0)
        {
            // Only Execute [ENTER] on Light-bars!
            m_commands_executed = 0;
            // Here Loop through and execute stacked Commands
            for(int ckey = 0; ckey != m_num_commands; ckey++)
            {
                if(m_command_record[ckey].control_key == m_command_record[m_command_index[m_choice]].control_key)
                {
                    ++m_commands_executed;
                    menuDoCommands(&m_command_record[ckey]);
                    strcpy(returnParameters,(const char*)m_command_record[ckey].command.c_str());
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
            for(int ckey = 0; ckey != m_num_commands; ckey++)
            {
                if(m_command_record[ckey].control_key == "ENTER")
                {
                    ++m_commands_executed;
                    menuDoCommands(&m_command_record[ckey]);
                    strcpy(returnParameters,(const char*)m_command_record[ckey].command.c_str());
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
            for(int ckey = 0; ckey != m_num_commands; ckey++)
            {
                // Loop and Run Stacked Commands.
                if(m_sequence == 32)
                {
                    // Else check if it's a space-bar
                    if(m_command_record[ckey].control_key == "SPACE")
                    {
                        ++m_commands_executed;
                        menuDoCommands(&m_command_record[ckey]);
                        strcpy(returnParameters,(const char*)m_command_record[ckey].command.c_str());
                    }
                }
                // Check any remaining Keys Hot Key Input.
                else if(m_command_record[ckey].control_key[0] == toupper(m_sequence) && m_command_record[ckey].control_key.size() < 2)
                {
                    ++m_commands_executed;
                    menuDoCommands(&m_command_record[ckey]);
                    strcpy(returnParameters,(const char*)m_command_record[ckey].command.c_str());
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
 * Main Entry Point for a Menu Pass Input in
 * The Menu System will process the new Action
 * Then Return pass-through commands if appropriate
 */
void MenuFunction::menuProcess(char *returnParameters,
                               const std::string &inputSequence,
                               int area)
{
    m_starting_position = area;
    strcpy(returnParameters, "");

    if(m_num_commands == 0)
    {
        // No Menu Commands Loaded, just return!
        menuReload();
        return;
    }
    // Startup the Lightbar System.
    menuLightBars(returnParameters, inputSequence);
}

/**
 * Menu System - Run Menu Command
 * Either handle pass-through, or execute a menu change.
 */
void MenuFunction::menuDoCommands(CommandRecord *cmdr)
{
    unsigned char c1 = cmdr->command[0];
    unsigned char c2 = cmdr->command[1];

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
                    m_curent_menu = cmdr->method_string;
                    m_isLoadNewMenu = true;
                    m_choice = 0;
                    break;

                case '/' : // Gosub Menu - Remember current Menu, then changes to new
                    m_gosub_menu.clear();
                    m_gosub_menu = m_curent_menu;
                    m_curent_menu.clear();
                    m_curent_menu = cmdr->method_string;
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
                    m_menu_io.sequenceToAnsi(cmdr->method_string);
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
