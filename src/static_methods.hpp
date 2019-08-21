#ifndef STATIC_METHODS_HPP
#define STATIC_METHODS_HPP

#include <string>

/**
 * @class StaticMethods
 * @author Michael Griffin
 * @date 10/07/2019
 * @file static_methods.hpp
 * @brief Global Static Methods For Easy Access
 */
class StaticMethods
{
public:
    /**
    * @brief Helper, appends forward/backward slash to path
    * @param value
    */
    static void pathSeperator(std::string &value)
    {
#ifdef _WIN32
        value.append("\\");
#else
        value.append("/");
#endif
    }

    /**
     * @brief Helper to append folder to path
     */
    static std::string getAssetPath(std::string program_path)
    {
        // Create Default Phone Book.
        std::string path = program_path;
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
    static std::string getDirectoryPath(std::string program_path)
    {
        // Create Default Phone Book.
        std::string path = getAssetPath(program_path);
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
    static std::string getAnsiPath(std::string program_path)
    {
        // Create Default Phone Book.
        std::string path = getAssetPath(program_path);
#ifdef _WIN32
        path.append("ansi\\");
#else
        path.append("ansi/");
#endif
        return path;
    }


    /**
     * @brief Helper to append font path
     */
    static std::string getFontPath(std::string program_path)
    {
        // Create Default Phone Book.
        std::string path = getAssetPath(program_path);
#ifdef _WIN32
        path.append("font\\");
#else
        path.append("font/");
#endif
        return path;
    }

};
#endif // STATIC_METHODS_HPP
