#include "menu_manager.hpp"
#include "menu_config.hpp"
#include "renderer.hpp"
#include "sequence_decoder.hpp"
#include "session.hpp"
#include "session_manager.hpp"

// Temp, replace with boost later on.
#include "tinyxml.hpp"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <vector>

MenuManager::MenuManager(
    std::string          program_path,
    renderer_ptr         renderer,
    sequence_decoder_ptr decoder
)
    : m_program_path(program_path)
    , m_menu_config(program_path)
    , m_sequence_decoder(decoder)
    , m_menu_function(decoder, program_path)
    , m_renderer(renderer)
    , m_lightbar_position(0)
    , m_directory_top_margin(1)
    , m_directory_bottom_margin(1)
    , m_current_page(0)
    , m_box_size(0)
    , m_current_theme_index(0) // NOTE Reload this from saved preference!
{
    std::cout << "MenuManager Created!" << std::endl;
}

MenuManager::~MenuManager()
{
    std::cout << "~MenuManager" << std::endl;
    std::vector<list_bar>()
        .swap(m_result);

    std::vector<SystemConnection>()
        .swap(m_systemConnection);
}

/**
 * @brief Helper to append folder to path
 */
std::string MenuManager::getAssetPath()
{
    // Create Default Phone Book.
    std::string path = m_program_path;
#ifdef _WIN32
    path.append("assets\\");
#else
    path.append("assets/");
#endif
    return path;
}

/**
 * @brief Helper to append directory path
 */
std::string MenuManager::getDirectoryPath()
{
    // Create Default Phone Book.
    std::string path = getAssetPath();
#ifdef _WIN32
    path.append("directory\\");
#else
    path.append("directory/");
#endif
    return path;
}

/**
 * @brief Helper to append ansi path
 */
std::string MenuManager::getAnsiPath()
{
    // Create Default Phone Book.
    std::string path = getAssetPath();
#ifdef _WIN32
    path.append("ansi\\");
#else
    path.append("ansi/");
#endif
    return path;
}

/**
 * @brief Reads in ANSI file into Buffer Only for Parsing.
 * @param FileName
 * @param buff
 */
void MenuManager::readinAnsi(std::string FileName, std::string &buff)
{
    std::string path = getAnsiPath();
    path.append(FileName);
    path.append(".ans");

    int sequence = 0;
    FILE *fp;
    if((fp = fopen(path.c_str(), "r+")) ==  nullptr)
    {
        std::cout << "ANSI not found: " << path << std::endl;
        return;
    }
    do
    {
        sequence = getc(fp);
        if(sequence != EOF)
        {
            buff += (char)sequence;
        }
    }
    while(sequence != EOF);
    fclose(fp);
}

/**
 * @brief Parse Ansi Template (Dialing Directory)
 * @param FileName
 */
void MenuManager::parseHeader(std::string FileName)
{
    std::cout << "parseHeader()" << std::endl;
    std::cout << "sFONT_SET: " << m_menu_config.m_font_set << std::endl;

    // Set the font type for the menu being displayed.
    m_renderer->m_surface_manager->setCurrentFont(m_menu_config.m_font_set);

    // Test if font changed, if so, then re-load it.
    if(m_renderer->m_surface_manager->didFontChange())
    {
        // loadBitmapImage
        if(!m_renderer->m_surface_manager->loadFont())
        {
            SDL_Delay(1500);
            assert(false);
            return;
        }
    }
    m_sequence_decoder->resetParser();
    m_menu_function.m_menu_io.displayMenuAnsi(FileName);
}

/**
 * @brief Read in and Parse, Setup Margins for Directory listing
 */
void MenuManager::readDirectoryListing()
{
    /**
     * NOTE this should be set up by the last used theme!
     * Right now it uses default on startup!
     */
    if (m_current_theme_index == 0)
    {
        m_menu_config.m_ini_name = "directory.ini";
    }
    else
    {
        m_menu_config.m_ini_name = "directory";
        m_menu_config.m_ini_name.append(std::to_string(m_current_theme_index));
        m_menu_config.m_ini_name.append(".ini");
    }

    m_menu_config.ddirectory_parse();
    m_directory_top_margin = m_menu_config.m_top_margin;
    m_directory_bottom_margin = m_menu_config.m_bottom_margin;
}

/**
 * Dial-directory - Change ANSI Template Theme
 */
bool MenuManager::changeTheme(int index)
{
    if(!m_menu_config.ddirectory_parse(index))
    {
        // Theme Doesn't Exist.
        return false;
    }
    // Set the new index, then reset the margins for the list.
    m_current_theme_index = index;
    m_link_list.m_top_margin = m_menu_config.m_top_margin;
    m_link_list.m_bottom_margin = m_menu_config.m_bottom_margin;
    return true;
}

/**
 * @brief Helper Function to Handle left and right padding
 * @return
 */
std::string MenuManager::padString(const std::string &value, const std::string &justify, int padding)
{
    std::string stringReplace = value;
    if(justify == "LEFT")
    {
        m_menu_function.m_menu_io.leftSpacing(stringReplace, padding);
        std::replace(
            stringReplace.begin(),
            stringReplace.end(),
            ' ', '.'
        );
    }
    else if(justify == "RIGHT")
    {
        m_menu_function.m_menu_io.rightSpacing(stringReplace, padding);
        std::replace(
            stringReplace.begin(),
            stringReplace.end(),
            ' ', '.'
        );
    }
    return stringReplace;
}

/**
 * @brief Build the List of Systems to Display Lightbars.
 * @return
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
    // 2. Current System Selected
    //
    // We cache array with all four choices so we can
    // easily switch between them in the listing.
    std::string path = getDirectoryPath();
    path.append(m_menu_config.m_mid_ansi_1);

    if((inStream = fopen(path.c_str(), "r+")) ==  nullptr)
    {
        std::cout << "Error Mid ANSI 1: " <<  path << std::endl;
        return result;
    }
    while(sequence != EOF)
    {
        sequence = getc(inStream);
        if(sequence != EOF) ansiScreen1 += sequence;
    }
    fclose(inStream);
    sequence = '\0';

    // Process the Second Mid Ansi File
    path = getDirectoryPath();
    path.append(m_menu_config.m_mid_ansi_2);
    if((inStream = fopen(path.c_str(), "r+")) ==  nullptr)
    {
        std::cout << "Error Mid ANSI 2: " <<  path << std::endl;
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
                                std::string stringReplace = std::move(padString(temp2, "LEFT", padding));
                                stringBuilder += stringReplace;
                                isLeftPadding = false;
                            }
                            else if(isRightPadding)
                            {
                                std::string stringReplace = std::move(padString(temp2, "RIGHT", padding));
                                stringBuilder += stringReplace;
                                isRightPadding = false;
                            }
                        }
                        else if(strcmp(mciCode,"NA") == 0)
                        {
                            std::string stringReplace =
                                m_systemConnection[currentSystem].name;
                            if(isLeftPadding)
                            {
                                m_menu_function.m_menu_io.leftSpacing(stringReplace, padding);
                                isLeftPadding = false;
                            }
                            else if(isRightPadding)
                            {
                                m_menu_function.m_menu_io.rightSpacing(stringReplace, padding);
                                isRightPadding = false;
                            }
                            stringBuilder += stringReplace;
                        }
                        else if(strcmp(mciCode,"FO") == 0)
                        {
                            // Load the Font from the selected system
                            FontSet font(
                                m_renderer->m_surface_manager->getFontFromSet(
                                    m_systemConnection[currentSystem].font
                                )
                            );
                            // Replace with the Font name from XML.
                            std::string stringReplace = font.name;
                            if(isLeftPadding)
                            {
                                m_menu_function.m_menu_io.leftSpacing(stringReplace, padding);
                                isLeftPadding = false;
                            }
                            else if(isRightPadding)
                            {
                                m_menu_function.m_menu_io.rightSpacing(stringReplace, padding);
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
                                m_menu_function.m_menu_io.leftSpacing(stringReplace, padding);
                                isLeftPadding = false;
                            }
                            else if(isRightPadding)
                            {
                                m_menu_function.m_menu_io.rightSpacing(stringReplace, padding);
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
                                m_menu_function.m_menu_io.leftSpacing(stringReplace, padding);
                                isLeftPadding = false;
                            }
                            else if(isRightPadding)
                            {
                                m_menu_function.m_menu_io.rightSpacing(stringReplace, padding);
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
                                m_menu_function.m_menu_io.leftSpacing(stringReplace, padding);
                                isLeftPadding = false;
                            }
                            else if(isRightPadding)
                            {
                                m_menu_function.m_menu_io.rightSpacing(stringReplace, padding);
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
                    _lightbar.inactive_lightbar = stringBuilder;
                    break; // Display High-light None.  ON
                case 1:
                    _lightbar.active_lightbar = stringBuilder;
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

/**
 * @brief Read in the XML Data File.
 * @return
 */
bool MenuManager::readDialDirectory()
{
    SystemConnection sysconn;
    std::string path = getAssetPath();
    path.append("dialdirectory.xml");
    TiXmlDocument doc(path.c_str());
    if(!doc.LoadFile())
    {
        std::cout << "Error Reading XML" << path << std::endl;
        return false;
    }
    TiXmlHandle hDoc(&doc);
    TiXmlElement* pElem;
    TiXmlHandle hRoot(0);

    // If vector already populated then clear to refresh it.
    if(m_systemConnection.size() > 0)
    {
        m_systemConnection.clear();
        std::vector<SystemConnection>().swap(m_systemConnection);
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
            sysconn.termType = pElem->Attribute("termType");
            // Add to Vector so we can parse in building the dialing directory.
            m_systemConnection.push_back(sysconn);
        }
    }
    std::cout << "readDialDirectory - Done" << std::endl;
    return true;
}

/**
 * @brief Create a new XML File.
 */
void MenuManager::createDialDirectory()
{
    // Create Default Phone Book.
    std::string path = getAssetPath();
    path.append("dialdirectory.xml");

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
    element3->SetAttribute("termType", "ANSI");

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
    element4->SetAttribute("termType", "ANSI");
    doc.SaveFile(path.c_str());
}

/**
 * @brief Write all Connections to XML
 */
void MenuManager::writeDialDirectory()
{
    // Create Default Phone Book.
    std::string path = getAssetPath();
    path.append("dialdirectory.xml");

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
        system->SetAttribute("termType", it.termType);
    }
    doc.SaveFile(path.c_str());
}

/**
 * @brief Readin and Parse the Dialing Directory.
 */
void MenuManager::setupDialDirectory()
{
    readDirectoryListing();
    m_link_list.m_top_margin = m_directory_top_margin;
    m_link_list.m_bottom_margin = m_directory_bottom_margin;

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
    m_result = std::move(buildDialList());
    m_link_list.getVectorList(m_result);

    // Loop Light bar Interface.
    parseHeader(m_menu_config.m_ansi_filename);
    m_box_size = m_link_list.m_bottom_margin - m_link_list.m_top_margin;
    m_current_page = m_lightbar_position / m_box_size;
}

/**
 * @brief Main Interface for Dialing Directory
 */
void MenuManager::updateDialDirectory()
{
    std::string outputBuffer;
    char outBuffer[1024] = {0};

    //  Make sure we have areas, then pull the list into the string buffer.
    if(m_result.size() > 0)
    {
        outputBuffer = m_link_list.drawVectorList(m_current_page, m_lightbar_position);
    }
    else
    {
        // Should get here, but we have a check anyhow.
        std::cout << "Error: No Systems found in the dialing directory!"
                  << std::endl;
        return;
    }

    m_link_list.m_total_lines = m_result.size();

    //outputBuffer.erase();
    outputBuffer += "|16"; // Clear Color Bleeding, reset background to black.

    // Show Current/Total Pages in Dialing Directory
    sprintf(outBuffer,"%s%.3d",(char *)m_menu_config.m_page_number.c_str(), m_link_list.m_current_page+1);
    outputBuffer += outBuffer;
    sprintf(outBuffer,"%s%.3d",(char *)m_menu_config.m_page_total.c_str(), m_link_list.m_total_pages);
    outputBuffer += outBuffer;

    // # of Systems in Dialing Directory
    sprintf(outBuffer,"%s%.3d",(char *)m_menu_config.m_max_systems.c_str(), m_link_list.m_total_lines);
    outputBuffer += outBuffer;

    // Parse Sequence to ANSI output and Send to Queue.
    m_menu_function.m_menu_io.sequenceToAnsi(outputBuffer);

    // Initial Menu Command Read In and Setup.
    m_menu_function.menuStart(m_menu_config.m_menu_prompt_text);
}

/**
 * @brief Pass-Through updates and returns changes.
 * @param inputSequence
 * @return
 */
int MenuManager::handleMenuUpdates(const std::string &inputSequence)
{
    char returnParameters[10] = {0};
    m_menu_function.menuProcess(returnParameters, inputSequence, m_lightbar_position);

    // If we got a pass through command back, then execute it!
    if(strlen(returnParameters) > 0)
    {
        std::string sequence = returnParameters;
        return(handleMenuActions(sequence));
    }
    return 0;
}

/**
 * @brief Handle Menu Actions from Input
 * @param inputSequence
 * @return
 */
int MenuManager::handleMenuActions(const std::string &inputSequence)
{
    std::string outputBuffer;
    char stringBuffer[1024] = {0};

    if(inputSequence[0] == '!')
    {
        switch(toupper(inputSequence[1]))
        {
            case 'U': // Page Up
                if(m_current_page != 0)
                {
                    --m_current_page;
                    // reset Bar to first Listing on each Page.
                    m_lightbar_position = m_current_page * m_box_size;
                    m_link_list.drawVectorList(m_current_page, m_lightbar_position);
                }
                break;

            case 'D': // Page Down
                if(m_current_page+1 != m_link_list.m_total_pages)
                {
                    ++m_current_page;
                    // reset Bar to first Listing on each Page.
                    m_lightbar_position = m_current_page * m_box_size;
                    m_link_list.drawVectorList(m_current_page, m_lightbar_position);
                }
                break;

                // hit ENTER
            case 'E': // Selected Current System to Dial
            {
                // Grab Handle to the Current Session/Session Manager to Add to Queue.
                session_ptr session = m_renderer->m_weak_session.lock();
                if (session)
                {
                    session_manager_ptr session_mgr = session->m_weak_session_manager.lock();
                    if (session_mgr)
                    {
                        // Create a New System Connection Pointer of Selected System
                        system_connection_ptr new_connection(
                                                new SystemConnection(
                                                    m_systemConnection[m_lightbar_position]
                                                ));
                        // Add the System to the Connection Queue.
                        session_mgr->m_new_connections.enqueue(new_connection);
                    }
                }
                return m_lightbar_position;
            }
            case '+': // Next Message - Move Down
                if(m_lightbar_position+1 >= m_link_list.m_listing.size() ||
                        m_link_list.m_listing.size() == 0)
                    break;

                ++m_lightbar_position;
                //Calculate if we go down, ++Current Area, are we on next page or not.
                // Because 0 Based, need to add +1
                // Test if we moved to next page.
                if((signed)m_lightbar_position+1 < (m_box_size*(m_current_page+1))+1)
                {
                    // Low-light Current, then Highlight Next.
                    sprintf(stringBuffer, "\x1b[%i;%iH|16%s",
                            m_link_list.m_current_selection, 1,
                            (char *)m_link_list.m_listing[m_lightbar_position-1].active_lightbar.c_str());

                    outputBuffer += stringBuffer;
                    m_link_list.m_current_selection += 1;

                    sprintf(stringBuffer, "\x1b[%i;%iH|16%s",
                            m_link_list.m_current_selection, 1,
                            (char *)m_link_list.m_listing[m_lightbar_position].inactive_lightbar.c_str());

                    outputBuffer += stringBuffer;
                    m_menu_function.m_menu_io.sequenceToAnsi(outputBuffer);
                    outputBuffer.erase();
                }
                else
                {
                    // Move to next Page!
                    ++m_current_page;
                    m_link_list.drawVectorList(m_current_page,m_lightbar_position);
                }
                break;
            case '-': // Previous Message - Move Up

                // Skipping to JMPINPUT bypasses redraws, much faster!
                if(m_lightbar_position > 0 && m_link_list.m_listing.size() != 0)
                    --m_lightbar_position;
                else
                    break;

                //Calculate if we go down, --Current Area, are we on next page or not.
                // Because 0 Based, need to add +1
                // Test if we moved to next page.
                if((signed)m_lightbar_position+1 > (m_box_size*(m_current_page)))
                {
                    // Still on Same Page
                    // Low-light Current, then Highlight Next.
                    sprintf(stringBuffer, "\x1b[%i;%iH|16%s", m_link_list.m_current_selection, 1,
                            (char *)m_link_list.m_listing[m_lightbar_position+1].active_lightbar.c_str());
                    outputBuffer = stringBuffer;
                    m_link_list.m_current_selection -= 1;

                    sprintf(stringBuffer, "\x1b[%i;%iH|16%s", m_link_list.m_current_selection, 1,
                            (char *)m_link_list.m_listing[m_lightbar_position].inactive_lightbar.c_str());
                    outputBuffer += stringBuffer;
                    m_menu_function.m_menu_io.sequenceToAnsi(outputBuffer);
                    outputBuffer.erase();
                }
                else
                {
                    // Move to next Page!
                    --m_current_page;
                    m_link_list.drawVectorList(m_current_page,m_lightbar_position);
                }
                break;

            case 'Q': // Quit Received, Hard Exit.
                return EOF;

            case '?': // Directory Help
                //ansiPrintf(sANSI_HELP);
                //getkey(true);
                parseHeader(m_menu_config.m_ansi_filename);
                m_link_list.drawVectorList(m_current_page,m_lightbar_position);
                break;

            case 'A': // About
                parseHeader((char *)"about.ans");

                // Get input (Blocking!) This should be rewritten later on.
                //parseHeader((char *)"about2.ans");

                // Get input (Blocking!) This should be rewritten later on.
                //parseHeader((char *)"et2.ans"); // Re-display Display ANSI
                m_link_list.drawVectorList(m_current_page,m_lightbar_position);
                break;

            case ']': // Next Theme
            {
                int idx = m_current_theme_index;
                ++idx;
                bool is_theme_changed = changeTheme(idx);
                if (is_theme_changed)
                {
                    std::string output = "|CS|CR|15You've switched to the ";
                    output.append(m_menu_config.m_theme_name);
                    output.append(" theme.");
                    m_menu_function.m_menu_io.sequenceToAnsi(output);
                }
                else
                {
                    m_menu_function.m_menu_io.sequenceToAnsi(
                        "|CS|CR|04You've hit the highest theme available."
                    );
                }
                m_menu_function.m_previous_menu.erase();
                // Now redraw the dialing directory
                parseHeader(m_menu_config.m_ansi_filename);
                m_link_list.drawVectorList(m_current_page, m_lightbar_position);
                break;
            }

            case '[': // Previous Theme
            {
                int idx = m_current_theme_index;
                --idx;
                bool is_theme_changed = changeTheme(idx);
                if (is_theme_changed)
                {
                    std::string output = "|CS|CR|15You've switched to the ";
                    output.append(m_menu_config.m_theme_name);
                    output.append(" theme.");
                    m_menu_function.m_menu_io.sequenceToAnsi(output);
                }
                else
                {
                    m_menu_function.m_menu_io.sequenceToAnsi(
                        "|CS|CR|04You've hit the lowest theme available."
                    );

                }
                // Now redraw the dialing directory
                m_menu_function.m_previous_menu.erase();
                parseHeader(m_menu_config.m_ansi_filename);
                m_link_list.drawVectorList(m_current_page, m_lightbar_position);
                break;
            }

            case 'F': // HOME / Front of List
                m_current_page = 0;
                // Reset Bar to first System in Listing
                m_lightbar_position = m_current_page * m_box_size;
                m_link_list.drawVectorList(m_current_page, m_lightbar_position);
                break;

            case 'B': // END / Back of List
                m_current_page = m_link_list.m_total_pages-1;
                // Reset Bar to Last System in Listing.
                m_lightbar_position = m_link_list.m_total_lines-1;
                m_link_list.drawVectorList(m_current_page, m_lightbar_position);
                break;

            default :
                break;
        }
    }
    return 0;
}
