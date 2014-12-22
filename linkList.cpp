// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "linkList.h"
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

    head = 0;
    current_node = 0;
    last = 0;

    Row      = 1;
    Top      = 1;
    Bot      = 1;
    Tot      = 1;
    Num      = 1;
    Lines    = 0;
    Page     = 1;
    TotPages = 0;
    pgRows   = 0;               // # of Rows on Last Page with ScrollDown
    quote    = FALSE;           // Turn on Colors for Quote Windows
    reader   = FALSE;           // Turn on Colors for Message Reader
    arealist = FALSE;
    PAGE_SCROLL = TRUE;         // Page Down Scrolling in List.
    CLEAR_BOX   = TRUE;         // Clear Box. Default.
    strcpy(LoColor,"");            // Message Text Color
    strcpy(HiColor,"");            // Message Text HiLight Color
    strcpy(TEXT_COLOR,"");      // Message Text Color
    strcpy(QUOTE_COLOR,"");     // Message Quote
    strcpy(SYS_COLOR,"");       // --- BBS Version
    strcpy(ORIGIN_COLOR,"");    //  * Origin Line ...
}

LinkList::~LinkList()
{
}

/**
 * MenuFunction Link List - Setup Title & Area/User Lists
 */
void LinkList::initList(std::string sLow, int alist)
{

    arealist = alist;       // Set if were using Area List
    // Mainly for Box_Start() Proper list placement.
    strcpy(LoColor,(char *)sLow.c_str());   // Message Text Color
}


/**
 * Copy message from link lists to <vector> buffer
 */
void LinkList::GetVector(vector<list_bar> listb)
{
    // Swap passed with Global
    listing.swap(listb);
}


/**
 * Remove all enteries in list
 */
void LinkList::dispose()
{

    //errlog2((char *)"dispose readll");

    dispose_list();
    //errlog2((char *)"dispose readll done!");
    return;

    /*
    int Total = line_total();

    if (current_node == 0) return;
    current_node = last;

    // Delete All the Lines
    for ( ; Total != 0; Total--) {
        Num = Total;
        delete_line_up();
    }
    //errlog2((char *)"dispose readll done!");
    */
}


/**
 * Wipe Out List Completely & Free Memory.
 */
void LinkList::dispose_list()
{

    //errlog2((char *)"dispose_list readll");

    LineRec *tmp;
    current_node = head;

    while(current_node != 0)
    {
        tmp = current_node;

        if (current_node->dn_link == 0) break;
        current_node = current_node->dn_link;
        delete tmp;
        tmp = 0;


    }
    //delete head;
    //delete last;
    current_node = 0;
    head = 0;
    last = 0;
}


// Merging all 3 Functions into a single one that takes care of listings.
// No longer need Row up or Down, universal accross all Lists.
// Previous functions are only kept for references!

/**
 * New Faster Full Screen Interface for Lightbars and Lists
 * This one fucntions does all start and  pageup/down
 * And Handles jumping to different pages.
 */
void LinkList::box_start_vector(unsigned long page, unsigned long list)
{
    std::string _editbox = LoColor, lData;
    char capture[200]= {0};

    Page = page;

    // Calcuate Box Size and Total Pages
    int boxsize = Bot - Top; // Fist Get Box Size
    Tot = listing.size();
    TotPages = Tot / boxsize;
    if (Tot % boxsize > 0)
    {
        ++TotPages;
    }
    if (Tot <= boxsize)
        TotPages = 1;


    // This doesn't work in full screen message read?!?!?!
    // Testing, only clear the box, if total pages > 1,
    // Otherwise no need to clear since it's fresh draw.
    if (TotPages > 1)
    {
        //Now clear the box First
        for (int t = 0; t < boxsize; t++)
        {
            sprintf(capture, "\x1b[%i;%iH\x1b[K", (Top)+t, 1);
            _editbox += capture;
        }
    }

    // Now Grab as many lines as will fit in the box
    for (int i = 1; i < boxsize+1; i++)
    {
        if (((boxsize*Page)+i)-1 >= (signed)listing.size()) break;

        // If Area has new message rotate output to new lightbars.
        if (listing[((boxsize*Page)+i)-1].isnew)
        {
            if ((signed)list+1 == (boxsize*Page)+i)
            {
                current_selection = Top+i-1; // Get current place in box to display.
                sprintf(capture, "\x1b[%i;%iH%s", Top+i-1, 1, (char *)listing[((boxsize*Page)+i)-1].ansi_4.c_str());
            }
            else
            {
                sprintf(capture, "\x1b[%i;%iH%s", Top+i-1, 1, (char *)listing[((boxsize*Page)+i)-1].ansi_3.c_str());
            }
        }
        else
        {
            if ((signed)list+1 == (boxsize*Page)+i)
            {
                current_selection = Top+i-1; // Get current place in box to display.
                sprintf(capture, "\x1b[%i;%iH%s", Top+i-1, 1, (char *)listing[((boxsize*Page)+i)-1].ansi_2.c_str());
            }
            else
            {
                sprintf(capture, "\x1b[%i;%iH%s", Top+i-1, 1, (char *)listing[((boxsize*Page)+i)-1].ansi_1.c_str());
            }
        }

        _editbox += capture;
    }

    // Write out Box.
    sprintf(capture, "\x1b[%i;%iH", Row+Top-1, 1);
    _editbox += capture;
    TheAnsiParser::Instance()->pipe2ansi((char *)_editbox.c_str());

}



