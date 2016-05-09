#include "surface_manager.hpp"
#include "window_manager.hpp"

#include <SDL2/SDL.h>

#include "tinyxml.hpp"

#include <iostream>
#include <cmath>
#include <cstdio>
#include <assert.h>

SurfaceManager::SurfaceManager(window_manager_ptr window_manager, std::string program_path)
    : m_weak_window_manager(window_manager)
    , m_programPath(program_path)
    , m_currentFont("vga8x16.bmp")
    , m_previousFont("")
    // Initialize Color Masks.
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
    , m_surfaceBits(32)
    , m_characterWidth(8)
    , m_characterHeight(16)
{
    std::cout << "SurfaceManager Created!" << std::endl;
}

SurfaceManager::~SurfaceManager()
{
    std::cout << "~SurfaceManager" << std::endl;
    // Clear All Fonts.
    std::vector<FontSet>().swap(m_fontSet);

    // Clear All Surfaces
    std::unordered_map<int, surface_ptr>().swap(m_surfaceList);

    // Clear All Textures
    std::unordered_map<int, texture_ptr>().swap(m_textureList);
}

/**
 * @brief Helper to append program path
 */
std::string SurfaceManager::getProgramPath()
{
    std::string path = m_programPath;
#ifdef _WIN32
    path.append("assets\\");
#else
    path.append("assets/");
#endif
    return path;
}

/**
 * @brief Helper to append font path
 */
std::string SurfaceManager::getFontPath()
{
    // Create Default Phone Book.
    std::string path = getProgramPath();
#ifdef _WIN32
    path.append("font\\");
#else
    path.append("font/");
#endif
    return path;
}


/**
 * @brief Add Surface to Container
 * @param value
 * @param surface
 */
void SurfaceManager::addSurface(int value, surface_ptr surface)
{
    m_surfaceList.insert(std::make_pair(value, surface));
}

/**
 * @brief Remove Surface From Container
 * @param value
 */
void SurfaceManager::delSurface(int value)
{
    auto it = m_surfaceList.find(value);
    if(it != m_surfaceList.end())
    {
        m_surfaceList.erase(it);
    }
}

/**
 * @brief Tests if Surface is in list.
 * @param value
 */
bool SurfaceManager::surfaceExists(int value)
{
    auto it = m_surfaceList.find(value);
    if(it != m_surfaceList.end())
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
    m_textureList.insert(std::make_pair(value, texture));
}

/**
 * @brief Remove Surface From Container
 * @param value
 */
void SurfaceManager::delTexture(int value)
{
    auto it = m_textureList.find(value);
    if(it != m_textureList.end())
    {
        m_textureList.erase(it);
    }
}

/**
 * @brief Tests if Surface is in list.
 * @param value
 */
bool SurfaceManager::textureExists(int value)
{
    auto it = m_textureList.find(value);
    if(it != m_textureList.end())
    {
        return true;
    }
    return false;
}

/**
 * @brief Set the Current Font name
 * @return
 */
void SurfaceManager::setCurrentFont(std::string font)
{
    m_previousFont = m_currentFont;
    m_currentFont = font;
}

/**
 * @brief Grab the Current Font name
 * @return
 */
std::string SurfaceManager::getCurrentFont()
{
    return m_currentFont;
}

/**
 * @brief Check if a new Font was Set
 * @return
 */
bool SurfaceManager::didFontChange()
{
    return (m_currentFont != m_previousFont);
}


/**
 * @brief Read in the XML Data File.
 * @return
 */
bool SurfaceManager::readFontSets()
{
    FontSet font_set;
    std::string path = getProgramPath();
    path.append("fontsets.xml");

    TiXmlDocument doc(path.c_str());
    if(!doc.LoadFile())
    {
        std::cout << "readFontSets() Error Reading: " << path << std::endl;
        return false;
    }
    TiXmlHandle hDoc(&doc);
    TiXmlElement* pElem;
    TiXmlHandle hRoot(0);

    // If vector already populated then clear to refresh it.
    if(m_fontSet.size() > 0)
    {
        m_fontSet.clear();
        std::vector<FontSet>().swap(m_fontSet);
    }
    // block: EtherTerm
    {
        std::cout << "readFontConfiguration - FirstChildElement" << std::endl;
        pElem=hDoc.FirstChildElement().Element();
        // should always have a valid root but handle gracefully if it does
        if(!pElem)
        {
            std::cout << "readFontConfiguration - EtherTerm Element not found!" << std::endl;
            return false;
        }
        std::cout << "Root Value: " << pElem->Value() << std::endl;
        // save this for later
        hRoot=TiXmlHandle(pElem);
    }
    // block: Fontset
    {
        //std::cout << "readDialDirectory - Phonebook" << std::endl;
        pElem=hRoot.FirstChild("Fontset").Element();
        std::cout << "Fontset Version: " << pElem->Attribute("version") << std::endl;
    }
    // block: Font
    {
        //std::cout << "readDialDirectory - BBS" << std::endl;
        pElem=hRoot.FirstChild("Fontset").FirstChild().Element();
        for(; pElem; pElem=pElem->NextSiblingElement())
        {
            font_set.name = pElem->Attribute("name");
            font_set.type = pElem->Attribute("type");
            pElem->QueryIntAttribute("width", &font_set.width);
            pElem->QueryIntAttribute("height", &font_set.height);
            font_set.filename = pElem->Attribute("filename");
            font_set.sequence = pElem->Attribute("sequence");

            // Add to Vector so we can parse
            m_fontSet.push_back(font_set);
        }
    }
    std::cout << "readFontConfiguration - Done" << std::endl;
    return true;
}

/**
 * @brief Searches and grab the fontset by filename
 * @param value
 * @return
 */
FontSet SurfaceManager::getFontFromSet(std::string value)
{
    FontSet font_set;
    for(FontSet f : m_fontSet)
    {
        if(f.filename == value)
        {
            font_set = f;
        }
    }
    return font_set;
}

/**
 * @brief Loads Fonts
 * @param fontName
 * @return
 */
bool SurfaceManager::loadFont()
{
    bool is_loaded = false;

    if(m_fontSet.size() == 0)
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

    FontSet font(getFontFromSet(m_currentFont));
    if(font.filename != m_currentFont)
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
        m_characterWidth = font.width;
        m_characterHeight = font.height;
    }
    return is_loaded;
}


/**
 * @brief Set a Specific Surface Pixel
 * @param surface
 * @param x
 * @param y
 * @return
 */
Uint32 SurfaceManager::getPixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp)
    {
        case 1:
            //std::cout << "getPixel 1" << std::endl;
            return *p;

        case 2:
            //std::cout << "getPixel 2" << std::endl;
            return *(Uint16 *)p;

        case 3:

            //std::cout << "getPixel 3" << std::endl;
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << m_characterHeight | p[1] << m_characterHeight | p[2];
            else
                return p[0] | p[1] << m_characterWidth | p[2] << m_characterHeight;

            break;

        case 4:
            //std::cout << "getPixel 4" << std::endl;
            return *(Uint32 *)p;

        default:
            return 0;
    }
}

/**
 * @brief Get a Specific Surface Pixel
 * @param surface
 * @param x
 * @param y
 * @param pixel
 */
void SurfaceManager::putPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x*bpp;

    switch(bpp)
    {
        case 1:
            //std::cout << "putPixel 1" << std::endl;
            *p = pixel;
            break;

        case 2:
            //std::cout << "putPixel 2" << std::endl;
            *(Uint16 *)p = pixel;
            break;

        case 3:
            //std::cout << "putPixel 3" << std::endl;
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
                p[0]=(pixel >> m_characterHeight) & 0xff;
                p[1]=(pixel >> m_characterWidth) & 0xff;
                p[2]=pixel & 0xff;
            }
            else
            {
                p[0]=pixel & 0xff;
                p[1]=(pixel >> m_characterWidth) & 0xff;
                p[2]=(pixel >> m_characterHeight) & 0xff;
            }
            break;

        case 4:
            //std::cout << "putPixel 4" << std::endl;
            *(Uint32 *) p = pixel;
            break;
    }
}


/**
 * @brief Taken from SDL GFX to Scale Down a Surface.
 * @param target
 * @param factor
 */
void SurfaceManager::scaleDownSurface(SDL_Surface *target, const uint32_t & factor)
{
    // downscaling directly to screen is not supported
    if(!target) return;

    if(target->w / factor > target->w ||
            target->h / factor > target->h)
        return;


    SDL_LockSurface(target);

    int32_t target_y = 0;
    for(uint32_t src_y = factor/2; src_y < target->h; src_y += factor)
    {
        int32_t target_x = 0;
        for(int32_t src_x = factor/2; src_x < target->w; src_x += factor)
        {
            Uint32 px = getPixel(target, src_x, src_y);
            putPixel(target, target_x, target_y, px);
            target_x++;
        }
        target_y++;
    }

    SDL_UnlockSurface(target);

}


/**
 * @brief Loads Bitmap Fonts Images from Files
 * @param fontName
 * @return
 */
bool SurfaceManager::loadBitmapFontImage()
{
    std::string path = getFontPath();

    if(m_currentFont == "vga16x32.bmp")
        m_currentFont = "vga8x16.bmp";

    path.append(m_currentFont);

    std::cout << "loading Font -> : " << path << std::endl;

    // Scale testing!
    if(m_currentFont == "vga16x32.bmp")
    {

        std::cout << "*** scaled font loading!"  << std::endl;

        /*
        // Check for new scaled surface,, load it then convert it to font surface!
        if(surfaceExists(SURFACE_FONT_SCALED))
        {
            delSurface(SURFACE_FONT_SCALED);
        }

        // Load the Font into the Surface.
        surface_ptr fontSurfaceScaled(
            new Surfaces(
                SDL_LoadBMP(path.c_str())
            )
        );

        if(!fontSurfaceScaled->exists())
        {
            SDL_Log("loadBitmapImage() fontSurfaceScaled font: %s", path.c_str());
            SDL_Delay(1500);
            assert(false);
        }

        // Convert and Add to Surface Manager.
        if(!fontSurfaceScaled->convert())
        {
            SDL_Log("Error Converting fontSurfaceScaled!");
            return false;
        }


        // Loading Bitmap first time, or reloading,  If already Exists remove it.
        if(surfaceExists(SURFACE_FONT))
        {
            delSurface(SURFACE_FONT);
        }

        // Testing hard code original size.
        path = getFontPath();
        path.append("vga8x16.bmp");

        // Load the Font into the Surface.
        surface_ptr fontSurface(
            new Surfaces(
                SDL_LoadBMP(path.c_str())
            )
        );
        */

        if(surfaceExists(SURFACE_FONT))
        {
            delSurface(SURFACE_FONT);
        }

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
            SDL_Log("Error Converting fontSurface!");
            return false;
        }

        /*
                SDL_Rect src, dest;
                src.h = fontSurfaceScaled->getSurface()->h;
                src.w = fontSurfaceScaled->getSurface()->w;
                src.x = 0;
                src.y = 0;

                dest.h = fontSurface->getSurface()->h;
                dest.w = fontSurface->getSurface()->w;
                dest.x = 0;
                dest.y = 0;*/


        // Clear for new copy.
        //fontSurface->clear();

        // SCale the surface down
        //scaleDownSurface(fontSurfaceScaled->getSurface(), 1);


        // not blit scale frontsurface scaled to useable font surface.
        /*
        SDL_BlitScaled(
            fontSurfaceScaled->getSurface(),
            nullptr,
            fontSurface->getSurface(),
            &dest
        );*/

        addSurface(SURFACE_FONT, fontSurface);

    }
    else
    {


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

        // Setup alpha Blending.
        SDL_SetSurfaceBlendMode(fontSurface->getSurface(), SDL_BLENDMODE_BLEND);

        // Enable RLE acceleration
        SDL_SetSurfaceRLE(fontSurface->getSurface(), 1);

        addSurface(SURFACE_FONT, fontSurface);
    }

    return true;
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
                    SDL_Log("%s: Error Setting Blend on Texture - %s", SDL_GetError());
                }

                //SDL_SetTextureAlphaMod( texture->getTexture(), 255 );
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
                    SDL_Log("%s: Error Setting Blend on Texture - %s", SDL_GetError());
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
    auto it = m_surfaceList.find(value);
    if(it != m_surfaceList.end())
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
                            m_characterWidth, m_characterHeight, m_surfaceBits,
                            m_redMask, m_greenMask, m_blueMask, m_alphaMask
                        )
                    )
                );

                // Setup alpha Blending.
                SDL_SetSurfaceBlendMode(surface->getSurface(), SDL_BLENDMODE_BLEND);

                // Enable RLE acceleration
                SDL_SetSurfaceRLE(surface->getSurface(), 1);

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
                                window->getWindowWidth(), window->getWindowHeight(), m_surfaceBits,
                                m_redMask, m_greenMask, m_blueMask, m_alphaMask
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
                            screenWidth, screenHeight, m_surfaceBits,
                            m_redMask, m_greenMask, m_blueMask, m_alphaMask
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
                            m_surfaceList[SURFACE_MAIN_SCREEN]->getSurface()->w, m_characterHeight, m_surfaceBits,
                            m_redMask, m_greenMask, m_blueMask, m_alphaMask
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
    if(SDL_MUSTLOCK(m_surfaceList[surfaceType]->getSurface()))
    {
        if(SDL_LockSurface(m_surfaceList[surfaceType]->getSurface()) < 0)
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
    if(SDL_MUSTLOCK(m_surfaceList[surfaceType]->getSurface()))
    {
        SDL_UnlockSurface(m_surfaceList[surfaceType]->getSurface());
    }
}

/**
 * @brief Fill the Surface with the Current Color Attributes.
 */
void SurfaceManager::fillSurfaceColor(int surfaceType, SDL_Rect *rect, SDL_Color *color)
{

    SDL_LockSurface(m_surfaceList[surfaceType]->getSurface());

    // Fill screen with current RGB Background colors.
    if(SDL_FillRect(
                m_surfaceList[surfaceType]->getSurface(),
                rect,
                SDL_MapRGBA(
                    m_surfaceList[surfaceType]->getSurface()->format,
                    color->r,
                    color->g,
                    color->b,
                    255
                )) < 0)
    {
        SDL_Log("fillSurfaceCurrentColor(): %s", SDL_GetError());
        assert(false);
    }

    SDL_UnlockSurface(m_surfaceList[surfaceType]->getSurface());
}
