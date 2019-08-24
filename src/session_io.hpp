#ifndef SESSION_IO_HPP
#define SESSION_IO_HPP

#include "common_io.hpp"

#include <memory>
#include <string>
#include <vector>
#include <map>

class Session;
typedef std::shared_ptr<Session> session_ptr;
typedef std::weak_ptr<Session> session_weak_ptr;

/**
 * @brief Saves All MCI and PIPE Matches with Offsets for replacment.
 */
typedef struct MapType
{
    std::string::size_type m_offset;
    std::string::size_type m_length;
    std::string::size_type m_match;
    std::string            m_code;
    bool                   m_or;
} MapType;


/**
 * @class SessionIO
 * @author Michael Griffin
 * @date 9/21/2015
 * @file session_io.hpp
 * @brief Higher Level I/O sepecific Data Parsing
 */
class SessionIO
{
public:

    // Types for Text Prompt formatting to file.
    typedef std::pair<std::string, std::string> M_StringPair;

    explicit SessionIO(session_ptr session);
    ~SessionIO();

    /**
     * @brief Single Key Input or Esc Sequence parser.
     *        Only Accepts single characters at a time then passes through.
     *        Bascially all input passes through till there is something to display
     *        Nothing loop on input in these functions to keep async.
     * @param character_buffer
     * @return
     */
    std::string getKeyInput(const std::string &character_buffer);

    /**
     * @brief Generates an input field with ANSI color background
     * @param field_name
     * @param len
     */
    void createInputField(std::string &field_name, int len);

    /**
    * @brief Input Field, returns output for keypresses, completed field returns in result
    * @param character_buffer
    * @param result
    * @param length
    * @param leadoff
    * @param hidden
    * @return
    */
    std::string getInputField(const std::string &character_buffer, // Input.
                              std::string &result,      // Returned at [ENTER]
                              int length = 30,          // Default 30 Bytes
                              std::string leadoff = "", // Default None
                              bool hidden = false);     // Default Not hidden

    /**
     * @brief Parses for ANSI Foreground Colors
     * @param foreground
     */
    std::string pipeReplaceForground(int foreground);

    /**
     * @brief Parses for ANSI Background Colors
     * @param background
     */
    std::string pipeReplaceBackground(int background);

    /**
     * @brief Parsed Pipe Colors Codes to ANSI ESC Sequence.
     * @param color_string
     * @return
     */
    std::string pipeColors(const std::string &color_string);


    /**
    * @brief Parsed Pipe Codes with 1 or 2 Digits
    * @param pipe_code
    * @return
    */
    std::string parsePipeWithCharsDigits(const std::string &code, int value);

    /**
    * @brief Seperate Pipe Codes with 1 or 2 Digits
    * @param pipe_code
    * @return
    */
    std::string seperatePipeWithCharsDigits(const std::string &pipe_code);

    /**
     * @brief Parse Pipe Codes with no Following Digits
     * @param pipe_code
     * @return
     */
    std::string parsePipeWithChars(const std::string &pipe_code);

    /**
     * @brief Parses Code Map and replaces screen codes with ANSI Sequences.
     * @param screen
     * @param code_map
     * @return
     */
    std::string parseCodeMap(const std::string &screen, std::vector<MapType> &code_map);

    /**
     * @brief Parses Code Map and replaces screen codes with Generic Items.
     * @param screen
     * @param code_map
     * @return
     */
    std::string parseCodeMapGenerics(const std::string &screen, const std::vector<MapType> &code_map);

    /**
     * @brief Parses string and returns code mapping and positions
     * @param sequence
     * @return
     */
    std::vector<MapType> parseToCodeMap(const std::string &sequence, const std::string &expression);

    /**
     * @brief Converts MCI Sequences to Ansi screen output.
     * @param sequence
     * @return
     */
    std::string pipe2ansi(const std::string &sequence);

    /**
     * @brief Converts MCI Color Sequences to Ansi screen output. {One off for Old DOS Systems that don't parse PIPE CODES
     * @param sequence
     * @return
     */
    std::string pipeColors2ansi(const std::string &sequence);

    /**
     * @brief Converts MCI Sequences to Code Maps for Multiple Parses of same string data
     * @param sequence
     * @return
     */
    std::vector<MapType> pipe2genericCodeMap(const std::string &sequence);

    /**
     * @brief Converts MCI Sequences to Code Maps for Prompt Strings
     * @param sequence
     * @return
     */
    std::vector<MapType> pipe2promptCodeMap(const std::string &sequence);

    /**
     * @brief Converts MCI Sequences to Code Maps for Prompt Formatting Strings
     * @param sequence
     * @return
     */
    std::vector<MapType> pipe2promptFormatCodeMap(const std::string &sequence);

    /**
     * @brief Checks a String if it matches the expression passed.
     * @param sequence
     * @param expression
     * @return
     */
    bool checkRegex(const std::string &sequence, const std::string &expression);

    /**
     * @brief Stores Key (MCI Code) Value (String for Replacement) in Mapping
     * @param key
     * @param value
     * @return
     */
    std::string parseTextPrompt(const M_StringPair &prompt);


    /**
     * @brief Parses Text Prompt String Pair
     * @param prompt
     * @return
     */
    void addMCIMapping(const std::string &key, const std::string &value);


    /**
     * @brief Clears all mappings
     */
    void clearAllMCIMapping();

    /**
     * @brief Get a Count of all Mapped MCI Codes
     * @return
     */
    int getMCIMappingCount();

    // Internal Methods
    session_weak_ptr                   m_weak_session;
    CommonIO                           m_common_io;

    std::map<std::string, std::string> m_mapped_codes; // MCI Code Translation for specific screens.

    const std::string STD_EXPRESSION = {"([|]{1}[0-9]{2})|([|]{1}[X][Y][0-9]{4})|"
                                        "([|]{1}[A-Z]{1,2}[0-9]{1,2})|([|]{1}[A-Z]{2})|"
                                        "([%]{2}[\\w]+[.]{1}[\\w]{3})|([%]{1}[A-Z]{2})|"
                                        "([%]{1}[0-9]{2})"
                                       };

    const std::string COLOR_EXPRESSION = {"([|]{1}[0-9]{2})"};

    const std::string MID_EXPRESSION = {"([|]{1}[A-Z]{1}[0-9]{1,2})|([|]{1}[A-Z]{2})"};

    const std::string PROMPT_EXPRESSION = {"([\\^]{1}[A-Z]{1})|([\\\\/=|@*:#)(]{1}$)"};

    const std::string FORMAT_EXPRESSION = {"([[]{1}[\\w\\W]+[]]{1})|([:]{1})"};
};

typedef std::shared_ptr<SessionIO> session_io_ptr;

#endif // MENU_IO_HPP
