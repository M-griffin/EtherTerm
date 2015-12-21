#ifndef MENUIO_HPP
#define MENUIO_HPP

#include "sequence_decoder.hpp"
#include <string>

/**
 * @class MenuIO
 * @author Michael Griffin
 * @date 12/20/2015
 * @file menu_io.hpp
 * @brief Menu IO handling
 * NOTES: Need to Rewrite GetLine() for passthrough
 */
class MenuIO
{
public:
    MenuIO(sequence_decoder_ptr decoder, std::string program_path);
    ~MenuIO();

    sequence_decoder_ptr m_sequence_decoder;
    std::string          m_program_path;

    /**
     * @brief Right string padding
     * @param str
     * @param space
     */
    void rightSpacing(std::string &str, int space);

    /**
     * @brief Left String Padding.
     * @param str
     * @param space
     */
    void leftSpacing(std::string &str, int space);

    /**
     * @brief Setup Text Input Fields
     * @param text
     * @param len
     */
    void inputField(std::string &text, int &len);

    /**
     * @brief Gets a Line of Input (DISABLED, NEEDS REWRITE!)
     * @param line
     * @param length
     * @param leadoff
     * @param hid
     */
    void getLine(char *line,             // Returns Input into Line
                 int   length,           // Max Input Length of String
                 char *leadoff = 0,      // Data to Display in Default String {Optional}
                 int   hid     = false); // If input is Echoed as hidden      {Optional}

    /**
     * @brief Foreground ESC Sequence Translation
     * @param data
     * @param fg
     */
    void foregroundColorSequence(char *data, int fg);

    /**
     * @brief Background ESC Sequence Translation
     * @param data
     * @param bg
     */
    void backgroundColorSequence(char *data, int bg);

    /**
     * @brief Parses MCI and PIPE Codes to ANSI Sequences
     * @param sequence
     */
    void sequenceToAnsi(const std::string &sequence);

    /**
     * @brief Reads in ANSI text file and pushes to Display
     * @param file_name
     */
    void displayAnsiFile(const std::string &file_name);

};

#endif // MENUIO_HPP
