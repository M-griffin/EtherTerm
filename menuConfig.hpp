#ifndef MENU_CONFIG_HPP
#define MENU_CONFIG_HPP

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include <string>

typedef struct MenuConfig
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
    std::string SetupThePath();
    bool ddirectory_exists();
    void ddirectory_create();
    void ddirectory_chkpar(std::string &data);
    void ddirectory_check(std::string &cfgdata);
    bool ddirectory_parse(int idx=0);

} MenuSystemConfig;

#endif // MENUSYSTEMCONFIG_HPP
