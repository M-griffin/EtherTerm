#ifndef SURFACE_MANAGER_HPP
#define SURFACE_MANAGER_HPP

#include "surface.hpp"
#include "texture.hpp"

#include <SDL2/SDL.h>

#include <unordered_map>
#include <iostream>
#include <cstdio>
#include <cassert>
#include <vector>
#include <cmath>

#include <memory>

class WindowManager;
typedef std::shared_ptr<WindowManager> window_manager_ptr;
typedef std::weak_ptr<WindowManager> window_manager_weak_ptr;

class FontSet;
typedef std::shared_ptr<FontSet> font_set_ptr;

class FontEntry;

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
    SurfaceManager(window_manager_ptr &window_manager, const std::string &program_path);
    ~SurfaceManager();

    // Handle to Current Window
    window_manager_weak_ptr  m_weak_window_manager;

    std::string    m_program_path;
    std::string    m_current_font;
    std::string    m_previous_font;

    // Holds list of all fonts in their parameters.
    font_set_ptr   m_font_set;

    // Surface List for easier Management.
    std::unordered_map<int, surface_ptr> m_surface_list;
    std::unordered_map<int, texture_ptr> m_texture_list;

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

    Uint32 m_red_mask,
           m_green_mask,
           m_blue_mask,
           m_alpha_mask;

    int m_surface_bits;
    int m_character_width;
    int m_character_height;

    /**
     * @brief Helper for Program Path
     */
    std::string getProgramPath();

    /**
     * @brief Helper to append font path
     */
    std::string getFontPath();

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
    void setCurrentFont(const std::string &font);

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
     * @brief Read in the XML Data File.
     * @return
     */
    bool readFontSets();

    /**
     * @brief Searches and grab the fontset by filename
     * @param value
     * @return
     */
    FontEntry getFontFromSet(const std::string &value);

    /**
     * @brief Loads Fonts
     * @return
     */
    bool loadFont();

    /**
     * @brief Loads Bitmap Fonts from Files
     * @return
     */
    bool loadBitmapFontImage();

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

    /**
     * @brief Fill the Surface with the Current Color Attributes.
     */
    void fillSurfaceColor(int surfaceType, SDL_Rect *rect, SDL_Color *color);

};

typedef std::shared_ptr<SurfaceManager> surface_manager_ptr;

#endif // SURFACE_MANAGER_HPP
