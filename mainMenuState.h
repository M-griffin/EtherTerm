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
    char INIPATH[255];
    char sININAME[255];
    int  iTop;
    int  iBot;
    std::string sPAGENUM;
    std::string sPAGETOTAL;
    std::string sMOREMSG_ON;
    std::string sMOREMSG_WORD_ON;
    std::string sMOREMSG_OFF;
    std::string sMOREMSG_WORD_OFF;
    std::string sTEXT_COLOR;
    std::string sTEXT_HILIGHT;
    std::string sMAX_AREAS;
    std::string sFONT_SET;
    // Theme
    std::string sANSI_FILE;
    std::string sANSI_HELP;
    std::string sTHEME_NAME;
    std::string sMENU_PROMPT;
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

    virtual ~MainMenuState() {}
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

    // MenuFunc
    LinkList      mLink;
    MenuFunction  mFunc;
    MenuRec      *menur2;
    CommandRec   *cmdr2;

    int  noc;           // Number of Commands in Loaded Menu
    int  System_Alive;  // System Exit

    // Title Scan
    unsigned long CURRENT_BAR;
    int tTop;
    int tBot;

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
