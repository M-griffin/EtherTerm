#ifndef SCREENBUFFER_HPP
#define SCREENBUFFER_HPP

#ifdef TARGET_OS_MAC
#include <SDL2/SDL.h>
#elif _WIN32
#include <SDL2/SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <boost/smart_ptr/shared_ptr.hpp>

#include <string>
#include <vector>

/**
 * @class ScreenPixel
 * @author Micahel Griffin
 * @date 12/18/2015
 * @file screen_buffer.hpp
 * @brief Holds Character/Sequence and Attributes of each position.
 */
class ScreenPixel
{
public:

    // Default for Object Creation Reused!
    ScreenPixel()
        : m_character_sequence()
        , m_foreground( {255, 255, 255, 0})
        , m_background( {0, 0, 0, 0})
    { }

    // Not used at this time!
    ScreenPixel(std::string sequence, SDL_Color fg, SDL_Color bg)
        : m_character_sequence(sequence)
        , m_foreground(fg)
        , m_background(bg)
    { }

    ~ScreenPixel()
    {
        m_character_sequence.erase();
    }

    std::string m_character_sequence;
    SDL_Color m_foreground;
    SDL_Color m_background;
};


/**
 * @class ScreenBuffer
 * @author Michael Griffin
 * @date 12/18/2015
 * @file screen_buffer.hpp
 * @brief Array to hold local Characters to match Screen Text
 */
class ScreenBuffer
{
public:
    ScreenBuffer(int term_width, int term_height);
    ~ScreenBuffer();

    int m_position;
    int m_x_position;
    int m_y_position;
    int m_term_height;
    int m_term_width;

    // Screen Array.
    ScreenPixel              m_sequence_buffer;
    std::vector<ScreenPixel> m_screen_buffer;

    /**
     * @brief handle screen buffer, Keeps track of all screen data
     * @param my_sequence
     * @param fg_color
     * @param bg_color
     */
    void setScreenBuffer(unsigned char my_sequence,
                         SDL_Color fg_color,
                         SDL_Color bg_color);

    /**
     * @brief Scrolls the Screen Buffer Up a line to match the internal SDL_Surface
     */
    void scrollScreenBuffer();

    /**
     * @brief Clear Range of Screen Buffer for Erase Sequences.
     * @param start
     * @param end
     */
    void clearScreenBufferRange(int start, int end);

    /**
     * @brief Clears all data in the buffer.
     */
    void clearScreenBuffer();

    /**
     * @brief Testing, Prints screen butter to console.
     */
    void getScreenBufferText();

    /**
     * @brief Gets Coordinates from the screen already translated to
     * Screen Buffer Positions, Now we pull the position and throw the
     * Text data into the Clipboard.
     * @param start_x_position
     * @param start_y_position
     * @param num_chars
     * @param num_rows
     */
    void bufferToClipboard(
        int start_x_position,
        int start_y_position,
        int num_chars,
        int num_rows);

};

typedef boost::shared_ptr<ScreenBuffer> screen_buffer_ptr;

#endif // SCREENBUFFER_HPP
