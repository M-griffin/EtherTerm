#ifndef __RENDERER_H_
#define __RENDERER_H_

#include "surface_manager.hpp"
#include "window_manager.hpp"

#ifdef TARGET_OS_MAC
#include <SDL2/SDL.h>
#ifdef _DEBBUG
#include <SDL2/SDL_ttf.h>
#endif
#elif _WIN32
#include <SDL2/SDL.h>
#ifdef _DEBBUG
#include <SDL_ttf.h>
#endif
#else
#include <SDL2/SDL.h>
#ifdef _DEBBUG
#include <SDL2/SDL_ttf.h>
#endif
#endif

#include <iostream>
#include <string>
#include <vector>

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/weak_ptr.hpp>

// Forward Deceleration
class Session;
typedef boost::shared_ptr<Session> session_ptr;
typedef boost::weak_ptr<Session> session_weak_ptr;


class InputHandler;
typedef boost::shared_ptr<InputHandler> input_handler_ptr;


/**
 * @class Renderer
 * @author Michael Griffin
 * @date 12/12/2015
 * @file renderer.hpp
 * @brief Handles Rendering and Building Surface/Texture Data
 */
class Renderer
{
public:

    Renderer(surface_manager_ptr surface,
             window_manager_ptr  window,
             input_handler_ptr   input,
             session_ptr         session);
    ~Renderer();

    // Handle to Managers
    surface_manager_ptr m_surface_manager;
    window_manager_ptr  m_window_manager;
    input_handler_ptr   m_input_handler;

    // Handle to Instance, make weak!
    session_weak_ptr    m_weak_session;

    // Define ANSI Color Schemes
    SDL_Color BLACK;
    SDL_Color BLUE;
    SDL_Color GREEN;
    SDL_Color CYAN;
    SDL_Color RED;
    SDL_Color MAGENTA;
    SDL_Color BROWN;

    SDL_Color GREY;
    SDL_Color DARK_GREY;
    SDL_Color LIGHT_BLUE;
    SDL_Color LIGHT_GREEN;
    SDL_Color LIGHT_CYAN;
    SDL_Color LIGHT_RED;
    SDL_Color LIGHT_MAGENTA;
    SDL_Color YELLOW;
    SDL_Color WHITE;

    // Active Colors
    SDL_Color   m_current_fg_color;
    SDL_Color   m_current_bg_color;

    // Term Scale
    int         m_term_width;
    int         m_term_height;

    // Scrolling Region
    bool        m_is_scroll_region_active;
    int         m_top_margin;
    int         m_bottom_margin;

private:

#ifdef _DEBBUG
    // WIP
    TTF_Font*   m_trueTypeFont; // UTF-8 Fonts.
#endif

    int         m_cursor_x_position;
    int         m_cursor_y_position;
    bool        m_is_scalling_surface;
    bool        m_is_utf8_output;

public:
    /**
     * @brief Startup Creation of Screen Surfaces in Memory
     */
    void initSurfaceTextures();

    /**
     * @brief Calculate the Box Dimensions for Copy/Paste Selection.
     * @param rect
     */
    void calcBoxSize(SDL_Rect &rect,
                     int sourceX, int sourceY,
                     int x, int y,
                     double charWidth, double charHeight);

    /**
     * @brief Translates Screen Coordinates to ScreenBuffer for Text.
     * @param x
     * @param y
     */
    void pullSelectionBuffer(int x, int y);

    /**
     * @brief Clear so texture is refreshed each selection
     */
    void clearSelectionTexture();

    /**
     * @brief On selected mouse movement area! We want to overlay a blended texture
     * @param x
     * @param y
     */
    void renderSelectionScreen(int x, int y);

    /**
     * @Brief Overrides the Scrolling region of the screen
     */
    void setScrollRegion(int top, int bot, int term_height);

    /**
     * @brief Scroll a specific set region of the screen only
     */
    void scrollRegionUp();

    /**
     * @brief Clips out the Top Row of the Screen,
     * Then Moves Entire Screen Up one Row to Scroll it.
     */
    void scrollScreenUp();

    /**
     * @brief Clears the Surface Back Buffer, and Global Texture
     * Clears the surface to the current background color
     * This is for ESC[2J proper behavior.
     */
    void clearScreenSurface();

    /**
     * @brief Fill entire lines with space and BLACK foreground/Background
     * Fast update to clear a row.
     * @param y
     * @param start
     * @param end
     */
    void renderClearLineScreen(int y, int start, int end);

    /**
     * @brief Erasing all of the screen below.
     * @param y
     * @param x
     */
    void renderClearLineAboveScreen(int y, int x);

    /**
     * @brief Erasing all of the screen below.
     * @param y
     * @param x
     */
    void renderClearLineBelowScreen(int y, int x);

    /**
     * @brief This will erase the current character or (s)
     * Also moves all following text left by 1.
     * @param x
     * @param y
     * @param num
     */
    void renderDeleteCharScreen(int x, int y, int num);

    /**
     * @brief Render The Bottom Row of the screen (After Scrolling)
     */
    void renderBottomScreen();

    /**
     * @brief Renders the MainScreen to the Texture.
     */
    void renderScreen();

    /**
     * @brief Renderer single character cell to texture.
     * @param x
     * @param y
     */
    void renderCharScreen(int x, int y);

    /**
     * @brief Setup the Cursor to Display at current position
     */
    void renderCursorOnScreen();

    /**
     * @brief Setup the Cursor to hide at current position
     */
    void renderCursorOffScreen();

    /**
     * @brief Pushed the Current MAIN Texture to the Renderer to Draw
     */
    void drawTextureScreen();

    /**
     * @brief Clears Renderer and Redraws Black Screen
     */
    void clearScreen();

    /**
     * @brief Compares (2) SDL Colors to determine if they are the same
     * @param src
     * @param dest
     * @return
     */
    int  compareSDL_Colors(SDL_Color &src, SDL_Color &dest);

    /**
     * @brief Recolors FG/BG colors per character cell.
     * @param foreground
     * @param background
     */
    void replaceColor(Uint32 foreground, Uint32 background);

    /**
     * @brief Setup the Cursor font, character and create from underscore
     * NOTE: this should be reworked, can't trust all font sets!
     * This is not the best way to handle this IMO.
     */
    void setupCursorCharacter();

    /**
     * @brief Draws/Recolors a Char Cell to x/y location on screen surface.
     * @param X
     * @param Y
     * @param asciicode
     */
    void drawCharacterCell(int X, int Y, int ascii_code);

    // Testing only, draws bitmap graphic.
    void drawString(int X, int Y, char text[]);
    void drawCharSet(int X, int Y);

};

typedef boost::shared_ptr<Renderer> renderer_ptr;

#endif
