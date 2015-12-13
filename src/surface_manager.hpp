#ifndef SURFACE_MANAGER_HPP
#define SURFACE_MANAGER_HPP

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
 * @class Surfaces
 * @author Michael Griffin
 * @date 12/11/2015
 * @file surface_manager.hpp
 * @brief Dynamic Container to Hold Surfaces
 */
class Surfaces
{

public:
    Surfaces(SDL_Surface *surface)
        : m_surface(surface)
        , m_is_converted(false)
    {
    }

    ~Surfaces()
    {
        std::cout << "surfaces: Surface Removed!" << std::endl;
        SDL_FreeSurface(m_surface);
    }

    SDL_Surface *getSurface()
    {
        if (!m_surface)
        {
            SDL_Log("Surface is null!");
        }
        return m_surface;
    }

    /**
     * @brief Convert The Surface to match Texutre Pixel Format
     * @return
     */
    bool convert()
    {
        if (!m_is_converted)
        {
            m_surface = SDL_ConvertSurfaceFormat(m_surface, SDL_PIXELFORMAT_ARGB8888, 0);
            if(!m_surface)
            {
                SDL_Log("Unable to Convert Surface");
                return false;
            }
            m_is_converted = true;
        }
        return true;
    }

    /**
     * @brief Clear the Surface with all Black.
     * @return
     */
    void clear()
    {
        SDL_FillRect(m_surface, nullptr, SDL_MapRGBA(m_surface->format, 0, 0, 0, 0));
    }

    /**
     * @brief Make sure Surface is not null!
     * @return
     */
    bool exists()
    {
        return (m_surface != nullptr);
    }

private:
    SDL_Surface *m_surface;
    bool         m_is_converted;

};

typedef boost::shared_ptr<Surfaces> surface_ptr;


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

    // Textures
    SDL_Texture*   m_globalTexture;     // Main Rendering Texture
    SDL_Texture*   m_selectionTexture;  // For Copy Text Selection Texture

    // Surface List for easier Management.
    std::unordered_map<int, surface_ptr> m_surfaceList;

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
        TEXTURE_GLOBAL = 0,
        TEXTURE_SELECTION
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

};

typedef boost::shared_ptr<SurfaceManager> surface_manager_ptr;

#endif // SURFACE_MANAGER_HPP
