#ifndef __RENDERER_H_
#define __RENDERER_H_

#include "surface_manager.hpp"
#include "window_manager.hpp"
#include "input_handler.hpp"

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
             window_manager_ptr window,
             input_handler_ptr input);
    ~Renderer();

    // Handle to Managers
    surface_manager_ptr m_surface_manager;
    window_manager_ptr  m_window_manager;
    input_handler_ptr   m_input_handler;

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
    SDL_Color m_currentFGColor;
    SDL_Color m_currentBGColor;

    /**
     * @brief STartup Creation of Screen Surfaces in Memory
     */
    void initSurfaceTextures();

    /**
     * @brief Calcuate the Box Dimensions for Copy/Paste Selection.
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
    void setScrollRegion(int top, int bot, int terminalHeight);

    /**
     * @brief Scroll a specific set region of the screen only
     */
    void scrollRegionUp();

    /**
     * @brief Clips out the Top Row of the Screen,
     * Then Moves Entire Screen Up one Row to Scroll it.
     */
    void scrollScreenUp();


    void clearScreenSurface();
    void renderDeleteCharScreen(int x, int y, int num);
    void renderClearLineAboveScreen(int y, int x);
    void renderClearLineBelowScreen(int y, int x);
    void renderClearLineScreen(int y, int start, int end);
    void renderBottomScreen();
    void renderScreen();
    void renderCharScreen(int x, int y);
    void renderCursorOnScreen();
    void renderCursorOffScreen();
    void drawTextureScreen();
    void clearScreen();
    int  compareSDL_Colors(SDL_Color &src, SDL_Color &dest);
    void replaceColor(SDL_Surface *src, Uint32 foreground, Uint32 background);
    void setupCursorChar();
    void drawChar(int X, int Y, int asciicode);

    //void drawString(int X, int Y, char text[]);
    //void drawCharSet(int X, int Y);

    // Term Scale
    int         m_termWidth;
    int         m_termHeight;

    // Scrolling Region
    bool        m_scrollRegionActive;
    int         m_topMargin;
    int         m_bottomMargin;

private:


#ifdef _DEBBUG
    TTF_Font*   trueTypeFont;      // UTF-8 Fonts.
#endif

    SDL_Rect    m_displayRect;
    SDL_Rect    m_rectBackground;

    int         m_cursorXPosition;
    int         m_cursorYPosition;
    bool        m_isUTF8Output;

};

typedef boost::shared_ptr<Renderer> renderer_ptr;

#endif
