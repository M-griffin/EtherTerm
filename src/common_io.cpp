#include "common_io.hpp"

#include <unistd.h>
#include <sys/types.h>

#ifdef TARGET_OS_MAC
#include <mach-o/dyld.h>
#elif _WIN32
#include <windows.h>
#endif

#include <unistd.h>
#include <sys/types.h>

#ifndef _WIN32
#include <pwd.h>
#endif

#include <cstdio>
#include <cstring>
#include <climits>
#include <cstdlib>

#include <stdexcept>

#include <iostream>
#include <string>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <sstream>
#include <fstream>
#include <map>

#include <clocale>  // locale
#include <cwchar>   // wchar_t wide characters

#include <UtfCpp/utf8.h>

/**
 * CP437 -> UTF-8 Character Translation Table
 */
wchar_t CP437_TABLE[] =
{
    L'\u0000', L'\u263A', L'\u263B', L'\u2665', L'\u2666', L'\u2663', // 5
    L'\u2660', L'\u2022', L'\u0008', L'\u0009', L'\u000A', L'\u2642', // 11
    L'\u2640', L'\u000D', L'\u266C', L'\u263C', L'\u25BA', L'\u25C4', // 17
    L'\u2195', L'\u203C', L'\u00B6', L'\u00A7', L'\u25AC', L'\u21A8', // 23
    L'\u2191', L'\u2193', L'\u2192', L'\u001B', L'\u221F', L'\u2194', // 29
    L'\u25B2', L'\u25BC', L'\u0020', L'\u0021', L'\u0022', L'\u0023',
    L'\u0024', L'\u0025', L'\u0026', L'\u0027', L'\u0028', L'\u0029',
    L'\u002A', L'\u002B', L'\u002C', L'\u002D', L'\u002E', L'\u002F',
    L'\u0030', L'\u0031', L'\u0032', L'\u0033', L'\u0034', L'\u0035',
    L'\u0036', L'\u0037', L'\u0038', L'\u0039', L'\u003A', L'\u003B',
    L'\u003C', L'\u003D', L'\u003E', L'\u003F', L'\u0040', L'\u0041',
    L'\u0042', L'\u0043', L'\u0044', L'\u0045', L'\u0046', L'\u0047',
    L'\u0048', L'\u0049', L'\u004A', L'\u004B', L'\u004C', L'\u004D',
    L'\u004E', L'\u004F', L'\u0050', L'\u0051', L'\u0052', L'\u0053',
    L'\u0054', L'\u0055', L'\u0056', L'\u0057', L'\u0058', L'\u0059',
    L'\u005A', L'\u005B', L'\u005C', L'\u005D', L'\u005E', L'\u005F',
    L'\u0060', L'\u0061', L'\u0062', L'\u0063', L'\u0064', L'\u0065',
    L'\u0066', L'\u0067', L'\u0068', L'\u0069', L'\u006A', L'\u006B',
    L'\u006C', L'\u006D', L'\u006E', L'\u006F', L'\u0070', L'\u0071',
    L'\u0072', L'\u0073', L'\u0074', L'\u0075', L'\u0076', L'\u0077',
    L'\u0078', L'\u0079', L'\u007A', L'\u007B', L'\u007C', L'\u007D',
    L'\u007E', L'\u007F', L'\u00C7', L'\u00FC', L'\u00E9', L'\u00E2',
    L'\u00E4', L'\u00E0', L'\u00E5', L'\u00E7', L'\u00EA', L'\u00EB',
    L'\u00E8', L'\u00EF', L'\u00EE', L'\u00EC', L'\u00C4', L'\u00C5',
    L'\u00C9', L'\u00E6', L'\u00C6', L'\u00F4', L'\u00F6', L'\u00F2',
    L'\u00FB', L'\u00F9', L'\u00FF', L'\u00D6', L'\u00DC', L'\u00A2',
    L'\u00A3', L'\u00A5', L'\u20A7', L'\u0192', L'\u00E1', L'\u00ED',
    L'\u00F3', L'\u00FA', L'\u00F1', L'\u00D1', L'\u00AA', L'\u00BA',
    L'\u00BF', L'\u2310', L'\u00AC', L'\u00BD', L'\u00BC', L'\u00A1',
    L'\u00AB', L'\u00BB', L'\u2591', L'\u2592', L'\u2593', L'\u2502',
    L'\u2524', L'\u2561', L'\u2562', L'\u2556', L'\u2555', L'\u2563',
    L'\u2551', L'\u2557', L'\u255D', L'\u255C', L'\u255B', L'\u2510',
    L'\u2514', L'\u2534', L'\u252C', L'\u251C', L'\u2500', L'\u253C',
    L'\u255E', L'\u255F', L'\u255A', L'\u2554', L'\u2569', L'\u2566',
    L'\u2560', L'\u2550', L'\u256C', L'\u2567', L'\u2568', L'\u2564',
    L'\u2565', L'\u2559', L'\u2558', L'\u2552', L'\u2553', L'\u256B',
    L'\u256A', L'\u2518', L'\u250C', L'\u2588', L'\u2584', L'\u258C',
    L'\u2590', L'\u2580', L'\u03B1', L'\u00DF', L'\u0393', L'\u03C0',
    L'\u03A3', L'\u03C3', L'\u00B5', L'\u03C4', L'\u03A6', L'\u0398',
    L'\u03A9', L'\u03B4', L'\u221E', L'\u03C6', L'\u03B5', L'\u2229',
    L'\u2261', L'\u00B1', L'\u2265', L'\u2264', L'\u2320', L'\u2321',
    L'\u00F7', L'\u2248', L'\u00B0', L'\u2219', L'\u00B7', L'\u221A',
    L'\u207F', L'\u00B2', L'\u25A0', L'\u00A0'
};

/**
 * @brief Determine where the executable is located.
 * This has only been tested in Windows, Linux, OSX.
 * @return
 */
std::string CommonIO::getProgramPath(const std::string &program_name)
{
    std::string program_path;
    std::string program = "/" + program_name;

    // First check for SYSTEM environmental variable:
    char *pPath;
    pPath = std::getenv((char *)"OBV2");
    if(pPath != nullptr)
    {
        std::cout << "Found OBV2 Enviroment set: " << pPath << std::endl;

        program_path = pPath;

#ifdef _WIN32
        if (program_path[program_path.size()-1] != '\\')
        {
            program_path.append("\\");
        }
#else
        if (program_path[program_path.size()-1] != '/')
        {
            program_path.append("/");
        }
#endif

        return program_path;
    }
    else
    {
        std::cout << "looking up program path." << std::endl;
    }

    // Get the Folder the Executable runs in.
#ifdef TARGET_OS_MAC
    char current_path[PATH_MAX];
    uint32_t size = sizeof(current_path);
    if(_NSGetExecutablePath(current_path, &size) != 0)
    {
        throw std::runtime_error("getProgramPath: OSX Path");
    }

    // Remove Executable
    program_path = current_path;
    std::string::size_type position;
    position = program_path.rfind(program);
    if(position != std::string::npos)
    {
        program_path.erase(position+1,program_path.size()-1);
    }

    // remove extra './'
    position = program_path.rfind("./");
    if(position != std::string::npos)
    {
        program_path.erase(position,2);
    }

#elif _WIN32
    // Get the Current Program path.
    char current_path[PATH_MAX];

    int result = GetModuleFileName(NULL, current_path, PATH_MAX-1);
    if(result == 0)
    {
        throw std::runtime_error("GetProgramPath: Win32 Path");
    }

    program_path = current_path;
    std::string::size_type position = program_path.rfind("\\", program_path.size()-1);
    if(position != std::string::npos)
    {
        program_path.erase(position+1);
    }
#else

    char exe_path[PATH_MAX] = {0};
    ssize_t result = readlink("/proc/self/exe", exe_path, PATH_MAX);
    if(result < 0)
    {
        throw std::runtime_error("getProgramPath: Linux Path");
    }

    const char* t = " \t\n\r\f\v";
    program_path = exe_path;
    std::cout << "ORIG PATH: " << program_path << std::endl;
    program_path = program_path.erase(program_path.find_last_not_of(t) + 1);
    program_path += "/";

    // Remove Executable
    std::string::size_type position;
    position = program_path.rfind(program);
    if(position != std::string::npos)
    {
        program_path.erase(position+1,program_path.size()-1);
    }

#endif
    return program_path;
}

#ifndef _WIN32
/**
 * @brief Get The BBS System Users HOME directory
 * @return
 */
std::string CommonIO::getSystemHomeDirectory()
{
    std::string home_directory;
    const char *homedir;

    homedir = getenv("HOME");
    if(!homedir)
    {
        homedir = getpwuid(getuid())->pw_dir;
        if(!homedir)
        {
            std::cout << "Error: Unable to locate bbs user's home directory: "
                      << std::endl;
            home_directory = "";
            return home_directory;
        }
    }

    home_directory = homedir;

    return home_directory;
}
#endif


/**
 * @brief Appends Path Seperator depending on environment.
 * @param path
 * @return
 */
void CommonIO::pathAppend(std::string &path)
{
#ifdef _WIN32
    path.append("\\");
#else
    path.append("/");
#endif
}


/**
 * @brief String Lengh counting actual characters not bytes
 * @param str
 * @return
 */
std::string::size_type CommonIO::numberOfChars(const std::string &str)
{
    std::string line = str;
    std::string::iterator end_it = utf8::find_invalid(line.begin(), line.end());
    if (end_it != line.end()) 
    {
        std::cout << "This part is fine: " << std::string(line.begin(), end_it) << std::endl;
    }
    
    // Get the line length (at least for the valid part)
    int length = utf8::distance(line.begin(), end_it);
    return length;
}

/**
 * @brief Left Trim Whitepsaces (Front)
 * @param str
 * @return
 */
std::string CommonIO::leftTrim(const std::string &str)
{
    std::string new_string = str;

    if(new_string.empty())
    {
        //std::cout << "Exception (Common::LeftTrim) string length == 0" << std::endl;
        return new_string;
    }

    new_string.erase(
        new_string.begin(),
        std::find_if(new_string.begin(),
                     new_string.end(),
                     std::not1(std::ptr_fun<int, int>(std::isspace)))
    );

    return new_string;
}

/**
 * @brief Right Trim Whitepsaces (Back)
 * @param str
 * @return
 */
std::string CommonIO::rightTrim(const std::string &str)
{
    std::string new_string = str;

    if(new_string.empty())
    {
        //std::cout << "Exception (Common::RightTrim) string length == 0" << std::endl;
        return new_string;
    }

    new_string.erase(std::find_if(
                         new_string.rbegin(),
                         new_string.rend(),
                         std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
                         new_string.end()
                    );

    return new_string;
}

/**
 * @brief Trim Whitepsaces from both ends
 * @param str
 * @return
 */
std::string CommonIO::trim(const std::string &str)
{
    std::string new_string = str;
    if(new_string.empty())
    {
        //std::cout << "Exception (Common::trim) string length == 0" << std::endl;
        return new_string;
    }
    return leftTrim(rightTrim(new_string));
}

/**
 * @brief Removes All Data after specified number of characters
 * @param str
 * @param start_position
 * @param end_position
 */
std::string CommonIO::eraseString(const std::string &str,
                                  std::string::size_type start_position,
                                  std::string::size_type end_position)
{
    // std::cout << "StringERASE: " << str << " : " << str.size() << std::endl;
    std::string new_string = str;
    //std::string::size_type count = 0;
    std::string::size_type string_size = new_string.size();

    // 0 defaults to end of string.
    // Otherwise End Position is added for number of characters to remove.
    if(end_position == 0)
    {
        end_position = string_size;
    }
    else
    {
        end_position = start_position + (end_position-1);
        // Make sure we can never go past end of string!
        if(end_position > string_size)
        {
            end_position = string_size;
        }
    }
   
    std::string new_string_builder = "";
    if(new_string.empty())
    {
        std::cout << "Exception (Common::EraseString) string length == 0" << std::endl;
        return new_string;
    }
    
    unsigned char_count = 0;
    std::string::iterator it = new_string.begin();
    std::string::iterator line_end = new_string.end();    
    
    while (it != line_end) {                
        //std::cout << "ut: " << *it << std::endl;
        uint32_t code_point = utf8::next(it, line_end);
        
        if(char_count < start_position || char_count > end_position)
        {
            //std::cout << "append" << std::endl;
            // This convert the uint32_t code point to char array
            // So each sequence can be writen as seperate byte.
            unsigned char character[5] = {0};
            utf8::append(code_point, character);
            new_string_builder += (char *)character;
        }
        
        //std::cout << "char_count: " << char_count << " " << code_point << std::endl;        
        ++char_count;
    }
    
    /*
    std::cout << "\r\nStringERASE start_position: " << start_position << std::endl;
    std::cout << "StringERASE end_position: " << end_position << std::endl;
    std::cout << "StringERASE OLD: " << str << " : " << str.size() << std::endl;
    std::cout << "StringERASE NEW: " << new_string << " : " << new_string.size() << std::endl;
    */
    return new_string_builder;
}

/**
 * @brief Right String Padding
 * @param str
 * @param space
 * @return
 */
std::string CommonIO::rightPadding(const std::string &str, std::string::size_type space)   // Pad Right
{
    std::string new_string = str;
    if(space == 0) return new_string;
    if(new_string.empty())
    {
        //std::cout << "Exception (Common::RightPadding) string length == 0" << std::endl;
        return new_string;
    }

    std::string padded_line = "";
    std::string::size_type s = numberOfChars(new_string);

    // if Line > Sapce, Erase to match length
    if(s > space)
    {
        std::string erased = eraseString(new_string, space);
        return erased;
    }

    for(std::string::size_type i = 0; i < (space-s); i++)
    {
        padded_line += ' ';
    }

    new_string.append(padded_line);
    return new_string;
}

/**
 * @brief Left String Padding
 * @param str
 * @param space
 * @return
 */
std::string CommonIO::leftPadding(const std::string &str, std::string::size_type space)
{
    std::string new_string = str;
    if(space == 0)
    {
        return new_string;
    }

    if(new_string.empty())
    {
        //std::cout << "Exception (Common::leftPadding) string length == 0" << std::endl;
        return new_string;
    }

    std::string::size_type s = numberOfChars(new_string);

    // if Line > Sapce, Erase to match length
    if(s >= space)
    {
        std::string erased = eraseString(new_string, 0, s-space);
        return erased;
    }

    for(std::string::size_type i = 0; i < (space-s); i++)
    {
        new_string.insert(0, " ");
    }

    return new_string;
}

/**
 * @brief Center String Padding (Note: Need to add Code to Strip Pipes and ANSI)
 * @param str
 * @param term_width
 * @return
 */
std::string CommonIO::centerPadding(const std::string &str, int term_width)
{
    std::string new_string = str;
    if(new_string.empty())
    {
        //std::cout << "Exception (Common::centerPadding) string empty" << std::endl;
        return new_string;
    }

    std::string::size_type length = numberOfChars(new_string);
    if(length == 0)
    {
        //std::cout << "Exception (Common::centerPadding) string length == 0" << std::endl;
        return new_string;
    }
    int space = term_width / 2;
    space -= term_width % 2;

    space -= length / 2;
    space -= length % 2;

    // Text larger then screen, then leave alone.
    if(space <= 0) return new_string;

    std::string padded_line;
    for(int i = 0; i < space; i++)
    {
        padded_line += ' ';
    }

    // Appending currnet data after Padding.
    new_string.insert(0,padded_line);
    return new_string;
}

/**
 * @brief Mask String
 * @param str
 * @return
 */
std::string CommonIO::maskString(const std::string &str)
{
    std::string new_string = str;
    if(new_string.empty())
    {
        //std::cout << "Exception (Common::maskString) string empty" << std::endl;
        return new_string;
    }

    std::string::size_type string_size = numberOfChars(new_string);
    if(string_size == 0)
    {
        //std::cout << "Exception (Common::maskString) string length == 0" << std::endl;
        return new_string;
    }

    new_string.erase();
    for(std::string::size_type i = 0; i < string_size; i++)
    {
        new_string.append("*");
    }

    return new_string;
}

/**
 * @brief Used for printing output multibyte (Unicode Translations)
 * @param wide_string
 */
std::string CommonIO::printWideCharacters(const std::wstring &wide_string)
{
    //using namespace boost::locale;
    //using namespace std;

    //generator gen;

    // Make system default locale global
    //std::locale loc = gen("");
    //locale::global(loc);
    //cout.imbue(loc);

    std::string output = "";

    std::mbstate_t state = std::mbstate_t();
    for(wchar_t wc : wide_string)
    {
        std::string mb(MB_CUR_MAX, '\0');
        int ret = std::wcrtomb(&mb[0], wc, &state);
        if(ret == 0)
        {
            break;
        }

        // Skip any Trailing / Embedded null from Wide -> multibtye
        // Conversion, don't send NULL's to the screen.
        for(char ch: mb)
        {
            if(ch != '\0')
            {
                //std::cout << ch << flush;
                output += ch;
            }
        }
    }

    return output;
}

/**
 * @brief Translation from CP437 to UTF-8 MultiByte Charactes
 * @param standard_string
 */
std::string CommonIO::translateUnicode(const std::string &standard_string)
{
    std::string output = "";
    std::wstring wide_string = L"";
    int ascii_value = 0;

    // Loop and write out after translation to Unicode
    for(std::string::size_type i = 0; i < standard_string.size(); i++)
    {
        ascii_value = std::char_traits<char>().to_int_type(standard_string[i]);
        if(ascii_value < 256)
        {
            wide_string += CP437_TABLE[ascii_value];
        }
        else
        {
            wide_string += standard_string[i];
        }
    }

    output += printWideCharacters(wide_string);
    return output;
}

/**
 * @brief Return the Escape Sequence Parsed.
 * @return
 */
std::string CommonIO::getEscapeSequence()
{
    //std::cout << "REAL Esc sequence: " << m_escape_sequence << std::endl;

    // Check if Sequences Exists, otherwise return blank.
    if(m_sequence_map.find(m_escape_sequence) != m_sequence_map.end())
    {
        return m_sequence_map[m_escape_sequence];
    }
    return "";
}

/**
 * @brief Parses Input and breaks out ESC Sequences.
 *        Sequences are handled 1 character at a time.
 * @return
 */
std::string CommonIO::parseInput(const std::string &character_buffer)
{
    //std::cout << "character_buffer: " << character_buffer << std::endl;
    int num = numberOfChars(character_buffer);
    if((num == 0  ||  character_buffer[0] == '\x1b') &&
            m_is_escape_sequence &&
            m_string_buffer.size() == 1)
    {
        // This a null after an ESC..
        m_is_escape_sequence = false;
        m_escape_sequence.erase();
        m_string_buffer.erase();
        return "\x1b";
    }
    else if(num != 1)
    {
        std::cout << "This function only expects single characters, text or unicode multi-byte."
                  << std::endl;
        return "";
    }

    // Don't process concurent multiple esc sequences.
    // if we get a second, return the first and check if we
    // are getting a single second ESC or start of Sequence.
    if(character_buffer[0] == 27 && character_buffer.size() == 1)
    {
        if(!m_is_escape_sequence)
        {
            m_is_escape_sequence = true;
            m_escape_sequence.erase();
            m_string_buffer.erase();
            m_string_buffer = character_buffer;
            return "";
        }
        else
        {
            // If we get here again and second char is another ESC,
            // to single the first key press!
            m_escape_sequence.erase();
            m_string_buffer.erase();
            return "\x1b";
        }
    }

    // Reset, handle if we got a second Escape or two ESC keys in a row.
    m_is_escape_sequence = false;

    // Check if were appending to current buffer or returning
    // Only appending if were in an Active ESC Sequence!
    if(!m_string_buffer.empty())
    {
        // Check for Overflow, Input ESC Sequences Should
        // Be Short and Sweet, over 8 then lets kill it here!
        if(m_string_buffer.size() >= 8)
        {
            m_is_escape_sequence = false;
            m_escape_sequence.erase();
            m_string_buffer.erase();
            return "";
        }

        // Parse ESC Sequence for Match on bracket [ or O.
        // First Bracket overwrite the ESC in the buffer for easier matching.
        if((character_buffer[0] == '[' && m_string_buffer[0] == '\x1b') ||
                (character_buffer[0] == 0x4f && m_string_buffer[0] == '\x1b'))
        {
            // Overwrite ESC and continue Sequence
            // No Need to Test for Esc in the Sequence Buffer, We'll
            // Test for everything following ESC.
            m_string_buffer.erase();
            m_string_buffer = character_buffer;
            return "";
        }
        else
        {
            switch(character_buffer[0])
            {
                case '\n': // Handle Bad Sequences with ENTER following
                    m_escape_sequence.erase();
                    return "\n";

                case '[': // [[ Double Brackets F1 Keys.
                    if(m_string_buffer == "[")
                    {
                        m_string_buffer += character_buffer;
                        return "";
                    }
                    else if(m_string_buffer == "[O")
                    {
                        // case '[': // F12 = [O[
                        m_string_buffer += character_buffer;
                        m_escape_sequence = m_string_buffer;
                        m_string_buffer.erase();
                        return "\x1b";
                    }
                    // Bad Sequence, just return ESC as is.
                    m_escape_sequence = m_string_buffer;
                    m_string_buffer.erase();
                    return "\x1b";

                    // Numbers all end in Tildes ~
                case '1': // Home
                case '2': // Insert
                case '3': // DEL
                case '4': // ESC
                case '5': // PG UP
                case '6': // PG DN
                case '7': // Function Keys.
                case '8': // Function Keys.
                case '9': // Function Keys.
                case '0': // Function Keys.
                case ';': // Seperator for Shift ARROW Keys! ie [1;2A
                    m_string_buffer += character_buffer;
                    return "";

                    // Only SCO F3 ends with [[O, otherwise it
                    // preceeds in other sequences.
                case 'O': // Precursor to Fucntion Keys [OA etc..
                    if(m_string_buffer == "[[")
                    {
                        // End of SCO Sequence
                        m_string_buffer += character_buffer;
                        m_escape_sequence = m_string_buffer;
                        m_string_buffer.erase();
                        return "\x1b";
                    }
                    // Continue;
                    m_string_buffer += character_buffer;
                    return "";

                case 'a': // shift up arrow
                case 'b': // shift dn arrow
                case 'c': // shift rt arrow
                case 'd': // shift lt arrow
                case 'e': // shift clear

                case 'A': // Up Arrow
                case 'B': // Dn Arrow
                case 'C': // Lt Arrow
                case 'D': // Rt Arrow
                case 'E': // F5
                case 'F': // End = 0F
                case 'G': // pg_down
                case 'K': // End
                case 'H': // Home
                case 'I': // page_up
                case 'M': // F1
                case 'N': // F2

                case 'P': // F1 / F4
                case 'Q': // F2 / F5
                case 'R': // F3 / F6
                case 'S': // F4 / F7
                case 'T': // F5 / F8
                case 'U': // PageDn / F9
                case 'V': // PageUP / F10
                case 'W': // F8 / F11
                case 'X': // F9 / F12
                case 'Y': // 10
                case 'Z': // 11
                    m_string_buffer += character_buffer;
                    m_escape_sequence = m_string_buffer;
                    m_string_buffer.erase();
                    return "\x1b";

                    // End of Number Sequence.
                case '~': // Function
                case '$': // Shift Function RXVT
                case '^': // CTRL Function RXVT
                    m_string_buffer += character_buffer;
                    m_escape_sequence = m_string_buffer;
                    m_string_buffer.erase();
                    return "\x1b";

                case '\0':
                    //current_buffer += ch;
                    m_escape_sequence = m_string_buffer;
                    m_string_buffer.erase();
                    return "\x1b";

                default :
                    // Not ESC Sequence.
                    // Done with loop.
                    //m_string_buffer += ' ';
                    m_escape_sequence = m_string_buffer;
                    m_string_buffer.erase();
                    return "\x1b";
            }
        }
    }

    // If ESC key received wtih no trailing sequence,
    // clear sequence buffer and just return ESC.
    if(m_string_buffer.size() == 1 && m_string_buffer[0] == 27)
    {
        m_string_buffer.erase();
        return "\x1b";
    }

    // Translate ENTER Key, *NIX Terms send CR always test for LF.
    // CRLF is a newline on output only, input is either or.
    if(character_buffer[0] == '\r')
    {
        return "\n";
    }
    //std::cout << "character_buffer: " << character_buffer << std::endl;
    return character_buffer;
}

/**
* @brief Returns the InputFieldBuffer
* @return
*/
std::string CommonIO::getInputBuffer()
{
    return m_line_buffer;
}


/**
 * @brief Returns processing of field input until NL is received.
 * @param line
 * @param length
 * @param leadoff
 * @param hidden
 * @return
 */
std::string CommonIO::getLine(const std::string &line,    // Parsed Char input in
                              int   length,               // Max Input Length of Field
                              const std::string &leadoff, // Data to Display in Default Field {Optional}
                              bool  hidden)               // If input is hidden or masked     {Optional}
{
    std::string output_buffer = "";    // Used for Data to send back
    std::string character_buffer = ""; // Used for Data being processed.

    // Flag on when to reset buffer and position for next run.
    if(m_is_new_getline)
    {
        m_is_new_getline = false;
        m_line_buffer.erase();
        m_column_position = 0;
    }
    // If were starting Off Input with a String already in buffer!  display it!
    if(m_is_new_leadoff && leadoff.size() > 0)
    {
        m_is_new_leadoff = false;
        m_column_position = leadoff.size();
        m_line_buffer += leadoff;

        // Trim if were past the length limit
        if((signed)numberOfChars(m_line_buffer) > length)
        {
            std::string temp = leftPadding(m_line_buffer, length);
            m_line_buffer = std::move(temp);
        }
    }

    // Gets Parsed input by Char, Multibyte or ESC Sequence.
    // Catch Aborts here!
    character_buffer = parseInput(line);
    if(character_buffer.size() == 0 || character_buffer[0] == '\0')
    {
        // No data or in mid ESC sequence
        // Need to wait for next character.
        return "\x1b"; // ""
    }
    // If we got an ENTER CR/LF then were done!
    // Set the Flag, so on next call to method, we reset, not before
    // Otherwise we'll clear the buffer we just filled.  :)
    if(character_buffer[0] == '\n')
    {
        m_is_new_getline = true;
        m_is_new_leadoff = true;
        return "\n";
    }
    // Escape in this case, ignore, later add movement in string
    std::string sequence = "";
    if(character_buffer[0] == 27)
    {
        sequence = getEscapeSequence();
        if(sequence.size() == 0)
        {
            // Received ESC, Abort!
            //std::cout << "Received ESC!!!!!" << std::endl;
            m_is_new_getline = true;
            m_is_new_leadoff = true;
            return "\x1b";
        }
        else
        {
            // Received DEL Escape Sequence.
            if(sequence == "del")
            {
                if(m_line_buffer.size() > 0)
                {
                    std::cout << "Received DEL ESC Sequence !!!!!" << std::endl;
                    std::string temp = eraseString(m_line_buffer, numberOfChars(m_line_buffer)-1, 1);
                    m_line_buffer = std::move(temp);
                    m_column_position = m_line_buffer.size();
                    return "\x1b[D \x1b[D";
                }
                else
                {
                    // Nothing to delete at begining Skip.
                    std::cout << "del at begining: " << character_buffer << std::endl;
                    return "empty";
                }
            }
            else
            {
                // Unhandled sequence! Skip and return
                std::cout << "Unhandled sequence: " << character_buffer << std::endl;
                return "empty"; // ""
            }
        }
    }
    // CTRL Y - Clear Line
    else if((int)character_buffer[0] == 25)
    {
        if(m_line_buffer.size() > 0)
        {
            std::cout << "ctrl y: " << character_buffer << std::endl;
            for(int i = numberOfChars(m_line_buffer); i > 0; i--)
            {
                output_buffer += "\x1b[D \x1b[D";
            }
            m_line_buffer.erase();
            m_column_position = 0;
            return output_buffer;
        }
        else
        {
            // At beginign of line, nothing to delete!
            std::cout << "ctrl y beginning of line: " << character_buffer << std::endl;
            return "empty";
        }
    }
    // Handle BS and DEL as both Destructive Backspace on Fields
    // At this time, arrow keys are not setup to move through the string!
    else if((int)character_buffer[0] == 0x08 || (int)character_buffer[0] == 0x7f || character_buffer[0] == '\b')
    {
        if(m_line_buffer.size() > 0)
        {
            std::cout << "backspace: " << character_buffer << std::endl;
            std::string temp = eraseString(m_line_buffer, numberOfChars(m_line_buffer)-1, 1);
            m_line_buffer = std::move(temp);
            m_column_position =  m_line_buffer.size();
            return "\x1b[D \x1b[D";
        }
        else
        {
            // At begining of Line, nothing to delete.
            std::cout << "At begining of Line, nothing to bs: " << character_buffer << std::endl;
            return "empty";
        }
    }
    // Normal Input processing, ASCII and Unicode. Add new functions for size!
    if(((signed)m_line_buffer.size() <= length) &&
            ((signed)(character_buffer.size() + m_line_buffer.size()) <= length))
    {
        if(hidden)
        {
            std::cout << "hidden field input: " << character_buffer << std::endl;
            m_line_buffer += character_buffer;
            m_column_position = numberOfChars(m_line_buffer);
            return "*";
        }
        else
        {
            std::cout << "normal field input: " << character_buffer << std::endl;
            m_line_buffer += character_buffer;
            m_column_position = numberOfChars(m_line_buffer);
            return character_buffer;
        }
    }
    std::cout << "Past the max length, nothing to add!" << std::endl;
    return "empty"; // ""
}


/**
 * @brief Helper Method to display bool as string.
 * @param value
 * @return
 */
std::string CommonIO::boolAlpha(bool value)
{
    if(value)
    {
        return "True";
    }
    return "False";
}


/**
 * @brief Parse / Replace MCI String from given string.
 * @param AnsiString
 * @param mcicode
 * @param replacement
 */
void CommonIO::parseLocalMCI(std::string &AnsiString, const std::string &mcicode, const std::string &replacement)
{
    std::string::size_type id1 = 0;
    do
    {
        // Parse New Message's MCI Code
        id1 = AnsiString.find(mcicode, 0);
        if(id1 != std::string::npos)
        {
            AnsiString.replace(id1, mcicode.size(), replacement);
            id1 = AnsiString.find(mcicode, 0);
        }
    }
    while(id1 != std::string::npos);
}

