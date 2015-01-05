
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "mainMenuState.h"
#include "menuFunction.h"
#include "inputHandler.h"
#include "terminal.h"
#include "telnetState.h"
#include "sshState.h"
#include "ansiParser.h"
#include "tinyxml.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm> // Sort and Reverse

#include <cstdio>
#include <ctime>
#include <cctype>
#include <cstring>
#include <cstdlib>

const std::string MainMenuState::menuID = "MENU";

// Callbacks
void MainMenuState::menuToTelnet()
{
    TheTerm::Instance()->getStateMachine()->changeState(new TelnetState());
}

void MainMenuState::menuToSSH()
{
    TheTerm::Instance()->getStateMachine()->changeState(new SSHState());
}
// end callbacks

void MainMenuState::update()
{
    TheTerm::SystemConnection sysCon;
    int ret = 0;
    if(!TheInputHandler::Instance()->isGlobalShutdown())
    {
        // Handles User Input and Window Events.
        //if(TheInputHandler::Instance()->update())
        //{
        // Pause before loading dialing directory.  no!
        //inputSequence = TheInputHandler::Instance()->getInputSequence();
        //std::cout << inputSequence << std::endl;
        // EOF == Exit, otherwise dial selected system.
        ret = startDialDirectory();
        switch(ret)
        {
            case EOF:
                // default.
                TheTerm::Instance()->setCurrentFont("vga8x16.bmp");
                if(TheTerm::Instance()->didFontChange())
                    TheTerm::Instance()->loadBitmapImage(TheTerm::Instance()->getCurrentFont());

                MenuFunction::ansiPrintf("outro.ans");
                SDL_Delay(1500);
                TheTerm::Instance()->setRenderReady(false);
                TheTerm::Instance()->quit();
                break;

            default:
                sysCon = TheTerm::Instance()->getSystemConnection();
                if(sysCon.protocol == "TELNET")
                {
                    TheTerm::Instance()->setCurrentFont(sysCon.font);
                    menuToTelnet();
                }
                else if(sysCon.protocol == "SSH")
                {
                    TheTerm::Instance()->setCurrentFont(sysCon.font);
                    menuToSSH();
                }
                else
                {
                    // error, unknown protocol
                }
        }
        TheTerm::Instance()->setRenderReady(true);
        //}
    }
    else
        TheTerm::Instance()->quit();
    // Delay in Menu Loop for Input, for CPU Usage.
    SDL_Delay(10);
}

void MainMenuState::render()
{
    std::cout << "MainMenuState::render()" << std::endl;
    // Testing Term, and setup for inital menu / dialing directory.
    /*
    TheTerm::Instance()->ClearScreenSurface();
    AnsiParser::Instance()->reset();

    if (TheTerm::Instance()->running())
    {
        // Set default Font for Intro, and Dialing Directory.
        TheTerm::Instance()->setCurrentFont("vga8x16.bmp");

        // If the font changed, then load the new image.
        if (TheTerm::Instance()->didFontChange())
            TheTerm::Instance()->loadBitmapImage(TheTerm::Instance()->getCurrentFont());

        mFunc.ansiPrintf("intro.ans");
    }*/
    TheTerm::Instance()->setRenderReady(false);
}

bool MainMenuState::onEnter()
{
    // Setup Defaults for Interface.
    tTop = 1;
    tBot = 1;
    CURRENT_BAR = 0;
    std::cout << "entering MainMenuState\n";
    // Set Render to Ready so we draw menu, Once it's drawn we toggle
    // This off, so it doesn't keep looping since it's not a game
    // with animation, this saves cpu usage.
    TheTerm::Instance()->setRenderReady(true);
    return true;
}

bool MainMenuState::onExit()
{
    // reset the input handler
    TheInputHandler::Instance()->reset();
    // Clear Dialing directory
    systemConnection.clear();
    std::vector<TheTerm::SystemConnection>().swap(systemConnection);
    std::cout << "exiting MainMenuState\n";
    return true;
}

void MainMenuState::setCallbacks(const std::vector<Callback>& callbacks)
{

}

/**
 * Start of DialDirectory INI Class
 */
bool MainMenuState_INI::ddirectory_exists()
{
    std::cout << "ddirectory_exists()" << std::endl;
    iTop = 0;
    iBot = 0;
#ifdef _WIN32
    std::string path = "assets\\";
#else
    std::string path = "assets/";
#endif
    path += sININAME;
    FILE *stream;
    stream = fopen(path.c_str(),"rb+");
    if(stream == NULL)
    {
        printf("Error unable to read msgtitle.ini, check permissions!");
        return false;
    }
    fclose(stream);
    return true;
}

/**
 * Create INI File
 */
void MainMenuState_INI::ddirectory_create()
{
    //std::cout << "ddirectory_create()" << std::endl;
#ifdef _WIN32
    std::string path = "assets\\";
#else
    std::string path = "assets/";
#endif
    path += sININAME;
    std::ofstream outStream2;
    outStream2.open(path.c_str(), std::ofstream::out | std::ofstream::trunc);
    if(!outStream2.is_open())
    {
        printf("\nError Creating: %s \n", path.c_str());
        return;
    }
    outStream2 << " * Generated By Program. " << std::endl;
    outStream2.close();
    return;
}

/**
 * Parse Helper
 */
void MainMenuState_INI::ddirectory_chkpar(std::string &data)
{
    //std::cout << "ddirectory_chkpar()" << std::endl;
    std::string temp1;
    std::string::size_type st1 = 0;
    std::string::size_type st2 = 0;
    std::string::size_type  ct = 0;

    st1 = data.find('"', 0);
    st2 = data.find('"', st1+1);
    ++st1;
    temp1 = data.substr(st1,st2);
    ct = st2 - st1;
    if(temp1.length() > ct)
        temp1.erase(ct,temp1.length());
    data = temp1;
}

/**
 * Get INI Values
 */
void MainMenuState_INI::ddirectory_check(std::string cfgdata)
{
    //std::cout << "ddirectory_check()" << std::endl;
    std::string::size_type id1 = 0;
    std::string temp = cfgdata;

    if(temp[0] == '#')
        return;
    else if(temp.find("set TOP ", 0) != std::string::npos)
    {
        ddirectory_chkpar(temp);
        id1 = atoi(temp.c_str());
        iTop = id1;
    }
    else if(temp.find("set BOT ", 0) != std::string::npos)
    {
        ddirectory_chkpar(temp);
        id1 = atoi(temp.c_str());
        iBot = id1;
    }
    else if(temp.find("set THEME_NAME ", 0) != std::string::npos)
    {
        ddirectory_chkpar(temp);
        sTHEME_NAME = temp;
    }
    else if(temp.find("set FONT_SET ", 0) != std::string::npos)
    {
        ddirectory_chkpar(temp);
        sFONT_SET = temp;
    }
    else if(temp.find("set ANSI_FILE ", 0) != std::string::npos)
    {
        ddirectory_chkpar(temp);
        sANSI_FILE = temp;
    }
    else if(temp.find("set MENU_PROMPT ", 0) != std::string::npos)
    {
        ddirectory_chkpar(temp);
        sMENU_PROMPT = temp;
    }
    else if(temp.find("set PAGENUM ", 0) != std::string::npos)
    {
        ddirectory_chkpar(temp);
        sPAGENUM = temp;
    }
    else if(temp.find("set PAGETOTAL ", 0) != std::string::npos)
    {
        ddirectory_chkpar(temp);
        sPAGETOTAL = temp;
    }
    else if(temp.find("set MOREMSG_ON ", 0) != std::string::npos)
    {
        ddirectory_chkpar(temp);
        sMOREMSG_ON = temp;
    }
    else if(temp.find("set MOREMSG_WORD_ON ", 0) != std::string::npos)
    {
        ddirectory_chkpar(temp);
        sMOREMSG_WORD_ON = temp;
    }
    else if(temp.find("set MOREMSG_OFF ", 0) != std::string::npos)
    {
        ddirectory_chkpar(temp);
        sMOREMSG_OFF = temp;
    }
    else if(temp.find("set MOREMSG_WORD_OFF ", 0) != std::string::npos)
    {
        ddirectory_chkpar(temp);
        sMOREMSG_WORD_OFF = temp;
    }
    else if(temp.find("set TEXT_COLOR ", 0) != std::string::npos)
    {
        ddirectory_chkpar(temp);
        sTEXT_COLOR = temp;
    }
    else if(temp.find("set MAX_AREAS ", 0) != std::string::npos)
    {
        ddirectory_chkpar(temp);
        sMAX_AREAS = temp;
    }
}

/**
 * Read / Parse INI File
 */
int MainMenuState_INI::ddirectory_parse(int idx)
{
    //std::cout << "ddirectory_parse()" << std::endl;
    if(!ddirectory_exists())
    {
        printf("Error unable to parse msgtitle.ini, check permissions!");
    }
    //ddirectory_create();
    char name[1024]  = {0};
    char name2[1024] = {0};

#ifdef _WIN32
    std::string path = "assets\\";
#else
    std::string path = "assets/";
#endif
    // Set for Theme, check idx number for Themeing,.
    sprintf(name, "%s%s",path.c_str(),sININAME);
    sprintf(name2,"%s%s%i.ini",path.c_str(),sININAME,idx);
    if(idx != 0) strcpy(name,name2);
    // Check if Theme Exists, if not return FALSE.
    FILE *stream;
    stream = fopen(name,"rb+");
    if(stream == NULL)
    {
        // File is not Present
        return FALSE;
    }
    fclose(stream);

    std::ifstream inStream;
    inStream.open(name);
    if(!inStream.is_open())
    {
        printf("Couldn't Open Config File: %s\n", name);
        return FALSE;
    }

    std::string cfgdata;
    for(;;)
    {
        std::getline(inStream,cfgdata,'\n');

        ddirectory_check(cfgdata);
        if(inStream.eof()) break;
    }
    inStream.close();
    return TRUE;
}


/**
 * Reads in Ansi file into Buffer Only
 */
void MainMenuState::readinAnsi(std::string FileName, std::string &buff)
{
    std::cout << "readinAnsi()" << std::endl;
#ifdef _WIN32
    std::string path = "assets\\";
#else
    std::string path = "assets/";
#endif
    path += FileName;
    path += ".ans";
    FILE *fp;
    std::string::size_type id1 = 0;
    int c = 0;
    if((fp = fopen(path.c_str(), "r+")) ==  NULL)
    {
        std::cout << "ANSI not found: " << path << std::endl;
        return;
    }
    do
    {
        c = getc(fp);
        if(c != EOF)
            buff += (char)c;
    }
    while(c != EOF);
    fclose(fp);
}

/**
 * DialDirectory - Parse ANSI Template
 */
void MainMenuState::parseHeader(std::string FileName)
{
    std::cout << "parseHeader()" << std::endl;
    std::cout << "sFONT_SET: " << sFONT_SET << std::endl;

    // Set the font type for the menu being displayed.
    TheTerm::Instance()->setCurrentFont(sFONT_SET);

    // Test if font changed, if so, then re-load it.
    if(TheTerm::Instance()->didFontChange())
        TheTerm::Instance()->loadBitmapImage(TheTerm::Instance()->getCurrentFont());

    TheAnsiParser::Instance()->reset();
    MenuFunction::ansiPrintf(FileName);
    //TheAnsiParser::Instance()->pipe2ansi( const_cast<char *>( buff.c_str() ));
}

/**
 * DialDirectory - Setup Reading INI File and init Theme
 */
void MainMenuState::setupList()
{
    //std::cout << "setupList()" << std::endl;
    sprintf(sININAME,"%s",(char *)"dialdirectory.ini");
    ddirectory_parse();
    tTop = iTop;
    tBot = iBot;
}

/**
 * DialDirectory - Change ANSI Template Theme
 */
int MainMenuState::changeTheme(int idx)
{
    //std::cout << "changeTheme()" << std::endl;
    // Check What Theme user has selected.
    if(ddirectory_parse(idx) == FALSE)
    {
        // Theme Doesn't Exist.
        return FALSE;
    }
    mLink.Top = iTop;
    mLink.Bot = iBot;
    return TRUE;
}

/**
 * Build Phonebook with List of Systems
 * This function creates 4 versions of each line,
 * one for default, defeault selected, new, then new selected.
 * This doesn't mark which are new or to be used.
 */
std::vector< list_bar > MainMenuState::buildDialList()
{
    //std::cout << "buildDialList()" << std::endl;
    list_bar                 tbar;
    std::vector<list_bar>    result;

    int c = 0;
    std::string temp3 = "";

    FILE *inStream;
    long  idx = 1;
    long  i   = 0;
    char  MCI[3]= {0};
    char  temp2[100]= {0};
    int   space  = 0;
    int   foundr = FALSE;
    int   foundl = FALSE;
    std::string ans  = "";
    std::string ans1 = "";
    std::string ans2 = "";
    std::string ans3 = "";
    std::string ans4 = "";
    // Prefetch ansi lightbar themes
    // These ansi are used as the light bars to be displayed,
    // We rotate between 4 ansi lightbars.
    //
    // 1. Unread Messages
    // 2. New Messages
    // 3. Current Selection of Unread
    // 4. Current Selection of New
    //
    // We cache array with all four choices so we can
    // easily switch between them in the listing.
#ifdef _WIN32
    std::string path = "assets\\";
#else
    std::string path = "assets/";
#endif
    path += "ddirectorymid1.ans";
    if((inStream = fopen(path.c_str(), "r+")) ==  NULL)
    {
        std::cout << "unable to read " <<  "ddirectorymid1.ans" << std::endl;
        return result;
    }
    while(c != EOF)
    {
        c = getc(inStream);
        if(c != EOF) ans1 += c;
    }
    fclose(inStream);
    c = '\0';

#ifdef _WIN32
    path = "assets\\";
#else
    path = "assets/";
#endif
    path += "ddirectorymid2.ans";
    if((inStream = fopen(path.c_str(), "r+")) ==  NULL)
    {
        std::cout << "unable to read " <<  "ddirectorymid2.ans" << std::endl;
        return result;
    }
    while(c != EOF)
    {
        c = getc(inStream);
        if(c != EOF) ans2 += c;
    }
    fclose(inStream);

    unsigned long max_titles = 0;
    unsigned long num_titles = 0;
    max_titles = systemConnection.size();
    std::cout << "systemConnection.size(): " << systemConnection.size() << std::endl;

    while(num_titles < max_titles)
    {
        if(num_titles == max_titles) break;
        temp3.erase();
        for(int icnt = 0; icnt < 2; icnt++)
        {
            switch(icnt)
            {
                case 0:
                    ans = ans2;
                    break; // Display HighLight None.  ON

                case 1:
                    ans = ans1;
                    break; // Dispaly Lowlight None    OFF

                default:
                    break;
            }
            i = 0;
            c = 0;
            // Parse MCI Codes per each light bar ansi file
            do
            {
                memset(&MCI,0,sizeof(MCI));
                c = ans[i];
                if(c == '\0') break;
                // Check for Spacing MCI Code
                switch(c)
                {
                    case '{' : // Left Justify
                        MCI[0] = ans[++i];
                        MCI[1] = ans[++i];
                        space = atoi(MCI);
                        if(space != 0)
                        {
                            foundl = TRUE;
                        }
                        else
                        {
                            temp3 += c;
                            temp3 += MCI;
                        }
                        break;

                    case '}' : // Right Justify
                        MCI[0] = ans[++i];
                        MCI[1] = ans[++i];
                        space = atoi(MCI);
                        if(space != 0)
                        {
                            foundr = TRUE;
                        }
                        else
                        {
                            temp3 += c;
                            temp3 += MCI;
                        }
                        break;

                    case '|' : // Pipe Codes
                        MCI[0] = ans[++i];
                        MCI[1] = ans[++i];
                        if(strcmp(MCI,"##") == 0)
                        {
                            sprintf(temp2,"%lu", num_titles+1);
                            if(foundl)
                            {
                                MenuFunction::leftSpacing(temp2,space);
                                foundl = FALSE;
                            }
                            else if(foundr)
                            {
                                MenuFunction::rightSpacing(temp2,space);
                                foundr = FALSE;
                            }
                            // Not using New/False, set Default.
                            tbar.isnew = FALSE;
                            temp3 += temp2;
                        }
                        else if(strcmp(MCI,"NA") == 0)
                        {
                            sprintf(temp2,"%s",systemConnection[num_titles].name.c_str());
                            if(foundl)
                            {
                                MenuFunction::leftSpacing(temp2,space);
                                foundl = FALSE;
                            }
                            else if(foundr)
                            {
                                MenuFunction::rightSpacing(temp2,space);
                                foundr = FALSE;
                            }
                            temp3 += temp2;
                        }
                        else if(strcmp(MCI,"FO") == 0)
                        {
                            // FIXME Temp - Translate Filename to Description
                            // Will add to the xml once it's ready.
                            if(systemConnection[num_titles].font == "vga8x16.bmp")
                                sprintf(temp2,"%s",(char *)"IBM-PC CP437 VGA 8x16");
                            else if(systemConnection[num_titles].font == "topazPlus-8x16.bmp")
                                sprintf(temp2,"%s",(char *)"AMIGA Topaz+ 1200 8x16");

                            //sprintf(temp2,"%s",systemConnection[num_titles].font.c_str());
                            if(foundl)
                            {
                                MenuFunction::leftSpacing(temp2,space);
                                foundl = FALSE;
                            }
                            else if(foundr)
                            {
                                MenuFunction::rightSpacing(temp2,space);
                                foundr = FALSE;
                            }
                            temp3 += temp2;
                        }
                        else if(strcmp(MCI,"PO") == 0)
                        {
                            sprintf(temp2,"%i",systemConnection[num_titles].port);
                            if(foundl)
                            {
                                MenuFunction::leftSpacing(temp2,space);
                                foundl = FALSE;
                            }
                            else if(foundr)
                            {
                                MenuFunction::rightSpacing(temp2,space);
                                foundr = FALSE;
                            }
                            temp3 += temp2;
                        }
                        else if(strcmp(MCI,"PR") == 0)
                        {
                            sprintf(temp2,"%s",systemConnection[num_titles].protocol.c_str());
                            if(foundl)
                            {
                                MenuFunction::leftSpacing(temp2,space);
                                foundl = FALSE;
                            }
                            else if(foundr)
                            {
                                MenuFunction::rightSpacing(temp2,space);
                                foundr = FALSE;
                            }
                            temp3 += temp2;
                        }
                        else
                        {
                            temp3 += c;
                            temp3 += MCI;
                        }
                        break;

                    case '\n' :
                        //  temp3 += '\r';
                        break;

                    default :
                        temp3 += c;
                        break;
                }
                ++i;
            }
            while(c != '\0');

            temp3 += "\r";
            switch(icnt)
            {
                case 0:
                    tbar . ansi_2 = temp3;
                    break; // Display HighLight None.  ON
                case 1:
                    tbar . ansi_1 = temp3;
                    break; // Dispaly Lowlight None    OFF

                default:
                    break;
            }
            temp3.erase();
        } // End of (4) Look for each string.
        result . push_back(tbar);
        temp3.erase();
        ++num_titles;
        ++idx;
    }
    return result;
}

bool MainMenuState::readDialDirectory()
{
    TheTerm::SystemConnection sysconn;
#ifdef _WIN32
    std::string path = "assets\\dialdirectory.xml";
#else
    std::string path = "assets/dialdirectory.xml";
#endif
    TiXmlDocument doc(path.c_str());
    if(!doc.LoadFile()) return false;
    TiXmlHandle hDoc(&doc);
    TiXmlElement* pElem;
    TiXmlHandle hRoot(0);

    // If vector already populated then clear to refresh it.
    if(systemConnection.size() >0)
    {
        systemConnection.clear();
        std::vector<TheTerm::SystemConnection>().swap(systemConnection);
    }
    // block: EtherTerm
    {
        std::cout << "readDialDirectory - FirstChildElement" << std::endl;
        pElem=hDoc.FirstChildElement().Element();
        // should always have a valid root but handle gracefully if it does
        if(!pElem)
        {
            std::cout << "readDialDirectory - EtherTerm Element not found!" << std::endl;
            return false;
        }
        std::cout << "Root Value: " << pElem->Value() << std::endl;
        // save this for later
        hRoot=TiXmlHandle(pElem);
    }
    // block: Phonebook
    {
        //std::cout << "readDialDirectory - Phonebook" << std::endl;
        pElem=hRoot.FirstChild("Phonebook").Element();
        std::cout << "Phonebook Version: " << pElem->Attribute("version") << std::endl;
    }
    // block: BBS
    {
        //std::cout << "readDialDirectory - BBS" << std::endl;
        pElem=hRoot.FirstChild("Phonebook").FirstChild().Element();
        for(; pElem; pElem=pElem->NextSiblingElement())
        {
            sysconn.name = pElem->Attribute("name");
            sysconn.ip = pElem->Attribute("ip");
            pElem->QueryIntAttribute("port", &sysconn.port);
            sysconn.protocol = pElem->Attribute("protocol");
            sysconn.login = pElem->Attribute("login");
            sysconn.password = pElem->Attribute("password");
            sysconn.font = pElem->Attribute("font");
            // Add to Vector so we can parse in building the dialing directory.
            systemConnection.push_back(sysconn);
        }
    }
    std::cout << "readDialDirectory - Done" << std::endl;
    return true;
}
void MainMenuState::createDialDirectory()
{
    // Create Default Phone Book.
#ifdef _WIN32
    std::string path = "assets\\dialdirectory.xml";
#else
    std::string path = "assets/dialdirectory.xml";
#endif
    TiXmlDocument doc;
    TiXmlDeclaration * decl = new TiXmlDeclaration("1.0", "", "");
    doc.LinkEndChild(decl);

    TiXmlElement * element = new TiXmlElement("EtherTerm");
    doc.LinkEndChild(element);

    TiXmlElement * element2 = new TiXmlElement("Phonebook");
    element->LinkEndChild(element2);
    element2->SetAttribute("version", "1.0");

    TiXmlElement * element3 = new TiXmlElement("BBS");
    element2->LinkEndChild(element3);

    element3->SetAttribute("name", "Haunting The Chapel");
    element3->SetAttribute("ip", "htc.zapto.org");
    element3->SetAttribute("port", "23");
    element3->SetAttribute("protocol", "TELNET");
    element3->SetAttribute("login", "");
    element3->SetAttribute("password", "");
    element3->SetAttribute("font", "vga8x16.bmp");

    TiXmlElement *element4 = new TiXmlElement("BBS");
    element2->LinkEndChild(element4);

    element4->SetAttribute("name", "1984");
    element4->SetAttribute("ip", "1984.ws");
    element4->SetAttribute("port", "23");
    element4->SetAttribute("protocol", "TELNET");
    element4->SetAttribute("login", "");
    element4->SetAttribute("password", "");
    element4->SetAttribute("font", "vga8x16.bmp");
    doc.SaveFile(path.c_str());
}

/**
 * Title Scan - Start Interface
 */
int MainMenuState::startDialDirectory()
{
    //std::cout << "startDialDirectory()" << std::endl;
    mLink.initList(sTEXT_COLOR, 1);
    int  CurrentPage  = 0;
    int  bSize        = 0;
    int  boxsize      = 0;
//    bool more         = FALSE;
//    bool showmore     = FALSE;

    // Reading in dialdirectory.ini values.
    setupList();
    std::cout << "ANSI File: "<< sANSI_FILE << std::endl;

    bSize = tBot - tTop;
    ++bSize;
    mLink.Top = tTop;
    mLink.Bot = tBot;

    // Build Title Scan into Vector List
    std::vector<list_bar> result;
    std::string inputSequence;

    // Readin or Crete Dialing Directory.
    if(!readDialDirectory())
    {
        createDialDirectory();
        if(!readDialDirectory())
        {
            cout << "Error: unable to create and rea dialdirectory.xml" << std::endl;
            return -1;
        }
    }
    result = buildDialList();
    // populate list with vector class
    mLink.GetVector(result);

    std::string _output;
    char outBuffer[1024]= {0};
    char mString[10]= {0};
    char rBuffer[1024]= {0};
    unsigned char ch = '\0';
    std::string tmp;
    std::string::size_type id1 = 0;
    // To Start out, keep this static lateron for next call.
    CURRENT_BAR = 0;

    // Loop Lightbar Interface.
    do
    {
        parseHeader(sANSI_FILE);
        // Jump to Current Page
        boxsize = mLink.Bot - mLink.Top;
        CurrentPage = CURRENT_BAR / boxsize;

        //  Make sure we have areas.
        if(result.size() > 0)
        {
            //std::cout << "box_start_vector(CurrentPage, CURRENT_BAR);" << std::endl;
            mLink.box_start_vector(CurrentPage, CURRENT_BAR);
        }
        else
        {
            // No Messages, return.
            //std::cout << "return EOF;" << std::endl;
            return EOF;
        }

        mLink.Tot = result.size();
        do
        {
            _output.erase();
            _output = "|16"; // Clear Color Bleeding, reset background to black.

            // Show Current/Total Pages in Dialing Directory
            sprintf(outBuffer,"%s%.3d",(char *)sPAGENUM.c_str(),mLink.Page+1);
            _output += outBuffer;
            sprintf(outBuffer,"%s%.3d",(char *)sPAGETOTAL.c_str(),mLink.TotPages);
            _output += outBuffer;

            // # of Systems in Dialing Directory
            sprintf(outBuffer,"%s%.3d",(char *)sMAX_AREAS.c_str(),mLink.Tot);
            _output += outBuffer;
            MenuFunction::pipe2ansi((char *)_output.c_str());

            mFunc._curmenu.clear();
            mFunc._curmenu = sMENU_PROMPT;
JMPINPUT1:
            mFunc.menu_readin();
            mFunc.menu_proc(mString, CURRENT_BAR);
            //std::cout << "mString: " << mString << std::endl;

            ch = mString[1];
            // For Menu CmdKey Input
            if(mString[0] == '!')
            {
                switch(toupper(ch))
                {
                    case 'N': // Multi Area Newscan, skip to next area.
                        mLink.dispose_list();
                        std::vector<list_bar>() . swap(result); // Free Vector Up.
                        return -2;

                    case 'U': // Page Up
                        if(CurrentPage != 0)
                        {
                            --CurrentPage;
                            // reset Bar to first Listing on each Page.
                            CURRENT_BAR = CurrentPage * boxsize;
                            mLink.box_start_vector(CurrentPage,CURRENT_BAR);
                        }
                        else
                            goto JMPINPUT1;
                        break;

                    case 'D': // Page Down
                        if(CurrentPage+1 != mLink.TotPages)
                        {
                            ++CurrentPage;
                            // reset Bar to first Listing on each Page.
                            CURRENT_BAR = CurrentPage * boxsize;
                            mLink.box_start_vector(CurrentPage,CURRENT_BAR);
                        }
                        else
                            goto JMPINPUT1;
                        break;

                        // hit ENTER
                    case 'E': // Selected Current System to Dial
                        mLink.dispose_list();
                        std::vector<list_bar>() . swap(result); // Free Vector Up.
                        // Pass the selected system to the TERM Instance so we can
                        // Pull it inside the TelnetState.
                        TheTerm::Instance()->setSystemConnection(systemConnection[CURRENT_BAR]);
                        return CURRENT_BAR;

                    case '+': // Next Message - Move Down

                        if(CURRENT_BAR+1 >= mLink.listing.size() || mLink.listing.size() == 0)
                            goto JMPINPUT1;
                        //std::cout << "Move to next bar." << std::endl;
                        ++CURRENT_BAR;
                        //Calculate if we go down, ++Current Area, are we on next page or not.
                        // Becasue 0 Based, need to add +1
                        // Test if we moved to next page.
                        if((signed)CURRENT_BAR+1 < (boxsize*(CurrentPage+1))+1)
                        {
                            // Still on Same Page
                            if(mLink.listing[CURRENT_BAR-1].isnew)
                            {
                                // Lowlight Current, then Highlight Next.
                                sprintf(rBuffer, "\x1b[%i;%iH|16%s", mLink.current_selection, 1, (char *)mLink.listing[CURRENT_BAR-1].ansi_3.c_str());
                            }
                            else
                            {
                                // Lowlight Current, then Highlight Next.
                                sprintf(rBuffer, "\x1b[%i;%iH|16%s", mLink.current_selection, 1, (char *)mLink.listing[CURRENT_BAR-1].ansi_1.c_str());
                            }
                            _output += rBuffer;
                            mLink.current_selection += 1;
                            if(mLink.listing[CURRENT_BAR].isnew)
                            {
                                sprintf(rBuffer, "\x1b[%i;%iH|16%s", mLink.current_selection, 1, (char *)mLink.listing[CURRENT_BAR].ansi_4.c_str());
                            }
                            else
                            {
                                sprintf(rBuffer, "\x1b[%i;%iH|16%s", mLink.current_selection, 1, (char *)mLink.listing[CURRENT_BAR].ansi_2.c_str());
                            }
                            _output += rBuffer;
                            MenuFunction::pipe2ansi((char *)_output.c_str());
                            _output.erase();
                            goto JMPINPUT1; //Not moving down a page.
                        }
                        else
                        {
                            // Move to next Page!
                            ++CurrentPage;
                            mLink.box_start_vector(CurrentPage,CURRENT_BAR);
                            break;
                        }
                    case '-': // Previous Messasge - Move Up

                        // Skipping to JMPINPUT bypasses redraws, much faster!
                        if(CURRENT_BAR > 0 && mLink.listing.size() != 0)
                            --CURRENT_BAR;
                        else
                            goto JMPINPUT1;

                        //Calculate if we go down, --Current Area, are we on next page or not.
                        // Becasue 0 Based, need to add +1
                        // Test if we moved to next page.
                        if((signed)CURRENT_BAR+1 > (boxsize*(CurrentPage)))
                        {
                            // Still on Same Page
                            // Lowlight Current, then Highlight Next.
                            if(mLink.listing[CURRENT_BAR+1].isnew)
                            {
                                sprintf(rBuffer, "\x1b[%i;%iH|16%s", mLink.current_selection, 1, (char *)mLink.listing[CURRENT_BAR+1].ansi_3.c_str());
                            }
                            else
                            {
                                sprintf(rBuffer, "\x1b[%i;%iH|16%s", mLink.current_selection, 1, (char *)mLink.listing[CURRENT_BAR+1].ansi_1.c_str());
                            }
                            _output = rBuffer;
                            mLink.current_selection -= 1;
                            if(mLink.listing[CURRENT_BAR].isnew)
                            {
                                sprintf(rBuffer, "\x1b[%i;%iH|16%s", mLink.current_selection, 1, (char *)mLink.listing[CURRENT_BAR].ansi_4.c_str());
                            }
                            else
                            {
                                sprintf(rBuffer, "\x1b[%i;%iH|16%s", mLink.current_selection, 1, (char *)mLink.listing[CURRENT_BAR].ansi_2.c_str());
                            }
                            _output += rBuffer;
                            MenuFunction::pipe2ansi((char *)_output.c_str());
                            _output.erase();
                            goto JMPINPUT1; //Not moving down a page.
                        }
                        else
                        {
                            // Move to next Page!
                            --CurrentPage;
                            mLink.box_start_vector(CurrentPage,CURRENT_BAR);
                        }
                        break;

                    case 'Q': // Quit Received, Hard Exit.
                        mLink.dispose_list();
                        std::vector<list_bar>() . swap(result); // Free Vector Up.
                        return EOF;

                    case '?': // Directory Help

//                    ansiPrintf(sANSI_HELP); // Display Ansi Help file,
                        //getkey(true);
                        parseHeader(sANSI_FILE); // Redisplay Display Ansi
                        mLink.box_start_vector(CurrentPage,CURRENT_BAR);
                        break;

                    case 'A': // About
                        parseHeader((char *)"about.ans"); // Redisplay Display Ansi
                        // Wait for input:
                        //std::cout << "Menu_Bars Input Loop" << std::endl;
                        MenuFunction::getkey();
                        parseHeader((char *)"about2.ans"); // Redisplay Display Ansi
                        MenuFunction::getkey();
                        // If Global Exit, return right away.
                        if(TheInputHandler::Instance()->isGlobalShutdown())
                        {
                            mLink.dispose_list();
                            std::vector<list_bar>() . swap(result); // Free Vector Up.
                            return EOF;
                        }
                        parseHeader((char *)"et2.ans"); // Redisplay Display Ansi
                        mLink.box_start_vector(CurrentPage,CURRENT_BAR);
                        break;
                        // Pass through, any functionaly that should
                        // Be handeled in Reader.
                        // to the Message Reader. ie post ,reply, delete...
                    default :
                        mLink.dispose_list();
                        std::vector<list_bar>() . swap(result); // Free Vector Up.
                        // Don't exit, just return with pass through command.
                        //CURRENT_BAR = EOF;
                        return (char)toupper(ch);
                }
            }
        }
        while(!TheInputHandler::Instance()->isGlobalShutdown());   // End do
    }
    while(!TheInputHandler::Instance()->isGlobalShutdown());   // End do
    return -1;
}
