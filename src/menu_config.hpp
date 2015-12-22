#ifndef MENU_CONFIG_HPP
#define MENU_CONFIG_HPP

#include <string>

/**
 * @class MenuConfig
 * @author Michael Griffin
 * @date 12/22/2015
 * @file menu_config.hpp
 * @brief Parses the Menu.INI for screen and system variables.
 */
class MenuConfig
{
public:

    MenuConfig(std::string program_path);
    ~MenuConfig();

    std::string m_program_path;

    std::string m_ini_path;
    std::string m_ini_name;
    int  m_top_margin;
    int  m_bottom_margin;
    std::string m_page_number;
    std::string m_page_total;
    std::string m_more_active;
    std::string m_more_active_text;
    std::string m_more_inactive;
    std::string m_more_inactive_text;
    std::string m_text_color_normal;
    std::string m_text_color_active;
    std::string m_max_systems;
    std::string m_font_set;
    // Theme
    std::string m_ansi_filename;
    std::string m_help_filename;
    std::string m_theme_name;
    std::string m_menu_prompt_text;

    // Message Read .ini File
    std::string SetupThePath();
    bool ddirectory_exists();
    void ddirectory_create();
    std::string ddirectory_chkpar(const std::string &data);
    void ddirectory_check(std::string &cfgdata);
    bool ddirectory_parse(int idx = 0);
};

#endif // MENUSYSTEMCONFIG_HPP
