// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

# include "menuFunction.h"
# include "inputHandler.h"
# include "sequenceParser.h"
# include "linkList.h"

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
    _loadnew = false;

    menur2   = 0;
    cmdr2    = 0;

    execnum  = 0;
    execnum2 = 0;
    execnum3 = 0;

    System_Alive = true;
    choice   = 0;

    // For Lightbar Starting Position
    // Now uses menu system for lightbars.
    CURRENT_AREA = 0;

}

MenuFunction::~MenuFunction()
{
   // std::cout << "~MenuFunction()" << std::endl;
    if (menur2   != 0) delete    menur2;
    if (cmdr2    != 0) delete [] cmdr2;

    if (execnum  != 0) delete [] execnum;
    if (execnum2 != 0) delete [] execnum2;
    if (execnum3 != 0) delete [] execnum3;

    menur2   = 0;
    cmdr2    = 0;

    execnum  = 0;
    execnum2 = 0;
    execnum3 = 0;
    choice   = 0;
}

/*
 * Parse Helper for Menu's / Commands
 */
void MenuFunction::chkparse(std::string &temp)
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
int MenuFunction::mnuparse(std::string cfgdata)
{
    std::string::size_type id1 = 0;

    // Disgards any Config lines with the # Character
    if (cfgdata[0] == '#') return false;

    id1 = cfgdata.find("MenuName ", 0);
    if (id1 != std::string::npos)
    {
        chkparse(cfgdata);
        menur2->MenuName = cfgdata;
        return false;
    }

    id1 = cfgdata.find("Directive ", 0);
    if (id1 != std::string::npos)
    {
        chkparse(cfgdata);
        menur2->Directive = cfgdata;
        return false;
    }

    id1 = cfgdata.find("MenuPrompt ", 0);
    if (id1 != std::string::npos)
    {
        chkparse(cfgdata);
        menur2->MenuPrompt = cfgdata;
        return false;
    }

    id1 = cfgdata.find("Acs ", 0);
    if (id1 != std::string::npos)
    {
        chkparse(cfgdata);
        menur2->Acs = cfgdata;
        return false;
    }

    id1 = cfgdata.find("Password ", 0);
    if (id1 != std::string::npos)
    {
        chkparse(cfgdata);
        menur2->Password = cfgdata;
        return false;
    }

    id1 = cfgdata.find("ForceInput ", 0);
    if (id1 != std::string::npos)
    {
        chkparse(cfgdata);
        menur2->ForceInput = atoi((char *)cfgdata.c_str());
        return false;
    }

    id1 = cfgdata.find("ForceHelpLevel ", 0);
    if (id1 != std::string::npos)
    {
        chkparse(cfgdata);
        menur2->ForceHelpLevel = atoi((char *)cfgdata.c_str());
        return false;
    }

    id1 = cfgdata.find("Lightbar ", 0);
    if (id1 != std::string::npos)
    {
        chkparse(cfgdata);
        if (cfgdata == "TRUE") menur2->Lightbar = true;
        else menur2->Lightbar = false;
        return false;
    }

    return false;
}

/*
 * Loop for Reading reading a menu
 */
int MenuFunction::menu_read(std::string MenuName)
{

    //std::cout << "MenuFunction::menu_read" << std::endl;
#ifdef _WIN32
    std::string path = "assets\\";
#else
    std::string path = "assets/";
#endif

    path.append(MenuName);
    path.append(".menu");

    // Else Read and Parse it
    ifstream iFS;
    iFS.open( path.c_str() );
    if (!iFS.is_open())
    {
//        errlog((char *)"Couldn't Open Menu Area: %s",(char *)path.c_str());
        return false;
    }

    std::string cfgdata;

    // New Menu Command, Not all Menus have it, Default to false!
   // menur2->MFlags.EOSPrompt = false;
    for (;;)
    {
        if(iFS.eof()) break;
        std::getline(iFS,cfgdata,'\n');
        if(iFS.eof()) break;
        if (mnuparse(cfgdata) == true) break;
    }
    iFS.close();

    //memcpy(menur,&_menur,sizeof(CommandRec));
    return true;
}

/*
 * Parse Commands Reads from a Menu File
 */
void MenuFunction::cmdparse(std::string cfgdata, int idx)
{
    std::string::size_type id1 = 0;
    char sText[200] = {0};

    // Disgards any Config lines with the # Character
    if (cfgdata[0] == '#') return ;

    sprintf(sText,"LDesc[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        chkparse(cfgdata);
        cmdr2[idx].LDesc = cfgdata;
        return;
    }

    sprintf(sText,"SDesc[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        chkparse(cfgdata);
        cmdr2[idx].SDesc = cfgdata;
        return;
    }

    sprintf(sText,"CKeys[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        chkparse(cfgdata);
        cmdr2[idx].CKeys = cfgdata;
        return;
    }

    sprintf(sText,"Acs[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        chkparse(cfgdata);
        cmdr2[idx].Acs = cfgdata;
        return;
    }

    sprintf(sText,"CmdKeys[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        chkparse(cfgdata);
        cmdr2[idx].CmdKeys = cfgdata;
        return;
    }

    sprintf(sText,"MString[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        chkparse(cfgdata);
        cmdr2[idx].MString = cfgdata;
        return;
    }

    sprintf(sText,"HiString[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        chkparse(cfgdata);
        cmdr2[idx].HiString = cfgdata;
        return;
    }

    sprintf(sText,"LoString[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        chkparse(cfgdata);
        cmdr2[idx].LoString = cfgdata;
        return;
    }

    sprintf(sText,"Xcoord[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        chkparse(cfgdata);
        cmdr2[idx].Xcoord = atoi((char *)cfgdata.c_str());
        return;
    }

    sprintf(sText,"Ycoord[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        chkparse(cfgdata);
        cmdr2[idx].Ycoord = atoi((char *)cfgdata.c_str());
        return;
    }

    sprintf(sText,"LBarCmd[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        chkparse(cfgdata);
        if (cfgdata == "TRUE")
            cmdr2[idx].LBarCmd = true;
        else
            cmdr2[idx].LBarCmd = false;
        return;
    }

    sprintf(sText,"SText[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        chkparse(cfgdata);
        if (cfgdata == "true")
            cmdr2[idx].SText = true;
        else
            cmdr2[idx].SText = false;
        return;
    }

    sprintf(sText,"STLen[%.03d]",idx);
    id1 = cfgdata.find(sText, 0);
    if (id1 != std::string::npos)
    {
        chkparse(cfgdata);
        cmdr2[idx].STLen = atoi((char *)cfgdata.c_str());
        return;
    }

}

/*
 * Check if Command Exists (Used for Counting Commands)
 */
int MenuFunction::cmdexist(std::string MenuName, int idx)
{
    //std::cout << "MenuFunction::cmdexist" << std::endl;

#ifdef _WIN32
    std::string path = "assets\\";
#else
    std::string path = "assets/";
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
int MenuFunction::cnt_cmds(std::string MenuName)
{
    int  cnt = 0;
    while (cmdexist(MenuName,cnt))
    {
        ++cnt;
    }
    return cnt;
}

/**
 * Loop to Read commands in a Menu File.
 */
int MenuFunction::cmds_read(std::string MenuName, int idx)
{

#ifdef _WIN32
    std::string path = "assets\\";
#else
    std::string path = "assets/";
#endif

    path.append(MenuName);
    path.append(".menu");

    int ret;
    ret = cmdexist(MenuName,idx);
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
        cmdparse(cfgdata,idx);
        if(iFS.eof()) break;
    }
    iFS.close();

    //memcpy(cmdr,&_cmdr,sizeof(CommandRec));
    return true;
}


/**
 * Loads All Commands into Class.
 */
void MenuFunction::insert_cmdr()
{

    int  idx = 0;
    while (cmds_read(_curmenu,idx))
    {
        ++idx;
    }

}

/**
 * reset or Set Inital Defaults before a menu is read.
 */
void MenuFunction::menu_reset()
{
    xx      = 1;                // Holds X Coord
    yy      = 1;                // Holds Y Coord

    noc     = 0;                // Number of Commands.
    iNoc    = 0;                // Holds Lightbar # of choices
    sNoc    = 0;                // Holds Scroll Text # of choices
    cntEscCmds = 0;             // Number of ESC Commands.

    c  = '\0';
    cc = '\0';                      // Hold Input / Lightbar Key
    EscHit = false;                 // Is Input key Escaped char, or Normal Key
    memset(&outBuff,0,sizeof(outBuff));   // Holds Formatted Lightbar Data
    output.erase();                 // Buffer for writing all lightbars at the same time
    sRand = 0;                      // Randmise Text Scrollies
    tScroll = false;
    executed = 0;                   // Test's for hot keys commands excuted, if non pass through loop
    // This is so scrolling isn't killed with invalid menu input!
    exe = 0;

}


/**
 * Check if Menu Exists
 */
int MenuFunction::menu_exists()
{
#ifdef _WIN32
    std::string path = "assets\\";
#else
    std::string path = "assets/";
#endif

    path.append(_curmenu);
    path.append(".menu");

    FILE *fstr;
    fstr = fopen(path.c_str(),"rb+");
    if(fstr == NULL)
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
void MenuFunction::menu_readin()
{
    //std::cout << "MenuFunction::menu_readin: " << _curmenu << std::endl;

    // If we have the same menu, abort and return, no reason to re-load same!
    ////errlog2("readin() 1 %s,%s",(char *)_premenu.c_str(),(char *)_curmenu.c_str());
    if (_premenu == _curmenu || _curmenu == "")
    {
        //std::cout << "MenuFunction::menu_readin:  _loadnew = false " << std::endl;
        _loadnew = false;
        return ;
    }

    int idx  = 0;
    int nogc = 0;

#ifdef _WIN32
    std::string path = "assets\\";
#else
    std::string path = "assets/";
#endif

    path.append(_curmenu);
    path.append(".menu");

    ////errlog2("readin() 3");
    /// Check if Menu Exists first!
    FILE *fstr;
    fstr = fopen(path.c_str(),"rb+");
    if(fstr == NULL)
    {
        _loadnew = false;
        return;
    }
    else
    {
        fclose(fstr);
    }

    // Setup the Default Setting and And Clear Allocations.
    menu_reset(); // Loading new Menu, Clear!

    if (menur2 != 0)
    {
        delete menur2;
        menur2 = 0;
    }

    menur2 = new MenuRec;
    if (!menur2)
    {
        return;
    }

    if(!menu_read(_curmenu))
        return;

    nogc = 0;
    noc = cnt_cmds(_curmenu);

    if (cmdr2 != 0)
    {
        delete [] cmdr2;
        cmdr2 = 0;
    }

    cmdr2 = new CommandRec[noc+nogc+1];
    if (!cmdr2)
    {
        return;
    }

    insert_cmdr();
    if (nogc > 0)
    {
        idx = 0;
        while(cmds_read((char *)"global",idx))
        {
            ++noc;
            ++idx;
        }
    }

    _loadnew = false;
}


/**
 * Clear Menu When Done.
 */
void MenuFunction::menu_clear()
{
    // Clear
    ////errlog2(" *** menu_clearbars()");
    if (execnum)  delete [] execnum;
    if (execnum2) delete [] execnum2;
    if (execnum3) delete [] execnum3;
    execnum  = 0;
    execnum2 = 0;
    execnum3 = 0;

}


/**
 * Geenrates Menu from commands like Telegard / Renegade.
 * Instead of using an ANSI File.
 */
void MenuFunction::display_menu(bool forcelevel)
{

    //std::cout << "MenuFunction::display_menu" << std::endl;

    bool ignore = false;
    char text [1024]= {0};
    char text2[1024]= {0};

    if (forcelevel==false)
    {
        if (menur2->ForceHelpLevel == 0) return;
    }

    std::string output = "|CS|07|16|CR";
    sprintf(text,"%s",(char *)menur2->MenuName.c_str() );
 //   cspacing(text);
    output += "|CR|CR|15|16";
    output += text;
    output += "|CR|CR|07|16";


    memset(&text,0,1024);
    unsigned long cnt=0;

    // Set this in menu.txt lateron, or an override.
    char desc_color[4]     = {"|07"};
    char cmd_color[4]      = {"|15"};
    char bracket_color[4]  = {"|08"};
    char bracket2_color[4] = {"|07"};

    strcpy(text, (char *)" %s%24s %s(%s%s%s)     " );

    for (long i = 0; i != noc; i++)
    {
        // Ignore Execute on First Load Commands
        if (cmdr2[i].CKeys == "FIRSTCMD")
        {
            ignore = true;
        }
        else // Ignore Lightbar Commands
            if (cmdr2[i].LBarCmd)
            {
                ignore = true;
            }
            else // Ignore Scrolling Text
            if (cmdr2[i].SText)
            {
                ignore = true;
            }
            else // Ignore Blank Keys
            if (cmdr2[i].CKeys == "")
            {
                ignore = true;
            }
            else // Ignore Stacked. Only Display First.
            if (cmdr2[i].LDesc == "")
            {
                ignore = true;
            }

        // Check Ignore Status to Write out.
        if (ignore)
        {
            ignore = false;
        }
        else
        {
            // Start Displaying Menu Text
            // Menu Commands spacing
            sprintf(text2, text ,
                    desc_color,
                    (char *)cmdr2[i].LDesc.c_str(),
                    bracket_color,
                    cmd_color,
                    (char *)cmdr2[i].CKeys.c_str(),
                    bracket2_color);
//            errlog (text2);
            output += text2;
            ++cnt;

            // 3 Rows, Menus
            if (menur2->ForceHelpLevel == 1)
            {
                if (cnt == 2)
                {
                    // Goto Next Line, reset.
                    cnt = 0;
                    output += "\r\n";
                }
            }
            else
            {
                // 1 Row, Matrix
                if (menur2->ForceHelpLevel == 2)
                {
                    // Goto Next Line, reset.
                    cnt = 0;
                    output += "\r\n";
                }
            }
        }
    }

    if (output.size() > 2)
    {
        output += "|CR|CR";
        pipe2ansi((char *)output.c_str(),0);
    }
    return;
}

/**
 * Used for Lightbars and Hotkey Menu's
 * This is also used in Full Screen Interfaces
 * for Lightbars slections and Return
 * PassThough commands from Menu Lightbar Prompts.
 */
void MenuFunction::menu_bars(char *inPut)
{
    //std::cout << "MenuFunction::menu_bars" << std::endl;
    // Handle Input Strings
    std::string inputSequence;

    std::vector<list_bar> lightbar;
    std::string::size_type id1 = 0;

    // If no Menu Commands Return! / Add fallback menu lateron!!
    if (noc == 0) return;

    // Read Menu Ansi to String that will hold lightbars, Push to screen sametime
    // Will speed up display and make ghosting not apear as much

    bool menufile = false;
    if (menur2->Directive != "")
    {
        ansiPrintf((char *)menur2->Directive.c_str());
        menufile = true;
    }

    // First time load, Map CommandKeys to HOTKEYS
    if (_premenu != _curmenu)
    {
        if (execnum != 0)
        {
            delete [] execnum;
            execnum = 0;
        }

        if (execnum == 0)
        {
            execnum = new short[noc+1];  // Set to # of comamnd for now, preload.
            if (!execnum)
            {
                return;
            }
        }

        // Check for Text Scrolling Commands
        if (execnum2 != 0)
        {
            delete [] execnum2;
            execnum2 = 0;
        }

        if (execnum2 == 0)
        {
            execnum2 = new short[noc+1]; // Set to # of comamnd for now, preload.
            if (!execnum2)
            {
                return;
            }
        }

        // Check for Text Scrolling Commands
        ////errlog2("menu_bars() 5.3 execnum3");
        if (execnum3 != 0)
        {
            ////errlog2("menu_bars() delete execnum3;");
            delete [] execnum3;
            execnum3 = 0;
        }

        if (execnum3 == 0)
        {
            execnum3 = new short[noc+1]; // Set to # of comamnd for now, preload.
            if (!execnum3)
            {
                return;
            }
        }

        iNoc = 0;
        sNoc = 0;
        exe  = 0;
        cntEscCmds = 0;

        for (int i = 0; i != noc; i++)
        {

            // If we find a FIRSTCMD, Execute it right away!
            if (cmdr2[i].CKeys == "FIRSTCMD")
            {
                ////errlog2("menu_bars() 6.2 - FIRSTCMD");
                menu_docmd(&cmdr2[i]);
                // Count /Commands, if we execute all FIRSTCMDS then return when completed.
                ++exe;
            }

            // Get Ligthbar Commands
            if (cmdr2[i].LBarCmd) //&& isANSI == true)
            {
                ////errlog2("menu_bars() 6.3 - Lightbar");
                execnum[iNoc] = i;
                ++iNoc;
            }

            if (cmdr2[i].SText)
            {
                ////errlog2("menu_bars() 6.4 - Scrolling");
                execnum2[sNoc] = i;
                ++sNoc;
            }

            //while running throguh each command, put into a vector formatted.
            // No ansi menu file, generate menu commands.
            if (!menufile && iNoc == 0)
            {
                //errlog((char *)"menu_bars() 6.4 - Setting Menu Command Layout");
                // Insert code here for generating generic menu with commands.
            }

            ////errlog2("menu_bars() 10");
            // Count Escape Keys in Menu System to override passthrough
            if (cmdr2[i].CKeys == "ESC")
            {
                execnum3[cntEscCmds] = i;
                ++cntEscCmds;
            }
            else if (cmdr2[i].CKeys == "LEFT")
            {
                execnum3[cntEscCmds] = i;
                ++cntEscCmds;
            }
            else if (cmdr2[i].CKeys == "RIGHT")
            {
                execnum3[cntEscCmds] = i;
                ++cntEscCmds;
            }
            else if (cmdr2[i].CKeys == "UP")
            {
                execnum3[cntEscCmds] = i;
                ++cntEscCmds;
            }
            else if (cmdr2[i].CKeys == "DOWN")
            {
                execnum3[cntEscCmds] = i;
                ++cntEscCmds;
            }

            else if (cmdr2[i].CKeys == "HOME")
            {
                execnum3[cntEscCmds] = i;
                ++cntEscCmds;
            }
            else if (cmdr2[i].CKeys == "END")
            {
                execnum3[cntEscCmds] = i;
                ++cntEscCmds;
            }

            else if (cmdr2[i].CKeys == "PAGEUP")
            {
                execnum3[cntEscCmds] = i;
                ++cntEscCmds;
            }
            else if (cmdr2[i].CKeys == "PAGEDN")
            {
                execnum3[cntEscCmds] = i;
                ++cntEscCmds;
            }
        }
        // Return if we executed all comamnds as FRISTCMD!
        if (exe == noc)
        {
            return;
        }

        // Make sure there is more then 1 to Randomize
        if (sNoc > 0)
        {
            srand(time(NULL));
            sRand = rand()%sNoc;
        }

        _premenu = _curmenu;
    }
    output.clear();

    if (iNoc == 0)
    {  }
    else
    {
        // Throw out a menu prompt if there is one available. - Add MCI Parsing too!
        // Use Same X Row, these got reversed somehow!!! grrrr
        xx = 1;
        yy = cmdr2[execnum[0]].Ycoord;
    }
    //std::cout << "iNoc: " << iNoc << std::endl;

    std::string sMenu = _curmenu;
    while (!TheInputHandler::Instance()->isGlobalShutdown())
    {
        // Lightbar menu then draw
        if (iNoc > 0)
        {
            //std::cout << "lightbars" << std::endl;
            // Setup of Remaining Lightbars in Low highlight Form
            for (int rep = 0; rep != iNoc; rep++)
            {
                if (rep != choice)
                {
                    xx = cmdr2[execnum[rep]].Xcoord;
                    yy = cmdr2[execnum[rep]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)cmdr2[execnum[rep]].LoString.c_str());
                    output += outBuff;
                }
                else
                {
                    xx = cmdr2[execnum[rep]].Xcoord;
                    yy = cmdr2[execnum[rep]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)cmdr2[execnum[rep]].HiString.c_str());
                    output += outBuff;
                }
            }

            // Moves cursor to end of line during lightbars.
            sprintf(outBuff,"\x1b[%i;79H",yy);
            output += outBuff;

            pipe2ansi((char*)output.c_str(),0);
        }
        // Not Lightbars, print out menu ansi.
        else
        {
            pipe2ansi((char*)output.c_str(),0);
        }

        //Can display global prompt here!
        output.clear();

        //Replcae Messages Left...
        output.clear();
        output = menur2->MenuPrompt;

        // If lightbar prompt, reset to begining of line.
        if (iNoc > 0)
        {
            xx = 1;
            sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)menur2->MenuPrompt.c_str());
            output = outBuff;
        }
        if (output.size() > 1)
        {
            pipe2ansi((char *)output.c_str(),0);
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
        c = inputSequence[0];

        // Check here for Arrow Key / Escaped Input was Received
        if ((int)c == 27)
        {
            //std::cout << "ESC RECEIVED" << std::endl;
            if (inputSequence.size() > 1)
            {
                cc = inputSequence[2];
                // Handle Hardware Input Sequences
                if (cc == '0')
                {
                    cc = inputSequence[3];
                }
            }
            else
                cc = '\0';
            EscHit = true;
        }
        else
        {
            EscHit = false;
        }

        ////errlog2("menu_bars() 22");
        output.clear();


        ////errlog2("menu_bars() 23 .0 ");
        executed  = 0;
        if (EscHit)
        {
            // Input Key is Escaped Meaning Arrow Keys
            //std::cout << "ESC Hit: " << cc << " cntEscCmds:" << cntEscCmds << std::endl;

            // Run through and check for any Menu commands that overive default passthrough!
            if (cntEscCmds > 0)
            {
                // ESC Commands in Menu might overide lightbars.
                for (int ckey = 0; ckey != (signed)cntEscCmds; ckey++)
                {
                    if (cc == '\0') // Stright ESC Key
                    {
                        if (cmdr2[execnum3[ckey]].CKeys == "ESC")
                        {
                            ++executed;
                            menu_docmd(&cmdr2[execnum3[ckey]]);
                            strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                        }
                    }
                    else if (cc == 'A')
                    {
                        if (cmdr2[execnum3[ckey]].CKeys == "UP")
                        {
                            ++executed;
                            menu_docmd(&cmdr2[execnum3[ckey]]);
                            strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                        }
                    }
                    else if (cc == 'B')
                    {
                        if (cmdr2[execnum3[ckey]].CKeys == "DOWN")
                        {
                            ++executed;
                            menu_docmd(&cmdr2[execnum3[ckey]]);
                            strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                        }
                    }
                    else if (cc == 'C')
                    {
                        if (cmdr2[execnum3[ckey]].CKeys == "RIGHT")
                        {
                            ++executed;
                            menu_docmd(&cmdr2[execnum3[ckey]]);
                            strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                        }
                    }
                    else if (cc == 'D')
                    {
                        if (cmdr2[execnum3[ckey]].CKeys == "LEFT")
                        {
                            ++executed;
                            menu_docmd(&cmdr2[execnum3[ckey]]);
                            strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                        }
                    }
                    else if (cc == 'V' || (cc == '5' && inputSequence[3] == '~'))
                    {
                        if (cmdr2[execnum3[ckey]].CKeys == "PAGEUP")
                        {
                            ++executed;
                            menu_docmd(&cmdr2[execnum3[ckey]]);
                            strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                        }
                    }
                    else if (cc == 'U' || (cc == '6' && inputSequence[3] == '~'))
                    {
                        if (cmdr2[execnum3[ckey]].CKeys == "PAGEDN")
                        {
                            ++executed;
                            menu_docmd(&cmdr2[execnum3[ckey]]);
                            strcpy(inPut,(const char*)cmdr2[execnum3[ckey]].CmdKeys.c_str());
                        }
                    }
                }
                // Executed == 0, Then Key Pressed was not valid! :)
                // Pass through the ESC then
                if (executed == 0)
                {
                    inPut[0] = cc;
                    // Translater to Terminal ESC Keys
                    if (cc == '0')
                        cc = inputSequence[3];
                }
                else
                {
                    //std::cout << "Executed, now passthrough back to interface the key pressed!" << std::endl;
                    return;
                }
            }

            // If true, means we pass A and B up/dn through to system!
            //elog ("Start Lightbar Processing... CMD: %c, cntEscCmds: %ld", cc, cntEscCmds);

            if (iNoc > 0)
            {
                ////errlog2("menu_bars() 23.4 - iNoc > 0");
                if (cc == 'D' && executed == 0)
                {
                    // Up & Left
                    ////errlog2("menu_bars() 23.4 - D");
                    //printf ("\n D!");

                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)cmdr2[execnum[choice]].LoString.c_str());
                    output += outBuff;

                    if (choice == 0) choice = iNoc-1;
                    else --choice;

                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s\x1b[%i;79H",yy,xx,(char *)cmdr2[execnum[choice]].HiString.c_str(),yy);
                    output += outBuff;
                    // Send Lightbar output
                    pipe2ansi((char*)output.c_str(),0);
                }
                else if (cc == 'C' && executed == 0)
                {
                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s",yy,xx,(char *)cmdr2[execnum[choice]].LoString.c_str());
                    output += outBuff;

                    if (choice == iNoc-1) choice = 0;
                    else ++choice;

                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf(outBuff,"\x1b[%i;%iH%s\x1b[%i;79H",yy,xx,(char *)cmdr2[execnum[choice]].HiString.c_str(),yy);
                    output += outBuff;

                    pipe2ansi((char*)output.c_str(),0);
                }

            } //</iNoc>
            else
            {
                //std::cout << "Normal Key Input!: " << (int)c << std::endl;

                // normal Key Input.
                if ((int)c == 10)
                {
                    executed = 0;
                    // Here Loop through and execute stacked Commands
                    for (int ckey = 0; ckey != noc; ckey++)
                    {
                        if ( cmdr2[ckey].CKeys == "ENTER" )
                        {
                            ++executed;
                            menu_docmd(&cmdr2[ckey]);
                            strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys.c_str());
                        }
                        else if ( cmdr2[ckey].CKeys == cmdr2[execnum[choice]].CKeys )
                        {
                            ++executed;
                            menu_docmd(&cmdr2[ckey]);
                            strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys.c_str());
                        }
                    }

                    if (executed > 0)
                    {
                        return;
                    }
                    else
                    {
                        // Redraw screen!
                        if (menur2->Directive != "")
                        {
                            ansiPrintf((char *)menur2->Directive.c_str());
                        }
                    }
                }
                //  Else go through and run stack commands on hot key pressed.
                else
                {
                    executed = 0;     // Normal Key Inputed, if Match's
                    std::string t1;
                    for (int ckey = 0; ckey != noc; ckey++)
                    {
                        // Loop and Run Stacked Commands.
                        if (c == 32)
                        {
                            // Else check if it's a spacebar
                            if (cmdr2[ckey].CKeys == "SPACE")
                            {
                                ++executed;
                                menu_docmd(&cmdr2[ckey]);
                                strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys.c_str());
                            }
                        }
                        else // Check any remaining Keys Hot Key Input.
                            if (cmdr2[ckey].CKeys[0] == toupper(c) && cmdr2[ckey].CKeys.size() < 2)
                            {
                                ++executed;
                                menu_docmd(&cmdr2[ckey]);
                                strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys.c_str());
                            }
                    }
                }
                // Executed == 0, Then Key Pressed was not valid!, and no stacked commands to loop :)
                if (executed > 0)
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

            if ((int)c == 13 && iNoc > 0)
            {
                // Only Executte [ENTER] on Lightbars!
                executed = 0;
                // Here Loop through and execute stacked Commands
                for (int ckey = 0; ckey != noc; ckey++)
                {
                    if (cmdr2[ckey].CKeys == cmdr2[execnum[choice]].CKeys)
                    {
                        ++executed;
                        menu_docmd(&cmdr2[ckey]);
                        strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys.c_str());
                    }
                }
                if (executed > 0)
                {
                    ////errlog2("Excuted Return Passthrough or Normal Key!");
                    return;
                }
                else
                {
                    // Redraw screen!
                    if (menur2->Directive != "")
                    {
                        ansiPrintf((char *)menur2->Directive.c_str());
                    }
                }
            }
            else if ((int)c == 13 && iNoc == 0)
            {
                // Check for ENTER CKEY!
                executed = 0;
                // Here Loop through and execute stacked Commands
                for (int ckey = 0; ckey != noc; ckey++)
                {
                    if (cmdr2[ckey].CKeys == "ENTER")
                    {
                        ++executed;
                        menu_docmd(&cmdr2[ckey]);
                        strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys.c_str());
                    }
                }
                if (executed > 0)
                {
                    return;
                }
                else
                {
                    // Redraw screen!
                    if (menur2->Directive != "")
                    {
                        ansiPrintf((char *)menur2->Directive.c_str());
                    }
                }
            }
            //  Else go through and run stack commands on hot key pressed.
            else
            {
                executed = 0;      // Normal Key Inputed, if Match's
                std::string t1;
                for (int ckey = 0; ckey != noc; ckey++)
                {
                    // Loop and Run Stacked Commands.
                    if (c == 32)
                    {
                        // Else check if it's a spacebar
                        if (cmdr2[ckey].CKeys == "SPACE")
                        {
                            ++executed;
                            menu_docmd(&cmdr2[ckey]);
                            strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys.c_str());
                        }
                    }
                    else // Check any remaining Keys Hot Key Input.
                        if (cmdr2[ckey].CKeys[0] == toupper(c) && cmdr2[ckey].CKeys.size() < 2)
                        {
                            ++executed;
                            menu_docmd(&cmdr2[ckey]);
                            strcpy(inPut,(const char*)cmdr2[ckey].CmdKeys.c_str());
                        }
                }
                // Executed == 0, Then Key Pressed was not valid!, and no stacked commands to loop :)
                if (executed > 0)
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
void MenuFunction::menu_proc(char *mString, uint32_t area)
{
    //std::cout << "MenuFunction::menu_proc" << std::endl;

    CURRENT_AREA = area;
    strcpy(mString,"");
    if (noc == 0)
    {
        menu_reset();
        System_Alive = false;
        return;
    }
    menu_bars(mString);
}

/**
 * Menu System - Run Menu Command
 */
void MenuFunction::menu_docmd(CommandRec *cmdr)
{
    unsigned char c1 = cmdr->CmdKeys[0];
    unsigned char c2 = cmdr->CmdKeys[1];

    std::string parse;
    std::string::size_type id1;

    //std::cout << "Do MenuCmd: CmdKey[0]: " << (char)c1 << " CmdKey[1]: " << (char)c2 << std::endl;
    std::string inputSequence;

    switch (c1)
    {
        // Message Reader Return right away
        case '!' :
        // These are Passthrough Menu Commands.
        break;

    case '-' :
        ////errlog2("menu_docmd!! 11");
        switch (c2)
        {

            case '^' : // Change Menu
                //strcpy(_curmenu,mString);
                _curmenu.clear();
                _curmenu = cmdr->MString;
                _loadnew = true;
                choice = 0;
                break;

            case '/' : // Gosub Menu - Remember current Menu, then changes to new
                _gosub.clear();
                _gosub = _curmenu;
                _curmenu.clear();
                _curmenu = cmdr->MString;
                _loadnew = true;
                choice = 0;
                break;

            case '!' : // Return to Original Menu from GoSub Menu.
                if (_gosub == "") break;
                _curmenu.clear();
                _curmenu = _gosub;
                _loadnew = true;
                choice = 0;
                break;

            case 'T' : // Display a line of Text
                pipe2ansi((char *)cmdr->MString.c_str(),0);
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

    //elog2("%i rspace",space);
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

    //elog2("%i lspace",space);
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
void MenuFunction::mask(char *str)
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
void MenuFunction::inputfield(char *text, int &len)
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
        repeat += "±";
    }

    if (!inc)
        sprintf(INPUT_COLOR,"|15|17");

    sprintf(text,"%s%s%s\x1b[%iD",(char *)temp.c_str(),INPUT_COLOR,repeat.c_str(),len);
    // Restore back to Default.

}

/*
 * Get Single Key Input.
 */
int MenuFunction::getkey()
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
void MenuFunction::getline(char *line,     // Returns Input into Line
                         int   length,   // Max Input Length of String
                         char *leadoff,  // Data to Display in Default String {Optional}
                         int   hid,      // If input is Echomail as hidden    {Optional}
                         char *chlist)   // Valid Input Char List             {Optional}
{
    int c, cc;
    int i   = 0;
    int Col = 0;
    int EscHit;
    std::string tLine;
    std::string output;
    char sLine[200]  = {0};
    char sList[200]  = {0};

    std::string input;
    std::string inputSequence;

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
        if(TheInputHandler::Instance()->update())
        {
            inputSequence = TheInputHandler::Instance()->getInputSequence();
        }
        else
        {
            SDL_Delay(10);
            continue;
        }
        c = inputSequence[0];

        if (c == '\r') c = '\n';

        // Valid Key List Checking....
        if (chlist != 0)
        {
            //if (strlen(sList) > 0) {
            if(c >= 'a' && c <= 'z')
            {
                c = toupper(c);
            }
            if(strchr(sList,c) || c == '\n' || c == '\r' || (int)c == 10)
            {
                //continue;
            }
            else
            {
                continue;
            }
            //}
        }
        // Escape in this case, ignore, later add movement in string
        if ((int)c == 27)
        {
            if (inputSequence.size() >= 3)
                cc = inputSequence[2];
            EscHit = true;
        }
        else
            EscHit = false;

        // Catch all Escaped Keys for Cursor Movement
        if (EscHit)
        {
            switch (cc)
            {
            case '3' : // Delete
                if (i != 0 || Col != 0)
                {
                    TheSequenceParser::Instance()->processSequence((std::string)"\x1b[D±\x1b[D");
                    input.erase(Col-1,1);
                    --i;
                    --Col;
                }
                break;

            default :
                break;
            }

        }
        else if ((int)c == 25)
        {
            // CTRL Y - Clear Line
            input.erase();
            i = Col;
            for (; i != 0; i--)
            {
                TheSequenceParser::Instance()->processSequence((std::string)"\x1b[D±\x1b[D");
            }
            i = 0;
            Col = i;
        }

        // delete 127
        // Do destructive backspace
        // on VT100 Terms 127 DEL == BS!
        // Since we have no DELETE in this, delete on 1 liens will works like BS.
        else if ((int)c == 0x08 || (int)c == 127 || int(c) == 8 || int(c) == 207 || (int)c == 0x7f)
        {
            //|| int(c) == 255 || int(c) == 0x0e00) {
            if (i != 0 || Col != 0)
            {
                TheSequenceParser::Instance()->processSequence((std::string)"\x1b[D±\x1b[D");
                input.erase(Col-1,1);
                --i;
                --Col;
            }
        }
        // Normal Key Input, Letters & numbers
        else if((int)c > 31 && (int)c < 126)
        {
            if (i != length-1)
            {
                if (hid)
                {
                    output = '*';
                    TheSequenceParser::Instance()->processSequence(output);
                }
                else
                {
                    output = c;
                    TheSequenceParser::Instance()->processSequence(output);
                }
                input += c;
                ++i;
                ++Col;
            }
        }
        else if (c == 10 || c == '\r' || c == '\n')
        {
            input += '\0';
            strncpy(line,(char *)input.c_str(),length);
            break;
        }
    }

    // If Global Exit, return right away.
    if (TheInputHandler::Instance()->isGlobalShutdown())
    {
        return;
    }

    // Restore Background color after input received.
    Col = 0;
    char sReplace[15]= {0};
    ansi_bg(sReplace, 16);
    pipe2ansi(sReplace,0);
}


/**
 * Set ANSI Foreground colors
 */
void MenuFunction::ansi_fg(char *data, int fg)
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
void MenuFunction::ansi_bg(char *data, int bg)
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
void MenuFunction::pipe2ansi(char* szString, int buffer)
{
    std::string::size_type  id1 = 0, test;    // Pipe Position
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
                        ansi_fg(szReplace, 0);
                        break;
                    case '1' :
                        ansi_fg(szReplace, 1);
                        break;
                    case '2' :
                        ansi_fg(szReplace, 2);
                        break;
                    case '3' :
                        ansi_fg(szReplace, 3);
                        break;
                    case '4' :
                        ansi_fg(szReplace, 4);
                        break;
                    case '5' :
                        ansi_fg(szReplace, 5);
                        break;
                    case '6' :
                        ansi_fg(szReplace, 6);
                        break;
                    case '7' :
                        ansi_fg(szReplace, 7);
                        break;
                    case '8' :
                        ansi_fg(szReplace, 8);
                        break;
                    case '9' :
                        ansi_fg(szReplace, 9);
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
                        ansi_fg(szReplace, 10);
                        break;
                    case '1' :
                        ansi_fg(szReplace, 11);
                        break;
                    case '2' :
                        ansi_fg(szReplace, 12);
                        break;
                    case '3' :
                        ansi_fg(szReplace, 13);
                        break;
                    case '4' :
                        ansi_fg(szReplace, 14);
                        break;
                    case '5' :
                        ansi_fg(szReplace, 15);
                        break;
                    case '6' :
                        ansi_bg(szReplace, 16);
                        break;
                    case '7' :
                        ansi_bg(szReplace, 17);
                        break;
                    case '8' :
                        ansi_bg(szReplace, 18);
                        break;
                    case '9' :
                        ansi_bg(szReplace, 19);
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
                        ansi_bg(szReplace, 20);
                        break;
                    case '1' :
                        ansi_bg(szReplace, 21);
                        break;
                    case '2' :
                        ansi_bg(szReplace, 22);
                        break;
                    case '3' :
                        ansi_bg(szReplace, 23);
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
                    ansi_bg(szReplace, 16);  // Clear Background Attribute first
                    strcat(szReplace,"\r\n");
                    AnsiString.replace(id1,3,szReplace);
                    id1 = 0;
                }
                // Clear Screen
                else if (strcmp(szTmp,"CS") == 0)
                {
                    ansi_bg(szReplace, 16);
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
                    ansi_bg(szReplace, 16);  // Clear Background Attribute first
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
void MenuFunction::ansiPrintf(std::string fileName)
{
    //std::cout << "ansiPrintf" << std::endl;
#ifdef _WIN32
    std::string path = "assets\\";
#else
    std::string path = "assets/";
#endif
    path += fileName;

    std::string buff;

    FILE *fp;
    std::string::size_type id1 = 0;

    int c = 0;
    if ((fp = fopen(path.c_str(), "r+")) ==  NULL)
    {
        std::cout << "ansiPrintf - no ansi found: " << fileName << std::endl;
        return;
    }
    do
    {
        c = getc(fp);
        if (c != EOF)
            buff += c;
    }
    while (c != EOF);

    fclose(fp);
    TheSequenceParser::Instance()->processSequence(buff);
}
