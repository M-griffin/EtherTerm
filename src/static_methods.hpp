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

};

#endif // STATIC_METHODS_HPP
