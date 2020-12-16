#include "font_manager.hpp"
#include "font_set_dao.hpp"
#include "font_set.hpp"


FontManager::FontManager(std::string path)
    : m_font_set_dao(new FontSetDao(path))
{
}

FontManager::~FontManager()
{
}

/**
 * @brief Verify File Exists, If not Generate default Entries
 * @return
 */
bool FontManager::validateFile()
{
    if(!m_font_set_dao->fileExists())
    {
        // Create a New file here with at least one system.
        font_set_ptr font_set(new FontSet());
        FontEntry entry;
        entry.index = 0;
        entry.name = "CP437 VGA";
        entry.type = "bmp";
        entry.width = 8;
        entry.height = 16;
        entry.filename = "vga8x16.bmp";
        entry.sequence = "[0;0 D";
        font_set->font_entries.push_back(entry);

        FontEntry entry2;
        entry2.index = 1;
        entry2.name = "CP437 VGA SCALED";
        entry2.type = "bmp";
        entry2.width = 8;
        entry2.height = 16;
        entry2.filename = "vga16x32.bmp";
        entry2.sequence = "";
        font_set->font_entries.push_back(entry2);

        FontEntry entry3;
        entry3.index = 2;
        entry3.name = "Amiga Topaz";
        entry3.type = "bmp";
        entry3.width = 8;
        entry3.height = 16;
        entry3.filename = "topaz8x16.bmp";
        entry3.sequence = "";
        font_set->font_entries.push_back(entry3);

        FontEntry entry4;
        entry4.index = 3;
        entry4.name = "Amiga Topaz 2";
        entry4.type = "bmp";
        entry4.width = 8;
        entry4.height = 16;
        entry4.filename = "topaz2-8x16";
        entry4.sequence = "";
        font_set->font_entries.push_back(entry4);

        FontEntry entry5;
        entry5.index = 4;
        entry5.name = "Amiga Topaz Plus";
        entry5.type = "bmp";
        entry5.width = 8;
        entry5.height = 16;
        entry5.filename = "topazPlus-8x16.bmp";
        entry5.sequence = "[0;40 D";
        font_set->font_entries.push_back(entry5);

        FontEntry entry6;
        entry6.index = 5;
        entry6.name = "Amiga MicroKnight";
        entry6.type = "bmp";
        entry6.width = 8;
        entry6.height = 16;
        entry6.filename = "microKnight-8x16.bmp";
        entry6.sequence = "[0;39 D";
        font_set->font_entries.push_back(entry6);

        FontEntry entry7;
        entry7.index = 6;
        entry7.name = "Amiga MicroKnight Plus";
        entry7.type = "bmp";
        entry7.width = 8;
        entry7.height = 16;
        entry7.filename = "microKnightPlus-8x16.bmp";
        entry7.sequence = "";
        font_set->font_entries.push_back(entry7);

        FontEntry entry8;
        entry8.index = 7;
        entry8.name = "Amiga Mo'soul";
        entry8.type = "bmp";
        entry8.width = 8;
        entry8.height = 16;
        entry8.filename = "mo'soul-8x16.bmp";
        entry8.sequence = "[0;38 D";
        font_set->font_entries.push_back(entry8);

        FontEntry entry9;
        entry9.index = 8;
        entry9.name = "Amiga Pot Noodle";
        entry9.type = "bmp";
        entry9.width = 8;
        entry9.height = 16;
        entry9.filename = "potNoodle-8x16.bmp";
        entry9.sequence = "[0;37 D";
        font_set->font_entries.push_back(entry9);


        if(!m_font_set_dao->saveFontSet(font_set))
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

/**
 * @brief Retrieve Font Sets from File
 * @return
 */
font_set_ptr FontManager::retrieveFontSet()
{
    return (m_font_set_dao->loadFontSet());
}
