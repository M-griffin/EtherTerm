#ifndef __LINKLIST_H
#define __LINKLIST_H

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include <vector>
#include <string>

using namespace std;

typedef struct list_bar
{
    std::string ansiString1;
    std::string ansiString2;

} list_bar;

typedef struct LineRec
{
    std::string data;
    LineRec     *nextNodeLink;
    LineRec     *previousNodeLink;

} LineRec;

typedef struct LinkList
{

public:
    // Main List for Holding All Data
    LineRec *headNode;
    LineRec *currentNode;
    LineRec *lastNode;

    // Passing Light bars between Interfaces
    std::vector<list_bar> listing;

    int currentRow;
    int topMargin;
    int bottomMargin;
    int totalLines;
    int currnetLineNumber;
    int currentPage;
    int totalPages;
    int rowsPerPage;
    int currentSelection; // Current Line in Box of Selection.

    LinkList();
    ~LinkList();

    void getVectorList(std::vector<list_bar> listbar);
    void clearVectorList();
    void drawVectorList(unsigned long page, unsigned long list);

} LinkList;

# endif
