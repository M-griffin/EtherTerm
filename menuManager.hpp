#ifndef MENUSYSTEM_HPP
#define MENUSYSTEM_HPP

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "linkList.hpp"
#include "menuFunction.hpp"
#include "menuConfig.hpp"
#include "renderer.hpp"

#include <string>
#include <vector>

class MenuManager :
    public MenuConfig
{
public:
    MenuManager() :
        LIGHTBAR_POSITION(0),
        m_directoryTopMargin(1),
        m_directoryBottomMargin(1),
        m_currentPage(0),
        m_boxsize(0)
    { }
    ~MenuManager()
    {
        std::vector<list_bar>()
            .swap(m_result);

        std::vector<TheRenderer::SystemConnection>()
            .swap(m_systemConnection);
    }

// Dialing Directory
    unsigned long LIGHTBAR_POSITION;
    int m_directoryTopMargin;
    int m_directoryBottomMargin;
    int m_currentPage;
    int m_boxsize;

    // Class Types for use
    LinkList      m_linkList;
    MenuFunction  m_menuFunction;

    std::vector<list_bar> m_result;
    std::vector<TheRenderer::SystemConnection> m_systemConnection;

    // Methods
    std::vector<list_bar> buildDialList();
    void  readinAnsi(std::string FileName, std::string &buff);
    void  parseHeader(std::string FileName);
    //bool  changeTheme(int idx);
    void  readDirectoryListing();

    bool  readDialDirectory();
    void  createDialDirectory();
    void  writeDialDirectory();

    void  setupDialDirectory();   // Setup for Inital Screen
    void  updateDialDirectory();  // Redraws Pages and Changes.

    int  handleMenuUpdates(const std::string &inputSequence);     // Take Input from update();

    // Handles Returned Menu Command From Selection.
    int  handleMenuActions(const std::string &inputSequence);

};

#endif // MENUSYSTEM_HPP
