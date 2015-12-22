#ifndef MENUSYSTEM_HPP
#define MENUSYSTEM_HPP

#include "menu_function.hpp"
#include "link_list.hpp"
#include "menu_config.hpp"
#include "renderer.hpp"
#include "sequence_decoder.hpp"

#include <string>
#include <vector>

class MenuManager
{

public:
    MenuManager(
        std::string          program_path,
        renderer_ptr         renderer,
        sequence_decoder_ptr decoder
    );

    ~MenuManager();

    std::string          m_program_path;
    MenuConfig           m_menu_config;

    sequence_decoder_ptr m_sequence_decoder;
    MenuFunction         m_menu_function;
    renderer_ptr         m_renderer;

    // Dialing Directory
    unsigned long m_lightbar_position;
    int m_directory_top_margin;
    int m_directory_bottom_margin;
    int m_current_page;
    int m_box_size;

    // Class Types for use
    LinkList      m_link_list;

    std::vector<list_bar> m_result;
    //std::vector<TheRenderer::SystemConnection> m_systemConnection;

    // Methods
    std::vector<list_bar> buildDialList();
    void  readinAnsi(std::string FileName, std::string &buff);
    void  parseHeader(std::string FileName);
    //bool  changeTheme(int idx);
    void  readDirectoryListing();

    bool  readDialDirectory();
    void  createDialDirectory();
    void  writeDialDirectory();

    // Setup for Inital Screen
    void  setupDialDirectory();

    // Redraws Pages and Changes.
    void  updateDialDirectory();

    // Take Input from update();
    int  handleMenuUpdates(const std::string &inputSequence);

    // Handles Returned Menu Command From Selection.
    int  handleMenuActions(const std::string &inputSequence);

};

#endif // MENUSYSTEM_HPP
