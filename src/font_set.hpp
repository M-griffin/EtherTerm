#ifndef FONT_SET_HPP
#define FONT_SET_HPP

#include <yaml-cpp/yaml.h>
#include <string>

/**
 * @class FontEntry
 * @author Michael Griffin
 * @date 07/29/2019
 * @file font_set.hpp
 * @brief Font Entry in the Font Set YAML file
 */
class FontEntry
{

public:

    int index;
    std::string name;
    std::string type;
    int         width;
    int         height;
    std::string filename;
    std::string sequence;

    explicit FontEntry()
        : index(-1)
        , name("a New Font Entry")
        , type("bmp")
        , width(8)
        , height(16)
        , filename("new_font.bmp")
        , sequence("")
    { }

    ~FontEntry() { }
};

/**
 * @class FontSet
 * @author Michael Griffin
 * @date 07/29/2019
 * @file font_set.hpp
 * @brief FontSet, holds Font Enteries for for the System Rendering and Display.
 */
class FontSet
{

public:

    static const std::string FILE_VERSION;

    std::string file_version;
    std::vector<FontEntry> font_entries;

    explicit FontSet()
        : file_version(FILE_VERSION)

    { }

    ~FontSet() { }
};



namespace YAML
{
/**
 * @class convert
 * @author Michael Griffin
 * @date 07/29/2019
 * @file font_set.hpp
 * @brief FontEntry convert for FontEntry Class
 */
template<>
struct convert<FontEntry>
{
    /**
     * @brief Overide for encoding the FontEntry Class
     * @param rhs
     * @return
     */
    static Node encode(const FontEntry &rhs)
    {
        Node node;
        node["index"]            = rhs.index;
        node["name"]             = rhs.name;
        node["width"]            = rhs.width;
        node["height"]           = rhs.height;
        node["filename"]         = rhs.filename;
        node["sequence"]         = rhs.sequence;

        return node;
    }

    /**
     * @brief Overide for the Load from File
     * @param node
     * @param rhs
     * @return
     */
    static bool decode(const Node& node, FontEntry& rhs)
    {
        rhs.index           = node["index"].as<int>();
        rhs.name            = node["name"].as<std::string>();
        rhs.type            = node["type"].as<std::string>();
        rhs.width           = node["width"].as<int>();
        rhs.height          = node["height"].as<int>();
        rhs.filename        = node["filename"].as<std::string>();
        rhs.sequence        = node["sequence"].as<std::string>();

        return true;
    }
};

/**
 * @class convert
 * @author Michael Griffin
 * @date 16/11/2016
 * @file menu.hpp
 * @brief YAML Conversion for FontSet Class
 */
template<>
struct convert<FontSet>
{
    /**
     * @brief Overide for encoding the DialingDirectory Class
     * @param rhs
     * @return
     */
    static Node encode(const FontSet &rhs)
    {
        Node node;
        node["file_version"]         = rhs.file_version;

        // Child Nodes for storying menu options.
        Node system;

        // Loop and encode each menu option
        for(auto &sys : rhs.font_entries)
        {
            system["index"]          = sys.index;
            system["name"]           = sys.name;
            system["type"]           = sys.type;
            system["width"]          = sys.width;
            system["height"]         = sys.height;
            system["filename"]       = sys.filename;
            system["sequence"]       = sys.sequence;

            node["font_entry"].push_back(sys);
        }

        return node;
    }

    /**
     * @brief Overide for the Load from File into DialingDirectory Object.
     * @param node
     * @param rhs
     * @return
     */
    static bool decode(const Node& node, FontSet& rhs)
    {
        rhs.file_version = node["file_version"].as<std::string>();

        // Iterate through so we can get MAPS with the same key "system_entries".
        std::string key = "";

        for(YAML::const_iterator it = node.begin(); it != node.end(); ++it)
        {
            key.clear();
            key = it->first.as<std::string>();

            if(key == "font_entry")
            {
                rhs.font_entries.push_back(it->second.as<FontEntry>());
            }
        }

        return true;
    }
};

}

typedef std::shared_ptr<FontEntry> font_entry_ptr;
typedef std::shared_ptr<FontSet> font_set_ptr;

#endif // FONT_SET_HPP
