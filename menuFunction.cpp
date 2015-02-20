// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

# include "menuFunction.h"
# include "inputHandler.h"
# include "sequenceParser.h"
# include "linkList.h"
# include "ansiParser.h"
# include "terminal.h"

# include <cstdio>
# include <cstdlib>
# include <ctime>
# include <cstring>   // gcc 4.3
# include <unistd.h>  // gcc 4.7

# include <string>   // string and wstring
# include <fstream>

using namespace std;

/*
 * Menu System Class
 */
MenuFunction::MenuFunction()
{
    isLoadNewMenu = false;
    menuRecord    = 0;
    commandRecord = 0;
    commandIndex  = 0;
    commandIndexLightbar = 0;
    isSystemActive = true;
    choice         = 0;
    // For Lightbar Starting Position
    // Now uses menu system for lightbars.
    currentSystem = 0;

}

MenuFunction::~MenuFunction()
{
    if (menuRecord    != 0)        delete    menuRecord;
    if (commandRecord != 0)        delete [] commandRecord;
    if (commandIndex  != 0)        delete [] commandIndex;
    if (commandIndexLightbar != 0) delete [] commandIndexLightbar;
    menuRecord    = 0;
    commandRecord = 0;
    commandIndex  = 0;
    commandIndexLightbar = 0;
    choice        = 0;
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
    if (st1 != std::string::npos &&
        st2 != std::string::npos)
    {
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
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

    // Disgards any Config lines with the # Character
    if (cfgdata[0] == '#') return false;

    id1 = cfgdata.find("MenuName ", 0);
    if (id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        menuRecord->MenuName = cfgdata;
        return false;
    }

    id1 = cfgdata.find("Directive ", 0);
    if (id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        menuRecord->Directive = cfgdata;
        return false;
    }

    id1 = cfgdata.find("MenuPrompt ", 0);
    if (id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        menuRecord->MenuPrompt = cfgdata;
        return false;
    }

    id1 = cfgdata.find("Lightbar ", 0);
    if (id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        if (cfgdata == "TRUE") menuRecord->Lightbar = true;
        else menuRecord->Lightbar = false;
        return false;
    }
    return false;
}

/*
 * Loop for Reading reading a menu
 */
int MenuFunction::menuReadData(std::string MenuName)
{
    std::string path = TheTerminal::Instance()->getProgramPath();
#ifdef _WIN32
    path += "assets\\";
#else
    path += "assets/";
#endif
    path.append(MenuName);
    path.append(".menu");

    std::cout << "Read Menu: " << path << std::endl;

    ifstream iFS;
    iFS.open( path.c_str() );
    if (!iFS.is_open())
    {
        return false;
    }

    std::string cfgdata;
    for (;;)
    {
        if(iFS.eof()) break;
        std::getline(iFS,cfgdata,'\n');
        if(iFS.eof()) break;
        if (menuParseData(cfgdata) == true) break;
    }
    iFS.close();
    return true;
}

/*
 * Parse Commands Reads from a Menu File
 */
void MenuFunction::commandsParse(std::string cfgdata, int idx)
{
    std::string::size_type id1 = 0;
    char sText[200] = {0};

    // Disgards any Config lines with the # Character
    if (cfgdata[0] == '#') return ;

    sprintf(sText,"LDesc[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        commandRecord[idx].LDesc = cfgdata;
        return;
    }

    sprintf(sText,"SDesc[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        commandRecord[idx].SDesc = cfgdata;
        return;
    }

    sprintf(sText,"CKeys[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        commandRecord[idx].CKeys = cfgdata;
        return;
    }

    sprintf(sText,"CmdKeys[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        commandRecord[idx].CmdKeys = cfgdata;
        return;
    }

    sprintf(sText,"MString[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        commandRecord[idx].MString = cfgdata;
        return;
    }

    sprintf(sText,"HiString[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        commandRecord[idx].HiString = cfgdata;
        return;
    }

    sprintf(sText,"LoString[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        commandRecord[idx].LoString = cfgdata;
        return;
    }

    sprintf(sText,"Xcoord[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        commandRecord[idx].Xcoord = atoi((char *)cfgdata.c_str());
        return;
    }

    sprintf(sText,"Ycoord[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        commandRecord[idx].Ycoord = atoi((char *)cfgdata.c_str());
        return;
    }

    sprintf(sText,"LBarCmd[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        dataParseHelper(cfgdata);
        if (cfgdata == "TRUE")
            commandRecord[idx].LBarCmd = true;
        else
            commandRecord[idx].LBarCmd = false;
        return;
    }
}

/*
 * Check if Command Exists (Used for Counting Commands)
 */
int MenuFunction::commandsExist(std::string MenuName, int idx)
{
    //std::cout << "MenuFunction::cmdexist" << std::endl;
    std::string path = TheTerminal::Instance()->getProgramPath();
#ifdef _WIN32
    path += "assets\\";
#else
    path += "assets/";
#endif

    path.append(MenuName);
    path.append(".menu");
    int ret = false;
    char sText[200]= {0};

    // Open file for reading and parsing.
    ifstream iFS2;
    iFS2.open( path.c_str() );
    if (!iFS2.is_open())
    {
        //errlog((char *)"Couldn't Open Menu Command [cmdexist()]: %s\n", path.c_str());
        return ret;
    }

    // Loop Through and Find the The Command
    sprintf(sText,"[CommandRec%.03d]",idx);
    std::string cfgdata;
    for (;;)
    {
        std::getline(iFS2,cfgdata,'\n');
        if (cfgdata.find(sText,0) != std::string::npos)
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
    while (commandsExist(MenuName,cnt))
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
    std::string path = TheTerminal::Instance()->getProgramPath();
#ifdef _WIN32
    path += "assets\\";
#else
    path += "assets/";
#endif
    path.append(MenuName);
    path.append(".menu");

    int ret;
    ret = commandsExist(MenuName,idx);
    if (ret < 1) return false;

    // Else Read and Parse it
    ifstream iFS;
    iFS.open( path.c_str() );
    if (!iFS.is_open())
    {
        //errlog((char *)"Couldn't Open Menu Commands: %s\n", path.c_str());
        return false;
    }

    std::string cfgdata;
    for (;;)
    {
        std::getline(iFS,cfgdata,'\n');
        commandsParse(cfgdata,idx);
        if(iFS.eof()) break;
    }
    iFS.close();
    return true;
}

/**
 * Loads All Commands into Class.
 */
void MenuFunction::menuReadCommands()
{
    int  idx = 0;
    while (commandsReadData(_curmenu,idx))
    {
        ++idx;
    }
}

/**
 * reset or Set Inital Defaults before a menu is read.
 */
void MenuFunction::menuReload()
{
    xPosition      = 1;                // Holds X Coord
    yPosition      = 1;                // Holds Y Coord
    numCommands    = 0;                // Number of Commands.
    numLightbarCommand = 0;            // Holds Lightbar # of choices
    cntEscCmds     = 0;                // Number of ESC Commands.

    sequence  = '\0';
    secondSequence = '\0';               // Hold Input / Lightbar Key
    isEscapeSequence = false;            // Is Input key Escaped char, or Normal Key
    memset(&outBuff,0,sizeof(outBuff));  // Holds Formatted Lightbar Data
    output.erase();                      // Buffer for writing all lightbars at the same time

    commandsExecuted = 0;                // Test's for hot keys commands excuted, if non pass through loop
    // This is so scrolling isn't killed with invalid menu input!
    firstCommandsExecuted = 0;
}

/**
 * Check if Menu Exists
 */
int MenuFunction::menuExists()
{
    std::string path = TheTerminal::Instance()->getProgramPath();
#ifdef _WIN32
    path += "assets\\";
#else
    path += "assets/";
#endif
    path.append(_curmenu);
    path.append(".menu");

    FILE *fstr;
    fstr = fopen(path.c_str(),"rb+");
    if(fstr == nullptr)
    {
        std::cout << "Menu not Found!:  " << _curmenu << std::endl;
        return false;
    }
    else
    {
        fclose(fstr);
    }
    return true;
}

/**
 * Function to Read in a Menu set in the class
 */
void MenuFunction::menuStart()
{
    ////errlog2("readin() 1 %s,%s",(char *)_premenu.c_str(),(char *)_curmenu.c_str());
    if (_premenu == _curmenu || _curmenu == "")
    {
        //std::cout << "MenuFunction::menu_readin:  _loadnew = false " << std::endl;
        isLoadNewMenu = false;
        return ;
    }

    int idx  = 0;
    int nogc = 0;

    std::string path = TheTerminal::Instance()->getProgramPath();
#ifdef _WIN32
    path += "assets\\";
#else
    path += "assets/";
#endif
    path.append(_curmenu);
    path.append(".menu");

    FILE *fstr;
    fstr = fopen(path.c_str(),"rb+");
    if(fstr == nullptr)
    {
        isLoadNewMenu = false;
        return;
    }
    else
    {
        fclose(fstr);
    }

    // Setup the Default Setting and And Clear Allocations.
    menuReload(); // Loading new Menu, Clear!
    if (menuRecord != 0)
    {
        delete menuRecord;
        menuRecord = 0;
    }
    menuRecord = new MenuRecord;
    if (!menuRecord)
    {
        return;
    }

    if(!menuReadData(_curmenu))
        return;

    nogc = 0;
    numCommands = commandsCount(_curmenu);
    if (commandRecord != 0)
    {
        delete [] commandRecord;
        commandRecord = 0;
    }
    commandRecord = new CommandRecord[numCommands+nogc+1];
    if (!commandRecord)
    {
        return;
    }

    menuReadCommands();
    if (nogc > 0)
    {
        idx = 0;
        while(commandsReadData((char *)"global",idx))
        {
            ++numCommands;
            ++idx;
        }
    }
    isLoadNewMenu = false;
}

/**
 * Clear Menu When Done.
 */
void MenuFunction::menuClearObjects()
{
    // Clear
    ////errlog2(" *** menu_clearbars()");
    if (commandIndex)         delete [] commandIndex;
    if (commandIndexLightbar) delete [] commandIndexLightbar;
    commandIndex         = 0;
    commandIndexLightbar = 0;
}

/**
 * Used for Lightbars and Hotkey Menu's
 * This is also used in Full Screen Interfaces
 * for Lightbars slections and Return
 * PassThough commands from Menu Lightbar Prompts.
 */
void MenuFunction::menuLightBars(char *inPut)
{
    // Handle Input Strings
    std::string inputSequence;

    std::vector<list_bar> lightbar;

    // If no Menu Commands Return! / Add fallback menu lateron!!
    if (numCommands == 0) return;

    // Read Menu Ansi to String that will hold lightbars, Push to screen sametime
    // Will speed up display and make ghosting not apear as much

    bool menufile = false;
    if (menuRecord->Directive != "")
    {
        displayAnsiFile((char *)menuRecord->Directive.c_str());
        menufile = true;
    }

    // First time load, Map CommandKeys to HOTKEYS
    // If menu has changed, then reload commands.
    if (_premenu != _curmenu)
    {
        if (commandIndex != 0)
        {
            delete [] commandIndex;
            commandIndex = 0;
        }
        commandIndex = new short[numCommands+1];  // Set to # of comamnd for now, preload.
        if (!commandIndex)
        {
            return;
        }
        if (commandIndexLightbar != 0)
        {
            delete [] commandIndexLightbar;
            commandIndexLightbar = 0;
        }
        commandIndexLightbar = new short[numCommands+1];
        if (!commandIndexLightbar)
        {
            return;
        }

        numLightbarCommand = 0;
        firstCommandsExecuted = 0;
        cntEscCmds = 0;
        for (int i = 0; i != numCommands; i++)
        {
            // If we find a FIRSTCMD, Execute it right away!
            if (commandRecord[i].CKeys == "FIRSTCMD")
            {
                ////errlog2("menu_bars() 6.2 - FIRSTCMD");
                menuDoCommands(&commandRecord[i]);
                // Count /Commands, if we execute all FIRSTCMDS then return when completed.
                ++firstCommandsExecuted;
            }
            // Get Ligthbar Commands
            if (commandRecord[i].LBarCmd) //&& isANSI == true)
            {
                ////errlog2("menu_bars() 6.3 - Lightbar");
                commandIndex[numLightbarCommand] = i;
                ++numLightbarCommand;
            }
            //while running throguh each command, put into a vector formatted.
            // No ansi menu file, generate menu commands.
            if (!menufile && numLightbarCommand == 0)
            {
                //errlog((char *)"menu_bars() 6.4 - Setting Menu Command Layout");
                // Insert code here for generating generic menu with commands.
            }
            ////errlog2("menu_bars() 10");
            // Count Escape Keys in Menu System to override passthrough
            if (commandRecord[i].CKeys == "ESC")
            {
                commandIndexLightbar[cntEscCmds] = i;
                ++cntEscCmds;
            }
            else if (commandRecord[i].CKeys == "LEFT")
            {
                commandIndexLightbar[cntEscCmds] = i;
                ++cntEscCmds;
            }
            else if (commandRecord[i].CKeys == "RIGHT")
            {
                commandIndexLightbar[cntEscCmds] = i;
                ++cntEscCmds;
            }
            else if (commandRecord[i].CKeys == "UP")
            {
                commandIndexLightbar[cntEscCmds] = i;
                ++cntEscCmds;
            }
            else if (commandRecord[i].CKeys == "DOWN")
            {
                commandIndexLightbar[cntEscCmds] = i;
                ++cntEscCmds;
            }

            else if (commandRecord[i].CKeys == "HOME")
            {
                commandIndexLightbar[cntEscCmds] = i;
                ++cntEscCmds;
            }
            else if (commandRecord[i].CKeys == "END")
            {
                commandIndexLightbar[cntEscCmds] = i;
                ++cntEscCmds;
            }

            else if (commandRecord[i].CKeys == "PAGEUP")
            {
                commandIndexLightbar[cntEscCmds] = i;
                ++cntEscCmds;
            }
            else if (commandRecord[i].CKeys == "PAGEDN")
            {
                commandIndexLightbar[cntEscCmds] = i;
                ++cntEscCmds;
            }
        }
        // Return if we executed all comamnds as FRISTCMD!
        if (firstCommandsExecuted == numCommands)
        {
            return;
        }
        _premenu = _curmenu;
    }
    output.clear();

    if (numLightbarCommand != 0)
    {
        // Throw out a menu prompt if there is one available. - Add MCI Parsing too!
        // Use Same X Row, these got reversed somehow!!! grrrr
        xPosition = 1;
        yPosition = commandRecord[commandIndex[0]].Ycoord;
    }

    std::string sMenu = _curmenu;
    while (!TheInputHandler::Instance()->isGlobalShutdown())
    {
        // Lightbar menu then draw
        if (numLightbarCommand > 0)
        {
            //std::cout << "lightbars" << std::endl;
            // Setup of Remaining Lightbars in Low highlight Form
            for (int rep = 0; rep != numLightbarCommand; rep++)
            {
                if (rep != choice)
                {
                    xPosition = commandRecord[commandIndex[rep]].Xcoord;
                    yPosition = commandRecord[commandIndex[rep]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s",yPosition,xPosition,(char *)commandRecord[commandIndex[rep]].LoString.c_str());
                    output += outBuff;
                }
                else
                {
                    xPosition = commandRecord[commandIndex[rep]].Xcoord;
                    yPosition = commandRecord[commandIndex[rep]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s",yPosition,xPosition,(char *)commandRecord[commandIndex[rep]].HiString.c_str());
                    output += outBuff;
                }
            }
            // Moves cursor to end of line during lightbars.
            sprintf(outBuff,"\x1b[%i;79H",yPosition);
            output += outBuff;
            sequenceToAnsi((char*)output.c_str(),0);
        }
        // Not Lightbars, print out menu ansi.
        else
        {
            sequenceToAnsi((char*)output.c_str(),0);
        }
        //Replcae Messages Left...
        output.clear();
        output = menuRecord->MenuPrompt;

        // If lightbar prompt, reset to begining of line.
        if (numLightbarCommand > 0)
        {
            xPosition = 1;
            sprintf(outBuff,"\x1b[%i;%iH%s",yPosition,xPosition,(char *)menuRecord->MenuPrompt.c_str());
            output = outBuff;
        }
        if (output.size() > 1)
        {
            sequenceToAnsi((char *)output.c_str(),0);
        }

        // Loop and get menu command input, arrow keys etc for movement.
        TheInputHandler::Instance()->reset();
        inputSequence.erase();

        //std::cout << "Menu_Bars Input Loop" << std::endl;
        while(!TheInputHandler::Instance()->isGlobalShutdown())
        {
            if(TheInputHandler::Instance()->update())
            {
                inputSequence = TheInputHandler::Instance()->getInputSequence();
                break;
            }
            SDL_Delay(20);
        }

        // If Global Exit, return right away.
        if (TheInputHandler::Instance()->isGlobalShutdown())
            return;

        //std::cout << "Menu_Bars Input received: " << inputSequence << std::endl;
        sequence = inputSequence[0];

       // Testing Screen Buffer.
       //TheAnsiParser::Instance()->clearScreenBuffer();
       //TheAnsiParser::Instance()->getScreenBufferText();

        // Check here for Arrow Key / Escaped Input was Received
        if ((int)sequence == 27)
        {
            //std::cout << "ESC RECEIVED" << std::endl;
            if (inputSequence.size() > 1)
            {
                secondSequence = inputSequence[2];
                // Handle Hardware Input Sequences
                if (secondSequence == '0')
                {
                    secondSequence = inputSequence[3];
                }
            }
            else
                secondSequence = '\0';
            isEscapeSequence = true;
        }
        else
        {
            isEscapeSequence = false;
        }
        output.clear();

        ////errlog2("menu_bars() 23 .0 ");
        commandsExecuted  = 0;
        if (isEscapeSequence)
        {
            // Input Key is Escaped Meaning Arrow Keys
            //std::cout << "ESC Hit: " << cc << " cntEscCmds:" << cntEscCmds << std::endl;
            // Run through and check for any Menu commands that overive default passthrough!
            if (cntEscCmds > 0)
            {
                // ESC Commands in Menu might overide lightbars.
                for (int ckey = 0; ckey != (signed)cntEscCmds; ckey++)
                {
                    if (secondSequence == '\0') // Stright ESC Key
                    {
                        if (commandRecord[commandIndexLightbar[ckey]].CKeys == "ESC")
                        {
                            ++commandsExecuted;
                            menuDoCommands(&commandRecord[commandIndexLightbar[ckey]]);
                            strcpy(inPut,(const char*)commandRecord[commandIndexLightbar[ckey]].CmdKeys.c_str());
                        }
                    }
                    else if (secondSequence == 'A')
                    {
                        if (commandRecord[commandIndexLightbar[ckey]].CKeys == "UP")
                        {
                            ++commandsExecuted;
                            menuDoCommands(&commandRecord[commandIndexLightbar[ckey]]);
                            strcpy(inPut,(const char*)commandRecord[commandIndexLightbar[ckey]].CmdKeys.c_str());
                        }
                    }
                    else if (secondSequence == 'B')
                    {
                        if (commandRecord[commandIndexLightbar[ckey]].CKeys == "DOWN")
                        {
                            ++commandsExecuted;
                            menuDoCommands(&commandRecord[commandIndexLightbar[ckey]]);
                            strcpy(inPut,(const char*)commandRecord[commandIndexLightbar[ckey]].CmdKeys.c_str());
                        }
                    }
                    else if (secondSequence == 'C')
                    {
                        if (commandRecord[commandIndexLightbar[ckey]].CKeys == "RIGHT")
                        {
                            ++commandsExecuted;
                            menuDoCommands(&commandRecord[commandIndexLightbar[ckey]]);
                            strcpy(inPut,(const char*)commandRecord[commandIndexLightbar[ckey]].CmdKeys.c_str());
                        }
                    }
                    else if (secondSequence == 'D')
                    {
                        if (commandRecord[commandIndexLightbar[ckey]].CKeys == "LEFT")
                        {
                            ++commandsExecuted;
                            menuDoCommands(&commandRecord[commandIndexLightbar[ckey]]);
                            strcpy(inPut,(const char*)commandRecord[commandIndexLightbar[ckey]].CmdKeys.c_str());
                        }
                    }
                    else if (secondSequence == 'V' || (secondSequence == '5' && inputSequence[3] == '~'))
                    {
                        if (commandRecord[commandIndexLightbar[ckey]].CKeys == "PAGEUP")
                        {
                            ++commandsExecuted;
                            menuDoCommands(&commandRecord[commandIndexLightbar[ckey]]);
                            strcpy(inPut,(const char*)commandRecord[commandIndexLightbar[ckey]].CmdKeys.c_str());
                        }
                    }
                    else if (secondSequence == 'U' || (secondSequence == '6' && inputSequence[3] == '~'))
                    {
                        if (commandRecord[commandIndexLightbar[ckey]].CKeys == "PAGEDN")
                        {
                            ++commandsExecuted;
                            menuDoCommands(&commandRecord[commandIndexLightbar[ckey]]);
                            strcpy(inPut,(const char*)commandRecord[commandIndexLightbar[ckey]].CmdKeys.c_str());
                        }
                    }
                }
                // Executed == 0, Then Key Pressed was not valid! :)
                // Pass through the ESC then
                if (commandsExecuted == 0)
                {
                    inPut[0] = secondSequence;
                    // Translater to Terminal ESC Keys
                    if (secondSequence == '0')
                        secondSequence = inputSequence[3];
                }
                else
                {
                    //std::cout << "Executed, now passthrough back to interface the key pressed!" << std::endl;
                    return;
                }
            }

            // If true, means we pass A and B up/dn through to system!
            //elog ("Start Lightbar Processing... CMD: %c, cntEscCmds: %ld", cc, cntEscCmds);
            if (numLightbarCommand > 0)
            {
                ////errlog2("menu_bars() 23.4 - iNoc > 0");
                if (secondSequence == 'D' && commandsExecuted == 0)
                {
                    xPosition = commandRecord[commandIndex[choice]].Xcoord;
                    yPosition = commandRecord[commandIndex[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s",yPosition,xPosition,
                        (char *)commandRecord[commandIndex[choice]].LoString.c_str());
                    output += outBuff;
                    if (choice == 0) choice = numLightbarCommand-1;
                    else --choice;
                    xPosition = commandRecord[commandIndex[choice]].Xcoord;
                    yPosition = commandRecord[commandIndex[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s\x1b[%i;79H",yPosition,xPosition,
                        (char *)commandRecord[commandIndex[choice]].HiString.c_str(),yPosition);
                    output += outBuff;
                    sequenceToAnsi((char*)output.c_str(),0);
                }
                else if (secondSequence == 'C' && commandsExecuted == 0)
                {
                    xPosition = commandRecord[commandIndex[choice]].Xcoord;
                    yPosition = commandRecord[commandIndex[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s",yPosition,xPosition,
                        (char *)commandRecord[commandIndex[choice]].LoString.c_str());
                    output += outBuff;
                    if (choice == numLightbarCommand-1) choice = 0;
                    else ++choice;
                    xPosition = commandRecord[commandIndex[choice]].Xcoord;
                    yPosition = commandRecord[commandIndex[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s\x1b[%i;79H",yPosition,xPosition,
                        (char *)commandRecord[commandIndex[choice]].HiString.c_str(),yPosition);
                    output += outBuff;
                    sequenceToAnsi((char*)output.c_str(),0);
                }

            } //</iNoc>
            else
            {
                //std::cout << "Normal Key Input!: " << (int)c << std::endl;
                // normal Key Input.
                if ((int)sequence == 10)
                {
                    commandsExecuted = 0;
                    // Here Loop through and execute stacked Commands
                    for (int ckey = 0; ckey != numCommands; ckey++)
                    {
                        if ( commandRecord[ckey].CKeys == "ENTER" )
                        {
                            ++commandsExecuted;
                            menuDoCommands(&commandRecord[ckey]);
                            strcpy(inPut,(const char*)commandRecord[ckey].CmdKeys.c_str());
                        }
                        else if ( commandRecord[ckey].CKeys == commandRecord[commandIndex[choice]].CKeys )
                        {
                            ++commandsExecuted;
                            menuDoCommands(&commandRecord[ckey]);
                            strcpy(inPut,(const char*)commandRecord[ckey].CmdKeys.c_str());
                        }
                    }
                    if (commandsExecuted > 0)
                    {
                        return;
                    }
                    else
                    {
                        // Redraw screen!
                        if (menuRecord->Directive != "")
                        {
                            displayAnsiFile((char *)menuRecord->Directive.c_str());
                        }
                    }
                }
                //  Else go through and run stack commands on hot key pressed.
                else
                {
                    commandsExecuted = 0;     // Normal Key Inputed, if Match's
                    std::string t1;
                    for (int ckey = 0; ckey != numCommands; ckey++)
                    {
                        // Loop and Run Stacked Commands.
                        if (sequence == 32)
                        {
                            // Else check if it's a spacebar
                            if (commandRecord[ckey].CKeys == "SPACE")
                            {
                                ++commandsExecuted;
                                menuDoCommands(&commandRecord[ckey]);
                                strcpy(inPut,(const char*)commandRecord[ckey].CmdKeys.c_str());
                            }
                        }
                        else // Check any remaining Keys Hot Key Input.
                            if (commandRecord[ckey].CKeys[0] == toupper(sequence) &&
                                commandRecord[ckey].CKeys.size() < 2)
                            {
                                ++commandsExecuted;
                                menuDoCommands(&commandRecord[ckey]);
                                strcpy(inPut,(const char*)commandRecord[ckey].CmdKeys.c_str());
                            }
                    }
                }
                // Executed == 0, Then Key Pressed was not valid!, and no stacked commands to loop :)
                if (commandsExecuted > 0)
                {
                    return;
                }
            } // </</iNoc>
        } // </EscHit>
        // Normal Key Input
        else
        {
            // If Enter, Return Cmd # of Lightbar Executed
            // also catch any stacked keys and execute in order!
            if ((int)sequence == 13 && numLightbarCommand > 0)
            {
                // Only Executte [ENTER] on Lightbars!
                commandsExecuted = 0;
                // Here Loop through and execute stacked Commands
                for (int ckey = 0; ckey != numCommands; ckey++)
                {
                    if (commandRecord[ckey].CKeys == commandRecord[commandIndex[choice]].CKeys)
                    {
                        ++commandsExecuted;
                        menuDoCommands(&commandRecord[ckey]);
                        strcpy(inPut,(const char*)commandRecord[ckey].CmdKeys.c_str());
                    }
                }
                if (commandsExecuted > 0)
                {
                    ////errlog2("Excuted Return Passthrough or Normal Key!");
                    return;
                }
                else
                {
                    // Redraw screen!
                    if (menuRecord->Directive != "")
                    {
                        displayAnsiFile((char *)menuRecord->Directive.c_str());
                    }
                }
            }
            else if ((int)sequence == 13 && numLightbarCommand == 0)
            {
                // Check for ENTER CKEY!
                commandsExecuted = 0;
                // Here Loop through and execute stacked Commands
                for (int ckey = 0; ckey != numCommands; ckey++)
                {
                    if (commandRecord[ckey].CKeys == "ENTER")
                    {
                        ++commandsExecuted;
                        menuDoCommands(&commandRecord[ckey]);
                        strcpy(inPut,(const char*)commandRecord[ckey].CmdKeys.c_str());
                    }
                }
                if (commandsExecuted > 0)
                {
                    return;
                }
                else
                {
                    // Redraw screen!
                    if (menuRecord->Directive != "")
                    {
                        displayAnsiFile((char *)menuRecord->Directive.c_str());
                    }
                }
            }
            //  Else go through and run stack commands on hot key pressed.
            else
            {
                commandsExecuted = 0;      // Normal Key Inputed, if Match's
                std::string t1;
                for (int ckey = 0; ckey != numCommands; ckey++)
                {
                    // Loop and Run Stacked Commands.
                    if (sequence == 32)
                    {
                        // Else check if it's a spacebar
                        if (commandRecord[ckey].CKeys == "SPACE")
                        {
                            ++commandsExecuted;
                            menuDoCommands(&commandRecord[ckey]);
                            strcpy(inPut,(const char*)commandRecord[ckey].CmdKeys.c_str());
                        }
                    }
                    else // Check any remaining Keys Hot Key Input.
                        if (commandRecord[ckey].CKeys[0] == toupper(sequence) && commandRecord[ckey].CKeys.size() < 2)
                        {
                            ++commandsExecuted;
                            menuDoCommands(&commandRecord[ckey]);
                            strcpy(inPut,(const char*)commandRecord[ckey].CmdKeys.c_str());
                        }
                }
                // Executed == 0, Then Key Pressed was not valid!, and no stacked commands to loop :)
                if (commandsExecuted > 0)
                {
                    return;
                }
            }
        } // </EscHit> After Else
        ////errlog2("menu_bars() 24.5 - While NEXT!");
    } // </while (1)>
}

/**
 * Main Entry Point for a Menu and Startup.
 */
void MenuFunction::menuProcess(char *mString, uint32_t area)
{
    currentSystem = area;
    strcpy(mString,"");
    if (numCommands == 0)
    {
        menuReload();
        return;
    }
    menuLightBars(mString);
}

/**
 * Menu System - Run Menu Command
 */
void MenuFunction::menuDoCommands(CommandRecord *cmdr)
{
    unsigned char c1 = cmdr->CmdKeys[0];
    unsigned char c2 = cmdr->CmdKeys[1];

    std::string parse;

    //std::cout << "Do MenuCmd: CmdKey[0]: " << (char)c1 << " CmdKey[1]: " << (char)c2 << std::endl;
    std::string inputSequence;

    switch (c1)
    {
        // Message Reader Return right away
        case '!' :
            // These are Passthrough Menu Commands.
            break;

        case '-' :
            switch (c2)
            {
                case '^' : // Change Menu
                    //strcpy(_curmenu,mString);
                    _curmenu.clear();
                    _curmenu = cmdr->MString;
                    isLoadNewMenu = true;
                    choice = 0;
                    break;

                case '/' : // Gosub Menu - Remember current Menu, then changes to new
                    _gosub.clear();
                    _gosub = _curmenu;
                    _curmenu.clear();
                    _curmenu = cmdr->MString;
                    isLoadNewMenu = true;
                    choice = 0;
                    break;

                case '!' : // Return to Original Menu from GoSub Menu.
                    if (_gosub == "") break;
                    _curmenu.clear();
                    _curmenu = _gosub;
                    isLoadNewMenu = true;
                    choice = 0;
                    break;

                case 'T' : // Display a line of Text
                    sequenceToAnsi((char *)cmdr->MString.c_str(),0);
                    //cmdr->MString.clear();
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

/**
 * Right String Padding
 */
void MenuFunction::rightSpacing(char *str, int space)   // Pad Right
{
    if (space == 0) return;

    std::string Line = str;
    int s = Line.size();
    // if Line > Sapce, Erase to Make it match!
    if (s >= space)
    {
        Line.erase(space,(s-space));
        strcpy(str,Line.c_str());
        return;
    }
    Line = "";
    for(int i = 0; i < (space-s); i++)
        Line += ' ';
    strcat(str,Line.c_str());
}

/**
 * Left String Padding
 */
void MenuFunction::leftSpacing(char *str, int space)   // Pad Left
{
    if (space == 0) return;

    std::string Line = "";
    int s = strlen(str);
    // if Line > Sapce, Erase to Make it match!
    if (s > space)
    {
        Line.erase(space,(s-space));
        strcpy(str,Line.c_str());
        return;
    }
    for(int i = 0; i < (space-s); i++)
        Line += ' ';
    Line += str;
    strcpy(str,Line.c_str());
}

/**
 * Mask Input
 */
void MenuFunction::maskInput(char *str)
{
    std::string Line = "";
    int s = strlen(str);

    for(int i = 0; i < s; i++)
        Line += '*';
    strcpy(str,Line.c_str());
}

/**
 * Sets up input field with background color
 *
 */
void MenuFunction::inputField(char *text, int &len)
{
    std::string repeat;
    std::string temp;
    char sTmp[3]  = {0};
    char sTmp2[3] = {0};
    // Parse for Input String Modifiers
    temp = text;
    int tLen, id1;
    char INPUT_COLOR[255]={0};
    bool inc = false; //found input color

    // Overide Input Length for Ansi
    id1 = temp.find("|IN",0);
    if (id1 != -1)
    {
        sTmp[0] = temp[id1+3];
        sTmp[1] = temp[id1+4];
        temp.erase(id1,5);
        tLen = atoi( sTmp );
        if (tLen < len) len = tLen; // Set new Length
    }

    // Overide Foreground/Background Input Field Colors
    id1 = -1;
    id1 = temp.find("|FB",0);
    if (id1 != -1)
    {
        memset(&sTmp,0,3);
        sTmp[0] = temp[id1+3]; // Foreground 00-15
        sTmp[1] = temp[id1+4];
        memset(&sTmp2,0,3);
        sTmp2[0] = temp[id1+5]; // Background 16-23
        sTmp2[1] = temp[id1+6];
        temp.erase(id1,7);
        sprintf(INPUT_COLOR,"|%s|%s",sTmp,sTmp2);
        inc = true;
    }

    for(int i = 0; i != len; i++)
    {
        repeat += " ";
    }

    if (!inc)
        sprintf(INPUT_COLOR,"|00|19");
    sprintf(text,"%s|07|16[%s%s|07|16]%s\x1b[%iD",
        (char *)temp.c_str(),INPUT_COLOR,repeat.c_str(),INPUT_COLOR,len+1);
}

/*
 * Get Single Key Input.
 */
int MenuFunction::getKey()
{
    std::string inputSequence;
    while (!TheInputHandler::Instance()->isGlobalShutdown())
    {
        if(TheInputHandler::Instance()->update())
        {            
            inputSequence = TheInputHandler::Instance()->getInputSequence();
            break;
        }
        else
        {            
            SDL_Delay(10);
        }
    }
    // If Global Exit, return right away.
    if (TheInputHandler::Instance()->isGlobalShutdown())
    {
        return EOF;
    }    
    return inputSequence[0];
}

/*
 * Get Input up to <ENTER>
 */
void MenuFunction::getLine(char *line,   // Returns Input into Line
                         int   length,   // Max Input Length of String
                         char *leadoff,  // Data to Display in Default String {Optional}
                         int   hid,      // If input is Echomail as hidden    {Optional}
                         char *chlist)   // Valid Input Char List             {Optional}
{
    int sequence, secondSequence;
    int i   = 0;
    int Col = 0;
    int isEscapeSequence;
    std::string tLine;
    std::string output;
    char sLine[200]  = {0};
    char sList[200]  = {0};

    std::string input;
    std::string inputSequence;

    // Cursor Blinking.
    int  cursorBlink = 0;
    bool startBlinking = false;
    time_t ttime, ttime2;

    ttime = SDL_GetTicks();

    #define DEL 0x7f

    // If were starting Off Input with a String already in buffer!  display it!
    if (leadoff != 0)
    {
        input = leadoff;
        i = input.size();
        Col = i;
        TheSequenceParser::Instance()->processSequence(input);
    }

    // This is a list of Valid inPut Keys
    // We will only grab chars in this list
    // Only if a list is passed to the Function.
    if (chlist != 0)
    {
        // Append ESC, CTRL Y, BACKSPACE, ENTER
        // For Line Movement and End of Line Keys so they are always valid input.
        sprintf(sLine,"%c%c%c%c%c%c%c",(char)27,(char)25,(char)0x7f,(char)0x08,(char)10,char(8),char(207));
        //strcat(chlist,sLine);
        sprintf(sList,"%s%s",chlist,sLine);
    }

    while (!TheInputHandler::Instance()->isGlobalShutdown())
    {
        if(TheInputHandler::Instance()->update() && !TheInputHandler::Instance()->isGlobalShutdown())
        {
            // We got data, turn off the cursor!
            ttime = SDL_GetTicks();
            startBlinking = false;
            cursorBlink = 0;
            // Get the Sequence.
            inputSequence = TheInputHandler::Instance()->getInputSequence();

            // Check for Abort, single ESC character.
            if (inputSequence == "\x1b" && inputSequence.size() == 1)
            {
                isEscapeSequence = false;
                strcpy(line,"\x1b");
                return;
            }
        }
        else
        {
            if (TheAnsiParser::Instance()->isCursorActive() && !TheInputHandler::Instance()->isGlobalShutdown())
            {
                startBlinking = true;
                // Setup Timer for Blinking Cursor
                // Initial State = On, then Switch to off in next loop.
                if(cursorBlink % 2 == 0)
                {
                    ttime2 = SDL_GetTicks();
                    if(startBlinking && (ttime2 - ttime) > 400)
                    {
                        TheTerminal::Instance()->renderCursorOffScreen();
                        TheTerminal::Instance()->drawTextureScreen();
                        --cursorBlink;
                        ttime = SDL_GetTicks();
                    }
                }
                else
                {
                    ttime2 = SDL_GetTicks();
                    if(startBlinking && (ttime2 - ttime) > 400)
                    {
                        TheTerminal::Instance()->renderCursorOnScreen();
                        TheTerminal::Instance()->drawTextureScreen();
                        ++cursorBlink;
                        ttime = SDL_GetTicks();
                    }
                }
            }

            SDL_Delay(10);
            continue;
        }

        // Catch any shutdown here before doing anymore.
        if (TheInputHandler::Instance()->isGlobalShutdown())
            return;

        sequence = inputSequence[0];

        if (sequence == '\r') sequence = '\n';

        // Valid Key List Checking....
        if (chlist != 0)
        {
            if(sequence >= 'a' && sequence <= 'z')
            {
                sequence = toupper(sequence);
            }
            if(strchr(sList,sequence) || sequence == '\n' ||
                sequence == '\r')
            {
                //continue;
            }
            else
            {
                continue;
            }
        }
        // Escape in this case, ignore, later add movement in string
        if ((int)sequence == 27)
        {
            if (inputSequence.size() >= 3)
                secondSequence = inputSequence[2];
            isEscapeSequence = true;
        }
        else
        {
            isEscapeSequence = false;                        
        }

        // Catch all Escaped Keys for Cursor Movement
        if (isEscapeSequence)
        {
            switch (secondSequence)
            {
                case '3' : // Delete
                    if (i != 0 || Col != 0)
                    {
                        sequenceToAnsi((char *)"\x1b[D \x1b[D");
                        input.erase(Col-1,1);
                        --i;
                        --Col;
                    }
                    break;

                default :
                    break;
            }    
        }
        else if ((int)sequence == 25)
        {
            // CTRL Y - Clear Line
            input.erase();
            i = Col;
            for (; i != 0; i--)
            {
                sequenceToAnsi((char *)"\x1b[D \x1b[D");
            }
            i = 0;
            Col = i;
        }

        // delete 127
        // Do destructive backspace
        // on VT100 Terms 127 DEL == BS!
        // Since we have no DELETE in this, delete on 1 liens will works like BS.
        else if ((int)sequence == 0x08 || (int)sequence == 127 || int(sequence) == 8 || int(sequence) == 207 || (int)sequence == 0x7f)
        {
            if (i != 0 || Col != 0)
            {
                sequenceToAnsi((char *)"\x1b[D \x1b[D");
                input.erase(Col-1,1);
                --i;
                --Col;
            }
        }
        // Normal Key Input, Letters & numbers
        else if((int)sequence > 31 && (int)sequence < 126)
        {
            if (i != length-1)
            {
                if (hid)
                {
                    sequenceToAnsi((char *)"*");
                }
                else
                {
                    output = sequence;
                    sequenceToAnsi((char *)output.c_str());
                }
                input += sequence;
                ++i;
                ++Col;
            }
        }
        else if (sequence == '\r' || sequence == '\n')
        {
            //input += '\0';
            strncpy(line,(char *)input.c_str(),length);
            break;
        }
    }

    // If Global Exit, return right away.
    if (TheInputHandler::Instance()->isGlobalShutdown())
    {
        return;
    }
}


/**
 * Set ANSI Foreground colors
 */
void MenuFunction::ansiForegroundColor(char *data, int fg)
{
    switch (fg)
    {
        case 0:
            strcat(data, "x[0;30m");
            break;
        case 1:
            strcat(data, "x[0;34m");
            break;
        case 2:
            strcat(data, "x[0;32m");
            break;
        case 3:
            strcat(data, "x[0;36m");
            break;
        case 4:
            strcat(data, "x[0;31m");
            break;
        case 5:
            strcat(data, "x[0;35m");
            break;
        case 6:
            strcat(data, "x[0;33m");
            break;
        case 7:
            strcat(data, "x[0;37m");
            break;
        case 8:
            strcat(data, "x[1;30m");
            break;
        case 9:
            strcat(data, "x[1;34m");
            break;
        case 10:
            strcat(data, "x[1;32m");
            break;
        case 11:
            strcat(data, "x[1;36m");
            break;
        case 12:
            strcat(data, "x[1;31m");
            break;
        case 13:
            strcat(data, "x[1;35m");
            break;
        case 14:
            strcat(data, "x[1;33m");
            break;
        case 15:
            strcat(data, "x[1;37m");
            break;
        default :
            break;
    }
    data[0] = '\x1b';
}

/**
 * Set ANSI Background colors
 */
void MenuFunction::ansiBackgroundColor(char *data, int bg)
{
    switch (bg)
    {
        case 16:
            strcat(data, "x[40m");
            break;
        case 17:
            strcat(data, "x[44m");
            break;
        case 18:
            strcat(data, "x[42m");
            break;
        case 19:
            strcat(data, "x[46m");
            break;
        case 20:
            strcat(data, "x[41m");
            break;
        case 21:
            strcat(data, "x[45m");
            break;
        case 22:
            strcat(data, "x[43m");
            break;
        case 23:
            strcat(data, "x[47m");
            break;
            // Default to none.
        case 24:
            strcat(data, "x[0m");
            break;
        default :
            break;
    }
    data[0] = '\x1b';
}

/**
 * Main Output for All String Data, Parses PIPE color and MCI Codes.
 *
 */
void MenuFunction::sequenceToAnsi(char* szString, int buffer)
{
    std::string::size_type  id1 = 0;  // Pipe Position
    char szTmp[3];         // Holds 1, 2nd digit of Pipe
    char szPos1[3];        // Hold XY Pos for Ansi Postion Codes
    char szPos2[3];        // Hold XY Pos for Ansi Postion Codes
    char szReplace[1024];  // Holds Converted Pipe 2 Ansi

    std::string fTmp;
    std::string AnsiString = szString;

    #define SP 0x20

    // Search for First Pipe
    id1 = 0;
    while (id1 != std::string::npos)
    {
        id1 = AnsiString.find("|",id1);
        if (id1 != std::string::npos)
        {
            memset(&szTmp,0,sizeof(szTmp));
            memset(&szReplace,0,sizeof(szReplace));
            szTmp[0] = AnsiString[id1+1];  // Get First # after Pipe
            szTmp[1] = AnsiString[id1+2];  // Get Second Number After Pipe
            if (szTmp[0] == '\0' || szTmp[0] == '\r' || szTmp[0] == EOF) break;
            if (szTmp[1] == '\0' || szTmp[1] == '\r' || szTmp[0] == EOF) break;

            //std::cout << "\r\n*** szTmp: " << szTmp << std::endl;
            if (isdigit(szTmp[0]) && isdigit(szTmp[1]))
            {
                switch (szTmp[0])
                {
                    // Parse from pipe to Foreground/back Color Ansi Sequences.
                    case '0' :
                        switch (szTmp[1])
                        {
                        case '0' :
                            ansiForegroundColor(szReplace, 0);
                            break;
                        case '1' :
                            ansiForegroundColor(szReplace, 1);
                            break;
                        case '2' :
                            ansiForegroundColor(szReplace, 2);
                            break;
                        case '3' :
                            ansiForegroundColor(szReplace, 3);
                            break;
                        case '4' :
                            ansiForegroundColor(szReplace, 4);
                            break;
                        case '5' :
                            ansiForegroundColor(szReplace, 5);
                            break;
                        case '6' :
                            ansiForegroundColor(szReplace, 6);
                            break;
                        case '7' :
                            ansiForegroundColor(szReplace, 7);
                            break;
                        case '8' :
                            ansiForegroundColor(szReplace, 8);
                            break;
                        case '9' :
                            ansiForegroundColor(szReplace, 9);
                            break;
                        default :
                            ++id1;
                            break;
                        }
                        break;

                    case '1' :
                        switch (szTmp[1])
                        {
                        case '0' :
                            ansiForegroundColor(szReplace, 10);
                            break;
                        case '1' :
                            ansiForegroundColor(szReplace, 11);
                            break;
                        case '2' :
                            ansiForegroundColor(szReplace, 12);
                            break;
                        case '3' :
                            ansiForegroundColor(szReplace, 13);
                            break;
                        case '4' :
                            ansiForegroundColor(szReplace, 14);
                            break;
                        case '5' :
                            ansiForegroundColor(szReplace, 15);
                            break;
                        case '6' :
                            ansiBackgroundColor(szReplace, 16);
                            break;
                        case '7' :
                            ansiBackgroundColor(szReplace, 17);
                            break;
                        case '8' :
                            ansiBackgroundColor(szReplace, 18);
                            break;
                        case '9' :
                            ansiBackgroundColor(szReplace, 19);
                            break;
                        default :
                            ++id1;
                            break;
                        }
                        break;

                    case '2' :
                        switch (szTmp[1])
                        {
                        case '0' :
                            ansiBackgroundColor(szReplace, 20);
                            break;
                        case '1' :
                            ansiBackgroundColor(szReplace, 21);
                            break;
                        case '2' :
                            ansiBackgroundColor(szReplace, 22);
                            break;
                        case '3' :
                            ansiBackgroundColor(szReplace, 23);
                            break;
                        default :
                            ++id1;
                            break;
                        }
                        break;

                    default :
                        ++id1;
                        break;
                }
                // Replace pipe code with Ansi Sequence
                if (strcmp(szReplace,"") != 0)
                    AnsiString.replace(id1,3,szReplace);
            }
            // Else not a Pipe Color / Parse for Screen Modification
            else if (szTmp[0] == 'C')
            {
                // Carriage Return / New Line
                if (strcmp(szTmp,"CR") == 0)
                {
                    ansiBackgroundColor(szReplace, 16);  // Clear Background Attribute first
                    strcat(szReplace,"\r\n");
                    AnsiString.replace(id1,3,szReplace);
                    id1 = 0;
                }
                // Clear Screen
                else if (strcmp(szTmp,"CS") == 0)
                {
                    ansiBackgroundColor(szReplace, 16);
                    // Set Scroll Region, Clear Background, Then Home Cursor.
                    //strcat(szReplace,"\x1b[1;25r\x1b[2J\x1b[1;1H");
                    strcat(szReplace,"\x1b[2J\x1b[1;1H");
                    AnsiString.replace(id1,3,szReplace);
                    id1 = 0;
                }
            }
            else
            {
                if (strcmp(szTmp,"XY") == 0)
                {
                    memset(&szPos1,0,sizeof(szPos1));
                    memset(&szPos2,0,sizeof(szPos2));
                    // X Pos
                    szPos1[0] = AnsiString[id1+3];
                    szPos1[1] = AnsiString[id1+4];
                    // Y Pos
                    szPos2[0] = AnsiString[id1+5];
                    szPos2[1] = AnsiString[id1+6];
                    ansiBackgroundColor(szReplace, 16);  // Clear Background Attribute first
                    sprintf(szReplace,"%s\x1b[%i;%iH",szReplace, atoi(szPos2),atoi(szPos1));
                    AnsiString.replace(id1,7,szReplace);
                }
                else
                {
                    // End of the Line, nothing parsed out so
                    // Skipp ahead past current code.
                    ++id1;
                }
            }
        }
        else break;
        id1 = AnsiString.find("|",id1);
    }
    TheSequenceParser::Instance()->processSequence(AnsiString);
}

/**
 * Reads in Ansi file into Buffer Only
 * For Testing the Ansi Parser.
 */
void MenuFunction::displayAnsiFile(std::string fileName)
{
    //std::cout << "ansiPrintf" << std::endl;
    std::string path = TheTerminal::Instance()->getProgramPath();
#ifdef _WIN32
    path += "assets\\";
#else
    path += "assets/";
#endif
    path += fileName;

    std::cout << "Display ANSI: " << path << std::endl;

    std::string buff;
    FILE *fp;
    int sequence = 0;
    if ((fp = fopen(path.c_str(), "r+")) ==  nullptr)
    {
        std::cout << "ansiPrintf - no ANSI found: " << fileName << std::endl;
        return;
    }
    do
    {
        sequence = getc(fp);
        if (sequence != EOF)
            buff += sequence;
    }
    while (sequence != EOF);
    fclose(fp);
    TheSequenceParser::Instance()->processSequence(buff);
}
