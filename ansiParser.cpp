
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "socketHandler.h"
#include "inputHandler.h"
#include "ansiParser.h"
#include "terminal.h"

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <stdint.h>

// Unicode Output Encoding.
#include <iostream> // cout
#include <clocale>  // locale
#include <cwchar>   // wchar_t wide characters
#include <string>   // string and wstring
#include <fstream>
#include <vector>
#include <sstream>


using namespace std;


/*
typedef struct myscreen
{
    std::string esc_sequence;
    std::string color_sequence;
    unsigned char c;
    int x_pos;
    int y_pos;

} myscreen;

myscreen screen_buff;
std::vector < myscreen > ansi_screen_buffer;
*/


/**
 * handle screen buffer
 * This function Insert Char a string position,
 * If position doesn't exist yet, it allocate
 * or Pad until we read the position.
 *
void screenbuffer(unsigned char c)
{

    // Keep track of the lonest line in buffer for Centering screen.
    if (x_position > max_x_position)
        max_x_position = x_position;

    screen_buff.c = c;
    screen_buff.x_pos = x_position;
    screen_buff.y_pos = y_position;

     // Move Cursor to next position after character insert.
    if (x_position > characters_per_line)
    {
        // Move to next line
        x_position = 1;
        ++y_position;
    }
    else
    {
        ++x_position;
    }

    ansi_screen_buffer.push_back(screen_buff);

    // Clear for next sequences.
    screen_buff.c = '\0';
    screen_buff.x_pos = 1;
    screen_buff.y_pos = 1;
    screen_buff.color_sequence.erase();
    screen_buff.esc_sequence.erase();

}
*/

AnsiParser* AnsiParser::globalInstance = 0;

AnsiParser::AnsiParser() :
x_position(1),
y_position(1),
max_x_position(0),
characters_per_line(80),
position(0),
has_scrolled(0),
saved_cursor_x(1),
saved_cursor_y(1),
color_attribute(0),
prev_color_attribute(0),
cleared_the_screen(0)

{ }




/**
 * Right String Padding
 */
void AnsiParser::rightSpacing(char *str, int space)   // Pad Right
{

    //elog2("%i rspace",space);
    if (space == 0) return;

    std::string Line = str;
    int s = Line.size();
    // if Line > Sapce, Erase to Make it match!
    if (s >= space)
    {
        Line.erase(space,(s-space));
        strcpy(str,Line.c_str());
        return;
    }
    Line = "";
    for(int i = 0; i < (space-s); i++)
        Line += ' ';
    strcat(str,Line.c_str());
}

/**
 * Left String Padding
 */
void AnsiParser::leftSpacing(char *str, int space)   // Pad Left
{

    //elog2("%i lspace",space);
    if (space == 0) return;

    std::string Line = "";
    int s = strlen(str);
    // if Line > Sapce, Erase to Make it match!
    if (s > space)
    {
        Line.erase(space,(s-space));
        strcpy(str,Line.c_str());
        return;
    }
    for(int i = 0; i < (space-s); i++)
        Line += ' ';
    Line += str;
    strcpy(str,Line.c_str());
}


/**
 * Mask Input
 */
void AnsiParser::mask(char *str)
{
    std::string Line = "";
    int s = strlen(str);

    for(int i = 0; i < s; i++)
        Line += '*';
    strcpy(str,Line.c_str());
}


/**
 * Sets up input field with background color
 *
 */
void AnsiParser::inputfield(char *text, int &len)
{
    std::string repeat;
    std::string temp;
    char sTmp[3]  = {0};
    char sTmp2[3] = {0};
    // Parse for Input String Modifiers
    temp = text;
    int tLen, id1;

    bool inc = false; //found input color

    // Overide Input Length for Ansi
    id1 = temp.find("|IN",0);
    if (id1 != -1)
    {
        sTmp[0] = temp[id1+3];
        sTmp[1] = temp[id1+4];
        temp.erase(id1,5);
        tLen = atoi( sTmp );
        if (tLen < len) len = tLen; // Set new Length
    }

    // Overide Foreground/Background Input Field Colors
    id1 = -1;
    id1 = temp.find("|FB",0);
    if (id1 != -1)
    {
        memset(&sTmp,0,3);
        sTmp[0] = temp[id1+3]; // Foreground 00-15
        sTmp[1] = temp[id1+4];

        memset(&sTmp2,0,3);
        sTmp2[0] = temp[id1+5]; // Background 16-23
        sTmp2[1] = temp[id1+6];
        temp.erase(id1,7);

        sprintf(INPUT_COLOR,"|%s|%s",sTmp,sTmp2);
        inc = true;
    }

    for(int i = 0; i != len; i++)
    {
        repeat += "±";
    }

    if (!inc)
        sprintf(INPUT_COLOR,"|15|17");

    sprintf(text,"%s%s%s\x1b[%iD",(char *)temp.c_str(),INPUT_COLOR,repeat.c_str(),len);
    // Restore back to Default.

}

/*
 * Get Single Key Input.
 */
int AnsiParser::getkey()
{
    std::string inputSequence;
    while (!TheInputHandler::Instance()->isGlobalShutdown())
    {
        if(TheInputHandler::Instance()->update())
        {
            inputSequence = TheInputHandler::Instance()->getInputSequence();
            break;
        }
        else
        {
            SDL_Delay(10);
        }
    }

    // If Global Exit, return right away.
    if (TheInputHandler::Instance()->isGlobalShutdown())
    {
        return EOF;
    }

    return inputSequence[0];
}

/*
 * Get Input up to <ENTER>
 */
void AnsiParser::getline(char *line,     // Returns Input into Line
                         int   length,   // Max Input Length of String
                         char *leadoff,  // Data to Display in Default String {Optional}
                         int   hid,      // If input is Echomail as hidden    {Optional}
                         char *chlist)   // Valid Input Char List             {Optional}
{
    int c, cc;
    int i   = 0;
    int Col = 0;
    int EscHit;
    std::string tLine;
    std::string output;
    char sLine[200]  = {0};
    char sList[200]  = {0};

    std::string input;
    std::string inputSequence;

    #define DEL 0x7f

    // If were starting Off Input with a String already in buffer!  display it!
    if (leadoff != 0)
    {
        input = leadoff;
        i = input.size();
        Col = i;
        ansiparse((unsigned char *)input.c_str(),0);
    }

    // This is a list of Valid inPut Keys
    // We will only grab chars in this list
    // Only if a list is passed to the Function.
    if (chlist != 0)
    {
        // Append ESC, CTRL Y, BACKSPACE, ENTER
        // For Line Movement and End of Line Keys so they are always valid input.
        sprintf(sLine,"%c%c%c%c%c%c%c",(char)27,(char)25,(char)0x7f,(char)0x08,(char)10,char(8),char(207));
        //strcat(chlist,sLine);
        sprintf(sList,"%s%s",chlist,sLine);
    }
    while (!TheInputHandler::Instance()->isGlobalShutdown())
    {
        if(TheInputHandler::Instance()->update())
        {
            inputSequence = TheInputHandler::Instance()->getInputSequence();
        }
        else
        {
            SDL_Delay(10);
            continue;
        }
        c = inputSequence[0];

        if (c == '\r') c = '\n';

        // Valid Key List Checking....
        if (chlist != 0)
        {
            //if (strlen(sList) > 0) {
            if(c >= 'a' && c <= 'z')
            {
                c = toupper(c);
            }
            if(strchr(sList,c) || c == '\n' || c == '\r' || (int)c == 10)
            {
                //continue;
            }
            else
            {
                continue;
            }
            //}
        }
        // Escape in this case, ignore, later add movement in string
        if ((int)c == 27)
        {
            if (inputSequence.size() >= 3)
                cc = inputSequence[2];
            EscHit = true;
        }
        else
            EscHit = false;

        // Catch all Escaped Keys for Cursor Movement
        if (EscHit)
        {
            switch (cc)
            {
            case '3' : // Delete
                if (i != 0 || Col != 0)
                {
                    ansiparse((unsigned char *)"\x1b[D±\x1b[D",0);
                    input.erase(Col-1,1);
                    --i;
                    --Col;
                }
                break;

            default :
                break;
            }

        }
        else if ((int)c == 25)
        {
            // CTRL Y - Clear Line
            input.erase();
            i = Col;
            for (; i != 0; i--)
            {
                ansiparse((unsigned char *)"\x1b[D±\x1b[D",0);
            }
            i = 0;
            Col = i;
        }

        // delete 127
        // Do destructive backspace
        // on VT100 Terms 127 DEL == BS!
        // Since we have no DELETE in this, delete on 1 liens will works like BS.
        else if ((int)c == 0x08 || (int)c == 127 || int(c) == 8 || int(c) == 207 || (int)c == 0x7f)
        {
            //|| int(c) == 255 || int(c) == 0x0e00) {
            if (i != 0 || Col != 0)
            {
                ansiparse((unsigned char *)"\x1b[D±\x1b[D",0);
                input.erase(Col-1,1);
                --i;
                --Col;
            }
        }
        // Normal Key Input, Letters & numbers
        else if((int)c > 31 && (int)c < 126)
        {
            if (i != length-1)
            {
                if (hid)
                {
                    output = '*';
                    ansiparse((unsigned char *)output.c_str(),0);
                }
                else
                {
                    output = c;
                    ansiparse((unsigned char *)output.c_str(),0);
                }
                input += c;
                ++i;
                ++Col;
            }
        }
        else if (c == 10 || c == '\r' || c == '\n')
        {
            input += '\0';
            strncpy(line,(char *)input.c_str(),length);
            break;
        }
    }

    // If Global Exit, return right away.
    if (TheInputHandler::Instance()->isGlobalShutdown())
    {
        return;
    }

    // Restore Background color after input received.
    Col = 0;
    char sReplace[15]= {0};
    ansi_bg(sReplace, 16);
    pipe2ansi(sReplace,0);
}


/**
 * Set ANSI Foreground colors
 */
void AnsiParser::ansi_fg(char *data, int fg)
{
    switch (fg)
    {
    case 0:
        strcat(data, "x[0;30m");
        break;
    case 1:
        strcat(data, "x[0;34m");
        break;
    case 2:
        strcat(data, "x[0;32m");
        break;
    case 3:
        strcat(data, "x[0;36m");
        break;
    case 4:
        strcat(data, "x[0;31m");
        break;
    case 5:
        strcat(data, "x[0;35m");
        break;
    case 6:
        strcat(data, "x[0;33m");
        break;
    case 7:
        strcat(data, "x[0;37m");
        break;
    case 8:
        strcat(data, "x[1;30m");
        break;
    case 9:
        strcat(data, "x[1;34m");
        break;
    case 10:
        strcat(data, "x[1;32m");
        break;
    case 11:
        strcat(data, "x[1;36m");
        break;
    case 12:
        strcat(data, "x[1;31m");
        break;
    case 13:
        strcat(data, "x[1;35m");
        break;
    case 14:
        strcat(data, "x[1;33m");
        break;
    case 15:
        strcat(data, "x[1;37m");
        break;
    default :
        break;
    }
    data[0] = '\x1b';
}

/**
 * Set ANSI Background colors
 */
void AnsiParser::ansi_bg(char *data, int bg)
{
    switch (bg)
    {
    case 16:
        strcat(data, "x[40m");
        break;
    case 17:
        strcat(data, "x[44m");
        break;
    case 18:
        strcat(data, "x[42m");
        break;
    case 19:
        strcat(data, "x[46m");
        break;
    case 20:
        strcat(data, "x[41m");
        break;
    case 21:
        strcat(data, "x[45m");
        break;
    case 22:
        strcat(data, "x[43m");
        break;
    case 23:
        strcat(data, "x[47m");
        break;
        // Default to none.
    case 24:
        strcat(data, "x[0m");
        break;
    default :
        break;
    }
    data[0] = '\x1b';
}


/**
 * Main Output for All String Data, Parses PIPE color and MCI Codes.
 *
 */
void AnsiParser::pipe2ansi(char* szString, int buffer)
{

    //std::cout << "\r\n*** pipe2ansi: " << std::endl;

    std::string::size_type  id1 = 0, test;    // Pipe Position
    char szTmp[3];         // Holds 1, 2nd digit of Pipe

    char szPos1[3];        // Hold XY Pos for Ansi Postion Codes
    char szPos2[3];        // Hold XY Pos for Ansi Postion Codes
    char szReplace[1024];  // Holds Converted Pipe 2 Ansi

    std::string fTmp;
    std::string AnsiString = szString;

    #define SP 0x20

    // Search for First Pipe
    id1 = 0;
    while (id1 != std::string::npos)
    {
        id1 = AnsiString.find("|",id1);

        if (id1 != std::string::npos)
        {
            memset(&szTmp,0,sizeof(szTmp));
            memset(&szReplace,0,sizeof(szReplace));
            szTmp[0] = AnsiString[id1+1];  // Get First # after Pipe
            szTmp[1] = AnsiString[id1+2];  // Get Second Number After Pipe
            if (szTmp[0] == '\0' || szTmp[0] == '\r' || szTmp[0] == EOF) break;
            if (szTmp[1] == '\0' || szTmp[1] == '\r' || szTmp[0] == EOF) break;

            //std::cout << "\r\n*** szTmp: " << szTmp << std::endl;

            if (isdigit(szTmp[0]) && isdigit(szTmp[1]))
            {
                switch (szTmp[0])
                {
                    // Parse from pipe to Foreground/back Color Ansi Sequences.
                case '0' :
                    switch (szTmp[1])
                    {
                    case '0' :
                        ansi_fg(szReplace, 0);
                        break;
                    case '1' :
                        ansi_fg(szReplace, 1);
                        break;
                    case '2' :
                        ansi_fg(szReplace, 2);
                        break;
                    case '3' :
                        ansi_fg(szReplace, 3);
                        break;
                    case '4' :
                        ansi_fg(szReplace, 4);
                        break;
                    case '5' :
                        ansi_fg(szReplace, 5);
                        break;
                    case '6' :
                        ansi_fg(szReplace, 6);
                        break;
                    case '7' :
                        ansi_fg(szReplace, 7);
                        break;
                    case '8' :
                        ansi_fg(szReplace, 8);
                        break;
                    case '9' :
                        ansi_fg(szReplace, 9);
                        break;
                    default :
                        ++id1;
                        break;
                    }
                    break;

                case '1' :
                    switch (szTmp[1])
                    {
                    case '0' :
                        ansi_fg(szReplace, 10);
                        break;
                    case '1' :
                        ansi_fg(szReplace, 11);
                        break;
                    case '2' :
                        ansi_fg(szReplace, 12);
                        break;
                    case '3' :
                        ansi_fg(szReplace, 13);
                        break;
                    case '4' :
                        ansi_fg(szReplace, 14);
                        break;
                    case '5' :
                        ansi_fg(szReplace, 15);
                        break;
                    case '6' :
                        ansi_bg(szReplace, 16);
                        break;
                    case '7' :
                        ansi_bg(szReplace, 17);
                        break;
                    case '8' :
                        ansi_bg(szReplace, 18);
                        break;
                    case '9' :
                        ansi_bg(szReplace, 19);
                        break;
                    default :
                        ++id1;
                        break;
                    }
                    break;

                case '2' :
                    switch (szTmp[1])
                    {
                    case '0' :
                        ansi_bg(szReplace, 20);
                        break;
                    case '1' :
                        ansi_bg(szReplace, 21);
                        break;
                    case '2' :
                        ansi_bg(szReplace, 22);
                        break;
                    case '3' :
                        ansi_bg(szReplace, 23);
                        break;
                    default :
                        ++id1;
                        break;
                    }
                    break;

                default :
                    ++id1;
                    break;
                }

                // Replace pipe code with Ansi Sequence
                if (strcmp(szReplace,"") != 0)
                    AnsiString.replace(id1,3,szReplace);
            }
            // Else not a Pipe Color / Parse for Screen Modification
            else if (szTmp[0] == 'C')
            {
                // Carriage Return / New Line
                if (strcmp(szTmp,"CR") == 0)
                {
                    ansi_bg(szReplace, 16);  // Clear Background Attribute first
                    strcat(szReplace,"\r\n");
                    AnsiString.replace(id1,3,szReplace);
                    id1 = 0;
                }
                // Clear Screen
                else if (strcmp(szTmp,"CS") == 0)
                {
                    ansi_bg(szReplace, 16);
                    // Set Scroll Region, Clear Background, Then Home Cursor.
                    //strcat(szReplace,"\x1b[1;25r\x1b[2J\x1b[1;1H");

                    strcat(szReplace,"\x1b[2J\x1b[1;1H");

                    AnsiString.replace(id1,3,szReplace);
                    id1 = 0;
                }
            }
            else
            {
                if (strcmp(szTmp,"XY") == 0)
                {
                    memset(&szPos1,0,sizeof(szPos1));
                    memset(&szPos2,0,sizeof(szPos2));
                    // X Pos
                    szPos1[0] = AnsiString[id1+3];
                    szPos1[1] = AnsiString[id1+4];
                    // Y Pos
                    szPos2[0] = AnsiString[id1+5];
                    szPos2[1] = AnsiString[id1+6];
                    ansi_bg(szReplace, 16);  // Clear Background Attribute first
                    sprintf(szReplace,"%s\x1b[%i;%iH",szReplace, atoi(szPos2),atoi(szPos1));
                    AnsiString.replace(id1,7,szReplace);
                }
                else
                {
                    // End of the Line, nothing parsed out so
                    // Skipp ahead past current code.
                    ++id1;
                }
            }
        }
        else break;
        id1 = AnsiString.find("|",id1);
    }

    ansiparse((unsigned char*)AnsiString.c_str(),0);
}

/*
 * Reset AnsiParser Sepcific Class Attributes
 */
void AnsiParser::reset()
{
    has_scrolled = 0;
    saved_cursor_x = 1;
    saved_cursor_y = 1;
    color_attribute = 0;
    prev_color_attribute = 0;
    cleared_the_screen = 0;

    // Reset Default Colors.
    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->grey;
    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->black;
}



/**
 * Reads in Ansi file into Buffer Only
 * For Testing the Ansi Parser.
 */
void AnsiParser::ansiPrintf(std::string fileName)
{
    //std::cout << "ansiPrintf" << std::endl;
#ifdef _WIN32
    std::string path = "assets\\";
#else
    std::string path = "assets/";
#endif
    path += fileName;

    std::string buff;

    FILE *fp;
    std::string::size_type id1 = 0;

    int c = 0;
    if ((fp = fopen(path.c_str(), "r+")) ==  NULL)
    {
        std::cout << "ansiPrintf - no ansi found: " << fileName << std::endl;
        return;
    }
    do
    {
        c = getc(fp);
        if (c != EOF)
            buff += c;
    }
    while (c != EOF);

    fclose(fp);
    ansiparse((unsigned char *)buff.c_str(),0);
}



/**
 * Main ANSI Parser Function
 * This function handles full ANSI translation to a screen_buffer
 * Plotting position and tracking colors for effects and reposition
 * and rebuilding the ANSI for screen display.
 * Does NOT Parse PIPE codes, they should parsed before sent here.
 */
void AnsiParser::ansiparse(unsigned char *buff, int center_screen)
{
    //std::cout << "\r\n*** Ansi Parser: " << buff << std::endl;

    std::string outchar;
    std::string output_screen; // Finished ANSI Screen
    std::string esc_sequence;

    // Reset Each run.
    position                   = 0;
    characters_per_line        = 80;
    max_x_position             = 0;

    int  param[10]             = {0};

    unsigned char c            = 0;
    int  p = 0, dig = 0;

    char more_params           = 0;
    char at_least_one_digit    = 0;
    char first_param_implied   = 0;

    int  line_wrapping         = false;
    std::string current_color;

    SDL_Color tempColor;          // Holds temp color when doing Inverse.

    // Setup Default Color Sequence Always start 7 FG 0 Background.
    // If position in ansi changes, the next sequence will overwrite it.
    //ansi_color_buffer.push_back (pair<int, string> (position, current_color));

    int starting_seq = 0;
    int ending_seq   = 0;

    std::string currentPosition;    // for ESC[6n reply

    // Toggle on the Renderer.
    TheTerm::Instance()->setRenderReady(true);

    int buflen = strlen((char *)buff);

    for (int z = 0; z < buflen; z++)
    {
        //printf("[%i] z: %i len: %i,",c, z,strlen((char *)buff));
        c = buff[z];

        //printf("%c,",c);

        // Skip over null string termination.
        if (c == '\0')
            continue;

        //printf("[AFTER]");

        //Handle escape sequence
        if (c == ESC)
        {
            // Store entire ESC Sequence
            starting_seq = z;

            //printf("T1,");

            // grab the left bracket
            if (z < buflen)
            {
                c = buff[++z];

                //printf("%c,",c);

                // Ansimation in TheDraw.
                if (z+1 < buflen && buff[z+1] == '7' && buff[z+2] == 'h')
                {
                    line_wrapping = true;
                }
            }

            //printf("T2,");

            more_params = true;
            first_param_implied = false;
            p = 0;

            while (more_params == true)
            {
                //printf("T3,");

                at_least_one_digit = false;
                ++z;
                for (dig = 0; (isdigit (c = buff[z])) && (dig < 3); dig++)
                {

                    //printf("T4,");
                    at_least_one_digit = true;

                    // 3 digits at most (255) in a byte size decimal number */
                    if (dig == 0)
                    {
                        //printf("T5,");
                        param[p] = c - '0';
                    }
                    else if (dig == 1)
                    {
                        //printf("T6,");
                        param[p] *= 10;
                        param[p] += c - '0';
                    }
                    else
                    {
                        //printf("T7,");
                        param[p] *= 100;
                        param[p] += c - '0';
                    }
                    //printf("T8,");
                    z++;
                }

                //printf("T9,");
                //   ESC[C     p should = 0
                //   ESC[6C    p should = 1
                //   ESC[1;1H  p should = 2
                //   ESC[;79H  p should = 2
                if (c != '?')    // Skip Screen Wrap (The Draw)
                {
                    //printf("T10,");
                    if ((at_least_one_digit == true) && (c == ';'))
                    {
                        //printf("T11,");
                        p++;
                    }
                    else if ((!(at_least_one_digit == true)) && (c == ';'))
                    {
                        //printf("T12,");
                        p++;
                        first_param_implied = true;
                    }
                    else if (at_least_one_digit)
                    {
                        //printf("T13,");
                        p++;
                        more_params = false;
                    }
                    else
                    {
                        //printf("T14,");
                        more_params = false;
                    }
                }

                //printf("T15,");

            } // End While (more_params)

            //printf("T16,");

            // Great ESC Sequence in entirity
            ending_seq = z;

            // loop and cut out full ESC Sequence to store it.
            // We only store with input key if it's parsed
            // Otherwise we skip it.
            for (int seq = starting_seq; seq < ending_seq+1; seq++)
                esc_sequence += buff[seq];

            //printf("\r\n ESC SEQ: %s - POS: %i,%i \r\n", (char *)esc_sequence.c_str(), x_position,y_position);

            // Handle specific escape sequences
            switch (c)
            {

            case CURSOR_POSITION:
            case CURSOR_POSITION_ALT:

                //printf("\r\n CURSOR_POSITION: p = %i,  param[0] = %i param[1] = %i \r\n", p,  param[0], param[1]);

                if (p == 0)
                {
                    x_position = 1;
                    y_position = 1;
                }
                else
                if (p == 1)
                {
                    x_position = 1;
                    y_position = param[0];
                    if (y_position > NUM_LINES)
                        y_position = NUM_LINES;
                    else
                    if (y_position < 1)
                        y_position = 1;
                }
                else
                if (first_param_implied)
                {
                    x_position = param[1];
                    if (x_position > characters_per_line)
                        x_position = characters_per_line;

                }
                else
                {
                    x_position = param[1];
                    y_position = param[0];

                    if (y_position > NUM_LINES)
                        y_position = NUM_LINES;
                    else
                    if (y_position < 1)
                        y_position = 1;

                    if (x_position > characters_per_line)
                        x_position = characters_per_line;
                    else
                    if    (x_position < 1 )
                        x_position = 1;

                }

                //screen_buff.esc_sequence += esc_sequence;
                esc_sequence.clear();
                break;

            case SCROLL_REGION:
                if (p == 0)
                {
                    // Reset to Full Screen.
                    TheTerm::Instance()->SetScrollRegion(0, 0);
                    break;
                }
                if (p == 2)
                {
                    // Enable scrolling from row1 to row2
                    TheTerm::Instance()->SetScrollRegion(param[0], param[1]);

                    // Move the cursor to the home position
                    x_position = 1;
                    y_position = 1;
                    break;
                }


            case CURSOR_PREV_LIVE:
            case CURSOR_UP:
                if (p == 0)
                {
                    if (y_position > 1)
                        --y_position;
                }
                else
                {
                    y_position -= param[0];
                    if (y_position < 1)
                        y_position = 1;
                }

                //screen_buff.esc_sequence += esc_sequence;
                esc_sequence.clear();

                break;

            case CURSOR_NEXT_LINE:
            case CURSOR_DOWN:
                if (p == 0)
                {
                    if (y_position < NUM_LINES)
                        ++y_position;
                }
                else
                {
                    y_position += param[0];
                    if (y_position > NUM_LINES)
                        y_position = NUM_LINES;
                }
                //screen_buff.esc_sequence += esc_sequence;
                esc_sequence.clear();
                break;

            case CURSOR_FORWARD:
                if (p == 0)
                {
                    //if (x_position < characters_per_line)
                    ++x_position;
                    if (x_position > characters_per_line)
                    {
                        x_position -= characters_per_line;
                        ++y_position;
                    }
                }
                else
                {
                    x_position += param[0];
                    // Some programs push ESC[C past the end
                    // of the line and expect us to loop and move
                    // Down Automatically.
                    if (x_position > characters_per_line)
                    {
                        x_position -= characters_per_line;
                        ++y_position;
                    }
                }
                //screen_buff.esc_sequence += esc_sequence;
                esc_sequence.clear();
                break;

            case CURSOR_BACKWARD:
                if (p == 0)
                {
                    if (x_position > 1)
                    {
                        --x_position;
                    }
                }
                else
                {
                    x_position -= param[0];
                    if (x_position < 1)
                        x_position = 1;
                }
                //screen_buff.esc_sequence += esc_sequence;
                esc_sequence.clear();
                break;

            case SAVE_CURSOR_POS:
                saved_cursor_x = x_position;
                saved_cursor_y = y_position;
                //screen_buff.esc_sequence += esc_sequence;
                esc_sequence.clear();
                break;

            case RESTORE_CURSOR_POS:
                x_position = saved_cursor_x;
                y_position = saved_cursor_y;
                //screen_buff.esc_sequence += esc_sequence;
                esc_sequence.clear();
                break;

            case CURSOR_X_POSITION: // XTERM
                if (p == 0)
                    x_position = 1;
                else
                {
                    x_position = param[0];
                    if (x_position < 1)
                        x_position = 1;
                    else if (x_position > characters_per_line)
                        x_position = characters_per_line;
                }
                //screen_buff.esc_sequence += esc_sequence;
                esc_sequence.clear();
                break;

            case ERASE_DISPLAY:
                //printf("\r\n\r\n CLEARTHESCREEN!! param[0] %i, %c \r\n\r\n", param[0],param[0]);
                if(p == 0)
                {
                    TheTerm::Instance()->ClearScreenSurface();

                    cleared_the_screen = true;

                    // Home Cursor, some bbs systems don't do this and makes a mess!!
                    x_position = 1;
                    y_position = 1;
                }
                if (p == 1 && param[0] == 2)
                {
                    //printf("\r\n\r\n CLEARTHESCREEN!! \r\n\r\n");
                    TheTerm::Instance()->ClearScreenSurface();

                    cleared_the_screen = true;

                    // Home Cursor, some bbs systems don't do this and makes a mess!!
                    x_position = 1;
                    y_position = 1;

                    //screen_buff.esc_sequence += esc_sequence;
                    esc_sequence.clear();
                }
                else
                    //outf << "ERASE_DISPLAY" << esc_sequence << endl;
                break;

            case ERASE_TO_EOL:
                //screen_buff.esc_sequence += esc_sequence;

                // Handle Rendering from 1 Bases to 0 Based so Subtract 1.
                if (p == 0) // Cursor to End of line
                {
                    //printf("\r\n EOL xpos %i, ypos %i \r\n", x_position, y_position);
                    TheTerm::Instance()->RenderClearLineScreen(y_position-1, x_position-1, characters_per_line); // test removeed -1
                }
                else
                if (p == 1 && param[0] == 1) // Start of Line to Cursor
                    TheTerm::Instance()->RenderClearLineScreen(y_position-1, 0, x_position);
                else
                if (p == 1 && param[0] == 2) // Entire Line
                    TheTerm::Instance()->RenderClearLineScreen(y_position-1, 0, characters_per_line);

                esc_sequence.clear();
                break;

            case SET_GRAPHICS_MODE:
                // Rebuild ESC Sequqnces and Attach Character Position
                // So when we draw out the ansi we can keep track
                // Of what color everything is.

                //esc_sequence.erase(); // Ignore ESC in color, we get that sepeartely.
                current_color.clear();
                //position = ((y_position-1) * characters_per_line) + (x_position-1);

                // Doesn't seem to get here.!
                if (p == 0)   // Change text attributes / All Attributes off
                {
                    //current_color = "\x1b[m";
                    prev_color_attribute = 0;
                    color_attribute = 0;
                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->grey;
                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->black;
                    //screen_buff.color_sequence += current_color;
                }
                else
                {
                    //current_color += "\x1b[";
                    // Current_Color now used to get intenestity or Attribute.

                    for (int i = 0; i < p; i++)
                    {

                        // Setup Iniqal Sequence
                        // if (i != 0)
                        //current_color += ";";

                        switch (param[i])
                        {
                        case 0: // All Attributes off
                            //current_color += "0";
                            prev_color_attribute = 0;
                            color_attribute = 0;

                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            ////printf("\r\nAll Attributes off: %s:%i", esc_sequence.c_str(), color_attribute);

                            // Set default till it's overwridden
                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->grey;
                            TheTerm::Instance()->currentBGColor = TheTerm::Instance()->black;

                            break;

                        case 1: // BOLD_ON (increase intensity)
                            prev_color_attribute = 1;
                            color_attribute = 1;

                            //std::cout << "Color_Attribute: " << color_attribute << endl;

                            // If current color is dark, flip it to light.
                            if (TheTerm::Instance()->CompareSDL_Colors(TheTerm::Instance()->currentFGColor,TheTerm::Instance()->black))
                                TheTerm::Instance()->currentFGColor = TheTerm::Instance()->darkGrey;
                            else
                            if (TheTerm::Instance()->CompareSDL_Colors(TheTerm::Instance()->currentFGColor,TheTerm::Instance()->red))
                                TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightRed;
                            else
                            if (TheTerm::Instance()->CompareSDL_Colors(TheTerm::Instance()->currentFGColor,TheTerm::Instance()->green))
                                TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightGreen;
                            else
                            if (TheTerm::Instance()->CompareSDL_Colors(TheTerm::Instance()->currentFGColor,TheTerm::Instance()->brown))
                                TheTerm::Instance()->currentFGColor = TheTerm::Instance()->yellow;
                            else
                            if (TheTerm::Instance()->CompareSDL_Colors(TheTerm::Instance()->currentFGColor,TheTerm::Instance()->blue))
                                TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightBlue;
                            else
                            if (TheTerm::Instance()->CompareSDL_Colors(TheTerm::Instance()->currentFGColor,TheTerm::Instance()->magenta))
                                TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightMagenta;
                            else
                            if (TheTerm::Instance()->CompareSDL_Colors(TheTerm::Instance()->currentFGColor,TheTerm::Instance()->cyan))
                                TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightCyan;
                            else
                            if (TheTerm::Instance()->CompareSDL_Colors(TheTerm::Instance()->currentFGColor,TheTerm::Instance()->grey))
                                TheTerm::Instance()->currentFGColor = TheTerm::Instance()->white;

                            break;

                        case 2: // FAINT (decreased intensity) (Not widely used).
                            color_attribute = 2;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            break;

                        case 3: // ITALICS ON
                            color_attribute = 3;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            break;

                        case 4:    // UNDERSCORE
                            color_attribute = 4;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            break;

                        case 5: // BLINK_ON
                            color_attribute = 5;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;

                            // Flip Colors for Ice Colors.
                            tempColor = TheTerm::Instance()->currentFGColor;
                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->currentBGColor;
                            TheTerm::Instance()->currentBGColor = tempColor;
                            break;

                        case 6: // BLINK RAPID MS-DOS ANSI.SYS; 150 per minute or more; not widely supported
                            color_attribute = 6;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;
                            break;

                        case 7: // REVERSE_VIDEO_ON
                            color_attribute = 7;
                            //std::cout << "Color_Attribute: " << color_attribute << endl;

                            tempColor = TheTerm::Instance()->currentFGColor;
                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->currentBGColor;
                            TheTerm::Instance()->currentBGColor = tempColor;
                            break;

                        case 8: // CONCEALED_ON
                            current_color+= "8";
                            break;

                        case 9: // STRICKTHROUGH
                            current_color += "9";
                            break;

                        case 22: // BOLD OFF
                            //current_color += "22";
                            prev_color_attribute = 0;
                            color_attribute = 0;
                            //std::cout << "Color_Attribute: BOLD OFF " << color_attribute << endl;
                            break;

                        case 23: // ITALICS OFF
                            current_color += "23";
                            //std::cout << "Color_Attribute: ITALICS OFF " << color_attribute << endl;
                            break;

                        case 24: // UNDERLINE OFF
                            current_color += "24";
                            //std::cout << "Color_Attribute: UNDERLINE OFF " << color_attribute << endl;
                            break;

                        case 27: // INVERSE OFF
                            //current_color += "27";
                            color_attribute = 0;
                            //std::cout << "Color_Attribute: INVERSE OFF " << color_attribute << endl;

                            tempColor = TheTerm::Instance()->currentFGColor;
                            TheTerm::Instance()->currentFGColor = TheTerm::Instance()->currentBGColor;
                            TheTerm::Instance()->currentBGColor = tempColor;
                            break;

                        case 28: // CONCEALED_OFF
                            current_color += "28";
                            break;

                        case 29: // STRIKETHROUGH OFF
                            current_color += "29";
                            break;

                        /* Foreground Colors */
                        case 30: // FG_BLACK

                            switch (color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->black;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->black; break;
                                        case 1:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->darkGrey; break;
                                        default: TheTerm::Instance()->currentBGColor = TheTerm::Instance()->black; break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->black; break;
                                        case 1:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->darkGrey; break;
                                        default: TheTerm::Instance()->currentBGColor = TheTerm::Instance()->black; break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->darkGrey;
                                    break;
                            }
                            break;

                        case 31: // FG_RED

                            switch (color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->red;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->red; break;
                                        case 1:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->lightRed; break;
                                        default: TheTerm::Instance()->currentBGColor = TheTerm::Instance()->red; break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->red; break;
                                        case 1:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->lightRed; break;
                                        default: TheTerm::Instance()->currentBGColor = TheTerm::Instance()->red; break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightRed;
                                    break;
                            }
                            break;

                        case 32: // FG_GREEN
                            //current_color += "32";

                            switch (color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->green;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->green; break;
                                        case 1:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->lightGreen; break;
                                        default: TheTerm::Instance()->currentBGColor = TheTerm::Instance()->green; break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->green; break;
                                        case 1:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->lightGreen; break;
                                        default: TheTerm::Instance()->currentBGColor = TheTerm::Instance()->green; break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightGreen;
                                    break;
                            }
                            break;

                        case 33: // FG_YELLOW

                            switch (color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->brown;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->brown; break;
                                        case 1:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->yellow; break;
                                        default: TheTerm::Instance()->currentBGColor = TheTerm::Instance()->brown; break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->brown; break;
                                        case 1:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->yellow; break;
                                        default: TheTerm::Instance()->currentBGColor = TheTerm::Instance()->brown; break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->yellow;
                                    break;
                            }
                            break;

                        case 34: // FG_BLUE

                            switch (color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->blue;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->blue; break;
                                        case 1:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->lightBlue; break;
                                        default: TheTerm::Instance()->currentBGColor = TheTerm::Instance()->blue; break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->blue; break;
                                        case 1:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->lightBlue; break;
                                        default: TheTerm::Instance()->currentBGColor = TheTerm::Instance()->blue; break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightBlue;
                                    break;
                            }
                            break;

                        case 35: // FG_MAGENTA

                            switch (color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->magenta;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->magenta; break;
                                        case 1:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->lightMagenta; break;
                                        default: TheTerm::Instance()->currentBGColor = TheTerm::Instance()->magenta; break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->magenta; break;
                                        case 1:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->lightMagenta; break;
                                        default: TheTerm::Instance()->currentBGColor = TheTerm::Instance()->magenta; break;
                                    }

                                    break;

                                default:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightMagenta;
                                    break;
                            }
                            break;

                        case 36: // FG_CYAN

                            switch (color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->cyan;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->cyan; break;
                                        case 1:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->lightCyan; break;
                                        default: TheTerm::Instance()->currentBGColor = TheTerm::Instance()->cyan; break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->cyan; break;
                                        case 1:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->lightCyan; break;
                                        default: TheTerm::Instance()->currentBGColor = TheTerm::Instance()->cyan; break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightCyan;
                                    break;
                            }
                            break;

                        case 37: // FG_WHITE

                            switch (color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->grey;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->grey; break;
                                        case 1:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->white; break;
                                        default: TheTerm::Instance()->currentBGColor = TheTerm::Instance()->grey; break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        case 0:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->grey; break;
                                        case 1:  TheTerm::Instance()->currentBGColor = TheTerm::Instance()->white; break;
                                        default: TheTerm::Instance()->currentBGColor = TheTerm::Instance()->grey; break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentFGColor = TheTerm::Instance()->white;
                                    break;
                            }
                            break;


                        /* Background Colors */
                        case 40: // BG_BLACK
                            switch (color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->black;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->black; break;
                                        //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->darkGrey; break;
                                        default: TheTerm::Instance()->currentFGColor = TheTerm::Instance()->darkGrey; break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->black; break;
                                        //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->darkGrey; break;
                                        default: TheTerm::Instance()->currentFGColor = TheTerm::Instance()->black; break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->black;
                                    break;
                            }
                            break;

                        case 41: // BG_RED
                            switch (color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->red;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->red; break;
                                        //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightRed; break;
                                        default: TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightRed; break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->red; break;
                                        //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightRed; break;
                                        default: TheTerm::Instance()->currentFGColor = TheTerm::Instance()->red; break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->red;
                                    break;
                            }
                            break;

                        case 42: // BG_GREEN
                            switch (color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->green;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->green; break;
                                        //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightGreen; break;
                                        default: TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightGreen; break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->green; break;
                                        //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightGreen; break;
                                        default: TheTerm::Instance()->currentFGColor = TheTerm::Instance()->green; break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->green;
                                    break;
                            }
                            break;

                        case 43: // BG_YELLOW
                            switch (color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->brown;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->brown; break;
                                        //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->yellow; break;
                                        default: TheTerm::Instance()->currentFGColor = TheTerm::Instance()->yellow; break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->brown; break;
                                        //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->yellow; break;
                                        default: TheTerm::Instance()->currentFGColor = TheTerm::Instance()->brown; break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->brown;
                                    break;
                            }
                            break;

                        case 44: // BG_BLUE
                            switch (color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->blue;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->blue; break;
                                        //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightBlue; break;
                                        default: TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightBlue; break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->blue; break;
                                        //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightBlue; break;
                                        default: TheTerm::Instance()->currentFGColor = TheTerm::Instance()->blue; break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->blue;
                                    break;
                            }
                            break;

                        case 45: // BG_MAGENTA
                            switch (color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->magenta;
                                    break;

                                case BLINK_ON:
                                    switch(prev_color_attribute)
                                    {
                                        //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->magenta; break;
                                        //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightMagenta; break;
                                        default: TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightMagenta; break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->magenta; break;
                                        //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightMagenta; break;
                                        default: TheTerm::Instance()->currentFGColor = TheTerm::Instance()->magenta; break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->magenta;
                                    break;
                            }
                            break;

                        case 46: // BG_CYAN
                            switch (color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->cyan;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->cyan; break;
                                        //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightCyan; break;
                                        default: TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightCyan; break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->cyan; break;
                                        //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->lightCyan; break;
                                        default: TheTerm::Instance()->currentFGColor = TheTerm::Instance()->cyan; break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->cyan;
                                    break;
                            }
                            break;

                        case 47: // BG_WHITE
                            switch (color_attribute)
                            {
                                case ALL_ATTRIBUTES_OFF:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->grey;
                                    break;

                                case BLINK_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->grey; break;
                                        //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->white; break;
                                        default: TheTerm::Instance()->currentFGColor = TheTerm::Instance()->white; break;
                                    }
                                    break;

                                case REVERSE_VIDEO_ON:
                                    // Flip to FG = BG, and BG = FG
                                    switch(prev_color_attribute)
                                    {
                                        //case 0:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->grey; break;
                                        //case 1:  TheTerm::Instance()->currentFGColor = TheTerm::Instance()->white; break;
                                        default: TheTerm::Instance()->currentFGColor = TheTerm::Instance()->grey; break;
                                    }
                                    break;

                                default:
                                    TheTerm::Instance()->currentBGColor = TheTerm::Instance()->grey;
                                    break;
                            }
                            break;

                        default :
                            //current_color += static_cast<ostringstream*>( &(ostringstream() << param[i]) )->str();
                            //printf("\r\nSET_GRAPHICS_MODE: %s", (char *)esc_sequence.c_str());
                            break;

                        } // End Switch
                    } // End For

                    //current_color += "m";
                    //screen_buff.color_sequence += current_color;

                } // End Else

            case RESET_MODE:
                //screen_buff.esc_sequence += esc_sequence;
                esc_sequence.clear();
                if (param[0] == 7)
                    line_wrapping = false;

            case SET_MODE:
                 if (param[0] == 7)
                {
                    line_wrapping = true;
                    //screen_buff.esc_sequence += esc_sequence;
                    esc_sequence.clear();
                }
                //else
                    //outf << "SET_MODE" << esc_sequence << endl;
                break;

            case SET_KEYBOARD_STRINGS:
                //outf << "SET_KEYBOARD_STRINGS" << esc_sequence << endl;
                // Ingored!
                break;

            // Respons back to Server with x/y position sequence.
            case ANSI_DETECTION:
                if (param[0] == 6)
                {
                    int current_y_position = 0;

                    // Make sure we don't respond with size taller then our
                    // Current Screen, Some BBS's use this respose to determine
                    // When to send a pause prompt.
                    if (y_position > NUM_LINES)
                        current_y_position = NUM_LINES;
                    else
                        current_y_position = y_position;

                    std::stringstream streamBuffer;
                    streamBuffer << "\x1b[" << current_y_position << ";" << x_position << "R";
                    currentPosition = streamBuffer.str();
                    //printf("\r\n ESC[6n currentPosition: %s",currentPosition.c_str());

                    if (TheSocketHandler::Instance()->isActive())
                    {
                        TheSocketHandler::Instance()->send((unsigned char *)currentPosition.c_str(),  currentPosition.size()+1);
                        // Check return value lateron on, not used at the moment.
                    }
                    else
                    {
                        std::cout << "ERROR TelnetState::telnet_build_NAWS_reply() (INPUT) ->isActive()" << std::endl;
                    }
                    //    socket_send((unsigned char *)currentPosition.c_str(), currentPosition.size()+1);

                    //screen_buff.esc_sequence += esc_sequence;
                    esc_sequence.clear();
                }
                //else
                    //outf << "ANSI_DETECTION" << esc_sequence << endl;
                break;

            default:
                // Rest are ignored.
                //outf << "Esc ignored" << esc_sequence << endl;
                break;

            } // End of Switch(c) Case Statements

        } // end of main escape sequence handler
        else   // otherwise output character using current color */
        {

            //printf("\r\n CHECK THIS! \r\n");

            // Back Spoce
            if ( c == '\b')
            {
                if (x_position > 1)
                    --x_position;
                continue;
            }


            // Handle New Line in ANSI Files properly.
            //if ( z+1 < buflen && c == '\r' && buff[z+1] == '\n' )
            if ( c == '\r' && buff[z+1] == '\n' )
            {
                //printf("CRLF");

                // Char Buffer, move to next position.
                ++z; // Incriment past \n (2) char combo.

                x_position = 1;
                ++y_position;

                // Doesn't seem to be needed anymore
                if (!line_wrapping)
                {
                    //screen_buff.color_sequence += "\x1b[40m";
                    //screenbuffer('\r');
                    esc_sequence.erase();
                }

                // Check here if we need to scroll the screen up 1 row.
                if (y_position > NUM_LINES || (TheTerm::Instance()->scrollRegionActive && y_position > TheTerm::Instance()->botMargin))
                {

                    // If we cleared the screen and hit bottom row, then
                    // The very first time we want to spit out the entire screen
                    // Since nothing has been drawn yet before we scroll the screen up
                    // Row by Row.
                    if (cleared_the_screen)
                    {
                        TheTerm::Instance()->RenderScreen();          // Surface to Texture
                        TheTerm::Instance()->DrawTextureScreen();  // Draw Texture to Screen
                        TheTerm::Instance()->ScrollScreenUp();     // Scroll the surface up

                        if (!TheTerm::Instance()->scrollRegionActive)
                            y_position = NUM_LINES;
                        else
                            y_position = TheTerm::Instance()->botMargin;

                        cleared_the_screen = false;
                    }
                    else
                    {

                        TheTerm::Instance()->RenderBottomScreen();    // Surface to Texture of Bottom Row.
                        TheTerm::Instance()->DrawTextureScreen();    // Testure to Screen
                        TheTerm::Instance()->ScrollScreenUp();       // Scroll up for next line.

                        if (!TheTerm::Instance()->scrollRegionActive)
                            y_position = NUM_LINES;
                        else
                            y_position = TheTerm::Instance()->botMargin;
                    }
                }

                //printf("\r\n xpos %i, ypos %i \r\n",x_position, y_position);
                continue;
            }

            if ( c == '\r')
            {
                //printf("CR");
                x_position = 1;

                // Doesn't seem to be needed anymore
                if (!line_wrapping)
                {
                    //screen_buff.color_sequence += "\x1b[40m";
                    //screenbuffer('\r');
                    esc_sequence.erase();
                }
                continue;
            }

            else if ( c == '\n')
            {
                //printf("LF");
                // Set position 0, casue next check incriments to 1.
                x_position = 1;
                ++y_position;

                // Doesn't seem to be needed anymore
                if (!line_wrapping)
                {
                    //screen_buff.color_sequence += "\x1b[40m";
                    //screenbuffer('\r');
                    esc_sequence.erase();
                }

                // Check here if we need to scroll the screen.
                if (y_position > NUM_LINES || (TheTerm::Instance()->scrollRegionActive && y_position > TheTerm::Instance()->botMargin))
                {
                    // If we cleared the screen and hit bottom row, then
                    // The very first time we want to spit out the entire screen
                    // Since nothing has been drawn yet before we scroll thes screen.
                    if (cleared_the_screen)
                    {
                        TheTerm::Instance()->RenderScreen();          // Surface to Texture
                        TheTerm::Instance()->DrawTextureScreen();  // Draw Texture to Screen
                        TheTerm::Instance()->ScrollScreenUp();     // Scroll the surface up

                        if (!TheTerm::Instance()->scrollRegionActive)
                            y_position = NUM_LINES;
                        else
                            y_position = TheTerm::Instance()->botMargin;
                        cleared_the_screen = false;
                    }
                    else
                    {
                        // Else we want to just append to the last line
                        // Move the last line to the Texture, then
                        // Redisplay the scrren.
                        TheTerm::Instance()->RenderBottomScreen();    // Surface to Texture of Bottom Row.
                        TheTerm::Instance()->DrawTextureScreen();    // Testure to Screen
                        TheTerm::Instance()->ScrollScreenUp();       // Scroll up for next line.

                        if (!TheTerm::Instance()->scrollRegionActive)
                            y_position = NUM_LINES;
                        else
                            y_position = TheTerm::Instance()->botMargin;
                    }
                }
                continue;
            }

            // Check for X_Position past colums in current line
            if (x_position > characters_per_line)
            {
                // Move to next line
                // If were in scroll region,
                // Then we'll test specifically to scroll the region only.
                if (!TheTerm::Instance()->scrollRegionActive)
                {
                    x_position = 1;
                    ++y_position;
                }
            }

            // A little more tricky now if were pushing text pass the bottom margin
            //from the scroll region or just x/y coording text like status bar
            //outside scroll region we don't want to scroll in this case.


            // Check if y_position greater then screen size
            // if we need to scroll the screen up 1 line.
            // This last check is for normal text being pushed to the screen,
            // NO CR/LR so check if we gone past the bottom margin of the screen.
            if (y_position > NUM_LINES || (TheTerm::Instance()->scrollRegionActive && y_position >= TheTerm::Instance()->botMargin && x_position > characters_per_line))
            {

                // If we cleared the screen and hit bottom row, then
                // The very first time we want to spit out the entire screen
                if (cleared_the_screen)
                {

                    // test if scrollign region is active and were drawing in it.
                    if (TheTerm::Instance()->scrollRegionActive &&
                            y_position >= TheTerm::Instance()->topMargin &&
                            y_position <= TheTerm::Instance()->botMargin)
                    {
                        TheTerm::Instance()->RenderScreen();          // Surface to Texture
                        TheTerm::Instance()->DrawTextureScreen();  // Draw Texture to Screen
                        TheTerm::Instance()->ScrollScreenUp();     // Scroll the surface up

                        y_position = TheTerm::Instance()->botMargin;
                        cleared_the_screen = false;

                        // Reset to begining of line.
                        if (x_position > characters_per_line)
                            x_position = 1;
                    }
                    else
                    if (y_position > NUM_LINES)
                    {
                        TheTerm::Instance()->RenderScreen();          // Surface to Texture
                        TheTerm::Instance()->DrawTextureScreen();  // Draw Texture to Screen
                        TheTerm::Instance()->ScrollScreenUp();     // Scroll the surface up
                        y_position = NUM_LINES;
                        cleared_the_screen = false;
                    }
                }
                else
                {
                    // Else we want to just append to the last line
                    // Move the last line to the Texture, then
                    // Redisplay the scrren.

                    // test if scrollign region is active and were drawing in it.
                    if (TheTerm::Instance()->scrollRegionActive &&
                            y_position >= TheTerm::Instance()->topMargin &&
                            y_position <= TheTerm::Instance()->botMargin)
                    {
                        TheTerm::Instance()->RenderBottomScreen();    // Surface to Texture of Bottom Row.
                        TheTerm::Instance()->DrawTextureScreen();    // Testure to Screen
                        TheTerm::Instance()->ScrollScreenUp();       // Scroll up for next line.
                        // Reset to begining of line.
                        if (x_position > characters_per_line)
                            x_position = 1;

                        y_position = TheTerm::Instance()->botMargin;
                    }
                    else
                    if (y_position > NUM_LINES)
                    {
                        TheTerm::Instance()->RenderBottomScreen();    // Surface to Texture of Bottom Row.
                        TheTerm::Instance()->DrawTextureScreen();    // Testure to Screen
                        TheTerm::Instance()->ScrollScreenUp();       // Scroll up for next line.

                        y_position = NUM_LINES;
                    }
                }
            }

            //printf(" \r\n XY: %i,%i ",x_position, y_position);

            // Render Char Screen Position 0 Based from 1 Based.
            TheTerm::Instance()->DrawChar(x_position-1, y_position-1, c);        // Char to Surface
            //TheTerm::Instance()->RenderCharScreen(x_position-1, y_position-1);   // Surface to Texture

            // After draining Character send Cursor to Next Position forward.
            ++x_position;

            // Not needed byt keep it clean
            // Wiull use for history buffer lateron.
            esc_sequence.erase();
        }

    }   // end while !feof

    // Display final Screen at end of Parsing.
    TheTerm::Instance()->RenderScreen();
    TheTerm::Instance()->DrawTextureScreen();
}



