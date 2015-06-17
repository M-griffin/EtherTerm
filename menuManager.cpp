
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "menuManager.hpp"
#include "menuConfig.hpp"
#include "sequenceParser.hpp"
#include "tinyxml.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdio>

/**
 * Reads in ANSI file into Buffer Only
 */
void MenuManager::readinAnsi(std::string FileName, std::string &buff)
{
    std::cout << "readinAnsi()" << std::endl;

    std::string path = SetupThePath();
    path += FileName;
    path += ".ans";
    FILE *fp;

    std::cout << "Load ANSI: " << path << std::endl;

    int sequence = 0;
    if((fp = fopen(path.c_str(), "r+")) ==  nullptr)
    {
        std::cout << "ANSI not found: " << path << std::endl;
        return;
    }
    do
    {
        sequence = getc(fp);
        if(sequence != EOF)
            buff += (char)sequence;
    }
    while(sequence != EOF);
    fclose(fp);
}

/**
 * Dial-directory - Parse ANSI Template
 */
void MenuManager::parseHeader(std::string FileName)
{
    std::cout << "parseHeader()" << std::endl;
    std::cout << "sFONT_SET: " << FONT_SET << std::endl;

    // Set the font type for the menu being displayed.
    TheTerminal::Instance()->setCurrentFont(FONT_SET);

    // Test if font changed, if so, then re-load it.
    if(TheTerminal::Instance()->didFontChange())
        TheTerminal::Instance()->loadBitmapImage(TheTerminal::Instance()->getCurrentFont());

    TheSequenceParser::Instance()->reset();
    MenuFunction::displayAnsiFile(FileName);
}

/**
 * Dial-directory - Setup Reading INI File and init Theme
 */
void MenuManager::readDirectoryListing()
{
    sprintf(INI_NAME,"%s",(char *)"dialdirectory.ini");
    ddirectory_parse();
    m_directoryTopMargin = TOP_MARGIN;
    m_directoryBottomMargin = BOTTOM_MARGIN;
}

/**
 * Dial-directory - Change ANSI Template Theme
 */
bool MenuManager::changeTheme(int index)
{
    if(!ddirectory_parse(index))
    {
        // Theme Doesn't Exist.
        return false;
    }
    m_linkList.m_topMargin = TOP_MARGIN;
    m_linkList.m_bottomMargin = BOTTOM_MARGIN;
    return true;
}

/**
 * Build Phone-book with List of Systems
 * This function creates 4 versions of each line,
 * one for default, default selected, new, then new selected.
 * This doesn't mark which are new or to be used.
 */
std::vector<list_bar> MenuManager::buildDialList()
{
    list_bar                 _lightbar;
    std::vector<list_bar>    result;

    int sequence = 0;
    std::string stringBuilder = "";

    FILE               *inStream;
    long index          = 1;
    char mciCode[3]     = {0};
    char temp2[100]     = {0};
    int  padding        = 0;
    bool isRightPadding = false;
    bool isLeftPadding  = false;

    std::string currnetScreen  = "";
    std::string ansiScreen1 = "";
    std::string ansiScreen2 = "";

    // Pre-fetch ANSI light bar themes
    // These ANSI are used as the light bars to be displayed,
    // We rotate between 2 ANSI light bars.
    // 1. Un-selected Systems
    // 4. Current System Selected
    //
    // We cache array with all four choices so we can
    // easily switch between them in the listing.
    std::string path = SetupThePath();
    path += "ddirectorymid1.ans";
    if((inStream = fopen(path.c_str(), "r+")) ==  nullptr)
    {
        std::cout << "unable to read " <<  "ddirectorymid1.ans" << std::endl;
        return result;
    }
    while(sequence != EOF)
    {
        sequence = getc(inStream);
        if(sequence != EOF) ansiScreen1 += sequence;
    }
    fclose(inStream);
    sequence = '\0';

    path = SetupThePath();
    path += "ddirectorymid2.ans";
    if((inStream = fopen(path.c_str(), "r+")) ==  nullptr)
    {
        std::cout << "unable to read " <<  "ddirectorymid2.ans" << std::endl;
        return result;
    }
    while(sequence != EOF)
    {
        sequence = getc(inStream);
        if(sequence != EOF) ansiScreen2 += sequence;
    }
    fclose(inStream);

    unsigned long maxSystems    = 0;
    unsigned long currentSystem = 0;
    maxSystems = m_systemConnection.size();
    std::cout << "systemConnection.size(): " << m_systemConnection.size() << std::endl;

    int counter = 0;
    while(currentSystem < maxSystems)
    {
        if(currentSystem == maxSystems) break;
        stringBuilder.erase();
        for(int ansiCount = 0; ansiCount < 2; ansiCount++)
        {
            switch(ansiCount)
            {
                case 0:
                    currnetScreen = ansiScreen2;
                    break; // Display High-light None.  ON

                case 1:
                    currnetScreen = ansiScreen1;
                    break; // Display Low-light None    OFF

                default:
                    break;
            }
            counter = 0;
            sequence = 0;
            // Parse MCI Codes per each light bar ANSI file
            do
            {
                memset(&mciCode,0,sizeof(mciCode));
                sequence = currnetScreen[counter];
                if(sequence == '\0') break;
                // Check for Spacing MCI Code
                switch(sequence)
                {
                    case '{' : // Left Justify
                        mciCode[0] = currnetScreen[++counter];
                        mciCode[1] = currnetScreen[++counter];
                        padding = atoi(mciCode);
                        if(padding != 0)
                        {
                            isLeftPadding = true;
                        }
                        else
                        {
                            stringBuilder += sequence;
                            stringBuilder += mciCode;
                        }
                        break;

                    case '}' : // Right Justify
                        mciCode[0] = currnetScreen[++counter];
                        mciCode[1] = currnetScreen[++counter];
                        padding = atoi(mciCode);
                        if(padding != 0)
                        {
                            isRightPadding = true;
                        }
                        else
                        {
                            stringBuilder += sequence;
                            stringBuilder += mciCode;
                        }
                        break;

                    case '|' : // Pipe Codes
                        mciCode[0] = currnetScreen[++counter];
                        mciCode[1] = currnetScreen[++counter];
                        if(strcmp(mciCode,"##") == 0)
                        {
                            sprintf(temp2,"%lu", currentSystem+1);
                            if(isLeftPadding)
                            {
                                std::string stringReplace = temp2;
                                MenuFunction::leftSpacing(stringReplace, padding);
                                std::replace(
                                    stringReplace.begin(), stringReplace.end(), ' ', '.');
                                isLeftPadding = false;
                                stringBuilder += stringReplace;
                            }
                            else if(isRightPadding)
                            {
                                std::string stringReplace = temp2;
                                MenuFunction::rightSpacing(stringReplace, padding);
                                std::replace(
                                    stringReplace.begin(), stringReplace.end(), ' ', '.');
                                isRightPadding = false;
                                stringBuilder += stringReplace;
                            }
                        }
                        else if(strcmp(mciCode,"NA") == 0)
                        {
                            std::string stringReplace =
                                m_systemConnection[currentSystem].name;
                            if(isLeftPadding)
                            {
                                MenuFunction::leftSpacing(stringReplace, padding);
                                isLeftPadding = false;
                            }
                            else if(isRightPadding)
                            {
                                MenuFunction::rightSpacing(stringReplace, padding);
                                isRightPadding = false;
                            }
                            stringBuilder += stringReplace;
                        }
                        else if(strcmp(mciCode,"FO") == 0)
                        {
                            // FIXME Temp - Translate File-name to Description
                            // Will add to the xml once it's ready.
                            std::string stringReplace;
                            if(m_systemConnection[currentSystem].font == "vga8x16.bmp")
                            {
                                stringReplace = "IBM-PC CP437 VGA  8x16";
                            }
                            else if(m_systemConnection[currentSystem].font == "topazPlus-8x16.bmp")
                            {
                                stringReplace = "AMIGA Topaz+ 1200 8x16";
                            }

                            if(isLeftPadding)
                            {
                                MenuFunction::leftSpacing(stringReplace, padding);
                                isLeftPadding = false;
                            }
                            else if(isRightPadding)
                            {
                                MenuFunction::rightSpacing(stringReplace, padding);
                                isRightPadding = false;
                            }
                            stringBuilder += stringReplace;
                        }
                        else if(strcmp(mciCode,"PO") == 0)
                        {
                            sprintf(temp2,"%i",
                                    m_systemConnection[currentSystem].port);
                            std::string stringReplace = temp2;
                            if(isLeftPadding)
                            {
                                MenuFunction::leftSpacing(stringReplace, padding);
                                isLeftPadding = false;
                            }
                            else if(isRightPadding)
                            {
                                MenuFunction::rightSpacing(stringReplace, padding);
                                isRightPadding = false;
                            }
                            stringBuilder += stringReplace;
                        }
                        else if(strcmp(mciCode,"PR") == 0)
                        {
                            std::string stringReplace =
                                m_systemConnection[currentSystem].protocol;
                            if(isLeftPadding)
                            {
                                MenuFunction::leftSpacing(stringReplace, padding);
                                isLeftPadding = false;
                            }
                            else if(isRightPadding)
                            {
                                MenuFunction::rightSpacing(stringReplace, padding);
                                isRightPadding = false;
                            }
                            stringBuilder += stringReplace;
                        }
                        else if(strcmp(mciCode,"KM") == 0)
                        {
                            std::string stringReplace =
                                m_systemConnection[currentSystem].keyMap;
                            if(isLeftPadding)
                            {
                                MenuFunction::leftSpacing(stringReplace, padding);
                                isLeftPadding = false;
                            }
                            else if(isRightPadding)
                            {
                                MenuFunction::rightSpacing(stringReplace, padding);
                                isRightPadding = false;
                            }
                            stringBuilder += stringReplace;
                        }
                        else
                        {
                            stringBuilder += sequence;
                            stringBuilder += mciCode;
                        }
                        break;

                    case '\n' :
                        //  stringBuilder += '\r';
                        break;

                    default :
                        stringBuilder += sequence;
                        break;
                }
                ++counter;
            }
            while(sequence != '\0');

            stringBuilder += "\r";
            switch(ansiCount)
            {
                case 0:
                    _lightbar.ansiString2 = stringBuilder;
                    break; // Display High-light None.  ON
                case 1:
                    _lightbar.ansiString1 = stringBuilder;
                    break; // Display Low-light None    OFF

                default:
                    break;
            }
            stringBuilder.erase();
        } // End of (4) Look for each string.
        result.push_back(_lightbar);
        stringBuilder.erase();
        ++currentSystem;
        ++index;
    }
    return result;
}

/*
 * Read All Systems in Dialing Directory
 */
bool MenuManager::readDialDirectory()
{
    TheTerminal::SystemConnection sysconn;
    std::string path = SetupThePath();
    path += "dialdirectory.xml";
    TiXmlDocument doc(path.c_str());
    if(!doc.LoadFile()) return false;
    TiXmlHandle hDoc(&doc);
    TiXmlElement* pElem;
    TiXmlHandle hRoot(0);

    // If vector already populated then clear to refresh it.
    if(m_systemConnection.size() >0)
    {
        m_systemConnection.clear();
        std::vector<TheTerminal::SystemConnection>().swap(m_systemConnection);
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
    // block: Phone-book
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
            sysconn.keyMap = pElem->Attribute("keyMap");
            // Add to Vector so we can parse in building the dialing directory.
            m_systemConnection.push_back(sysconn);
        }
    }
    std::cout << "readDialDirectory - Done" << std::endl;
    return true;
}

/*
 * Create a new initial dialing directory.
 */
void MenuManager::createDialDirectory()
{
    // Create Default Phone Book.
    std::string path = SetupThePath();
    path += "dialdirectory.xml";

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
    element3->SetAttribute("keyMap", "ANSI");

    TiXmlElement *element4 = new TiXmlElement("BBS");
    element2->LinkEndChild(element4);

    element4->SetAttribute("name", "1984");
    element4->SetAttribute("ip", "1984.ws");
    element4->SetAttribute("port", "23");
    element4->SetAttribute("protocol", "TELNET");
    element4->SetAttribute("login", "");
    element4->SetAttribute("password", "");
    element4->SetAttribute("font", "vga8x16.bmp");
    element4->SetAttribute("keyMap", "VT100");
    doc.SaveFile(path.c_str());
}

/*
 * Write All connections to the dialing directory
 */
void MenuManager::writeDialDirectory()
{
    // Create Default Phone Book.
    std::string path = SetupThePath();
    path += "dialdirectory.xml";

    TiXmlDocument doc;
    TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "", "");
    doc.LinkEndChild(decl);

    TiXmlElement *root = new TiXmlElement("EtherTerm");
    doc.LinkEndChild(root);

    TiXmlElement *phonebook = new TiXmlElement("Phonebook");
    root->LinkEndChild(phonebook);
    phonebook->SetAttribute("version", "1.0");

    // Loop and write out all System Connections
    for(auto &it : m_systemConnection)
    {
        TiXmlElement *system = new TiXmlElement("BBS");
        phonebook->LinkEndChild(system);

        system->SetAttribute("name", it.name);
        system->SetAttribute("ip", it.ip);
        system->SetAttribute("port", it.port);
        system->SetAttribute("protocol", it.protocol);
        system->SetAttribute("login", it.login);
        system->SetAttribute("password", it.password);
        system->SetAttribute("font", it.font);
        system->SetAttribute("keyMap", it.keyMap);
    }
    doc.SaveFile(path.c_str());
}

void MenuManager::setupDialDirectory()
{
    readDirectoryListing();
    m_linkList.m_topMargin = m_directoryTopMargin;
    m_linkList.m_bottomMargin = m_directoryBottomMargin;

    // Read-in or Crete Dialing Directory.
    if(!readDialDirectory())
    {
        createDialDirectory();
        if(!readDialDirectory())
        {
            std::cout << "Error: creating dialdirectory.xml"
                      << std::endl;
            return;
        }
    }

    /*
     * Build And populate the dialing directory into a list.
     */
    m_result = buildDialList();
    m_linkList.getVectorList(m_result);

    // Loop Light bar Interface.
    parseHeader(ANSI_FILE);
    m_boxsize = m_linkList.m_bottomMargin - m_linkList.m_topMargin;
    m_currentPage = LIGHTBAR_POSITION / m_boxsize;
}
/**
 * Title Scan - Start Interface
 */
void MenuManager::updateDialDirectory()
{
    std::string outputBuffer;
    char outBuffer[1024] = {0};

    //  Make sure we have areas.
    if(m_result.size() > 0)
    {
        m_linkList.drawVectorList(m_currentPage, LIGHTBAR_POSITION);
    }
    else
    {
        // Should get here, but we have a check anyhow.
        std::cout << "Error: No Systems found in the dialing directory!"
                  << std::endl;
        return;
    }

    m_linkList.m_totalLines = m_result.size();

    outputBuffer.erase();
    outputBuffer = "|16"; // Clear Color Bleeding, reset background to black.

    // Show Current/Total Pages in Dialing Directory
    sprintf(outBuffer,"%s%.3d",(char *)PAGE_NUMBER.c_str(), m_linkList.m_currentPage+1);
    outputBuffer += outBuffer;
    sprintf(outBuffer,"%s%.3d",(char *)PAGE_TOTAL.c_str(), m_linkList.m_totalPages);
    outputBuffer += outBuffer;

    // # of Systems in Dialing Directory
    sprintf(outBuffer,"%s%.3d",(char *)MAX_SYSTEMS.c_str(), m_linkList.m_totalLines);
    outputBuffer += outBuffer;

    // Parse Sequence to ANSI Ouput and Send to Queue.
    MenuFunction::sequenceToAnsi(outputBuffer);

    // Inital Menu Command ReadIn and Setup.
    m_menuFunction.menuStart(MENU_PROMPT_TEXT);
}

/*
 * If input is received, the pass it to the Menu System
 * for a Return Action or Interface Update!
 */
int MenuManager::handleMenuUpdates(const std::string &inputSequence)
{
    char returnParameters[10] = {0};
    m_menuFunction.menuProcess(returnParameters, inputSequence, LIGHTBAR_POSITION);

    // If we got a passthrough command back, then execute it!
    if (strlen(returnParameters) > 0)
    {
        std::string sequence = returnParameters;
        return(handleMenuActions(sequence));
    }
    return 0;
}

/*
 * Handles Comamnd Keys returned from the menu
 * For what command to execute.
 */
int MenuManager::handleMenuActions(const std::string &inputSequence)
{
    std::string outputBuffer;
    char stringBuffer[1024] = {0};

    if(inputSequence[0] == '!')
    {
        switch(toupper( inputSequence[1] ))
        {
            case 'U': // Page Up
                if(m_currentPage != 0)
                {
                    --m_currentPage;
                    // reset Bar to first Listing on each Page.
                    LIGHTBAR_POSITION = m_currentPage * m_boxsize;
                    m_linkList.drawVectorList(m_currentPage,LIGHTBAR_POSITION);
                }
                break;

            case 'D': // Page Down
                if(m_currentPage+1 != m_linkList.m_totalPages)
                {
                    ++m_currentPage;
                    // reset Bar to first Listing on each Page.
                    LIGHTBAR_POSITION = m_currentPage * m_boxsize;
                    m_linkList.drawVectorList(m_currentPage,LIGHTBAR_POSITION);
                }
                break;

                // hit ENTER
            case 'E': // Selected Current System to Dial
                //m_linkList.clearVectorList();
                //std::vector<list_bar>().swap(m_result); // Free Vector Up.
                // Pass the selected system to the TERM Instance so we can
                // Pull it inside the TelnetState.
                TheTerminal::Instance()->setSystemConnection(m_systemConnection[LIGHTBAR_POSITION]);
                return LIGHTBAR_POSITION;

            case '+': // Next Message - Move Down

                if(LIGHTBAR_POSITION+1 >= m_linkList.m_listing.size() ||
                        m_linkList.m_listing.size() == 0)
                    break;
                //std::cout << "Move to next bar." << std::endl;
                ++LIGHTBAR_POSITION;
                //Calculate if we go down, ++Current Area, are we on next page or not.
                // Because 0 Based, need to add +1
                // Test if we moved to next page.
                if((signed)LIGHTBAR_POSITION+1 < (m_boxsize*(m_currentPage+1))+1)
                {
                    // Low-light Current, then Highlight Next.
                    sprintf(stringBuffer, "\x1b[%i;%iH|16%s",
                        m_linkList.m_currentSelection, 1,
                        (char *)m_linkList.m_listing[LIGHTBAR_POSITION-1].ansiString1.c_str());

                    outputBuffer += stringBuffer;
                    m_linkList.m_currentSelection += 1;

                    sprintf(stringBuffer, "\x1b[%i;%iH|16%s",
                        m_linkList.m_currentSelection, 1,
                        (char *)m_linkList.m_listing[LIGHTBAR_POSITION].ansiString2.c_str());

                    outputBuffer += stringBuffer;
                    MenuFunction::sequenceToAnsi(outputBuffer);
                    outputBuffer.erase();
                }
                else
                {
                    // Move to next Page!
                    ++m_currentPage;
                    m_linkList.drawVectorList(m_currentPage,LIGHTBAR_POSITION);
                }
                break;
            case '-': // Previous Message - Move Up

                // Skipping to JMPINPUT bypasses redraws, much faster!
                if(LIGHTBAR_POSITION > 0 && m_linkList.m_listing.size() != 0)
                    --LIGHTBAR_POSITION;
                else
                    break;

                //Calculate if we go down, --Current Area, are we on next page or not.
                // Because 0 Based, need to add +1
                // Test if we moved to next page.
                if((signed)LIGHTBAR_POSITION+1 > (m_boxsize*(m_currentPage)))
                {
                    // Still on Same Page
                    // Low-light Current, then Highlight Next.
                    sprintf(stringBuffer, "\x1b[%i;%iH|16%s", m_linkList.m_currentSelection, 1,
                        (char *)m_linkList.m_listing[LIGHTBAR_POSITION+1].ansiString1.c_str());
                    outputBuffer = stringBuffer;
                    m_linkList.m_currentSelection -= 1;

                    sprintf(stringBuffer, "\x1b[%i;%iH|16%s", m_linkList.m_currentSelection, 1,
                        (char *)m_linkList.m_listing[LIGHTBAR_POSITION].ansiString2.c_str());
                    outputBuffer += stringBuffer;
                    MenuFunction::sequenceToAnsi(outputBuffer);
                    outputBuffer.erase();
                }
                else
                {
                    // Move to next Page!
                    --m_currentPage;
                    m_linkList.drawVectorList(m_currentPage,LIGHTBAR_POSITION);
                }
                break;

            case 'Q': // Quit Received, Hard Exit.
                return EOF;

            case '?': // Directory Help
                // Not Implimented Yet!
                //ansiPrintf(sANSI_HELP);
                //getkey(true);
                parseHeader(ANSI_FILE);
                m_linkList.drawVectorList(m_currentPage,LIGHTBAR_POSITION);
                break;

            case 'A': // About
                parseHeader((char *)"about.ans");
                MenuFunction::getKey();
                parseHeader((char *)"about2.ans");
                MenuFunction::getKey();
                parseHeader((char *)"et2.ans"); // Re-display Display ANSI
                m_linkList.drawVectorList(m_currentPage,LIGHTBAR_POSITION);
                break;

            default :
                break;
        }
    }
    return 0;
}

