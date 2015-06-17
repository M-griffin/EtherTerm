#ifndef __LINKLIST_H
#define __LINKLIST_H

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include <vector>
#include <string>

typedef struct list_bar
{
    std::string ansiString1;
    std::string ansiString2;

} list_bar;


class LinkList
{

public:    

    int m_currentRow;
    int m_topMargin;
    int m_bottomMargin;
    int m_totalLines;
    int m_currnetLineNumber;
    int m_currentPage;
    int m_totalPages;
    int m_rowsPerPage;
    int m_currentSelection; // Current Line in Box of Selection.

    // Passing Light bars between Interfaces
    std::vector<list_bar> m_listing;

    LinkList();
    ~LinkList();

    void getVectorList(std::vector<list_bar> listbar);
    void clearVectorList();
    void drawVectorList(unsigned long page, unsigned long list);

};

# endif
