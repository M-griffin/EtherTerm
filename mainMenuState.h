#ifndef __MAINMENU_STATE__
#define __MAINMENU_STATE__

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include <string>
#include <vector>
#include "menuState.h"
#include "termObject.h"
#include "linkList.h"
#include "menuFunction.h"

typedef struct MainMenuState_INI
{
    char INI_PATH[255];
    char INI_NAME[255];
    int  TOP_MARGIN;
    int  BOTTOM_MARGIN;
    std::string PAGE_NUMBER;
    std::string PAGE_TOTAL;
    std::string MORE_MESSAGE_ON;
    std::string MORE_MESSAGE_TEXT_ON;
    std::string MORE_MESSAGE_OFF;
    std::string MORE_MESSAGE_TEXT_OFF;
    std::string TEXT_COLOR;
    std::string TEXT_COLOR_HILIGHT;
    std::string MAX_SYSTEMS;
    std::string FONT_SET;
    // Theme
    std::string ANSI_FILE;
    std::string ANSI_HELP_FILE;
    std::string THEME_NAME;
    std::string MENU_PROMPT_TEXT;
    // Message Read .ini File
    bool ddirectory_exists();
    void ddirectory_create();
    void ddirectory_chkpar(std::string &data);
    void ddirectory_check(std::string cfgdata);
    int  ddirectory_parse(int idx=0);

} MainMenuState_INI;

/*
 * This Class Holds a connection to the MenuState -> SocketState -> Term
 * This is where we will inital either Telnet or SSH
 */
class MainMenuState : public MenuState, MainMenuState_INI
{
public:

    virtual ~MainMenuState()
        { std::cout << "Shutting Down MainMenuState" << std::endl;}
    virtual void update();
    virtual void render();
    virtual bool onEnter();
    virtual bool onExit();
    virtual std::string getStateID() const
    {
        return menuID;
    }

private:

    virtual void setCallbacks(const std::vector<Callback>& callbacks);
    // call back functions for menu items
    static void menuToTelnet();
    static void menuToSSH();

    std::string inputSequence;
    static const std::string menuID;

    // Class Types for use
    LinkList      _linkList;
    MenuFunction  _menuFunction;
    MenuRec      *_menuRecord;
    CommandRec   *_commandRecord;

    int  numberOfCommands; // Number of Commands in Loaded Menu
    int  systemActive;     // System Exit

    // Dialing Directory
    unsigned long LIGHTBAR_POSITION;
    int directoryTopMargin;
    int directoryBottomMargin;

    std::vector<TheTerm::SystemConnection> systemConnection;
    std::vector<list_bar> buildDialList();

    void  readinAnsi(std::string FileName, std::string &buff);
    void  parseHeader(std::string FileName);
    int   changeTheme(int idx);
    void  setupList();

    bool  readDialDirectory();
    void  createDialDirectory();
    int   startDialDirectory();
    // Term
    std::vector<TermObject*> termObjects;
};

#endif
