#ifndef SURFACE_MANAGER_HPP
#define SURFACE_MANAGER_HPP

#include "surface.hpp"
#include "texture.hpp"
#include "window_manager.hpp"

#ifdef TARGET_OS_MAC
#include <SDL2/SDL.h>
#elif _WIN32
#include <SDL2/SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <iostream>
#include <cmath>
#include <cstdio>
#include <unordered_map>
#include <assert.h>

#include <boost/smart_ptr/shared_ptr.hpp>


/**
 * @class SurfaceManager
 * @author Michael Griffin
 * @date 11/16/2015
 * @file surface_manager.hpp
 * @brief Manages SDL Surfaces and Textures
 */
class SurfaceManager
{
public:
    SurfaceManager(window_manager_ptr window_manager, std::string program_path);
    ~SurfaceManager();

    // Handle to Current Window
    window_manager_ptr     m_window_manager;

   
    std::string    m_programPath;
    std::string    m_currentFont;
    std::string    m_previousFont;

    // Surface List for easier Management.
    std::unordered_map<int, surface_ptr> m_surfaceList;
    std::unordered_map<int, texture_ptr> m_textureList;

    // Handle Surface Alias
    enum
    {
        SURFACE_MAIN_SCREEN = 0,
        SURFACE_MAIN_SCALED,
        SURFACE_FONT,
        SURFACE_CHARACTER,
        SURFACE_CURSOR_ON,
        SURFACE_CURSOR_OFF,
        SURFACE_BOTTOM_ROW,
    };

    // Handle Texture Alias
    enum
    {
        TEXTURE_MAIN_SCREEN = 0,
        TEXTURE_SELECTION,
        TEXTURE_HILIGHT
    };

    Uint32 m_redMask,
           m_greenMask,
           m_blueMask,
           m_alphaMask;

    int m_surfaceWidth;
    int m_surfaceHeight;
    int m_windowWidth;
    int m_windowHeight;
    int m_surfaceBits;
    int m_characterWidth;
    int m_characterHeight;

    /**
     * @brief Add Surface to Container
     * @param value
     * @param surface
     */
    void addSurface(int value, surface_ptr surface);

    /**
     * @brief Remove Surface From Container
     * @param value
     */
    void delSurface(int value);

    /**
     * @brief Tests if Surface is in list.
     * @param value
     */
    bool surfaceExists(int value);

    /**
     * @brief Add Surface to Container
     * @param value
     * @param surface
     */
    void addTexture(int value, texture_ptr texture);

    /**
     * @brief Remove Surface From Container
     * @param value
     */
    void delTexture(int value);

    /**
     * @brief Tests if Surface is in list.
     * @param value
     */
    bool textureExists(int value);

    /**
     * @brief Set the Current Fontname
     * @return
     */
    void setCurrentFont(std::string font);

    /**
     * @brief Grab the Current Fontname
     * @return
     */
    std::string getCurrentFont();

    /**
     * @brief Check if a new Font was Set
     * @return
     */
    bool didFontChange();

    /**
     * @brief Loads Bitmap Fonts from Files
     * @param fontName
     * @return
     */
    bool loadBitmapImage(std::string fontName);

    /**
     * @brief Creates / Recreate Textures
     * @param textureType
     * @param surface
     */
    void createTexture(int textureType, SDL_Surface *surface);

    /**
     * @brief Fills or Clears a Surface with Black.
     * @param surface
     */
    void clearSurface(int value);

    /**
     * @brief Handle Converting and Adding to SurfaceManager.
     * @param surfaceType
     * @param surface
     */
    void convertAndAdd(int surfaceType, surface_ptr surface);

    /**
     * @brief Create RGB Surface, then Converts to RGBA on the fly.
     * @param surfaceType
     */
    void createSurface(int surfaceType);

    /**
     * @brief Lock Surface to Modify Pixels
     */
    void lockSurface(int surfaceType);

    /**
     * @brief Unlock Surface Modify Pixels Completed.
     */
    void unlockSurface(int surfaceType);

};

typedef boost::shared_ptr<SurfaceManager> surface_manager_ptr;

#endif // SURFACE_MANAGER_HPP
