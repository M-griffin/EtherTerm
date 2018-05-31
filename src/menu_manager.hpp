#ifndef MENUSYSTEM_HPP
#define MENUSYSTEM_HPP

#include "menu_function.hpp"
#include "link_list.hpp"
#include "menu_config.hpp"

#include <string>
#include <vector>
#include <memory>


// Forward Declarations.
class SequenceDecoder;
typedef std::shared_ptr<SequenceDecoder> sequence_decoder_ptr;

class Renderer;
typedef std::shared_ptr<Renderer> renderer_ptr;

/**
 * @class SystemConnection
 * @author Michael Griffin
 * @date 12/22/2015
 * @file menu_manager.hpp
 * @brief Connection data pulled from XML
 */
class SystemConnection
{
public:
    std::string name;
    std::string ip;
    int         port;
    std::string protocol;
    std::string login;
    std::string password;  // Make this encrypted with more time.
    std::string font;
    std::string keyMap;    // ANSI, VT100 etc..
    std::string termType;  // ANSI, XTERM etc..
    std::string termSize;      // 80x25, 80x50

};

typedef std::shared_ptr<SystemConnection> system_connection_ptr;


/**
 * @class MenuManager
 * @author Michael Griffin
 * @date 12/22/2015
 * @file menu_manager.hpp
 * @brief Handle the Menu System / Dialing Directory interface.
 */
class MenuManager
{

public:
    MenuManager(
        const std::string    &program_path,
        renderer_ptr         &renderer,
        sequence_decoder_ptr &decoder
    );

    ~MenuManager();

    std::string          m_program_path;
    MenuConfig           m_menu_config;

    sequence_decoder_ptr m_sequence_decoder;
    MenuFunction         m_menu_function;
    renderer_ptr         m_renderer;

    // Dialing Directory
    unsigned long        m_lightbar_position;
    int                  m_directory_top_margin;
    int                  m_directory_bottom_margin;
    int                  m_current_page;
    int                  m_box_size;
    int                  m_current_theme_index;

    // Class Types for use
    LinkList                      m_link_list;
    std::vector<list_bar>         m_result;
    std::vector<SystemConnection> m_systemConnection;

    /**
     * @brief Helper to append folder to path
     */
    std::string getAssetPath();

    /**
     * @brief Helper to append directory path
     */
    std::string getDirectoryPath();

    /**
     * @brief Helper to append ansi path
     */
    std::string getAnsiPath();

    /**
     * @brief Helper Function to Handle left and right padding
     * @return
     */
    std::string padString(const std::string &value, const std::string &justify, int padding);

    std::vector<list_bar> buildDialList();
    void  readinAnsi(std::string FileName, std::string &buff);
    void  parseHeader(std::string FileName);
    bool  changeTheme(int idx);
    void  readDirectoryListing();

    bool  readDialDirectory();
    void  createDialDirectory();
    void  writeDialDirectory();

    // Setup for Initial Screen
    void  setupDialDirectory();

    // Redraws Pages and Changes.
    void  updateDialDirectory();

    // Take Input from update();
    int  handleMenuUpdates(const std::string &inputSequence);

    // Handles Returned Menu Command From Selection.
    int  handleMenuActions(const std::string &inputSequence);

};

class MenuManager;
typedef std::shared_ptr<MenuManager> menu_manager_ptr;

#endif // MENUSYSTEM_HPP
