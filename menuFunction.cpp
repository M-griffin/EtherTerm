// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

# include "menuFunction.hpp"
# include "inputHandler.hpp"
# include "sequenceManager.hpp"
# include "linkList.hpp"
# include "sequenceParser.hpp"
# include "terminal.hpp"

# include <cstdio>
# include <cstdlib>
# include <ctime>
# include <cstring>   // gcc 4.3
# include <unistd.h>  // gcc 4.7

# include <string>   // string and wstring
# include <fstream>


/*
 * Menu System Class
 */
MenuFunction::MenuFunction() :
    m_numCommands(0),
    m_commandsExecuted(0),
    m_firstCommandsExecuted(0),
    m_startingPosition(0),
    m_xPosition(0),
    m_yPosition(0),
    m_numLightbarCommand(0),
    m_choice(0),
    m_cntEscCmds(0),
    m_sequence(0),
    m_secondSequence(0),
    m_isEscapeSequence(false),
    m_isSystemActive(true),
    m_menuRecord(),
    m_isLoadNewMenu(true)
{
    strcpy(MENU_PATH, "");
    strcpy(m_outBuff, "");
}

MenuFunction::~MenuFunction()
{
    std::vector<CommandRecord>().swap(m_commandRecord);
    std::vector<int>().swap(m_commandIndex);
    std::vector<int>().swap(m_commandIndexLightbar);
}

/*
 * Setup Menu Path
 */
std::string MenuFunction::setupMenuPath()
{
    std::string path = TheTerminal::Instance()->getProgramPath();
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
        m_menuRecord.MenuName = cfgdata;
        return false;
    }

    id1 = cfgdata.find("Directive ", 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        m_menuRecord.Directive = cfgdata;
        return false;
    }

    id1 = cfgdata.find("MenuPrompt ", 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        m_menuRecord.MenuPrompt = cfgdata;
        return false;
    }

    id1 = cfgdata.find("Lightbar ", 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        if(cfgdata == "TRUE")
            m_menuRecord.Lightbar = true;
        else
            m_menuRecord.Lightbar = false;
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
    while(!iFS.eof())
    {
        std::getline(iFS,cfgdata,'\n');
        if(iFS.eof())
            break;
        else if(menuParseData(cfgdata) == true)
            break;
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
        cmdRecord->LDesc = cfgdata;
        return;
    }

    sprintf(sText,"SDesc[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        cmdRecord->SDesc = cfgdata;
        return;
    }

    sprintf(sText,"CKeys[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        cmdRecord->CKeys = cfgdata;
        return;
    }

    sprintf(sText,"CmdKeys[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        cmdRecord->CmdKeys = cfgdata;
        return;
    }

    sprintf(sText,"MString[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        cmdRecord->MString = cfgdata;
        return;
    }

    sprintf(sText,"HiString[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        cmdRecord->HiString = cfgdata;
        return;
    }

    sprintf(sText,"LoString[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        cmdRecord->LoString = cfgdata;
        return;
    }

    sprintf(sText,"Xcoord[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        cmdRecord->Xcoord = atoi((char *)cfgdata.c_str());
        return;
    }

    sprintf(sText,"Ycoord[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        cmdRecord->Ycoord = atoi((char *)cfgdata.c_str());
        return;
    }

    sprintf(sText,"LBarCmd[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if(id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        if(cfgdata == "TRUE")
            cmdRecord->LBarCmd = true;
        else
            cmdRecord->LBarCmd = false;
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
    for(;;)
    {
        std::getline(iFS2,cfgdata,'\n');
        if(cfgdata.find(sText,0) != std::string::npos)
            ret = true;
        if(iFS2.eof()) break;
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
    m_commandRecord.push_back(cmdRecord);
    return true;
}

/**
 * Loads All Commands into Class.
 */
void MenuFunction::menuReadCommands()
{
    int idx = 0;
    while(commandsReadData(m_curentMenu, idx))
    {
        ++idx;
    }
}

/**
 * reset or Set Initial Defaults before a menu is read.
 */
void MenuFunction::menuReload()
{
    m_xPosition          = 1;   // Holds X Coord
    m_yPosition          = 1;   // Holds Y Coord
    m_numCommands        = 0;   // Number of Commands.
    m_numLightbarCommand = 0;   // Holds Light-bar # of choices
    m_cntEscCmds         = 0;   // Number of ESC Commands.

    m_sequence  = '\0';
    m_secondSequence = '\0';                 // Hold Input / Light-bar Key
    m_isEscapeSequence = false;              // Is Input key Escaped char, or Normal Key
    memset(&m_outBuff,0,sizeof(m_outBuff));  // Holds Formatted Light-bar Data
    m_output.erase();                        // Buffer for writing all light-bars at the same time
    m_commandsExecuted = 0;                  // Test's for hot keys commands executed, if non pass through loop
    // This is so scrolling isn't killed with invalid menu input!
    m_firstCommandsExecuted = 0;

    // Clear Out the Menu Record
    m_menuRecord.Directive.erase();
    m_menuRecord.Lightbar = false;
    m_menuRecord.MenuName.erase();
    m_menuRecord.MenuPrompt.erase();
}

/**
 * Check if Menu Exists
 *
 * Not used at this time
 *
int MenuFunction::menuExists()
{
    std::string path = setupMenuPath();
    path.append(m_curentMenu);
    path.append(".menu");

    FILE *fstr;
    fstr = fopen(path.c_str(),"rb+");
    if(!fstr)
    {
        std::cout << "Error: Menu not Found!: "
                  << m_curentMenu
                  << std::endl;
        return false;
    }
    else
    {
        fclose(fstr);
    }
    return true;
}
*/

/**
 * Function to Read in a Menu set in the class
 */
void MenuFunction::menuStart(std::string currentMenu)
{
    if(m_previousMenu == currentMenu)
    {
        m_isLoadNewMenu = false;
        return ;
    }

    // Setup the current Menu Passed.
    m_curentMenu = currentMenu;

    std::string path = setupMenuPath();
    path.append(m_curentMenu);
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
    if(!menuReadData(m_curentMenu))
    {
        std::cout << "Error: menuStart() - Unable to Read Current Menu "
                  << m_curentMenu
                  << std::endl;
        return;
    }
    m_numCommands = commandsCount(m_curentMenu);

    // Clear And Reload Menu Commands.
    std::vector<CommandRecord>().swap(m_commandRecord);
    menuReadCommands();
    m_isLoadNewMenu = false;

    if(m_numCommands == 0)
    {
        std::cout << "Error: No menu commands registered" << std::endl;
        return;
    }

    // Read Menu ANSI to String that will hold light-bars, Push to screen same-time
    // Will speed up display and make ghosting not appear as much
    if(m_menuRecord.Directive != "")
    {
        displayAnsiFile(m_menuRecord.Directive);
    }

    // First time load, Map Command Keys to HOTKEYS
    // If menu has changed, then reload commands.
    if(m_previousMenu != m_curentMenu)
    {
        // Reset Vector Lists.
        menuClearObjects();

        m_numLightbarCommand = 0;
        m_firstCommandsExecuted = 0;
        m_cntEscCmds = 0;
        for(int i = 0; i != m_numCommands; i++)
        {
            // If we find a FIRSTCMD, Execute it right away!
            if(m_commandRecord[i].CKeys == "FIRSTCMD")
            {
                menuDoCommands(&m_commandRecord[i]);
                ++m_firstCommandsExecuted;
            }

            // Get Light-bar Number of Commands
            // Registered as Light bars
            if(m_commandRecord[i].LBarCmd)
            {
                m_commandIndex.push_back(i);
                ++m_numLightbarCommand;
            }

            // Check for Function Key Commands.
            if(m_commandRecord[i].CKeys == "ESC")
            {
                m_commandIndexLightbar.push_back(i);
                ++m_cntEscCmds;
            }
            else if(m_commandRecord[i].CKeys == "LEFT")
            {
                m_commandIndexLightbar.push_back(i);
                ++m_cntEscCmds;
            }
            else if(m_commandRecord[i].CKeys == "RIGHT")
            {
                m_commandIndexLightbar.push_back(i);
                ++m_cntEscCmds;
            }
            else if(m_commandRecord[i].CKeys == "UP")
            {
                m_commandIndexLightbar.push_back(i);
                ++m_cntEscCmds;
            }
            else if(m_commandRecord[i].CKeys == "DOWN")
            {
                m_commandIndexLightbar.push_back(i);
                ++m_cntEscCmds;
            }

            else if(m_commandRecord[i].CKeys == "HOME")
            {
                m_commandIndexLightbar.push_back(i);
                ++m_cntEscCmds;
            }
            else if(m_commandRecord[i].CKeys == "END")
            {
                m_commandIndexLightbar.push_back(i);
                ++m_cntEscCmds;
            }
            else if(m_commandRecord[i].CKeys == "PAGEUP")
            {
                m_commandIndexLightbar.push_back(i);
                ++m_cntEscCmds;
            }
            else if(m_commandRecord[i].CKeys == "PAGEDN")
            {
                m_commandIndexLightbar.push_back(i);
                ++m_cntEscCmds;
            }
        }
        // Return if we executed all commands as FIRSTCMD!
        if(m_firstCommandsExecuted == m_numCommands)
        {
            return;
        }
        m_previousMenu = m_curentMenu;
    }
    m_output.clear();

    if(m_numLightbarCommand != 0)
    {
        // Throw out a menu prompt if there is one available. - Add MCI Parsing too!
        // Use Same X Row, these got reversed somehow!!! grrrr
        m_xPosition = 1;
        m_yPosition = m_commandRecord[m_commandIndex[0]].Ycoord;
    }

    // Light-bar menu then draw
    if(m_numLightbarCommand > 0)
    {
        // Setup of Remaining Light-bars in Low highlight Form
        for(int rep = 0; rep != m_numLightbarCommand; rep++)
        {
            if(rep != m_choice)
            {
                m_xPosition = m_commandRecord[m_commandIndex[rep]].Xcoord;
                m_yPosition = m_commandRecord[m_commandIndex[rep]].Ycoord;
                sprintf(m_outBuff,"\x1b[%i;%iH%s",m_yPosition,m_xPosition,
                        (char *)m_commandRecord[m_commandIndex[rep]].LoString.c_str());
                m_output += m_outBuff;
            }
            else
            {
                m_xPosition = m_commandRecord[m_commandIndex[rep]].Xcoord;
                m_yPosition = m_commandRecord[m_commandIndex[rep]].Ycoord;
                sprintf(m_outBuff,"\x1b[%i;%iH%s",m_yPosition,m_xPosition,
                        (char *)m_commandRecord[m_commandIndex[rep]].HiString.c_str());
                m_output += m_outBuff;
            }
        }
        // Moves cursor to end of line during light-bars.
        sprintf(m_outBuff,"\x1b[%i;79H",m_yPosition);
        m_output += m_outBuff;
        sequenceToAnsi(m_output);
    }
    // Not Light-bars, print out menu ANSI.
    else
    {
        sequenceToAnsi(m_output);
    }
    //Replace Messages Left...
    m_output.clear();
    m_output = m_menuRecord.MenuPrompt;

    // If light-bar prompt, reset to beginning of line.
    if(m_numLightbarCommand > 0)
    {
        m_xPosition = 1;
        sprintf(m_outBuff,"\x1b[%i;%iH%s",m_yPosition,m_xPosition,
                (char *)m_menuRecord.MenuPrompt.c_str());
        m_output = m_outBuff;
    }
    if(m_output.size() > 1)
    {
        sequenceToAnsi(m_output);
    }
}

/**
 * Clear Menu When Done.
 */
void MenuFunction::menuClearObjects()
{
    std::vector<int>().swap(m_commandIndex);
    std::vector<int>().swap(m_commandIndexLightbar);
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
    // If Global Exit, return right away.
    if(TheInputHandler::Instance()->isGlobalShutdown())
        return;

    //std::cout << "Menu Bars Input received: " << inputSequence << std::endl;
    m_sequence = inputSequence[0];

    // Check here for Arrow Key / Escaped Input was Received
    if((int)m_sequence == 27)
    {
        //std::cout << "ESC RECEIVED" << std::endl;
        if(inputSequence.size() > 1)
        {
            m_secondSequence = inputSequence[2];
            // Handle Hardware Input Sequences
            if(m_secondSequence == '0')
            {
                m_secondSequence = inputSequence[3];
            }
        }
        else
            m_secondSequence = '\0';
        m_isEscapeSequence = true;
    }
    else
    {
        m_isEscapeSequence = false;
    }
    m_output.clear();

    ////errlog2("menu_bars() 23 .0 ");
    m_commandsExecuted  = 0;
    if(m_isEscapeSequence)
    {
        // Input Key is Escaped Meaning Arrow Keys
        // std::cout << "ESC Hit: " << cc << " cntEscCmds:" << cntEscCmds << std::endl;
        // Run through and check for any Menu commands that overdrive default pass-through!
        if(m_cntEscCmds > 0)
        {
            // ESC Commands in Menu might override light-bars.
            for(int ckey = 0; ckey != (signed)m_cntEscCmds; ckey++)
            {
                if(m_secondSequence == '\0')  // Straight ESC Key
                {
                    if(m_commandRecord[m_commandIndexLightbar[ckey]].CKeys == "ESC")
                    {
                        ++m_commandsExecuted;
                        menuDoCommands(&m_commandRecord[m_commandIndexLightbar[ckey]]);
                        strcpy(returnParameters,(const char*)m_commandRecord[m_commandIndexLightbar[ckey]].CmdKeys.c_str());
                    }
                }
                else if(m_secondSequence == 'A')
                {
                    if(m_commandRecord[m_commandIndexLightbar[ckey]].CKeys == "UP")
                    {
                        ++m_commandsExecuted;
                        menuDoCommands(&m_commandRecord[m_commandIndexLightbar[ckey]]);
                        strcpy(returnParameters,(const char*)m_commandRecord[m_commandIndexLightbar[ckey]].CmdKeys.c_str());
                    }
                }
                else if(m_secondSequence == 'B')
                {
                    if(m_commandRecord[m_commandIndexLightbar[ckey]].CKeys == "DOWN")
                    {
                        ++m_commandsExecuted;
                        menuDoCommands(&m_commandRecord[m_commandIndexLightbar[ckey]]);
                        strcpy(returnParameters,(const char*)m_commandRecord[m_commandIndexLightbar[ckey]].CmdKeys.c_str());
                    }
                }
                else if(m_secondSequence == 'C')
                {
                    if(m_commandRecord[m_commandIndexLightbar[ckey]].CKeys == "RIGHT")
                    {
                        ++m_commandsExecuted;
                        menuDoCommands(&m_commandRecord[m_commandIndexLightbar[ckey]]);
                        strcpy(returnParameters,(const char*)m_commandRecord[m_commandIndexLightbar[ckey]].CmdKeys.c_str());
                    }
                }
                else if(m_secondSequence == 'D')
                {
                    if(m_commandRecord[m_commandIndexLightbar[ckey]].CKeys == "LEFT")
                    {
                        ++m_commandsExecuted;
                        menuDoCommands(&m_commandRecord[m_commandIndexLightbar[ckey]]);
                        strcpy(returnParameters,(const char*)m_commandRecord[m_commandIndexLightbar[ckey]].CmdKeys.c_str());
                    }
                }
                else if(m_secondSequence == 'V' ||
                        (m_secondSequence == '5' && inputSequence[3] == '~'))
                {
                    if(m_commandRecord[m_commandIndexLightbar[ckey]].CKeys == "PAGEUP")
                    {
                        ++m_commandsExecuted;
                        menuDoCommands(&m_commandRecord[m_commandIndexLightbar[ckey]]);
                        strcpy(returnParameters,(const char*)m_commandRecord[m_commandIndexLightbar[ckey]].CmdKeys.c_str());
                    }
                }
                else if(m_secondSequence == 'U' ||
                        (m_secondSequence == '6' && inputSequence[3] == '~'))
                {
                    if(m_commandRecord[m_commandIndexLightbar[ckey]].CKeys == "PAGEDN")
                    {
                        ++m_commandsExecuted;
                        menuDoCommands(&m_commandRecord[m_commandIndexLightbar[ckey]]);
                        strcpy(returnParameters,(const char*)m_commandRecord[m_commandIndexLightbar[ckey]].CmdKeys.c_str());
                    }
                }
            }
            // Executed == 0, Then Key Pressed was not valid! :)
            // Pass through the ESC then
            if(m_commandsExecuted == 0)
            {
                returnParameters[0] = m_secondSequence;
                // Translate to Terminal ESC Keys
                if(m_secondSequence == '0')
                    m_secondSequence = inputSequence[3];
            }
            else
            {
                //std::cout << "Executed, now pass-through back to interface the key pressed!" << std::endl;
                return;
            }
        }

        // If true, means we pass A and B up/dn through to system!
        //elog ("Start Light-bar Processing... CMD: %c, cntEscCmds: %ld", cc, cntEscCmds);
        if(m_numLightbarCommand > 0)
        {
            ////errlog2("menu_bars() 23.4 - iNoc > 0");
            if(m_secondSequence == 'D' && m_commandsExecuted == 0)
            {
                m_xPosition = m_commandRecord[m_commandIndex[m_choice]].Xcoord;
                m_yPosition = m_commandRecord[m_commandIndex[m_choice]].Ycoord;
                sprintf(m_outBuff,"\x1b[%i;%iH%s",m_yPosition,m_xPosition,
                        (char *)m_commandRecord[m_commandIndex[m_choice]].LoString.c_str());
                m_output += m_outBuff;
                if(m_choice == 0) m_choice = m_numLightbarCommand-1;
                else --m_choice;
                m_xPosition = m_commandRecord[m_commandIndex[m_choice]].Xcoord;
                m_yPosition = m_commandRecord[m_commandIndex[m_choice]].Ycoord;
                sprintf(m_outBuff,"\x1b[%i;%iH%s\x1b[%i;79H",m_yPosition,m_xPosition,
                        (char *)m_commandRecord[m_commandIndex[m_choice]].HiString.c_str(),m_yPosition);
                m_output += m_outBuff;
                sequenceToAnsi(m_output);
            }
            else if(m_secondSequence == 'C' && m_commandsExecuted == 0)
            {
                m_xPosition = m_commandRecord[m_commandIndex[m_choice]].Xcoord;
                m_yPosition = m_commandRecord[m_commandIndex[m_choice]].Ycoord;
                sprintf(m_outBuff,"\x1b[%i;%iH%s",m_yPosition,m_xPosition,
                        (char *)m_commandRecord[m_commandIndex[m_choice]].LoString.c_str());
                m_output += m_outBuff;
                if(m_choice == m_numLightbarCommand-1) m_choice = 0;
                else ++m_choice;
                m_xPosition = m_commandRecord[m_commandIndex[m_choice]].Xcoord;
                m_yPosition = m_commandRecord[m_commandIndex[m_choice]].Ycoord;
                sprintf(m_outBuff,"\x1b[%i;%iH%s\x1b[%i;79H",m_yPosition,m_xPosition,
                        (char *)m_commandRecord[m_commandIndex[m_choice]].HiString.c_str(),m_yPosition);
                m_output += m_outBuff;
                sequenceToAnsi(m_output);
            }

        } //</iNoc>
        else
        {
            //std::cout << "Normal Key Input!: " << (int)c << std::endl;
            // normal Key Input.
            if((int)m_sequence == 10)
            {
                m_commandsExecuted = 0;
                // Here Loop through and execute stacked Commands
                for(int ckey = 0; ckey != m_numCommands; ckey++)
                {
                    if(m_commandRecord[ckey].CKeys == "ENTER")
                    {
                        ++m_commandsExecuted;
                        menuDoCommands(&m_commandRecord[ckey]);
                        strcpy(returnParameters,(const char*)m_commandRecord[ckey].CmdKeys.c_str());
                    }
                    else if(m_commandRecord[ckey].CKeys == m_commandRecord[m_commandIndex[m_choice]].CKeys)
                    {
                        ++m_commandsExecuted;
                        menuDoCommands(&m_commandRecord[ckey]);
                        strcpy(returnParameters,(const char*)m_commandRecord[ckey].CmdKeys.c_str());
                    }
                }
                if(m_commandsExecuted > 0)
                {
                    return;
                }
            }
            //  Else go through and run stack commands on hot key pressed.
            else
            {
                m_commandsExecuted = 0;     // Normal Key Inputted, if Match's
                for(int ckey = 0; ckey != m_numCommands; ckey++)
                {
                    // Loop and Run Stacked Commands.
                    if(m_sequence == 32)
                    {
                        // Else check if it's a space-bar
                        if(m_commandRecord[ckey].CKeys == "SPACE")
                        {
                            ++m_commandsExecuted;
                            menuDoCommands(&m_commandRecord[ckey]);
                            strcpy(returnParameters,(const char*)m_commandRecord[ckey].CmdKeys.c_str());
                        }
                    }
                    // Check any remaining Keys Hot Key Input.
                    else if(m_commandRecord[ckey].CKeys[0] == toupper(m_sequence) &&
                            m_commandRecord[ckey].CKeys.size() < 2)
                    {
                        ++m_commandsExecuted;
                        menuDoCommands(&m_commandRecord[ckey]);
                        strcpy(returnParameters,(const char*)m_commandRecord[ckey].CmdKeys.c_str());
                    }
                }
            }
            // Executed == 0, Then Key Pressed was not valid!, and no stacked commands to loop :)
            if(m_commandsExecuted > 0)
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
        if((int)m_sequence == 13 && m_numLightbarCommand > 0)
        {
            // Only Execute [ENTER] on Light-bars!
            m_commandsExecuted = 0;
            // Here Loop through and execute stacked Commands
            for(int ckey = 0; ckey != m_numCommands; ckey++)
            {
                if(m_commandRecord[ckey].CKeys == m_commandRecord[m_commandIndex[m_choice]].CKeys)
                {
                    ++m_commandsExecuted;
                    menuDoCommands(&m_commandRecord[ckey]);
                    strcpy(returnParameters,(const char*)m_commandRecord[ckey].CmdKeys.c_str());
                }
            }
            // Executed Return Pass-through or Normal Key!
            if(m_commandsExecuted > 0)
            {
                return;
            }
        }
        else if((int)m_sequence == 13 && m_numLightbarCommand == 0)
        {
            // Check for ENTER CKEY!
            m_commandsExecuted = 0;
            // Here Loop through and execute stacked Commands
            for(int ckey = 0; ckey != m_numCommands; ckey++)
            {
                if(m_commandRecord[ckey].CKeys == "ENTER")
                {
                    ++m_commandsExecuted;
                    menuDoCommands(&m_commandRecord[ckey]);
                    strcpy(returnParameters,(const char*)m_commandRecord[ckey].CmdKeys.c_str());
                }
            }
            if(m_commandsExecuted > 0)
            {
                return;
            }
        }
        //  Else go through and run stack commands on hot key pressed.
        else
        {
            m_commandsExecuted = 0;      // Normal Key Inputted, if Match's
            for(int ckey = 0; ckey != m_numCommands; ckey++)
            {
                // Loop and Run Stacked Commands.
                if(m_sequence == 32)
                {
                    // Else check if it's a space-bar
                    if(m_commandRecord[ckey].CKeys == "SPACE")
                    {
                        ++m_commandsExecuted;
                        menuDoCommands(&m_commandRecord[ckey]);
                        strcpy(returnParameters,(const char*)m_commandRecord[ckey].CmdKeys.c_str());
                    }
                }
                // Check any remaining Keys Hot Key Input.
                else if(m_commandRecord[ckey].CKeys[0] == toupper(m_sequence) && m_commandRecord[ckey].CKeys.size() < 2)
                {
                    ++m_commandsExecuted;
                    menuDoCommands(&m_commandRecord[ckey]);
                    strcpy(returnParameters,(const char*)m_commandRecord[ckey].CmdKeys.c_str());
                }
            }
            // Executed == 0, Then Key Pressed was not valid!, and no stacked commands to loop :)
            if(m_commandsExecuted > 0)
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
    m_startingPosition = area;
    strcpy(returnParameters, "");

    if(m_numCommands == 0)
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
    unsigned char c1 = cmdr->CmdKeys[0];
    unsigned char c2 = cmdr->CmdKeys[1];

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
                    m_curentMenu.clear();
                    m_curentMenu = cmdr->MString;
                    m_isLoadNewMenu = true;
                    m_choice = 0;
                    break;

                case '/' : // Gosub Menu - Remember current Menu, then changes to new
                    m_gosubMenu.clear();
                    m_gosubMenu = m_curentMenu;
                    m_curentMenu.clear();
                    m_curentMenu = cmdr->MString;
                    m_isLoadNewMenu = true;
                    m_choice = 0;
                    break;

                case '!' : // Return to Original Menu from GoSub Menu.
                    if(m_gosubMenu == "") break;
                    m_curentMenu.clear();
                    m_curentMenu = m_gosubMenu;
                    m_isLoadNewMenu = true;
                    m_choice = 0;
                    break;

                case 'T' : // Display a line of Text
                    sequenceToAnsi(cmdr->MString);
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
