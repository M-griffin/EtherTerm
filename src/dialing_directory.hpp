#ifndef DIALING_DIRECTORY_HPP
#define DIALING_DIRECTORY_HPP

#include "system_entry.hpp"

#include <memory>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include <yaml-cpp/yaml.h>


/**
 * @class DialingDirectory
 * @author Michael Griffin
 * @date 07/08/2019
 * @file dial_directory.hpp
 * @brief Dialing Directory, holds System Entries and File Version in YAML
 */
class DialingDirectory
{

public:

    static const std::string FILE_VERSION;

    std::string file_version;
    std::vector<SystemEntry> system_entries;

    explicit DialingDirectory()
        : file_version(FILE_VERSION)

    { }

    ~DialingDirectory() { }
};



namespace YAML
{

/**
 * @class convert
 * @author Michael Griffin
 * @date 16/11/2016
 * @file menu.hpp
 * @brief YAML Conversion for DialingDirectory Class
 */
template<>
struct convert<DialingDirectory>
{
    /**
     * @brief Overide for encoding the DialingDirectory Class
     * @param rhs
     * @return
     */
    static Node encode(const DialingDirectory &rhs)
    {
        Node node;
        node["file_version"]       = rhs.file_version;

        // Child Nodes for storying menu options.
        Node system;

        // Loop and encode each menu option
        for(auto &sys : rhs.system_entries)
        {
            system["index"]            = sys.index;
            system["name"]             = sys.name;
            system["ip"]               = sys.ip;
            system["port"]             = sys.port;
            system["protocol"]         = sys.protocol;
            system["login"]            = sys.login;
            system["password"]         = sys.password;
            system["font"]             = sys.font;
            system["key_map"]          = sys.key_map;
            system["term_type"]        = sys.term_type;
            system["term_size"]        = sys.term_size;
            system["use_pipe_colors"]  = sys.use_pipe_colors;

            node["system_entry"].push_back(sys);
        }

        return node;
    }

    /**
     * @brief Overide for the Load from File into DialingDirectory Object.
     * @param node
     * @param rhs
     * @return
     */
    static bool decode(const Node& node, DialingDirectory& rhs)
    {
        rhs.file_version = node["file_version"].as<std::string>();

        // Iterate through so we can get MAPS with the same key "system_entries".
        std::string key = "";

        for(YAML::const_iterator it = node.begin(); it != node.end(); ++it)
        {
            key.clear();
            key = it->first.as<std::string>();

            if(key == "system_entry")
            {
                rhs.system_entries.push_back(it->second.as<SystemEntry>());
            }
        }

        return true;
    }
};

}

typedef std::shared_ptr<DialingDirectory> dial_directory_ptr;

#endif // DIALING_DIRECTORY_HPP
