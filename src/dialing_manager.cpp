#include "dialing_manager.hpp"
#include "dialing_directory_dao.hpp"
#include "dialing_directory.hpp"
#include "static_methods.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <regex>

DialingManager::DialingManager(std::string path)
    : m_dial_directory_dao(new DialingDirectoryDao(path))
    , m_program_path(path)
{
}

DialingManager::~DialingManager()
{
}

/**
 * @brief Match Regex Expression and return results.
 * @param str_data
 * @param re
 * @return
 */
std::string DialingManager::searchForRegexMatch(std::string str_data, std::regex re)
{
    std::string result = "";
    std::smatch sm;

    try
    {
        if(std::regex_search(str_data, sm, re) && sm.size() > 1)
        {
            result = sm.str(1);
            std::cout << "MATCH: " << result << std::endl;
        }
    }
    catch(const std::regex_error& e)
    {
        std::cout << "regex_error caught: " << e.what() << '\n';

        if(e.code() == std::regex_constants::error_brack)
        {
            std::cout << "The code was error_brack\n";
        }
    }

    return result;
}

/**
 * @brief Check if the file exists Previous XML Dialing Directory
 * @return
 */
bool DialingManager::previousXmlFileExists()
{
    std::string path = StaticMethods::getAssetPath(m_program_path);
    path.append("dialdirectory.xml");

    std::ifstream ifs(path);

    if(!ifs.is_open())
    {
        return false;
    }

    ifs.close();
    return true;
}

/**
 * @brief Convert Previous XML Attributes to Yaml Format.
 */
void DialingManager::convertXmltoYaml()
{
    std::string path = StaticMethods::getAssetPath(m_program_path);
    path.append("dialdirectory.xml");

    std::ifstream ifs(path);

    if(!ifs.is_open())
    {
        std::cout << "Unable to open file: " << path << std::endl;
        return;
    }

    std::string str_data;
    int system_counter = 0;

    dial_directory_ptr dial_directory = nullptr;

    // Check for existing or new Yaml File.
    if(!m_dial_directory_dao->fileExists())
    {
        // Create a NEw file here with at least one system.
        dial_directory.reset(new DialingDirectory());
    }
    else
    {
        dial_directory = retrieveDialingDirectory();

        if(dial_directory != nullptr)
        {
            system_counter = dial_directory->system_entries.size();
        }
    }


    while(std::getline(ifs, str_data, '\n'))
    {
        std::regex re_name("name=\"([\\s\\S]*?)\"");
        std::string name = searchForRegexMatch(str_data, re_name);

        std::regex re_ip("ip=\"([\\s\\S]*?)\"");
        std::string ip = searchForRegexMatch(str_data, re_ip);

        std::regex re_port("port=\"([\\s\\S]*?)\"");
        std::string port = searchForRegexMatch(str_data, re_port);

        std::regex re_prot("protocol=\"([\\s\\S]*?)\"");
        std::string protocol = searchForRegexMatch(str_data, re_prot);

        std::regex re_login("login=\"([\\s\\S]*?)\"");
        std::string login = searchForRegexMatch(str_data, re_login);

        std::regex re_pass("password=\"([\\s\\S]*?)\"");
        std::string password = searchForRegexMatch(str_data, re_pass);

        std::regex re_font("font=\"([\\s\\S]*?)\"");
        std::string font = searchForRegexMatch(str_data, re_font);

        std::regex re_key("keyMap=\"([\\s\\S]*?)\"");
        std::string key_map = searchForRegexMatch(str_data, re_key);

        std::regex re_type("termType=\"([\\s\\S]*?)\"");
        std::string term_type = searchForRegexMatch(str_data, re_type);

        std::regex re_size("termSize=\"([\\s\\S]*?)\"");
        std::string term_size = searchForRegexMatch(str_data, re_size);

        // Make sure we have a match before generating an entry.
        if(name.size() > 0 && ip.size() > 0 && protocol.size() > 0 && font.size() > 0)
        {
            SystemEntry entry;
            entry.index = system_counter++;
            entry.name = name;
            entry.ip = ip;
            entry.port = stringToNumber<int>(port);
            entry.protocol = protocol;
            entry.login = login;
            entry.password = password;
            entry.font = font;
            entry.key_map = key_map;
            entry.term_type = term_type;
            entry.term_size = term_size;

            dial_directory->system_entries.push_back(entry);
        }
    }

    ifs.close();

    if(dial_directory->system_entries.size() > 0 && !m_dial_directory_dao->saveDialingDirectory(dial_directory))
    {
        return;
    }

    // Once completed, rename / move the file to .bak so it's not processed again.
    std::string rename_to = path;
    rename_to.append(".bak");

    if(std::rename(path.c_str(), rename_to.c_str()))
    {
        std::cout << "Error Renaming dialdirectoy.xml file, I reccomend you rename or remove it manually." << std::endl;
        return;
    }

}


bool DialingManager::validateFile()
{
    if(!m_dial_directory_dao->fileExists())
    {
        // Create a NEw file here with at least one system.
        dial_directory_ptr dial_directory(new DialingDirectory());
        SystemEntry entry;
        entry.index = 0;
        entry.name = "Haunting The Chapel";
        entry.ip = "htc.zapto.org";
        entry.port = 23;
        entry.protocol = "Telnet";
        entry.login = "";
        entry.password = "";
        entry.font = "vga8x16.bmp";
        entry.key_map = "VT100";
        entry.term_type = "ANSI";
        entry.term_size = "80x25";

        dial_directory->system_entries.push_back(entry);

        if(!m_dial_directory_dao->saveDialingDirectory(dial_directory))
        {
            return false;
        }
    }
    else
    {
        return true;
    }

    return true;
}

dial_directory_ptr DialingManager::retrieveDialingDirectory()
{
    return (m_dial_directory_dao->loadDialingDirectory());
}
