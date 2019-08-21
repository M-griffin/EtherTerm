#ifndef FONTMANAGER_HPP
#define FONTMANAGER_HPP

#include <string>
#include <memory>

class FontSet;
typedef std::shared_ptr<FontSet> font_set_ptr;

class FontSetDao;
typedef std::shared_ptr<FontSetDao> font_set_dao_ptr;

/**
 * @class FontManager
 * @author Michael Griffin
 * @date 08/16/2019
 * @file font_manager.hpp
 * @brief Manages The Font Sets used for Rendering
 */
class FontManager
{
public:
    FontManager(std::string path);
    ~FontManager();

    bool validateFile();
    font_set_ptr retrieveFontSet();

private:

    font_set_dao_ptr m_font_set_dao;
};

typedef std::shared_ptr<FontManager> font_manager_ptr;

#endif // FONTMANAGER_HPP
