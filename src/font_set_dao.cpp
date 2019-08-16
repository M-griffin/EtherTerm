#include "font_set_dao.hpp"
#include "font_set.hpp"
#include "static_methods.hpp"

#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <algorithm>
#include <cassert>

// Setup the file version for the dialing directory file.
const std::string FontSet::FILE_VERSION = "1.0.0";

FontSetDao::FontSetDao(std::string path)
    : m_font_set(new FontSet())
    , m_program_path(path)
    , m_filename("font_set.yaml")
{
}

FontSetDao::~FontSetDao()
{
}

/**
 * @brief Check if the file exists and we need to create a new one.
 * @return
 */
bool FontSetDao::fileExists()
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
bool FontSetDao::saveFontSet(font_set_ptr font_set)
{
    std::string path = StaticMethods::getAssetPath(m_program_path);
    path.append(m_filename);

    YAML::Emitter out;

    out << YAML::BeginMap;
    out << YAML::Flow;

    // Start Creating the Key/Value Output for the Menu File.
    out << YAML::Key << "file_version" << YAML::Value << FontSet::FILE_VERSION;

    // Loop and encode each menu option
    for(unsigned int i = 0; i < font_set->font_entries.size(); i++)
    {
        auto &opt = font_set->font_entries[i];

        out << YAML::Key << "font_entry";
        out << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "index" << YAML::Value << opt.index;
        out << YAML::Key << "name" << YAML::Value << opt.name;
        out << YAML::Key << "width" << YAML::Value << opt.width;
        out << YAML::Key << "height" << YAML::Value << opt.height;
        out << YAML::Key << "filename" << YAML::Value << opt.filename;
        out << YAML::Key << "sequence" << YAML::Value << opt.sequence;

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
void FontSetDao::encode(const FontSet &rhs)
{
    m_font_set->file_version = rhs.file_version;
    m_font_set->font_entries = rhs.font_entries;

    // Unforunatally YAML does not keep ordering properly.
    // So we sort on the index field
    sort(
        m_font_set->font_entries.begin(), m_font_set->font_entries.end(),
        [ ](const FontEntry& lhs, const FontEntry& rhs)
    {
        return lhs.index < rhs.index;
    });
}

/**
 * @brief Loads a DialingDirectory into a smart pointer
 * @return
 */
font_set_ptr FontSetDao::loadFontSet()
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
            std::cout << m_filename << " is empty!" << std::endl;
            return nullptr;
        }

        std::string file_version = node["file_version"].as<std::string>();
        std::cout << m_filename << " File Version= " << file_version << std::endl;

        if(file_version != FontSet::FILE_VERSION)
        {
            std::cout << m_filename << " File Version= " << file_version << ", Expected Version= "
                      << FontSet::FILE_VERSION  << std::endl;
            return nullptr;
        }

        // Moves the Loaded config to the Class shared pointer instance for return.
        FontSet dir = node.as<FontSet>();
        encode(dir);

        // Loaded with System Entries, then Successful
        if(m_font_set->font_entries.size() > 0)
        {
            return m_font_set;
        }
    }
    catch(YAML::Exception &ex)
    {
        std::cout << "YAML::LoadFile " << m_filename << " " << ex.what() << " Missing required field maybe."
                  << std::endl;
    }
    catch(std::exception &ex)
    {
        std::cout << "Unexpected YAML::LoadFile " << m_filename << " " << ex.what() << std::endl;
    }

    return nullptr;
}
