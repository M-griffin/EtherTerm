#ifndef FONT_SET_DAO_HPP
#define FONT_SET_DAO_HPP

#include <memory>
#include <string>

class FontSet;
typedef std::shared_ptr<FontSet> font_set_ptr;

/**
 * @class FontSetDao
 * @author Micahel Griffin
 * @date 07/29/2019
 * @file font_set_dao.hpp
 * @brief Data Access Objects for The Font Set
 */
class FontSetDao
{
public:
    FontSetDao(std::string path);
    ~FontSetDao();

    /**
     * @brief Check if the file exists and we need to create a new one.
     * @return
     */
    bool fileExists();

    /**
     * @brief Creates and Saves a newly Generated File.
     * @param menu
     * @return
     */
    bool saveFontSet(font_set_ptr font_set);

    /**
     * @brief Loads a Item into the stub for access.
     * @return
     */
    font_set_ptr loadFontSet();

    /**
     * @brief Moves the Loaded Item to the shared pointer.
     * @param rhs
     * @return
     */
    void encode(const FontSet &rhs);

    /**
     * @brief Retrieve the existing Filename
     * @return
     */
    std::string getFileName() const
    {
        return m_filename;
    }


    font_set_ptr       m_font_set;
    std::string        m_program_path;
    std::string        m_filename;
};

typedef std::shared_ptr<FontSetDao> font_set_dao_ptr;


#endif // FONT_SET_DAO_HPP
