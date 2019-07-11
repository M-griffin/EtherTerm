#ifndef DIALING_DIRECTORY_HPP
#define DIALING_DIRECTORY_HPP

#include <memory>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include <yaml-cpp/yaml.h>


/**
 * @class SystemEntry
 * @author Michael Griffin
 * @date 07/08/2019
 * @file dial_directory.hpp
 * @brief System Entry in the Dialing Directory in YAML
 */
class SystemEntry
{

public:

    int         index;
    std::string name;
    std::string ip;
    int         port;
    std::string protocol;
    std::string login;
    std::string password;
    std::string font;
    std::string key_map;
    std::string term_type;
    std::string term_size;

    explicit SystemEntry()
        : index(-1)
        , name("A New System")
        , ip("localhost")
        , port(23)
        , protocol("TELNET")
        , login("")
        , password("")
        , font("vga8x16.bmp")
        , key_map("ANSI")
        , term_type("ANSI")
        , term_size("80x25")
    { }

    ~SystemEntry() { }
};

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
 * @date 07/08/2019
 * @file dialing_directory.hpp
 * @brief SystemEntry convert for SystemEntry Class
 */
template<>
struct convert<SystemEntry>
{
    /**
     * @brief Overide for encoding the SystemEntry Class
     * @param rhs
     * @return
     */
    static Node encode(const SystemEntry &rhs)
    {
        Node node;
        node["index"]            = rhs.index;
        node["name"]             = rhs.name;
        node["ip"]               = rhs.ip;
        node["port"]             = rhs.port;
        node["protocol"]         = rhs.protocol;
        node["login"]            = rhs.login;
        node["password"]         = rhs.password;
        node["font"]             = rhs.font;
        node["key_map"]          = rhs.key_map;
        node["term_type"]        = rhs.term_type;
        node["term_size"]        = rhs.term_size;

        return node;
    }

    /**
     * @brief Overide for the Load from File
     * @param node
     * @param rhs
     * @return
     */
    static bool decode(const Node& node, SystemEntry& rhs)
    {
        rhs.index           = node["index"].as<int>();
        rhs.name            = node["name"].as<std::string>();
        rhs.ip              = node["ip"].as<std::string>();
        rhs.port            = node["port"].as<int>();
        rhs.protocol        = node["protocol"].as<std::string>();
        rhs.login           = node["login"].as<std::string>();
        rhs.password        = node["password"].as<std::string>();
        rhs.font            = node["font"].as<std::string>();
        rhs.key_map         = node["key_map"].as<std::string>();
        rhs.term_type       = node["term_type"].as<std::string>();
        rhs.term_size       = node["term_size"].as<std::string>();

        return true;
    }
};

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

            node["system_entries"].push_back(sys);
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

            if(key == "system_entries")
            {
                rhs.system_entries.push_back(it->second.as<SystemEntry>());
            }
        }

        return true;
    }
};

}

typedef std::shared_ptr<SystemEntry> system_entry_ptr;
typedef std::shared_ptr<DialingDirectory> dial_directory_ptr;

#endif // DIALING_DIRECTORY_HPP
