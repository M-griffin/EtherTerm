#ifndef MENUSYSTEM_HPP
#define MENUSYSTEM_HPP

#include "menu_function.hpp"
#include "link_list.hpp"
#include "menu_config.hpp"
#include "static_methods.hpp"

#include <string>
#include <vector>
#include <memory>


// Forward Declarations.
class SequenceDecoder;
typedef std::shared_ptr<SequenceDecoder> sequence_decoder_ptr;

class Renderer;
typedef std::shared_ptr<Renderer> renderer_ptr;

class SystemEntry;
typedef std::shared_ptr<SystemEntry> system_entry_ptr;


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
    std::vector<SystemEntry>      m_system_entries;

    /**
     * @brief Helper Function to Handle left and right padding
     * @return
     */
    std::string padString(const std::string &value, const std::string &justify, int padding);

    std::vector<list_bar> buildDialList();
    void  readinAnsi(std::string file_name, std::string &buff);
    void  parseHeader(std::string file_name);
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
    int  handleMenuUpdates(const std::string &input_sequence);

    // Handles Returned Menu Command From Selection.
    int  handleMenuActions(const std::string &input_sequence);

};

class MenuManager;
typedef std::shared_ptr<MenuManager> menu_manager_ptr;

#endif // MENUSYSTEM_HPP
