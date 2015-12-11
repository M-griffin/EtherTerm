#ifndef SURFACE_MANAGER_HPP
#define SURFACE_MANAGER_HPP

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
#include <cmath>
#include <cstdio>
#include <vector>
#include <assert.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

class SurfaceManager;
typedef boost::shared_ptr<SurfaceManager> surface_manager_ptr;

/**
 * @class SurfaceManager
 * @author Michael Griffin
 * @date 11/16/2015
 * @file surface_manager.hpp
 * @brief Manages SDL Surfaces and Textures
 */
class SurfaceManager
    : public boost::enable_shared_from_this<SurfaceManager>
{
public:
    SurfaceManager(window_manager_ptr window_manager);
    ~SurfaceManager();

    void create();

    // Handle to Current Window
    window_manager_ptr     m_window_manager;

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

    SDL_Color m_currentFGColor;
    SDL_Color m_currentBGColor;

    // Scrolling Region
    bool           m_scrollRegionActive;
    int            m_topMargin;
    int            m_bottomMargin;

    std::string    m_programPath;
    std::string    m_windowTitle;
    std::string    m_currentFont;
    std::string    m_previousFont;

    // Surfaces
    SDL_Surface*   m_tmpSurface;        // Char Cell
    SDL_Surface*   m_cursorOnSurface;   // Char Cell for showing the cursor.
    SDL_Surface*   m_cursorOffSurface;  // Char Cell for clearing the cursor.
    SDL_Surface*   m_screenSurface;     // Internal Screen Surface
    SDL_Surface*   m_bottomSurface;     // Last Line of Screen Surface for Scrolling
    SDL_Surface*   m_fontCachedSurface; // Current Cached Font Surface

    SDL_Texture*   m_globaltexture;           // Main Texture
    SDL_Texture*   m_selectionTexture;  // For Copy Text Selection

    // Textures
    std::vector<SDL_Surface *> m_surfaceList;

    // Handle Surface Alias
    enum
    {
        TEMP_SURFACE = 0,
        CURSOR_ON_SURFACE,
        CURSOR_OFF_SURFACE,
        SCREEN_SURFACE,
        BOTTOM_SURFACE,
        CACHED_SURFACE
    };

    // Handle Texture Alias
    enum
    {
        GLOBAL_TEXTURE = 0,
        SELECTION_TEXTURE
    };

#ifdef _DEBBUG
    TTF_Font*      trueTypeFont;      // UTF-8 Fonts.
#endif

    Uint32 m_redMask,
           m_greenMask,
           m_blueMask,
           m_alphaMask;

    int m_surfaceWidth;
    int m_surfaceHeight;
    int m_windowWidth;
    int m_windowHeight;
    int m_surfaceBits;

    SDL_Rect m_displayRect;
    SDL_Rect m_rectBackground;

    int  m_characterWidth;
    int  m_characterHeight;
    int  m_cursorXPosition;
    int  m_cursorYPosition;

    bool m_isChangingState;
    bool m_isRunning;
    bool m_isUTF8Output;


};

#endif // SURFACE_MANAGER_HPP
