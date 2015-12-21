// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "link_list.hpp"
#include "menu_function.hpp"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>

/**
 * MenuFunction Link List Class
 */
LinkList::LinkList() :
    m_current_row(1),
    m_top_margin(0),
    m_bottom_margin(0),
    m_total_lines(1),
    m_currnet_line_number(1),
    m_current_page(1),
    m_total_pages(0),
    m_rows_per_page(0),
    m_current_selection(0)
{ }

LinkList::~LinkList()
{
    // Clear
    std::vector<list_bar>().swap(m_listing);
}

/**
 * Copy message from link lists to <vector> buffer
 */
void LinkList::getVectorList(std::vector<list_bar> listbar)
{
    // Swap passed with Global
    m_listing.swap(listbar);
}

/**
 * New Faster Full Screen Interface for Light-bars and Lists
 * This one functions does all start and  page up/down
 * And Handles jumping to different pages.
 */
std::string LinkList::drawVectorList(unsigned long page, unsigned long list)
{
    std::string stringBuilder;
    char capture[200]= {0};
    m_current_page = page;

    // Calculate Box Size and Total Pages
    if (m_bottom_margin == 0 && m_top_margin == 0)
    {
        std::cout << "Error: LinkList Margins not set!" << std::endl;
        return "";
    }
    int boxsize = m_bottom_margin - m_top_margin; // Fist Get Box Size
    m_total_lines = m_listing.size();
    m_total_pages = m_total_lines / boxsize;

    if(m_total_lines % boxsize > 0)
    {
        ++m_total_pages;
    }
    if(m_total_lines <= boxsize)
        m_total_pages = 1;

    if(m_total_pages > 1)
    {
        //Now clear the box First
        for(int t = 0; t < boxsize; t++)
        {
            sprintf(capture, "\x1b[%i;%iH\x1b[K", (m_top_margin)+t, 1);
            stringBuilder += capture;
        }
    }

    // Now Grab as many lines as will fit in the box
    for(int i = 1; i < boxsize+1; i++)
    {
        if(((boxsize*m_current_page)+i)-1 >= (signed)m_listing.size())
            break;

        // If Area has new message rotate output to new light bars.
        if((signed)list+1 == (boxsize*m_current_page)+i)
        {
            m_current_selection = m_top_margin+i-1; // Get current place in box to display.
            sprintf(capture, "\x1b[%i;%iH%s", m_top_margin+i-1, 1,
                (char *)m_listing[((boxsize*m_current_page)+i)-1]
                    .inactive_lightbar.c_str());
        }
        else
        {
            sprintf(capture, "\x1b[%i;%iH%s", m_top_margin+i-1, 1,
                (char *)m_listing[((boxsize*m_current_page)+i)-1]
                    .active_lightbar.c_str());
        }
        stringBuilder += capture;
    }
    // Write out Box.
    sprintf(capture, "\x1b[%i;%iH", m_current_row+m_top_margin-1, 1);
    stringBuilder += capture;
    return stringBuilder;
}
