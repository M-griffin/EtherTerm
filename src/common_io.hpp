#ifndef COMMONIO_HPP
#define COMMONIO_HPP

#include <iostream>
#include <string>
#include <map>
#include <stdint.h>

/**
 * @class CommonIO
 * @author Michael Griffin
 * @date 9/21/2015
 * @file common_io.hpp
 * @brief Low Level IO ASCII and UTF-8 Aware with locale support.
 */
class CommonIO
{

public:

    explicit CommonIO()
        : m_escape_sequence("")
        , m_string_buffer("")
        , m_incoming_data("")
        , m_line_buffer("")
        , m_column_position(0)
        , m_is_escape_sequence(false)
        , m_is_new_getline(true)
        , m_is_new_leadoff(true)
    {
        // Arror Keys, Hardware OA are translated to [A on the fly
        m_sequence_map.insert(std::make_pair("[A",    "up_arrow"));
        m_sequence_map.insert(std::make_pair("[B",    "dn_arrow"));
        m_sequence_map.insert(std::make_pair("[C",    "rt_arrow"));
        m_sequence_map.insert(std::make_pair("[D",    "lt_arrow"));

        // Hardware Keys, or Numpad.
        m_sequence_map.insert(std::make_pair("OA",    "up_arrow"));
        m_sequence_map.insert(std::make_pair("OB",    "dn_arrow"));
        m_sequence_map.insert(std::make_pair("OC",    "rt_arrow"));
        m_sequence_map.insert(std::make_pair("OD",    "lt_arrow"));
        m_sequence_map.insert(std::make_pair("OE",    "clear"));
        m_sequence_map.insert(std::make_pair("OF",    "end"));
        m_sequence_map.insert(std::make_pair("OH",    "home"));

        // Shift Arrow Keys
        m_sequence_map.insert(std::make_pair("[1;2A", "shift_up_arrow"));
        m_sequence_map.insert(std::make_pair("[1;2B", "shift_dn_arrow"));
        m_sequence_map.insert(std::make_pair("[1;2C", "shift_rt_arrow"));
        m_sequence_map.insert(std::make_pair("[1;2D", "shift_lt_arrow"));

        // Shift TAB
        m_sequence_map.insert(std::make_pair("[Z",    "shift_tab"));

        // Function Keys ANSI
        m_sequence_map.insert(std::make_pair("[@",    "insert"));
        m_sequence_map.insert(std::make_pair("[H",    "home"));
        m_sequence_map.insert(std::make_pair("[K",    "end"));
        m_sequence_map.insert(std::make_pair("[F",    "end")); // = 0F
        m_sequence_map.insert(std::make_pair("[V",    "pg_up"));
        m_sequence_map.insert(std::make_pair("[U",    "pg_dn"));
        m_sequence_map.insert(std::make_pair("[OP",   "f1"));
        m_sequence_map.insert(std::make_pair("[OQ",   "f2"));
        m_sequence_map.insert(std::make_pair("[OR",   "f3"));
        m_sequence_map.insert(std::make_pair("[OS",   "f4"));
        m_sequence_map.insert(std::make_pair("[OT",   "f5"));
        m_sequence_map.insert(std::make_pair("[[17~", "f6"));
        m_sequence_map.insert(std::make_pair("[[18~", "f7"));
        m_sequence_map.insert(std::make_pair("[[19~", "f8"));
        m_sequence_map.insert(std::make_pair("[[20~", "f9"));
        m_sequence_map.insert(std::make_pair("[[21~", "f10"));
        m_sequence_map.insert(std::make_pair("[[23~", "f11"));
        m_sequence_map.insert(std::make_pair("[[24~", "f12"));

        // VT-100 Putty
        m_sequence_map.insert(std::make_pair("[1~",   "home"));
        m_sequence_map.insert(std::make_pair("[2~",   "insert"));
        m_sequence_map.insert(std::make_pair("[3~",   "del"));
        m_sequence_map.insert(std::make_pair("[4~",   "end"));
        m_sequence_map.insert(std::make_pair("[5~",   "pg_up"));
        m_sequence_map.insert(std::make_pair("[6~",   "pg_dn"));
        m_sequence_map.insert(std::make_pair("[OU",   "f6"));
        m_sequence_map.insert(std::make_pair("[OV",   "f7"));
        m_sequence_map.insert(std::make_pair("[OW",   "f8"));
        m_sequence_map.insert(std::make_pair("[OX",   "f9"));
        m_sequence_map.insert(std::make_pair("[OY",   "f10"));
        m_sequence_map.insert(std::make_pair("[OZ",   "f11"));
        m_sequence_map.insert(std::make_pair("[O[",   "f12"));

        // Linux Console
        m_sequence_map.insert(std::make_pair("[[A",   "f1"));
        m_sequence_map.insert(std::make_pair("[[B",   "f2"));
        m_sequence_map.insert(std::make_pair("[[C",   "f3"));
        m_sequence_map.insert(std::make_pair("[[D",   "f4"));
        m_sequence_map.insert(std::make_pair("[[E",   "f5"));

        // SCO
        m_sequence_map.insert(std::make_pair("[L",    "insert"));
        m_sequence_map.insert(std::make_pair("[I",    "pg_up"));
        m_sequence_map.insert(std::make_pair("[G",    "pg_dn"));

        m_sequence_map.insert(std::make_pair("[[M",   "f1"));
        m_sequence_map.insert(std::make_pair("[[N",   "f2"));
        m_sequence_map.insert(std::make_pair("[[O",   "f3"));
        m_sequence_map.insert(std::make_pair("[[P",   "f4"));
        m_sequence_map.insert(std::make_pair("[[Q",   "f5"));
        m_sequence_map.insert(std::make_pair("[[R",   "f6"));
        m_sequence_map.insert(std::make_pair("[[S",   "f7"));
        m_sequence_map.insert(std::make_pair("[[T",   "f8"));
        m_sequence_map.insert(std::make_pair("[[U",   "f9"));
        m_sequence_map.insert(std::make_pair("[[V",   "f10"));
        m_sequence_map.insert(std::make_pair("[[W",   "f11"));
        m_sequence_map.insert(std::make_pair("[[X",   "f12"));

        // rxvt
        m_sequence_map.insert(std::make_pair("[7~",   "home"));
        m_sequence_map.insert(std::make_pair("[8~",   "end"));

        // Shift Arrow Keys
        m_sequence_map.insert(std::make_pair("[a",    "shift_up_arrow"));
        m_sequence_map.insert(std::make_pair("[b",    "shift_dn_arrow"));
        m_sequence_map.insert(std::make_pair("[c",    "shift_rt_arrow"));
        m_sequence_map.insert(std::make_pair("[d",    "shift_lt_arrow"));
        m_sequence_map.insert(std::make_pair("[e",    "shift_clear"));

        // Shift Function
        m_sequence_map.insert(std::make_pair("[2$",   "insert"));
        m_sequence_map.insert(std::make_pair("[3$",   "del"));
        m_sequence_map.insert(std::make_pair("[5$",   "pg_up"));
        m_sequence_map.insert(std::make_pair("[6$",   "pg_dn"));
        m_sequence_map.insert(std::make_pair("[7$",   "home"));
        m_sequence_map.insert(std::make_pair("[8$",   "end"));

        // Ctrl
        m_sequence_map.insert(std::make_pair("Oa",    "ctrl_up_arrow"));
        m_sequence_map.insert(std::make_pair("Ob",    "ctrl_dn_arrow"));
        m_sequence_map.insert(std::make_pair("Oc",    "ctrl_rt_arrow"));
        m_sequence_map.insert(std::make_pair("Od",    "ctrl_lt_arrow"));
        m_sequence_map.insert(std::make_pair("Oe",    "ctrl_clear"));

        // Shift Function
        m_sequence_map.insert(std::make_pair("[2^",   "ctrl_insert"));
        m_sequence_map.insert(std::make_pair("[3^",   "ctrl_del"));
        m_sequence_map.insert(std::make_pair("[5^",   "ctrl_pg_up"));
        m_sequence_map.insert(std::make_pair("[6^",   "ctrl_pg_dn"));
        m_sequence_map.insert(std::make_pair("[7^",   "ctrl_home"));
        m_sequence_map.insert(std::make_pair("[8^",   "ctrl_end"));
    }

    ~CommonIO()
    {
        std::cout << "~CommonIO" << std::endl;
        m_sequence_map.clear();
        m_escape_sequence.erase();
    }

    /* This function will read the OS specific functions
     * To Determine where the executable is located.
     * This has only been tested in Windows, Linux, OSX.
     */
    std::string getProgramPath(const std::string &program_name);

    /**
     * Get The BBS System Users HOME directory
     */
    std::string getSystemHomeDirectory();
    
    /**
     * @brief Appends Path Seperator depending on environment.
     * @param path
     * @return 
     */
    void pathAppend(std::string &path);

    /**
     * String Lengh counting actual characters not bytes
     * This is for mixed ASCII And UTF-8 Strings.
     */
    std::string::size_type numberOfChars(const std::string &str);

    /**
     * Left Trim Whitepsaces (Front)
     */
    std::string leftTrim(const std::string &str);

    /**
     * Right Trim Whitepsaces (Back)
     */
    std::string rightTrim(const std::string &str);

    /**
     * Trim Whitepsaces from both ends
     */
    std::string trim(const std::string &str);

    /**
     * UTF-8 Aware
     * Removes All Characters Between Start and End Positions.
     */
    std::string eraseString(const std::string &str,
                            std::string::size_type start_position,
                            std::string::size_type end_position = 0);
    /**
     * Right String Padding
     */
    std::string rightPadding(const std::string &str, std::string::size_type space);

    /**
     * Left String Padding
     */
    std::string leftPadding(const std::string &str, std::string::size_type space);

    /**
     * Center String Padding
     * Note: Need to add ANSI / PIPE parsing!
     */
    std::string centerPadding(const std::string &str, int term_width);

    /**
     * Mask String
     */
    std::string maskString(const std::string &str);

    /**
     * @brief Used for printing multibyte (Unicode Translations)
     * @param wide_string
     */
    std::string printWideCharacters(const std::wstring &wide_string);

    /**
     * @brief Translation from CP437 to UTF-8 MultiByte Charactes
     * @param standard_string
     */
    std::string translateUnicode(const std::string &standard_string);


    /**
    * @brief Return the Escape Sequence
    * @return
    */
    std::string getEscapeSequence();

    /**
     * @brief Parses Input and breaks out ESC Sequences.
     * @return
     */
    std::string parseInput(const std::string &character_buffer);

    /**
    * @brief Returns the InputFieldBuffer
    * @return
    */
    std::string getInputBuffer();

    /**
     * @brief Returns processing of InputField until NL is received.
     * @param line
     * @param length
     * @param leadoff
     * @param hidden
     * @return
     */
    std::string getLine(const std::string &line,    // Parsed Char input in
                        int   length,               // Max Input Length of Field
                        const std::string &leadoff, // Data to Display in Default Field {Optional}
                        bool  hidden);              // If input is hidden or masked     {Optional}


    /**
    * @brief Helper Method to display bool as string.
    * @param value
    * @return
    */
    std::string boolAlpha(bool value);

    /**
     * @brief Parse / Replace MCI String from given string.
     * @param AnsiString
     * @param mcicode
     * @param replacement
     */
    void parseLocalMCI(std::string &AnsiString, const std::string &mcicode, const std::string &replacement);

    /**
     * @brief Template search quick find
     * @param element
     * @param container
     * @return
     */
    template<class Element, class Container>
    bool in_array(const Element &element, const Container &container)
    {
        return std::find(std::begin(container), std::end(container), element)
               != std::end(container);
    }

private:

    /**
     * @brief Globals for Input Methods
     * Unique per session so don't want these static.
     */
    std::string m_escape_sequence; // ParseInput
    std::string m_string_buffer;   // ParseInput
    std::string m_incoming_data;   // ParseInput
    std::string m_line_buffer;     // GetLine
    int m_column_position;         // GetLine
    bool m_is_escape_sequence;     // ParseInput
    bool m_is_new_getline;         // GetLine
    bool m_is_new_leadoff;         // GetLine

    // Parameterized ESC Sequcnes Translations.
    std::map<std::string, std::string> m_sequence_map;
};
#endif
