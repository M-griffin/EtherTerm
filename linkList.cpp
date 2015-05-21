// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "linkList.h"
#include "menuFunction.h"
#include "mainMenuState.h"
#include "ansiParser.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>

using namespace std;

/**
 * MenuFunction Link List Class
 */
LinkList::LinkList()
{
    headNode          = 0;
    currentNode       = 0;
    lastNode          = 0;
    currentRow        = 1;
    topMargin         = 1;
    bottomMargin      = 1;
    totalLines        = 1;
    currnetLineNumber = 1;
    currentPage       = 1;
    totalPages        = 0;
    rowsPerPage       = 0;               // # of Rows on Last Page with Scroll-down
}

LinkList::~LinkList()
{
}

/**
 * Copy message from link lists to <vector> buffer
 */
void LinkList::getVectorList(vector<list_bar> listbar)
{
    // Swap passed with Global
    listing.swap(listbar);
}

/**
 * Wipe Out List Completely & Free Memory.
 */
void LinkList::clearVectorList()
{
    LineRec *tmp;
    currentNode = headNode;
    while(currentNode != 0)
    {
        tmp = currentNode;
        if(currentNode->previousNodeLink == 0) break;
        currentNode = currentNode->previousNodeLink;
        delete tmp;
        tmp = 0;
    }
    currentNode = 0;
    headNode = 0;
    lastNode = 0;
}

/**
 * New Faster Full Screen Interface for Light-bars and Lists
 * This one functions does all start and  page up/down
 * And Handles jumping to different pages.
 */
void LinkList::drawVectorList(unsigned long page, unsigned long list)
{
    std::string stringBuilder;
    char capture[200]= {0};
    currentPage = page;

    // Calculate Box Size and Total Pages
    int boxsize = bottomMargin - topMargin; // Fist Get Box Size
    totalLines = listing.size();
    totalPages = totalLines / boxsize;
    if(totalLines % boxsize > 0)
    {
        ++totalPages;
    }
    if(totalLines <= boxsize)
        totalPages = 1;

    if(totalPages > 1)
    {
        //Now clear the box First
        for(int t = 0; t < boxsize; t++)
        {
            sprintf(capture, "\x1b[%i;%iH\x1b[K", (topMargin)+t, 1);
            stringBuilder += capture;
        }
    }

    // Now Grab as many lines as will fit in the box
    for(int i = 1; i < boxsize+1; i++)
    {
        if(((boxsize*currentPage)+i)-1 >= (signed)listing.size())
            break;

        // If Area has new message rotate output to new light bars.
        if((signed)list+1 == (boxsize*currentPage)+i)
        {
            currentSelection = topMargin+i-1; // Get current place in box to display.
            sprintf(capture, "\x1b[%i;%iH%s", topMargin+i-1, 1, (char *)listing[((boxsize*currentPage)+i)-1].ansiString2.c_str());
        }
        else
        {
            sprintf(capture, "\x1b[%i;%iH%s", topMargin+i-1, 1, (char *)listing[((boxsize*currentPage)+i)-1].ansiString1.c_str());
        }
        stringBuilder += capture;
    }
    // Write out Box.
    sprintf(capture, "\x1b[%i;%iH", currentRow+topMargin-1, 1);
    stringBuilder += capture;
    MenuFunction::sequenceToAnsi((char *)stringBuilder.c_str());
}
