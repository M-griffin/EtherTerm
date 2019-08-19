#include "dialing_directory_dao.hpp"
#include "dialing_directory.hpp"
#include "static_methods.hpp"

#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <algorithm>
#include <cassert>

// Setup the file version for the dialing directory file.
const std::string DialingDirectory::FILE_VERSION = "1.0.0";

DialingDirectoryDao::DialingDirectoryDao(std::string path)
    : m_dial_directory(new DialingDirectory())
    , m_program_path(path)
    , m_filename("dialing_directory.yaml")
{
}

DialingDirectoryDao::~DialingDirectoryDao()
{
}

/**
 * @brief Check if the file exists and we need to create a new one.
 * @return
 */
bool DialingDirectoryDao::fileExists()
{
    std::string path = StaticMethods::getAssetPath(m_program_path);
    path.append(m_filename);

    std::ifstream ifs(path);

    if(!ifs.is_open())
    {
        return false;
    }

    ifs.close();
    return true;
}

/**
 * @brief Creates and Saves a newly Generated Menu File.
 * @param cfg
 * @return
 */
bool DialingDirectoryDao::saveDialingDirectory(dial_directory_ptr dialing_directory)
{
    std::string path = StaticMethods::getAssetPath(m_program_path);
    path.append(m_filename);

    YAML::Emitter out;

    out << YAML::BeginMap;
    out << YAML::Flow;

    // Start Creating the Key/Value Output for the Menu File.
    out << YAML::Key << "file_version" << YAML::Value << DialingDirectory::FILE_VERSION;

    // Loop and encode each menu option
    for(unsigned int i = 0; i < dialing_directory->system_entries.size(); i++)
    {
        auto &opt = dialing_directory->system_entries[i];

        out << YAML::Key << "system_entry";
        out << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "index" << YAML::Value << opt.index;
        out << YAML::Key << "name" << YAML::Value << opt.name;
        out << YAML::Key << "ip" << YAML::Value << opt.ip;
        out << YAML::Key << "port" << YAML::Value << opt.port;
        out << YAML::Key << "protocol" << YAML::Value << opt.protocol;
        out << YAML::Key << "login" << YAML::Value << opt.login;
        out << YAML::Key << "password" << YAML::Value << opt.password;
        out << YAML::Key << "font" << YAML::Value << opt.font;
        out << YAML::Key << "key_map" << YAML::Value << opt.key_map;
        out << YAML::Key << "term_type" << YAML::Value << opt.term_type;
        out << YAML::Key << "term_size" << YAML::Value << opt.term_size;
        out << YAML::EndMap;
    }

    out << YAML::EndMap;
    std::ofstream ofs(path);

    if(!ofs.is_open())
    {
        std::cout << "Error, unable to write to " << path << std::endl;
        return false;
    }

    ofs << out.c_str();
    ofs.close();

    return true;
}

/**
 * @brief Copies Loaded DialingDirectory to shared pointer and sorts entries
 * @param rhs
 * @return
 */
void DialingDirectoryDao::encode(const DialingDirectory &rhs)
{
    m_dial_directory->file_version   = rhs.file_version;
    m_dial_directory->system_entries = rhs.system_entries;

    // Unforunatally YAML does not keep ordering properly.
    // So we sort on the index field
    sort(
        m_dial_directory->system_entries.begin(), m_dial_directory->system_entries.end(),
        [ ](const SystemEntry& lhs, const SystemEntry& rhs)
    {
        return lhs.index < rhs.index;
    });
}

/**
 * @brief Loads a DialingDirectory into a smart pointer
 * @return
 */
dial_directory_ptr DialingDirectoryDao::loadDialingDirectory()
{
    std::string path = StaticMethods::getAssetPath(m_program_path);
    path.append(m_filename);

    YAML::Node node;

    // Load the file into the class.
    try
    {
        // Load file fresh.
        node = YAML::LoadFile(path);

        if(node.size() == 0)
        {
            std::cout << path << " is empty!" << std::endl;
            return nullptr;
        }

        std::string file_version = node["file_version"].as<std::string>();
        std::cout << m_filename << " File Version= " << file_version << std::endl;

        if(file_version != DialingDirectory::FILE_VERSION)
        {
            std::cout << m_filename << " File Version= " << file_version << ", Expected Version= "
                      << DialingDirectory::FILE_VERSION  << std::endl;
            return nullptr;
        }

        // Moves the Loaded config to the Class shared pointer instance for return.
        DialingDirectory dir = node.as<DialingDirectory>();
        encode(dir);

        // Loaded with System Entries, then Successful
        if(m_dial_directory->system_entries.size() > 0)
        {
            return m_dial_directory;
        }
    }
    catch(YAML::Exception &ex)
    {
        std::cout << "YAML::LoadFile " << path << " " << ex.what() << " Missing required field maybe."
                  << std::endl;
    }
    catch(std::exception &ex)
    {
        std::cout << "Unexpected YAML::LoadFile " << path << " " << ex.what() << std::endl;
    }

    return nullptr;
}
