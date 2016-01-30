#ifndef __LINKLIST_H
#define __LINKLIST_H

#include <vector>
#include <string>

/**
 * @class list_bar
 * @author Michael Griffin
 * @date 12/20/2015
 * @file linkList.hpp
 * @brief Struct for Listbars
 */
typedef struct list_bar
{
    std::string active_lightbar;
    std::string inactive_lightbar;

} list_bar;

/**
 * @class LinkList
 * @author Blue
 * @date 12/20/2015
 * @file linkList.hpp
 * @brief Manage Lightbar pages and position changes.
 */
class LinkList
{

public:

    int m_current_row;
    int m_top_margin;
    int m_bottom_margin;
    int m_total_lines;
    int m_currnet_line_number;
    int m_current_page;
    int m_total_pages;
    int m_rows_per_page;
    int m_current_selection; // Current Line in Box of Selection.

    // Passing Light bars between Interfaces
    std::vector<list_bar> m_listing;

    LinkList();
    ~LinkList();

    void getVectorList(std::vector<list_bar> listbar);
    std::string drawVectorList(unsigned long page, unsigned long list);

};

# endif
