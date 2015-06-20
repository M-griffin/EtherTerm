// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "linkList.hpp"
#include "menuFunction.hpp"
#include "sequenceManager.hpp"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>

/**
 * MenuFunction Link List Class
 */
LinkList::LinkList() :
    m_currentRow(1),
    m_topMargin(0),
    m_bottomMargin(0),
    m_totalLines(1),
    m_currnetLineNumber(1),
    m_currentPage(1),
    m_totalPages(0),
    m_rowsPerPage(0),
    m_currentSelection(0)
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
 * Wipe Out List Completely & Free Memory.
 *
 * Not used at This Time
 *
void LinkList::clearVectorList()
{
    std::vector<list_bar>().swap(m_listing);
}
*/

/**
 * New Faster Full Screen Interface for Light-bars and Lists
 * This one functions does all start and  page up/down
 * And Handles jumping to different pages.
 */
void LinkList::drawVectorList(unsigned long page, unsigned long list)
{
    std::string stringBuilder;
    char capture[200]= {0};
    m_currentPage = page;

    // Calculate Box Size and Total Pages
    if (m_bottomMargin == 0 && m_topMargin == 0)
    {
        std::cout << "Error: LinkList Margins not set!" << std::endl;
        return;
    }
    int boxsize = m_bottomMargin - m_topMargin; // Fist Get Box Size
    m_totalLines = m_listing.size();
    m_totalPages = m_totalLines / boxsize;

    if(m_totalLines % boxsize > 0)
    {
        ++m_totalPages;
    }
    if(m_totalLines <= boxsize)
        m_totalPages = 1;

    if(m_totalPages > 1)
    {
        //Now clear the box First
        for(int t = 0; t < boxsize; t++)
        {
            sprintf(capture, "\x1b[%i;%iH\x1b[K", (m_topMargin)+t, 1);
            stringBuilder += capture;
        }
    }

    // Now Grab as many lines as will fit in the box
    for(int i = 1; i < boxsize+1; i++)
    {
        if(((boxsize*m_currentPage)+i)-1 >= (signed)m_listing.size())
            break;

        // If Area has new message rotate output to new light bars.
        if((signed)list+1 == (boxsize*m_currentPage)+i)
        {
            m_currentSelection = m_topMargin+i-1; // Get current place in box to display.
            sprintf(capture, "\x1b[%i;%iH%s", m_topMargin+i-1, 1,
                (char *)m_listing[((boxsize*m_currentPage)+i)-1]
                    .ansiString2.c_str());
        }
        else
        {
            sprintf(capture, "\x1b[%i;%iH%s", m_topMargin+i-1, 1,
                (char *)m_listing[((boxsize*m_currentPage)+i)-1]
                    .ansiString1.c_str());
        }
        stringBuilder += capture;
    }
    // Write out Box.
    sprintf(capture, "\x1b[%i;%iH", m_currentRow+m_topMargin-1, 1);
    stringBuilder += capture;
    MenuFunction::sequenceToAnsi(stringBuilder);
}
