#include "surface_manager.hpp"

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
#include <assert.h>

SurfaceManager::SurfaceManager(window_manager_ptr window_manager)
    : m_window_manager(window_manager)
    , BLACK(         {   0,   0,   0,   0 })
    , BLUE(          {   0,   0, 171,   0 })
    , GREEN(         {   0, 171,   0,   0 })
    , CYAN(          {   0, 171, 171,   0 })
    , RED(           { 171,   0,   0,   0 })
    , MAGENTA(       { 171,   0, 171,   0 })
    , BROWN(         { 171,  87,   0,   0 })
    , GREY(          { 171, 171, 171,   0 })
    , DARK_GREY(     {  87,  87,  87,   0 })
    , LIGHT_BLUE(    {  87,  87, 255,   0 })
    , LIGHT_GREEN(   {  87, 255,  87,   0 })
    , LIGHT_CYAN(    {  87, 255, 255,   0 })
    , LIGHT_RED(     { 255,  87,  87,   0 })
    , LIGHT_MAGENTA( { 255,  87, 255,   0 })
    , YELLOW(        { 255, 255,  87,   0 })
    , WHITE(         { 255, 255, 255,   0 })
    , m_currentFGColor(GREY)
    , m_currentBGColor(BLACK)
    , m_programPath("")
    , m_windowTitle("")
    , m_currentFont("vga8x16.bmp")
    , m_previousFont("")
    , m_tmpSurface(nullptr)        // Char Cell
    , m_cursorOnSurface(nullptr)   // Char Cell for the cursor.
    , m_cursorOffSurface(nullptr)  // Char Cell for the cursor.
    , m_screenSurface(nullptr)     // Internal Screen Buffer
    , m_bottomSurface(nullptr)     // Last Line of Screen
    , m_fontCachedSurface(nullptr) // Cached Font CharacterSet
    , m_selectionTexture(nullptr)  // Texture for Copy/Paste Selection

       // Initalize Color Masks.
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    , m_redMask(0xff000000)
    , m_greenMask(0x00ff0000)
    , m_blueMask(0x0000ff00)
    , m_alphaMask(0x000000ff)
#else
    , m_redMask(0x000000ff)
    , m_greenMask(0x0000ff00)
    , m_blueMask(0x00ff0000)
    , m_alphaMask(0xff000000)
#endif
    , m_surfaceWidth(640)
    , m_surfaceHeight(400)
    , m_windowWidth(640)
    , m_windowHeight(400)
    , m_surfaceBits(32)
    , m_characterWidth(8)
    , m_characterHeight(16)
    , m_cursorXPosition(0)
    , m_cursorYPosition(0)
    , m_isUTF8Output(false) // Default to No, if Yes Block Font Switching!
{
    m_surfaceList.reserve(6);
    m_surfaceList.resize(6);
}

SurfaceManager::~SurfaceManager()
{
}

