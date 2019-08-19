#include "surface_manager.hpp"
#include "window_manager.hpp"
#include "font_manager.hpp"
#include "font_set.hpp"
#include "static_methods.hpp"

#include <SDL2/SDL.h>

#include <iostream>
#include <cmath>
#include <cstdio>
#include <assert.h>

SurfaceManager::SurfaceManager(window_manager_ptr &window_manager, const std::string &program_path)
    : m_weak_window_manager(window_manager)
    , m_program_path(program_path)
    , m_current_font("vga8x16.bmp")
    , m_previous_font("")
      // Initialize Color Masks.
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    , m_red_mask(0xff000000)
    , m_green_mask(0x00ff0000)
    , m_blue_mask(0x0000ff00)
    , m_alpha_mask(0x000000ff)
#else
    , m_red_mask(0x000000ff)
    , m_green_mask(0x0000ff00)
    , m_blue_mask(0x00ff0000)
    , m_alpha_mask(0xff000000)
#endif
    , m_surface_bits(32)
    , m_character_width(8)
    , m_character_height(16)
{
    std::cout << "SurfaceManager Created!" << std::endl;
}

SurfaceManager::~SurfaceManager()
{
    std::cout << "~SurfaceManager" << std::endl;

    // Clear All Surfaces
    std::unordered_map<int, surface_ptr>().swap(m_surface_list);

    // Clear All Textures
    std::unordered_map<int, texture_ptr>().swap(m_texture_list);
}


/**
 * @brief Add Surface to Container
 * @param value
 * @param surface
 */
void SurfaceManager::addSurface(int value, surface_ptr surface)
{
    m_surface_list.insert(std::make_pair(value, surface));
}

/**
 * @brief Remove Surface From Container
 * @param value
 */
void SurfaceManager::delSurface(int value)
{
    auto it = m_surface_list.find(value);

    if(it != m_surface_list.end())
    {
        m_surface_list.erase(it);
    }
}

/**
 * @brief Tests if Surface is in list.
 * @param value
 */
bool SurfaceManager::surfaceExists(int value)
{
    auto it = m_surface_list.find(value);

    if(it != m_surface_list.end())
    {
        return true;
    }

    return false;
}

/**
 * @brief Add Surface to Container
 * @param value
 * @param surface
 */
void SurfaceManager::addTexture(int value, texture_ptr texture)
{
    m_texture_list.insert(std::make_pair(value, texture));
}

/**
 * @brief Remove Surface From Container
 * @param value
 */
void SurfaceManager::delTexture(int value)
{
    auto it = m_texture_list.find(value);

    if(it != m_texture_list.end())
    {
        m_texture_list.erase(it);
    }
}

/**
 * @brief Tests if Surface is in list.
 * @param value
 */
bool SurfaceManager::textureExists(int value)
{
    auto it = m_texture_list.find(value);

    if(it != m_texture_list.end())
    {
        return true;
    }

    return false;
}

/**
 * @brief Set the Current Font name
 * @return
 */
void SurfaceManager::setCurrentFont(const std::string &font)
{
    m_previous_font = m_current_font;
    m_current_font = font;
}

/**
 * @brief Grab the Current Font name
 * @return
 */
std::string SurfaceManager::getCurrentFont()
{
    return m_current_font;
}

/**
 * @brief Check if a new Font was Set
 * @return
 */
bool SurfaceManager::didFontChange()
{
    return (m_current_font != m_previous_font);
}


/**
 * @brief Read in the XML Data File.
 * @return
 */
bool SurfaceManager::readFontSets()
{

    font_manager_ptr font_manager(new FontManager(m_program_path));
    m_font_set = font_manager->retrieveFontSet();

    return true;
}

/**
 * @brief Searches and grab the fontset by filename
 * @param value
 * @return
 */
FontEntry SurfaceManager::getFontFromSet(const std::string &value)
{
    FontEntry entry;
    entry.index = -1;

    for(FontEntry f : m_font_set->font_entries)
    {
        if(f.filename == value)
        {
            return f;
        }
    }

    return entry;
}

/**
 * @brief Loads Fonts
 * @param fontName
 * @return
 */
bool SurfaceManager::loadFont()
{
    bool is_loaded = false;

    if(m_font_set->font_entries.size() == 0)
    {
        is_loaded = readFontSets();

        if(is_loaded)
        {
            std::cout << "Error, Fontsets not loaded!" << std::endl;
            return false;
        }

        // reset.
        is_loaded = false;
    }

    std::cout << "m_font_set->font_entries.size(): " << m_font_set->font_entries.size() << std::endl;
    FontEntry font = getFontFromSet(m_current_font);

    if(font.index == -1 || font.filename != m_current_font)
    {
        std::cout << "Error, unable to load front from set!" << std::endl;
        assert(false);
    }

    // If type = bmp or bitmap.
    if(font.type == "bmp")
    {
        is_loaded = loadBitmapFontImage();
    }
    // True Type (WIP) for Unicode fonts.
    else if(font.type == "ttf")
    {  }

    // If font loaded, then read and set the properties.
    if(is_loaded)
    {
        /**
         * Important note, if these change, but there static right now
         * Then we need to recrete the surfaces with new calculations for
         * 80x25 by calculating width and height in pixels 8x16 = 640x400.
         */
        // Set the character dimenions
        m_character_width = font.width;
        m_character_height = font.height;
    }

    return is_loaded;
}

/**
 * @brief Loads Bitmap Fonts Images from Files
 * @param fontName
 * @return
 */
bool SurfaceManager::loadBitmapFontImage()
{
    std::string path = StaticMethods::getFontPath(m_program_path);
    path.append(m_current_font);

    std::cout << "loading Font -> : " << path << std::endl;

    // Loading Bitmap first time, or reloading,  If already Exists remove it.
    if(surfaceExists(SURFACE_FONT))
    {
        delSurface(SURFACE_FONT);
    }

    // Load the Font into the Surface.
    surface_ptr fontSurface(
        new Surfaces(
            SDL_LoadBMP(path.c_str())
        )
    );

    if(!fontSurface->exists())
    {
        SDL_Log("loadBitmapImage() fontSurface font: %s", path.c_str());
        SDL_Delay(1500);
        assert(false);
    }

    // Convert and Add to Surface Manager.
    if(!fontSurface->convert())
    {
        SDL_Log("Error Converting Font Surface!");
        return false;
    }

    addSurface(SURFACE_FONT, fontSurface);

    return true;
    /*
     * WIP, Implement this!
     */
    /*
       // Redraw Cursor with new font!
       // When no data received, this is when we want to show the cursor!
       // Setup cursor in current x/y position Cursor.
       if(TheSequenceParser::Instance()->isCursorActive())
       {
           setupCursorChar();
           renderCursorOnScreen();
           drawTextureScreen();
       }

       m_previous_font = m_current_font; // Current to Previous
       m_current_font  = fontName;    // New to Current.
       return m_cachedSurface != nullptr;*/
}


/**
 * @brief Creates / Recreate Textures
 * @param textureType
 * @param surface
 */
void SurfaceManager::createTexture(int textureType, SDL_Surface *surface)
{
    // Handle to Window Manager
    window_manager_ptr window = m_weak_window_manager.lock();

    if(!window)
    {
        return;
    }

    switch(textureType)
    {
        case TEXTURE_MAIN_SCREEN:
        {
            // If Exists, Recreate it.
            if(textureExists(textureType))
            {
                delTexture(textureType);
            }

            // Handle to Window Manager
            window_manager_ptr window = m_weak_window_manager.lock();

            if(!window)
            {
                return;
            }

            // Get the Actual size of the Renderer to match the surface for scaling.

            // m_is_scalling_surface!!  Add Toggle for Scaling here,
            // Otherwise should use same size as main surface!
            int screenWidth, screenHeight;
            SDL_GetRendererOutputSize(
                window->getRenderer(),
                &screenWidth,
                &screenHeight
            );

            // Create Texture: SDL_TEXTUREACCESS_STREAMING
            texture_ptr texture(
                new Textures(
                    SDL_CreateTexture(
                        window->getRenderer(),
                        SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STREAMING,
                        screenWidth, screenHeight
                    )
                )
            );

            addTexture(textureType, texture);

            // Set the Blend Mode of the Texture NONE
            if(SDL_SetTextureBlendMode(
                        texture->getTexture(),
                        SDL_BLENDMODE_NONE) < 0)
            {
                SDL_Log("Error Setting Blend on Texture - %s", SDL_GetError());
            }
        }
        break;

        case TEXTURE_SELECTION:
        case TEXTURE_HILIGHT:
        {
            // If Exists, Recreate it.
            if(textureExists(textureType))
            {
                delTexture(textureType);
            }

            // Create Texture: SDL_TEXTUREACCESS_TARGET
            texture_ptr texture(
                new Textures(
                    SDL_CreateTexture(
                        window->getRenderer(),
                        SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_TARGET,
                        surface->w, surface->h
                    )
                )
            );

            addTexture(textureType, texture);

            // Set the Blend Mode of the Texture ADD
            if(SDL_SetTextureBlendMode(
                        texture->getTexture(),
                        SDL_BLENDMODE_ADD) < 0)
            {
                SDL_Log("Error Setting Blend on Texture - %s", SDL_GetError());
            }
        }
        break;

        default:
            break;
    }
}

/**
 * @brief Fills or Clears a Surface with Black.
 * @param surface
 */
void SurfaceManager::clearSurface(int value)
{
    auto it = m_surface_list.find(value);

    if(it != m_surface_list.end())
    {
        (*it).second->clear();
    }
}

/**
 * @brief Handle Converting Surface and Adding to SurfaceManager.
 * @param surface
 */
void SurfaceManager::convertAndAdd(int surfaceType, surface_ptr surface)
{
    // Convert the Pixel Format.
    if(surface->exists())
    {
        if(!surface->convert())
        {
            SDL_Log("Unable to create charSurface");
            assert(false);
        }

        // Success, add the Surface to the Manager
        addSurface(surfaceType, surface);
    }
    else
    {
        SDL_Log("Unable to create charSurface");
        assert(false);
    }
}

/**
 * @brief Create RGB Surface, then Converts to RGBA on the fly.
 * @param surfaceType
 */
void SurfaceManager::createSurface(int surfaceType)
{
    // Run Through Surface Alias and create surface.
    switch(surfaceType)
    {
        // Handle Individual Character Surfaces
        case SURFACE_CHARACTER:
        case SURFACE_CURSOR_ON:
        case SURFACE_CURSOR_OFF:
        {
            // If Exists, Recreate it.
            if(surfaceExists(surfaceType))
            {
                delSurface(surfaceType);
            }

            // Create Surface with Smart pointer.
            surface_ptr surface(
                new Surfaces(
                    SDL_CreateRGBSurface(
                        SDL_SWSURFACE,
                        m_character_width, m_character_height, m_surface_bits,
                        m_red_mask, m_green_mask, m_blue_mask, m_alpha_mask
                    )
                )
            );

            // Setup alpha Blending.
            //SDL_SetSurfaceBlendMode(surface->getSurface(), SDL_BLENDMODE_BLEND);

            // Enable RLE acceleration
            //SDL_SetSurfaceRLE(surface->getSurface(), 1);

            convertAndAdd(surfaceType, surface);
        }
        break;

        // Handles Main Screen Surfaces
        case SURFACE_MAIN_SCREEN:
        {
            // If Exists, Recreate it.
            if(surfaceExists(surfaceType))
            {
                delSurface(surfaceType);
            }

            window_manager_ptr window = m_weak_window_manager.lock();

            if(window)
            {
                // Create Surface with Smart pointer
                surface_ptr surface(
                    new Surfaces(
                        SDL_CreateRGBSurface(
                            SDL_SWSURFACE,
                            window->getWindowWidth(), window->getWindowHeight(), m_surface_bits,
                            m_red_mask, m_green_mask, m_blue_mask, m_alpha_mask
                        )
                    )
                );

                convertAndAdd(surfaceType, surface);
            }
        }
        break;

        // Handles Screen Surface Scaled to Match Texture Dimensions.
        case SURFACE_MAIN_SCALED:
        {
            // If Exists, Recreate it.
            if(surfaceExists(surfaceType))
            {
                delSurface(surfaceType);
            }

            // Handle to Window Manager
            window_manager_ptr window = m_weak_window_manager.lock();

            if(!window)
            {
                return;
            }

            // Get the Actual size of the Renderer to match the surface for scaling.
            int screenWidth, screenHeight;
            SDL_GetRendererOutputSize(
                window->getRenderer(),
                &screenWidth,
                &screenHeight
            );

            // Create Surface with Smart pointer
            surface_ptr surface(
                new Surfaces(
                    SDL_CreateRGBSurface(
                        SDL_SWSURFACE,
                        screenWidth, screenHeight, m_surface_bits,
                        m_red_mask, m_green_mask, m_blue_mask, m_alpha_mask
                    )
                )
            );

            convertAndAdd(surfaceType, surface);
        }
        break;

        // Handles Last Row of a Surface for Scrolling
        // Currently used for Main Screen, might need scaled version instead?
        case SURFACE_BOTTOM_ROW:
        {
            // If Exists, Recreate it.
            if(surfaceExists(surfaceType))
            {
                delSurface(surfaceType);
            }

            if(!surfaceExists(SURFACE_MAIN_SCREEN))
            {
                SDL_Log("Main Screen Doesn't Exist for Bottom Row Surface!");
                assert(false);
            }

            // Create Surface with Smart pointer, uses with of Main Screen, Must exist first!
            surface_ptr surface(
                new Surfaces(
                    SDL_CreateRGBSurface(
                        SDL_SWSURFACE,
                        m_surface_list[SURFACE_MAIN_SCREEN]->getSurface()->w, m_character_height, m_surface_bits,
                        m_red_mask, m_green_mask, m_blue_mask, m_alpha_mask
                    )
                )
            );

            convertAndAdd(surfaceType, surface);
        }
        break;

        default:
            break;
    }
}

/**
 * @brief Lock Surface to Modify Pixels
 */
void SurfaceManager::lockSurface(int surfaceType)
{
    if(SDL_MUSTLOCK(m_surface_list[surfaceType]->getSurface()))
    {
        if(SDL_LockSurface(m_surface_list[surfaceType]->getSurface()) < 0)
        {
            SDL_Log("lockSurface(): %s", SDL_GetError());
            assert(false);
        }
    }
}

/**
 * @brief Unlock Surface Modify Pixels Completed.
 */
void SurfaceManager::unlockSurface(int surfaceType)
{
    if(SDL_MUSTLOCK(m_surface_list[surfaceType]->getSurface()))
    {
        SDL_UnlockSurface(m_surface_list[surfaceType]->getSurface());
    }
}

/**
 * @brief Fill the Surface with the Current Color Attributes.
 */
void SurfaceManager::fillSurfaceColor(int surfaceType, SDL_Rect *rect, SDL_Color *color)
{

    //SDL_LockSurface(m_surface_list[surfaceType]->getSurface());

    // Fill screen with current RGB Background colors.
    if(SDL_FillRect(
                m_surface_list[surfaceType]->getSurface(),
                rect,
                SDL_MapRGBA(
                    m_surface_list[surfaceType]->getSurface()->format,
                    color->r,
                    color->g,
                    color->b,
                    color->a
                )) < 0)
    {
        SDL_Log("fillSurfaceCurrentColor(): %s", SDL_GetError());
        assert(false);
    }

    //SDL_UnlockSurface(m_surface_list[surfaceType]->getSurface());
}
