#ifndef __LINKLIST_H
#define __LINKLIST_H

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include <unistd.h>
#include <sys/types.h>
#include <vector>
#include <string>

using namespace std;

typedef struct list_bar
{
    uint16_t     x; // Screen Cords
    uint16_t     y; // Screen Cord
    uint32_t isnew; // has new / or count of total.
    uint32_t msgnum;
    uint32_t msgid;  // CRC32
    uint32_t repid;  // CRC32
    std::string ansi_1;
    std::string ansi_2;
    std::string ansi_3;
    std::string ansi_4;

} list_bar;


typedef struct LineRec
{
    std::string data;      // Main Data String
    uint16_t    flag;      // Message Quote, Text Flagged True/False
    uint16_t    lineNum;   // Current Line Number
    LineRec     *up_link;
    LineRec     *dn_link;

} LineRec;

typedef struct AreaRec
{
    list_bar lbar;
    struct AreaRec *next;

} AreaRec;


typedef struct LinkList
{

public:

    unsigned long CURRENT_AREA;

    // Main List for Holding All Data
    LineRec *head;
    LineRec *current_node;
    LineRec *last;
    AreaRec *node;

    // Passing Lightbars between Interfaces
    std::vector<list_bar> listing;

    int Row;
    int Top;
    int Bot;
    int Tot;
    int Num;
    int Lines;
    int Page;
    int TotPages;
    int reader;
    int quote;
    int arealist;
    int pgRows;

    int headerrows;
    int current_selection; // Current Line in Box of Selection.

    int PAGE_SCROLL;
    int CLEAR_BOX;

    // Quote and Title Scans
    char LoColor[10];
    char HiColor[10];

    // Message Reader
    char TEXT_COLOR[10];
    char QUOTE_COLOR[10];
    char SYS_COLOR[10];
    char ORIGIN_COLOR[10];

    LinkList();
    ~LinkList();

    void initList(std::string, int alist);
    void GetVector(std::vector<list_bar> listb);

    void dispose();
    void dispose_list();

    void box_start_vector(unsigned long page, unsigned long list);

} LinkList;

# endif


