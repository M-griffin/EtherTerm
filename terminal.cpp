
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "terminal.hpp"
#include "sequenceParser.hpp"
#include "mainMenuState.hpp"
#include "inputHandler.hpp"

#ifdef TARGET_OS_MAC
#include <SDL2/SDL.h>
#ifdef _DEBBUG
#include <SDL2/SDL_ttf.h>
#endif
#elif _WIN32
#include <SDL.h>
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

Terminal* Terminal::m_globalInstance = nullptr;

Terminal::Terminal() :

    BLACK( {   0,   0,   0,   0 }),
       BLUE( {   0,   0, 171,   0 }),
       GREEN( {   0, 171,   0,   0 }),
       CYAN( {   0, 171, 171,   0 }),
       RED( { 171,   0,   0,   0 }),
       MAGENTA( { 171,   0, 171,   0 }),
       BROWN( { 171,  87,   0,   0 }),
       GREY( { 171, 171, 171,   0 }),
       DARK_GREY( {  87,  87,  87,   0 }),
       LIGHT_BLUE( {  87,  87, 255,   0 }),
       LIGHT_GREEN( {  87, 255,  87,   0 }),
       LIGHT_CYAN( {  87, 255, 255,   0 }),
       LIGHT_RED( { 255,  87,  87,   0 }),
       LIGHT_MAGENTA( { 255,  87, 255,   0 }),
       YELLOW( { 255, 255,  87,   0 }),
       WHITE( { 255, 255, 255,   0 }),
       m_globalWindow(nullptr),
       m_globalRenderer(nullptr),
       m_tmpSurface(nullptr),       // Char Cell
       m_cursorOnSurface(nullptr),  // Char Cell for the cursor.
       m_cursorOffSurface(nullptr), // Char Cell for the cursor.
       m_screenSurface(nullptr),    // Internal Screen Buffer
       m_bottomSurface(nullptr),    // Last Line of Screen
       m_cachedSurface(nullptr),    // Cached Font CharacterSet
       m_globalTexture(nullptr),    // Texture for User Screen
       m_selectionTexture(nullptr), // Texture for Copy/Paste Selection
       m_globalTermStateMachine(nullptr),
       m_surfaceWidth(0),
       m_surfaceHeight(0),
       m_windowWidth(0),
       m_windowHeight(0),
       m_surfaceBits(32),
       m_characterWidth(0),
       m_characterHeight(0),
       m_cursorXPosition(0),
       m_cursorYPosition(0),
       m_isChangingState(false),
       m_isRunning(false),
       m_isUTF8Output(false) // Default to No, if Yes Block Font Switching!

{
    // Initalize Color Masks.
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    m_redMask   = 0xff000000;
    m_greenMask = 0x00ff0000;
    m_blueMask  = 0x0000ff00;
    m_alphaMask = 0x000000ff;
#else
    m_redMask   = 0x000000ff;
    m_greenMask = 0x0000ff00;
    m_blueMask  = 0x00ff0000;
    m_alphaMask = 0xff000000;
#endif

    // Setup Default Terminal Colors
    m_currentFGColor = GREY;
    m_currentBGColor = BLACK;

    // Not yet setup! read from XML file!!
    //globalFontFiles.push_back("vga8x16.bmp");
    //globalFontFiles.push_back("topaz2-8x16.bmp");
    //globalFontFiles.push_back("topazPlus-8x16.bmp");

    // Set defaults;
    clearSystemConnection();
    m_currentFont = "vga8x16.bmp";
    m_previousFont = "";

    // Setup Surfaces in List
    // Not in use yet, just subs for now.
    m_surfaceList.reserve(6);
    m_surfaceList.resize(6);

}

Terminal::~Terminal()
{
    // we must clean up after ourselves to prevent memory leaks
    m_globalRenderer = nullptr;
    m_globalWindow = nullptr;
    std::vector<SDL_Surface *>().swap(m_surfaceList);
    std::cout << "*** Term Released" << std::endl;
}


/*
 * Destroy and Recreate the Window
 * SDL is broken in OSX so We need to to switch
 * The resolution and Window Sizes properly.
 */
void Terminal::restartWindowSize(bool fullScreen)
{
    if(m_globalWindow)
    {
        //std::cout << "Destroy SDL Window" << std::endl;
        SDL_DestroyWindow(m_globalWindow);
        m_globalWindow = nullptr;
    }

    // init the window
    if(fullScreen)
    {
        m_globalWindow =
            SDL_CreateWindow(m_windowTitle.c_str(), SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, m_windowWidth, m_windowHeight,
#ifndef _WIN32               // Linux / OSX FullScreen Desktop Rez Only!
                             SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
#else                        // Windows do true Fullscreen.
                             //SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
                             SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
#endif
        if(!m_globalWindow) // window init success
        {
            SDL_Log("Terminal::restartWindowSize() SDL_CreateWindow globalWindow: %s",
                    SDL_GetError());
            assert(m_globalWindow);
        }
    }
    else
    {
        m_globalWindow =
            SDL_CreateWindow(m_windowTitle.c_str(), SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, m_windowWidth, m_windowHeight,
                             SDL_WINDOW_SHOWN);
        if(!m_globalWindow) // window init success
        {
            SDL_Log("Terminal::restartWindowSize() SDL_CreateWindow globalWindow: %s",
                    SDL_GetError());
            assert(m_globalWindow);
        }
    }
}

/*
 * Destroy and Recreate the Terminal
 * Mode is to toggle Texture Filtering On or Off.
 * SDL is broken in OSX so We need to to switch
 * The resolution and Window Sizes properly.
 */
void Terminal::restartWindowRenderer(std::string mode)
{


// Test for Windows if we can just recreate the texture.
#ifndef _WIN32

    if(m_globalRenderer)
    {
        //std::cout << "Destroy SDL Terminal" << std::endl;
        SDL_DestroyRenderer(m_globalRenderer);
        m_globalRenderer = nullptr;
    }

    m_globalRenderer =
        SDL_CreateRenderer(m_globalWindow, -1, SDL_RENDERER_ACCELERATED |
                           SDL_RENDERER_TARGETTEXTURE);

    if(!m_globalRenderer)
    {
        SDL_Log("restartWindowRenderer globalRenderer: %s",
                SDL_GetError());
        assert(m_globalRenderer);
    }

#endif

    // First Clear the Rexture so it will get recreated with new HINT
    if(m_globalTexture)
    {
        //std::cout << "SDL_DestroyTexture globalTexture" << std::endl;
        SDL_DestroyTexture(m_globalTexture);
        m_globalTexture = nullptr;
    }
    /*
     * 0 = disable vsync
     * 1 = enable vsync
     */
    if(!SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0"))
    {
        SDL_Log("restartWindowRenderer SDL_SetHint SDL_HINT_RENDER_VSYNC: %s",
                SDL_GetError());
    }
    /*
     * 0 or nearest = nearest pixel sampling
     * 1 or linear  = linear filtering       (supported by OpenGL and Direct3D)
     * 2 or best    = anisotropic filtering  (supported by Direct3D)
     */
#ifndef _WIN32
    // Linux / OSX we have to use texture filter on Full screen
    // Sad but true, True Full Screen is no go on these platforms!
    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, mode.c_str()))
#else
    // Windows we want full quality Full Screen!  :)
    //if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, mode.c_str()))
#endif

    {
        SDL_Log("restartWindowRenderer SDL_SetHint SDL_HINT_RENDER_SCALE_QUALITY: %s",
                SDL_GetError());
    }
    /*
     * 0 disable 3D acceleration
     * 1 enable 3D acceleration, using the default renderer
     * X enable 3D acceleration, using X where X is one of the valid
     *  rendering drivers. (e.g. "direct3d", "opengl", etc.)
     */
    if(!SDL_SetHint(SDL_HINT_FRAMEBUFFER_ACCELERATION, "1"))
    {
        SDL_Log("restartWindowRenderer SDL_SetHint SDL_HINT_RENDER_SCALE_QUALITY: %s",
                SDL_GetError());
    }

    // Clear and Refresh the Terminal
    if(SDL_SetRenderDrawColor(m_globalRenderer, 0x00, 0x00, 0x00, 0xFF) < 0)
    {
        SDL_Log("restartWindowRenderer SDL_SetRenderDrawColor globalRenderer: %s",
                SDL_GetError());
        return;
    }
    if(SDL_SetRenderTarget(m_globalRenderer, nullptr) < 0)
    {
        SDL_Log("restartWindowRenderer SDL_SetRenderTarget NULL: %s",
                SDL_GetError());
    }

    //int screenWidth, screenHeight;
    //SDL_GetRendererOutputSize(globalRenderer,&screenWidth,&screenHeight);
    //SDL_RenderSetLogicalSize(globalRenderer,screenWidth,screenHeight);

    if(SDL_RenderClear(m_globalRenderer) < 0)
    {
        SDL_Log("restartWindowRenderer() SDL_RenderClear globalRenderer: %s",
                SDL_GetError());
    }
    // Recreate the texture surface
    renderScreen();
}

/*
 * Terminal Startup and Init.
 */
bool Terminal::init(const char* title,
                    int swidth, int sheight,
                    int wwidth, int wheight,
                    int fwidth, int fheight)
{
    int flags = 0;
    // store the Term width and height
    m_windowTitle     = title;
    m_surfaceWidth    = swidth;
    m_surfaceHeight   = sheight;
    m_windowWidth     = wwidth;
    m_windowHeight    = wheight;
    m_characterWidth  = fwidth;
    m_characterHeight = fheight;

    // Resizing offset pixel in texture resize, shaded blocks are uneven!
    //flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
    flags = SDL_WINDOW_SHOWN; //  | SDL_WINDOW_ALLOW_HIGHDPI;

    // attempt to initialise SDL
    //if(SDL_Init(SDL_INIT_EVERYTHING) == 0)
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Terminal::init() SDL_Init: SDL_INIT_VIDEO %s",
                SDL_GetError());
        return false;
    }

    std::cout << "SDL init success" << std::endl;

    /* initialize SDL_net */
    if(SDLNet_Init() < 0)
    {
        SDL_Log(
            "Terminal::init() SDLNet_Init: %s",
            SDL_GetError());
        return false;
    }

    std::cout << "SDLNet_Init success" << std::endl;

#ifdef _DEBBUG
    //Also need to init SDL_ttf
    if(TTF_Init() != 0)
    {
        SDL_Log(
            "Terminal::init() SDL_TTF Init: %s", SDL_GetError());
        return false;
    }

    std::cout << "SDL_TTF Init success" << std::endl;
#endif

    /*
     * 0 = disable vsync
     * 1 = enable vsync
     */
    if(!SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0"))
    {
        SDL_Log("Terminal::init() SDL_SetHint SDL_HINT_RENDER_VSYNC: %s",
                SDL_GetError());

        // Not a show stopper, continue on!
    }
    /*
     * 0 or nearest = nearest pixel sampling
     * 1 or linear  = linear filtering       (supported by OpenGL and Direct3D)
     * 2 or best    = anisotropic filtering  (supported by Direct3D)
     */
    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
    {
        SDL_Log("Terminal::init() SDL_SetHint SDL_HINT_RENDER_SCALE_QUALITY: %s",
                SDL_GetError());

        // Not a show stopper, continue on!
    }
    /*
     * 0 disable 3D acceleration
     * 1 enable 3D acceleration, using the default renderer
     * X enable 3D acceleration, using X where X is one of the valid
     *  rendering drivers. (e.g. "direct3d", "opengl", etc.)
     */
    if(!SDL_SetHint(SDL_HINT_FRAMEBUFFER_ACCELERATION, "1"))
    {
        SDL_Log("Terminal::init() SDL_SetHint SDL_HINT_RENDER_SCALE_QUALITY: %s",
                SDL_GetError());

        // Not a show stopper, continue on! but maybe!!
    }

    // init the window
    m_globalWindow =
        SDL_CreateWindow(m_windowTitle.c_str(), SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, m_windowWidth, m_windowHeight, flags);
    if(!m_globalWindow) // window init success
    {
        SDL_Log("Terminal::init() SDL_CreateWindow globalWindow: %s",
                SDL_GetError());
        return false;
    }

    std::cout << "SDL_Window creation success" << std::endl;

    m_globalRenderer =
        SDL_CreateRenderer(m_globalWindow, -1, SDL_RENDERER_ACCELERATED |
                           SDL_RENDERER_TARGETTEXTURE);
    if(!m_globalRenderer) // renderer init success
    {
        SDL_Log("Terminal::init() SDL_CreateRenderer globalRenderer: %s",
                SDL_GetError());
        return false;
    }

    std::cout << "SDL_Renderer creation success" << std::endl;

    if(SDL_SetRenderDrawColor(m_globalRenderer, 0x00, 0x00, 0x00, 0xFF) < 0)
    {
        SDL_Log("Terminal::init() SDL_SetRenderDrawColor globalRenderer: %s",
                SDL_GetError());
        return false;
    }

    // add some sound effects - TODO move to better place
    /*
    TheSoundManager::Instance()->load("assets/ElectroRock.ogg", "music1", SOUND_MUSIC);
    TheSoundManager::Instance()->load("assets/boom.wav", "explode", SOUND_SFX);
    TheSoundManager::Instance()->playMusic("music1", -1);
    */

    m_isRunning = true; // everything inited successfully, start the main loop
    return true;
}

void Terminal::update()
{
    m_globalTermStateMachine->update();
}

void Terminal::clean()
{
    // Call from a private member to free properly.
    freeSurfaceTextures();

    // Shutdown SDL_Sockets.
    std::cout << "Shutting Down SDL_Net" << std::endl;
    SDLNet_Quit();

    //globalFontFiles.clear();
    //vector<std::string>().swap(globalFontFiles);

    std::cout << "Shutting Down TermStateMachine" << std::endl;
    if(m_globalTermStateMachine)
    {
        m_globalTermStateMachine->clean();
        delete m_globalTermStateMachine;
    }

#ifdef _DEBBUG
    std::cout << "TTF_Quit" << std::endl;
    TTF_Quit();
#endif

    std::cout << "SDL_Quit" << std::endl;
    SDL_Quit();
}


/*
 * Free all Surfaces and Textures
 * Seems we have to do this from a private member
 * Otherwise the program loses the pointers.
 */
void Terminal::freeSurfaceTextures()
{
    // Clear Cached Surface.
    std::cout << std::endl << "*** Releasing Surfaces and Textures"
    << std::endl;


    //Free texture if it exists
    if(m_globalTexture)
    {
        std::cout << "SDL_DestroyTexture globalTexture" << std::endl;
        SDL_DestroyTexture(m_globalTexture);
        m_globalTexture = nullptr;
    }
    if(m_selectionTexture)
    {
        std::cout << "SDL_DestroyTexture selectionTexture" << std::endl;
        SDL_DestroyTexture(m_selectionTexture);
        m_selectionTexture = nullptr;
    }

    if(m_globalWindow)
    {
        std::cout << "Destroy SDL Window" << std::endl;
        SDL_DestroyWindow(m_globalWindow);
        m_globalWindow = nullptr;
    }
    if(m_globalRenderer)
    {
        std::cout << "Destroy SDL Terminal" << std::endl;
        SDL_DestroyRenderer(m_globalRenderer);
        m_globalRenderer = nullptr;
    }

    if(m_cachedSurface)
    {
        std::cout << "SDL_FreeSurface chachedSurface" << std::endl;
        SDL_FreeSurface(m_cachedSurface);     // Bitmap    -> Fonts
        m_cachedSurface = nullptr;
    }
    if(m_screenSurface)
    {
        std::cout << "SDL_FreeSurface screenSurface" << std::endl;
        SDL_FreeSurface(m_screenSurface);      // Screen    -> Texture
        m_screenSurface = nullptr;
    }
    if(m_tmpSurface)
    {
        std::cout << "SDL_FreeSurface tmpSurface" << std::endl;
        SDL_FreeSurface(m_tmpSurface);         // Char Cell -> Screen
        m_tmpSurface = nullptr;
    }
    if(m_cursorOffSurface)
    {
        std::cout << "SDL_FreeSurface cursorOffSurface" << std::endl;
        SDL_FreeSurface(m_cursorOffSurface);   // Cursor Off Character Cell
        m_cursorOffSurface = nullptr;
    }
    if(m_bottomSurface)
    {
        std::cout << "SDL_FreeSurface bottomSurface" << std::endl;
        SDL_FreeSurface(m_bottomSurface);      // Bottom Line of Scrolling Region.
        m_bottomSurface = nullptr;
    }
}

/*
 * Store Images later in a zip file.
 * Not in use at this time!
 *
 * Not used at this time
 *
bool Terminal::loadBitmapImageFromPak()
{
    // Require minizip lib.
    // useful things from zlib/unzip.h
    unzFile            zip;
    unz_file_info    info;
    Uint8            *buffer;
    int                result;

    SDL_RWops        *rw;

    zip = unzOpen( "data.zip" );
    if ( zip == NULL ) {
        printf("Error opening unzOpen('zipfile.zip')\n");
        return false;
    }

    result = unzLocateFile( zip, "ImageInZipfile.bmp", 0 );
    if ( result != UNZ_OK ) {
        printf("unzLocateFile(\"ImageInZipfile.bmp\") != UNZ_OK(%d)\n", result);
    }

    result = unzGetCurrentFileInfo( zip, &info, NULL, 0, NULL, 0, NULL, 0 );
    if ( result != UNZ_OK ) {
        printf("unzGetCurrentFileInfo() != UNZ_OK (%d)\n", result);
        return false;
    }

    result = unzOpenCurrentFile( zip );
    if ( result != UNZ_OK ) {
        printf("unzOpenCurrentFile() != UNZ_OK (%d)\n", result);
        return false;
    }

    buffer = (Uint8*)malloc(info.uncompressed_size);
    result = unzReadCurrentFile( zip, buffer, info.uncompressed_size );
    if ( result != (signed)info.uncompressed_size ) {
        //printf("unzReadCurrentFile() Error (%d != %d)\n",
            result, info.uncompressed_size);
        return false;
    }

    rw = SDL_RWFromMem(buffer, info.uncompressed_size);
    if(!rw) {
        printf("Error with SDL_RWFromMem: %s\n", SDL_GetError() );
        return false;
    }

    // The function that does the loading doesn't change at all
    chachedSurface = SDL_LoadBMP_RW(rw, 0);
    if (!chachedSurface)
    {
        std::cout << "Unable to SDL_LoadBMP: " << SDL_GetError() << std::endl;
        return false;
    }

    previousFont = currentFont;
    return chachedSurface != NULL;

    // Clean up after ourselves
    free(buffer);
    SDL_FreeRW(rw);
    unzClose(zip);

    return m_cachedSurface != nullptr;
}
*/

/**
* Loads the Bitmap Font Image
* Default fonts are White on Black, inital setup for 32 cols by 8 rows
* of Characters Cells at 9x16.
*/
bool Terminal::loadBitmapImage(std::string fontName)
{
    //std::cout << "LoadBitmap: " << path << std::endl;
    //Load image at specified path
    std::string path = TheTerminal::Instance()->getProgramPath();
#ifdef _WIN32
    path += "assets\\";
#else
    path += "assets/";
#endif
    path += fontName;
    m_cachedSurface = SDL_LoadBMP(path.c_str());
    if(!m_cachedSurface)
    {
        SDL_Log("loadBitmapImage() SDL_LoadBMP font: %s",
                m_currentFont.c_str());
        return false;
    }

    // Redraw Cursor with new font!
    // When no data received, this is when we want to show the cursor!
    // Setup cursor in current x/y position Cursor.
    if(TheSequenceParser::Instance()->isCursorActive())
    {
        setupCursorChar();
        renderCursorOnScreen();
        drawTextureScreen();
    }

    m_previousFont = m_currentFont; // Current to Previous
    m_currentFont  = fontName;    // New to Current.
    return m_cachedSurface != nullptr;
}


/*
 * Creates a Texture from the Surfaces Passed.
 * And Sets the default Blend Mode.
 */
void Terminal::createTexture(int textureType, SDL_Surface *surface)
{
    switch(textureType)
    {
        case GLOBAL_TEXTURE:
            if(m_globalTexture)
            {
                SDL_DestroyTexture(m_globalTexture);
            }
            m_globalTexture =
                SDL_CreateTexture(m_globalRenderer,
                                  SDL_PIXELFORMAT_ARGB8888,
                                  SDL_TEXTUREACCESS_STREAMING,
                                  surface->w, surface->h);
            if(m_globalTexture)
            {
                if(SDL_SetTextureBlendMode(
                            m_globalTexture,
                            SDL_BLENDMODE_BLEND) < 0)
                {
                    SDL_Log("%s: Error Setting Blend on Texture - %s",
                            SDL_GetError());
                }
            }
            break;

        case SELECTION_TEXTURE:
            if(m_selectionTexture)
            {
                SDL_DestroyTexture(m_selectionTexture);
            }
            m_selectionTexture =
                SDL_CreateTexture(m_globalRenderer,
                                  SDL_PIXELFORMAT_ARGB8888,
                                  SDL_TEXTUREACCESS_TARGET,
                                  surface->w, surface->h);
            if(m_selectionTexture)
            {
                if(SDL_SetTextureBlendMode(
                            m_selectionTexture,
                            SDL_BLENDMODE_ADD) < 0)
                {
                    SDL_Log("%s: Error Setting Blend on Texture - %s",
                            SDL_GetError());
                }
            }
            break;

        default:
            break;
    }
}

/*
 * Fill a Surfaces with Black or Clears.
 */
void Terminal::fillSurface(SDL_Surface *surface)
{
    SDL_FillRect(surface, nullptr,
                 SDL_MapRGB(surface->format, 0, 0, 0));
}

/*
 * Converts Surfaces to Standard Pixel Format
 */
void Terminal::convertSurface(int surfaceType)
{
    // Run Through Surface Alias and Create Surface
    switch(surfaceType)
    {
        case TEMP_SURFACE:
            if(m_tmpSurface)
            {
                m_tmpSurface = SDL_ConvertSurfaceFormat(
                                   m_tmpSurface,
                                   SDL_PIXELFORMAT_ARGB8888,
                                   0);
            }
            if(!m_tmpSurface)
            {
                SDL_Log("Unable to Convert tmpSurface");
                SDL_FreeSurface(m_tmpSurface);
            }
            break;

        case CURSOR_ON_SURFACE:
            if(m_cursorOnSurface)
            {
                m_cursorOnSurface = SDL_ConvertSurfaceFormat(
                                        m_cursorOnSurface,
                                        SDL_PIXELFORMAT_ARGB8888,
                                        0);
            }
            if(!m_cursorOnSurface)
            {
                SDL_Log("Unable to Convert cursorOnSurface");
                SDL_FreeSurface(m_cursorOnSurface);
            }
            break;

        case CURSOR_OFF_SURFACE:
            if(m_cursorOffSurface)
            {
                m_cursorOffSurface = SDL_ConvertSurfaceFormat(
                                         m_cursorOffSurface,
                                         SDL_PIXELFORMAT_ARGB8888,
                                         0);
            }
            if(!m_cursorOffSurface)
            {
                SDL_Log("Unable to Convert cursorOffSurface");
                SDL_FreeSurface(m_cursorOffSurface);
            }
            break;

        case SCREEN_SURFACE:
            if(m_screenSurface)
            {
                m_screenSurface = SDL_ConvertSurfaceFormat(
                                      m_screenSurface,
                                      SDL_PIXELFORMAT_ARGB8888,
                                      0);
            }
            if(!m_screenSurface)
            {
                SDL_Log("Unable to Convert screenSurface");
                SDL_FreeSurface(m_screenSurface);
            }
            break;

        case BOTTOM_SURFACE:
            if(m_bottomSurface)
            {
                m_bottomSurface = SDL_ConvertSurfaceFormat(
                                      m_bottomSurface,
                                      SDL_PIXELFORMAT_ARGB8888,
                                      0);
            }
            if(!m_bottomSurface)
            {
                SDL_Log("Unable to Convert bottomSurface");
                SDL_FreeSurface(m_bottomSurface);
            }
            break;

        case CACHED_SURFACE:
            if(m_cachedSurface)
            {
                m_cachedSurface = SDL_ConvertSurfaceFormat(
                                      m_cachedSurface,
                                      SDL_PIXELFORMAT_ARGB8888,
                                      0);
            }
            if(!m_cachedSurface)
            {
                SDL_Log("Unable to Convert cachedSurface");
                SDL_FreeSurface(m_cachedSurface);
            }
            break;

        default:
            break;
    }
}

/*
 * Creates SDL Surfaces for RGB
 */
void Terminal::createSurface(int surfaceType)
{
    // Run Through Surface Alias and create surface.
    switch(surfaceType)
    {
        case TEMP_SURFACE:
            if(m_tmpSurface)
            {
                SDL_FreeSurface(m_tmpSurface);
            }
            m_tmpSurface = SDL_CreateRGBSurface(
                               SDL_SWSURFACE,
                               m_characterWidth, m_characterHeight, m_surfaceBits,
                               m_redMask, m_greenMask, m_blueMask, m_alphaMask);
            if(m_tmpSurface)
            {
                convertSurface(surfaceType);
            }
            break;

        case CURSOR_ON_SURFACE:
            if(m_cursorOnSurface)
            {
                SDL_FreeSurface(m_cursorOnSurface);
            }
            m_cursorOnSurface = SDL_CreateRGBSurface(
                                    SDL_SWSURFACE,
                                    m_characterWidth, m_characterHeight, m_surfaceBits,
                                    m_redMask, m_greenMask, m_blueMask, m_alphaMask);
            if(m_cursorOnSurface)
            {
                convertSurface(surfaceType);
            }
            break;

        case CURSOR_OFF_SURFACE:
            if(m_cursorOffSurface)
            {
                SDL_FreeSurface(m_cursorOffSurface);
            }
            m_cursorOffSurface = SDL_CreateRGBSurface(
                                     SDL_SWSURFACE,
                                     m_characterWidth, m_characterHeight, m_surfaceBits,
                                     m_redMask, m_greenMask, m_blueMask, m_alphaMask);
            if(m_cursorOffSurface)
            {
                convertSurface(surfaceType);
            }
            break;

        case SCREEN_SURFACE:
            if(m_screenSurface)
            {
                SDL_FreeSurface(m_screenSurface);
            }
            m_screenSurface = SDL_CreateRGBSurface(
                                  SDL_SWSURFACE,
                                  m_surfaceWidth, m_surfaceHeight, m_surfaceBits,
                                  m_redMask, m_greenMask, m_blueMask, m_alphaMask);
            if(m_screenSurface)
            {
                convertSurface(surfaceType);
            }
            break;

        case BOTTOM_SURFACE:
            if(m_bottomSurface)
            {
                SDL_FreeSurface(m_bottomSurface);
            }
            m_bottomSurface = SDL_CreateRGBSurface(
                                  SDL_SWSURFACE,
                                  m_screenSurface->w, m_characterHeight, m_surfaceBits,
                                  m_redMask, m_greenMask, m_blueMask, m_alphaMask);
            if(m_bottomSurface)
            {
                convertSurface(surfaceType);
            }
            break;

        case CACHED_SURFACE:
            if(m_cachedSurface)
            {
                SDL_FreeSurface(m_cachedSurface);
            }
            m_cachedSurface = SDL_CreateRGBSurface(
                                  SDL_SWSURFACE,
                                  m_characterWidth, m_characterHeight, m_surfaceBits,
                                  m_redMask, m_greenMask, m_blueMask, m_alphaMask);
            if(m_cachedSurface)
            {
                convertSurface(surfaceType);
            }
            break;
        default:
            break;
    }
}

/**
 * Initialize the Render, Loads the Fonts and Create the Surfaces.
 */
bool Terminal::initSurfaceTextures()
{
    // Setup the Surface we will plot each character cell to.
    createSurface(SCREEN_SURFACE);
    if(!m_screenSurface)
    {
        SDL_Log("initSurfaceTextures() SDL_CreateRGBSurface screenSurface: %s",
                SDL_GetError());
        return false;
    }

    // Fill the Surface with Black to initalize it.
    fillSurface(m_screenSurface);

    // Setup Temp Surface that the size of each Character Cell.
    createSurface(TEMP_SURFACE);
    if(!m_tmpSurface)
    {
        SDL_Log("initSurfaceTextures() SDL_CreateRGBSurface tmpSurface: %s",
                SDL_GetError());
        return false;
    }

    // Setup Cursor Surface that the size of a Character Cell.
    // Holds the original Char when we overwrite it with an blinking cursor.
    createSurface(CURSOR_ON_SURFACE);
    if(!m_cursorOnSurface)
    {
        SDL_Log("initSurfaceTextures() SDL_CreateRGBSurface cursorOnSurface: %s",
                SDL_GetError());
        return false;
    }

    // Setup Cursor Surface that the size of a Character Cell.
    // Holds the original Char when we overwrite it with an blinking cursor.
    createSurface(CURSOR_OFF_SURFACE);
    if(!m_cursorOffSurface)
    {
        SDL_Log("initSurfaceTextures() SDL_CreateRGBSurface cursorOffSurface: %s",
                SDL_GetError());
        return false;
    }

    // Setup the Surface we will plot each character cell to.
    createSurface(BOTTOM_SURFACE);
    if(!m_bottomSurface)
    {
        SDL_Log("initSurfaceTextures() SDL_CreateRGBSurface bottomSurface: %s",
                SDL_GetError());
        return false;
    }

    // Fill the Surface with Black to initalize it.
    fillSurface(m_bottomSurface);

    // start the menu state
    m_globalTermStateMachine = new TermStateMachine();
    if(!m_globalTermStateMachine)
    {
        SDL_Log("initSurfaceTextures() globalTermStateMachine TermStateMachine");
        return false;
    }

    // Load into Main Menu System (Dialing Directory)
    m_globalTermStateMachine->changeState(new MainMenuState());
    return true;
}


/*
 * Once we have a screen selection, we will now pull the
 * Rect radius and grab all mirrowing text from the screen
 * buffer and copy it to the clip board. Copy/Paste.
 */
void Terminal::pullSelectionBuffer(int x, int y)
{
    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(m_globalRenderer, &screenWidth, &screenHeight);

    // We need to Translate the Screen Width vs Rows and Width to
    // get actual the grid size of the Characters to snap everything correctly.
    double charWidth, charHeight;
    double value;

    value = (double)screenHeight / 25.0;
    charHeight = value; //round(abs(value));
    value = (double)screenWidth / 80.0;
    charWidth = value; //round(abs(value));

    // First we need to convert the current Screen size to 640x400
    // So we can calcuate the actual pixel size of each resized character cell.
    SDL_Rect rect;

    // We clip off botom 80, so that we get proper 8x16
    // Display without Extra pixel borders around the screen,
    // Texture Filter results in Pixel Bleeding.

    int sourceX = TheInputHandler::Instance()->getMouseSourceXPosition();
    int sourceY = TheInputHandler::Instance()->getMouseSourceYPosition();

    std::cout << sourceX << ":" << x << ", ";
    std::cout << sourceY << ":" << y << std::endl;

    // This is the source position that we Xy     = Source
    //                                      []
    // are starting our rect box at.          xy  = Motion

    // Next We'll need to snap this to the nearest char height and width.
    // Moving up on the top most region, and down on the bottom most region.

    // First determine the smallest of source and current plot positions
    // We want to make the Lowest Values our starting point so we either
    // use source or motions depending where the mouse is for top left.

    // This is the Top Region Snap to nearest Char Width/Height
    // This is the Top Region
    if(sourceX < x)
    {
        // TOP Stationary -> Right ->> OK!
        rect.x = floor((double)sourceX / charWidth) * charWidth;
    }
    else
    {
        // Top -> Left ->> OK!
        rect.x = floor((double)x / charWidth) * charWidth;
    }

    if(sourceY < y)
    {
        // Top Stationary ->> OK!
        rect.y = floor((double)sourceY / charHeight) * charHeight;
    }
    else
    {
        // Top -> Up ->> OK!
        rect.y = floor((double)y / charHeight) * charHeight;
    }

    // Width and height are calcuated by the different from motion to source
    // Well need to round these values to the width and height of a character!
    // Also need to swap source for the lowest value so we can select both
    // above and behind the starting points.

    // This is the bottom Region
    if(sourceX < x)
    {
        // Bottom Width RIGHT ->> OK!
        rect.w = charWidth + (floor((double)x / charWidth) * charWidth) -  rect.x ;
    }
    else
    {
        // Bottom Stationary ->> OK!
        rect.w = charWidth + (floor((double)sourceX / charWidth) * charWidth) - rect.x; // back
    }
    if(sourceY < y)
    {
        // Bottom -> Down ->> OK!
        //rect.h = charHeight +
        rect.h = charHeight + (floor((double)y / charHeight) * charHeight) - rect.y;
    }
    else
    {
        // Bottom -> Stationary ->> OK!
        rect.h = charHeight + (floor((double)sourceY / charHeight) * charHeight) - rect.y; // back
    }

    // If were inbetween lines, Height = 0, then add char Height to the bottom
    // So we always have a row!
    if(rect.h == 0)
        rect.h += round(charHeight);

    if(rect.w == 0)
        rect.w += round(charWidth);

    // Now that we have the excat coordinates of the selected text.
    // We need to translate this to the screenbuffer positions
    // So that we can grab the text that was selected on the screen.

    // Get Starting Position
    int startColumn = rect.x / charWidth;
    int startRow    = rect.y / charHeight;
    // Now figure out how many character per row
    int length      = rect.w / charWidth;
    // Now figure out how many rows of character to pull
    int numberRows  = rect.h / charHeight;

    // Use coords to pull screen text directly from screen buffer.
    TheSequenceParser::Instance()->bufferToClipboard(
        startColumn, startRow,
        length, numberRows);
}

/*
 * Clear the Selection Texture
 * Needed for Windows Full Screen Mode Switches, otherwise it doesn't
 * Repopulate properly.  Clear so texture is refreshed each selection
 * We don't want to do it each render casue that slows it down!
 */
void Terminal::clearSelectionTexture()
{
    SDL_DestroyTexture(m_selectionTexture);
    m_selectionTexture = nullptr;
}

/*
 * On selected mouse movement area! We want to overlay a blended texture
 * To give the affect of highlighting without having to resort to
 * grabbing screen buffer text.and redrawing it character by character
 * This should in effect give fast updates to the screen with mouse movement.
 */
void Terminal::renderSelectionScreen(int x, int y)
{
    // Redraw screen so we don't increase lighting intensitity
    // As the selection keeps redrawing!  Cool effect though!!
    SDL_Rect rectorig;
    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(m_globalRenderer,&screenWidth,&screenHeight);

    // We clip off botom 80, so that we get proper 8x16
    // Display without Extra pixel borders around the screen,
    // Texture Filter results in Pixel Bleeding.
    rectorig.w = m_surfaceWidth  - 40; // 680 - 640 = 40
    rectorig.h = m_surfaceHeight - 80; // 480 - 400 = 80
    rectorig.x = 0,
             rectorig.y = 0;

    // Destination
    m_displayRect.w = screenWidth;
    m_displayRect.h = screenHeight;
    m_displayRect.x = 0;
    m_displayRect.y = 0;

    // Redraw the screen between selection changes so we have nothing left
    // from previous coordinates.
    if(SDL_RenderCopy(m_globalRenderer, m_globalTexture, &rectorig, &m_displayRect) < 0)
    {
        SDL_Log("renderSelectionScreen() SDL_RenderCopy globalTexture: %s",
                SDL_GetError());
    }

    // Next create a texture to overlay for highlighting
    if(!m_selectionTexture)
    {
        createTexture(SELECTION_TEXTURE,m_screenSurface);
        if(!m_selectionTexture)
        {
            SDL_Log(
                "renderSelectionScreen() SDL_CreateTexture selectionTexture: %s",
                SDL_GetError());
            assert(m_selectionTexture);
        }

        // On need to set this the first time!!
        // Set Render Target to the Texture.
        if(SDL_SetRenderTarget(m_globalRenderer, m_selectionTexture) < 0)
        {
            SDL_Log("renderSelectionScreen() SDL_SetRenderTarget selectionTexture: %s",
                    SDL_GetError());
        }

        if(SDL_RenderClear(m_globalRenderer) < 0)
        {
            SDL_Log("renderSelectionScreen() SDL_RenderClear selectionTexture: %s",
                    SDL_GetError());
        }

        // Fill new texture with Dark Cyan Highlight.
        if(SDL_SetRenderDrawColor(m_globalRenderer, 0, 50 , 50, 255) < 0)
        {
            SDL_Log("renderSelectionScreen() SDL_SetRenderDrawColor selectionTexture: %s",
                    SDL_GetError());
        }

        // Can test filling to RECT to speed this up after it's working!
        if(SDL_RenderFillRect(m_globalRenderer, nullptr) < 0)
        {
            SDL_Log("renderSelectionScreen() SDL_RenderFillRect selectionTexture: %s",
                    SDL_GetError());
        }

        //Reset back to Global Render, then push update over current texture
        if(SDL_SetRenderTarget(m_globalRenderer, nullptr) < 0)
        {
            SDL_Log("renderSelectionScreen() SDL_SetRenderTarget NULL: %s",
                    SDL_GetError());
        }
    }

    // We need to Translate the Screen Width vs Rows and Width to
    // get actual the grid size of the Characters to snap everything correctly.
    double charWidth, charHeight;
    double value;

    value = (double)screenHeight / 25.0;
    charHeight = value; //round(abs(value));
    value = (double)screenWidth / 80.0;
    charWidth = value; //round(abs(value));

    // First we need to convert the current Screen size to 640x400
    // So we can calcuate the actual pixel size of each resized character cell.
    SDL_Rect rect;

    // We clip off botom 80, so that we get proper 8x16
    // Display without Extra pixel borders around the screen,
    // Texture Filter results in Pixel Bleeding.

    int sourceX = TheInputHandler::Instance()->getMouseSourceXPosition();
    int sourceY = TheInputHandler::Instance()->getMouseSourceYPosition();

    // This is the source position that we Xy     = Source
    //                                      []
    // are starting our rect box at.          xy  = Motion

    // Next We'll need to snap this to the nearest char height and width.
    // Moving up on the top most region, and down on the bottom most region.

    // First determine the smallest of source and current plot positions
    // We want to make the Lowest Values our starting point so we either
    // use source or motions depending where the mouse is for top left.

    // This is the Top Region
    if(sourceX < x)
    {
        // TOP Stationary -> Right ->> OK!
        rect.x = floor((double)sourceX / charWidth) * charWidth;
    }
    else
    {
        // Top -> Left ->> OK!
        rect.x = floor((double)x / charWidth) * charWidth;
    }

    if(sourceY < y)
    {
        // Top Stationary ->> OK!
        rect.y = floor((double)sourceY / charHeight) * charHeight;
    }
    else
    {
        // Top -> Up ->> OK!
        rect.y = floor((double)y / charHeight) * charHeight;
    }

    // Width and height are calcuated by the different from motion to source
    // Well need to round these values to the width and height of a character!
    // Also need to swap source for the lowest value so we can select both
    // above and behind the starting points.

    // This is the bottom Region
    if(sourceX < x)
    {
        // Bottom Width RIGHT ->> OK!
        rect.w = charWidth + (floor((double)x / charWidth) * charWidth) -  rect.x ;
    }
    else
    {
        // Bottom Stationary ->> OK!
        rect.w = charWidth + (floor((double)sourceX / charWidth) * charWidth) - rect.x; // back
    }
    if(sourceY < y)
    {
        // Bottom -> Down ->> OK!
        rect.h = charHeight + (floor((double)y / charHeight) * charHeight) - rect.y;
    }
    else
    {
        // Bottom -> Stationary ->> OK!
        rect.h = charHeight + (floor((double)sourceY / charHeight) * charHeight) - rect.y; // back
    }

    // If were inbetween lines, Height = 0, then add char Height to the bottom
    // So we always have a row!
    if(rect.h == 0)
        rect.h += round(charHeight);

    if(rect.w == 0)
        rect.w += round(charWidth);

    // Draw First Highlight Overlay
    if(SDL_RenderCopy(m_globalRenderer, m_selectionTexture, nullptr, &rect) < 0)
    {
        SDL_Log("renderSelectionScreen() SDL_RenderCopy selectionTexture: %s",
                SDL_GetError());
    }

    // 2 Pixel expanded shaded border (I like it!)
    rect.x -= 4;
    rect.y -= 4;
    rect.w += 8;
    rect.h += 8;

    // Draw Border Overlay
    if(SDL_RenderCopy(m_globalRenderer, m_selectionTexture, nullptr, &rect) < 0)
    {
        SDL_Log("renderSelectionScreen() SDL_RenderCopy selectionTexture (Overlay): %s",
                SDL_GetError());
    }
    SDL_RenderPresent(m_globalRenderer);
}

/*
 * Overrides the Scrolling region of the screen
 */
void Terminal::setScrollRegion(int top, int bot, int terminalHeight)
{
    // If Region is larger then the Terminal,
    // Ignore it!
    if(top == 1 && bot >= terminalHeight)
    {
        m_scrollRegionActive = false;
        return;
    }
    // 0,0 is reset.
    if(top == 0 && bot == 0)
    {
        m_scrollRegionActive = false;
        return;
    }
    else
    {
        // Make sure Bottom Region
        // Is the same as our terminal length.
        if(bot > terminalHeight)
            bot = terminalHeight;
        m_scrollRegionActive = true;
    }
    // Set Scrolling Margins
    m_topMargin = top;
    m_bottomMargin = bot;
}

/*
 * Scroll a specific set region of the screen only
 */
void Terminal::scrollRegionUp()
{
    SDL_Rect area;

    // Clear Out last line after Scrolling up.
    area.x = 0;
    area.y = m_characterHeight * (m_bottomMargin-1);
    area.w = m_surfaceWidth;
    area.h = m_characterHeight;

    int bpp = m_screenSurface->format->BytesPerPixel;
    Uint8 *pixelNewPos = (Uint8 *)m_screenSurface->pixels;
    Uint8 *pixelTopPos = (Uint8 *)m_screenSurface->pixels;

    // Move position to start
    pixelTopPos += m_screenSurface->w * (m_characterHeight * (m_topMargin -1)) * bpp;

    // Start from One line below the Top Margin.
    pixelNewPos += m_screenSurface->w * (m_characterHeight * m_topMargin) * bpp;

    if(SDL_MUSTLOCK(m_screenSurface))
    {
        if(SDL_LockSurface(m_screenSurface) < 0)
        {
            SDL_Log("scrollRegionUp() SDL_LockSurface screenSurface: %s",
                    SDL_GetError());
            return;
        }
    }

    // Move the Entire Screen Up a 1 Row of Characters.
    memmove(pixelTopPos, pixelNewPos,
            (m_screenSurface->w *
             (m_characterHeight * (m_bottomMargin - m_topMargin))) * bpp);

    if(SDL_MUSTLOCK(m_screenSurface))
        SDL_UnlockSurface(m_screenSurface);

    // Clear out very last line of surface region.
    if(SDL_FillRect(m_screenSurface, &area,
                    SDL_MapRGB(m_screenSurface->format, 0, 0, 0)) < 0)
    {
        SDL_Log("scrollRegionUp() SDL_FillRect screenSurface: %s",
                SDL_GetError());
    }

    // If first Time, Crete Texture.
    if(!m_globalTexture)
    {
        createTexture(GLOBAL_TEXTURE, m_screenSurface);
        if(!m_globalTexture)
        {
            SDL_Log("scrollRegionUp() SDL_CreateTexture globalRenderer: %s",
                    SDL_GetError());
            assert(m_globalTexture);
        }
    }
    if(SDL_UpdateTexture(m_globalTexture, nullptr,
                         m_screenSurface->pixels,
                         m_screenSurface->pitch) < 0)
    {
        SDL_Log("scrollRegionUp() SDL_UpdateTexture globalTexture: %s",
                SDL_GetError());
    }
}

/*
 * Clips out the Top Row of the Screen,
 * Then Moves Entire Screen Up one Row to Scroll it.
 */
void Terminal::scrollScreenUp()
{
    if(m_scrollRegionActive)
    {
        scrollRegionUp();
        return;
    }

    int bpp = m_screenSurface->format->BytesPerPixel;
    Uint8 *pixelNewPos = (Uint8 *)m_screenSurface->pixels;
    Uint8 *pixelOldPos = (Uint8 *)m_screenSurface->pixels;

    // Move position to start of 2nd Line,
    // Then copying greater line down to previous.
    pixelNewPos += (m_screenSurface->w * m_characterHeight) * bpp;

    if(SDL_MUSTLOCK(m_screenSurface))
    {
        if(SDL_LockSurface(m_screenSurface) < 0)
        {
            SDL_Log("scrollScreenUp() SDL_LockSurface screenSurface: %s",
                    SDL_GetError());
            return;
        }
    }

    // Move the Entire Screen Up a 1 Row of Characters.
    memmove(pixelOldPos, pixelNewPos,
            (m_screenSurface->w * (m_screenSurface->h - m_characterHeight)) * bpp);

    if(SDL_MUSTLOCK(m_screenSurface))
        SDL_UnlockSurface(m_screenSurface);

    if(!m_globalTexture)
    {
        createTexture(GLOBAL_TEXTURE, m_screenSurface);
        if(!m_globalTexture)
        {
            SDL_Log("scrollScreenUp() SDL_CreateTexture globalRenderer: %s",
                    SDL_GetError());
            assert(m_globalTexture);
        }
    }
    // Write Scrolled screen to Texture.
    if(SDL_UpdateTexture(m_globalTexture, nullptr,
                         m_screenSurface->pixels,
                         m_screenSurface->pitch) < 0)
    {
        SDL_Log("scrollScreenUp() SDL_UpdateTexture globalTexture: %s",
                SDL_GetError());
    }
}

/**
 * Clears the Surface Back Buffer, and Global Texture
 * Renders the Screen Blank for new set of data.
 *
 * Updated, Now clears the surface to the current background color
 * This is for ESC[2J proper behavior.
 */
void Terminal::clearScreenSurface()
{
    // Fill screen with current RGB Background colors.
    if(SDL_FillRect(m_screenSurface, nullptr,
                    SDL_MapRGB(m_screenSurface->format,
                               m_currentBGColor.r, m_currentBGColor.g, m_currentBGColor.b)) < 0)
    {
        SDL_Log("clearScreenSurface() SDL_FillRect screenSurface: %s",
                SDL_GetError());
    }

    if(!m_globalTexture)
    {
        createTexture(GLOBAL_TEXTURE, m_screenSurface);
        if(!m_globalTexture)
        {
            SDL_Log("clearScreenSurface() SDL_CreateTexture globalRenderer: %s",
                    SDL_GetError());
            assert(m_globalTexture);
        }
    }

    if(SDL_UpdateTexture(m_globalTexture, nullptr,
                         m_screenSurface->pixels,
                         m_screenSurface->pitch) < 0)
    {
        SDL_Log("clearScreenSurface() SDL_UpdateTexture globalRenderer: %s",
                SDL_GetError());
    }
    if(SDL_RenderClear(m_globalRenderer) < 0)
    {
        SDL_Log("clearScreenSurface() SDL_RenderClear globalRenderer: %s",
                SDL_GetError());
    }
}

/**
 * Fill entire lines with space and BLACK foreground/Background
 * Fast update to clear a row.
 */
void Terminal::renderClearLineScreen(int y, int start, int end)
{
    // Save Background Color so we can switch back to it.
    SDL_Color originalGB = m_currentBGColor;
    m_currentBGColor = BLACK;

    // Clear out entire line.
    for(int i = start; i < end; i++)
    {
        drawChar(i, y, 32); // 32 Space Character
        renderCharScreen(i, y);
    }
    m_currentBGColor = originalGB;
}

/**
 * Loops from starting y position to bottom of screen
 * Erasing all of the screen below.
 * Fill entire lines with space and BLACK foreground/Background
 * Fast update to clear a row.
 */
void Terminal::renderClearLineAboveScreen(int y, int x)
{
    std::cout << "renderClearLineAboveScreen: " << x << ";" << y << std::endl;
    // Save Background Color so we can switch back to it.
    SDL_Color originalGB = m_currentBGColor;
    m_currentBGColor = BLACK;

    int startPosition = x;

    // Clear out entire lines Up The Screen the screen.
    // Fix with term variables lateron.
    for(int ii = y; (ii+1) > 0; ii--)
    {
        for(int i = startPosition; (i+1) > 0; i--)
        {
            // x;y ' '
            drawChar(i, ii, 32); // 32 space character
            renderCharScreen(i, ii);
        }
        // Reset to starting position for following lines.
        startPosition = 79;
    }
    m_currentBGColor = originalGB;
}

/**
 * Loops from starting y position to bottom of screen
 * Erasing all of the screen below.
 * Fill entire lines with space and BLACK foreground/Background
 * Fast update to clear a row.
 */
void Terminal::renderClearLineBelowScreen(int y, int x)
{
    // Save Background Color so we can switch back to it.
    SDL_Color originalGB = m_currentBGColor;
    m_currentBGColor = BLACK;

    int startPosition = x;

    // Clear out entire lines down the screen.
    // Fix with term variables lateron.
    for(int ii = y; ii < 25; ii++)
    {
        for(int i = startPosition; i < 80; i++)
        {
            // x;y ' '
            drawChar(i, ii, 32); // 32 space character
            renderCharScreen(i, ii);
        }
        startPosition = 0;
    }
    m_currentBGColor = originalGB;
}

/**
 * This will erase the current character or (s)
 * by moving all text on right side of cursor
 * Left x number of space, on the same line.
 */
void Terminal::renderDeleteCharScreen(int x, int y, int num)
{
    SDL_Rect pick;
    SDL_Rect dest;

    // next we want to copy out for copy back.
    pick.w = m_screenSurface->w - ((x + num) * m_characterWidth);
    pick.h = m_characterHeight;
    pick.x = (x + num) * m_characterWidth;
    pick.y = m_characterHeight * y;

    // destination
    dest.w = m_screenSurface->w - ((x + num) * m_characterWidth);
    dest.h = m_characterHeight;
    dest.x = x * m_characterWidth;
    dest.y = m_characterHeight * y;

    // Move surface to Surface
    if(SDL_BlitSurface(m_screenSurface ,&pick, m_screenSurface, &dest) < 0)
    {
        SDL_Log("renderDeleteCharScreen() SDL_BlitSurface bottomSurface: %s",
                SDL_GetError());
        return;
    }

    // Next we want to fill the hole from pull the text left with current
    // Colors background attributes as per the spec.
    int start = 80 - num;
    int end = 80;

    // Draw Char is 0 based.
    for(int i = start; i < end; i++)
        drawChar(i,y,32);  // 32 space character

    renderScreen();
    drawTextureScreen();
}


/**
 * Render The Bottom Row of the screen
 * So were only writing the to the bottom line of the screen only
 * And not redrawing the entire screen each time. Used After Scrolling
 * The Screen up.
 */
void Terminal::renderBottomScreen()
{
    SDL_Rect pick;
    SDL_Rect rect;

    if(!m_bottomSurface)
    {
        SDL_Log("renderBottomScreen() bottomSurface CreateRGBSurface failed.");
        return;
    }

    pick.w = m_screenSurface->w;
    pick.h = m_characterHeight;
    pick.x = 0;
    if(m_scrollRegionActive)
        pick.y = m_characterHeight * (m_bottomMargin-1);
    else
        pick.y = m_characterHeight * (24);

    rect.w = m_bottomSurface->w;
    rect.h = m_bottomSurface->h;
    rect.x = 0;
    if(m_scrollRegionActive)
        rect.y = m_characterHeight * (m_bottomMargin-1);
    else
        rect.y = m_characterHeight * (24);

    if(SDL_FillRect(m_bottomSurface, nullptr,
                    SDL_MapRGB(m_bottomSurface->format, 0, 0, 0)) < 0)
    {
        SDL_Log("renderBottomScreen() SDL_FillRect bottomSurface: %s",
                SDL_GetError());
        return;
    }

    if(SDL_BlitSurface(m_screenSurface ,&pick, m_bottomSurface, nullptr) < 0)
    {
        SDL_Log("renderBottomScreen() SDL_BlitSurface bottomSurface: %s",
                SDL_GetError());
        return;
    }

    if(!m_globalTexture)
    {
        createTexture(GLOBAL_TEXTURE, m_screenSurface);
        if(!m_globalTexture)
        {
            SDL_Log("renderBottomScreen() SDL_CreateTexture globalTexture: %s",
                    SDL_GetError());
            assert(m_globalTexture);
        }
    }

    if(SDL_UpdateTexture(m_globalTexture, &rect,
                         m_bottomSurface->pixels,
                         m_bottomSurface->pitch) < 0)
    {
        SDL_Log("renderBottomScreen() SDL_UpdateTexture globalTexture: %s",
                SDL_GetError());
    }
}

/**
 * Basically Plots each Char to Texture
 * Updates super fast now!! oh yea!!
 */
void Terminal::renderScreen()
{
    SDL_Rect pick;

    pick.x = 0;
    pick.y = 0;
    pick.w = m_surfaceWidth;
    pick.h = m_surfaceHeight;

    if(!m_globalTexture)
    {
        createTexture(GLOBAL_TEXTURE, m_screenSurface);
        if(!m_globalTexture)
        {
            SDL_Log("renderScreen() SDL_CreateTexture globalTexture: %s",
                    SDL_GetError());
            assert(m_globalTexture);
        }
    }

    if(SDL_UpdateTexture(m_globalTexture, &pick,
                         m_screenSurface->pixels,
                         m_screenSurface->pitch) < 0)
    {
        SDL_Log("renderScreen() SDL_UpdateTexture globalTexture: %s",
                SDL_GetError());
    }
}

/**
 * Basically Plots each Char to Texture
 * Updates super fast now!! oh yea!!
 */
void Terminal::renderCharScreen(int x, int y)
{
    SDL_Rect rect;

    rect.w = m_tmpSurface->w;
    rect.h = m_tmpSurface->h;
    rect.x = x * m_characterWidth;
    rect.y = y * m_characterHeight;

    if(!m_globalTexture)
    {
        createTexture(GLOBAL_TEXTURE, m_screenSurface);
        if(!m_globalTexture)
        {
            SDL_Log("renderCharScreen() SDL_CreateTexture globalTexture: %s",
                    SDL_GetError());
            assert(m_globalTexture);
        }
    }

    if(SDL_UpdateTexture(m_globalTexture, &rect,
                         m_screenSurface->pixels,
                         m_screenSurface->pitch) < 0)
    {
        SDL_Log("renderCharScreen() SDL_UpdateTexture globalTexture: %s",
                SDL_GetError());
    }
}

/**
 * Basically Plots each Char to Texture
 * Updates super fast now!! oh yea!!
 */
void Terminal::renderCursorOnScreen()
{
    SDL_Rect rect;

    // Check if the position has changed, if so, then skip!
    if(m_cursorXPosition != TheSequenceParser::Instance()->x_position-1 ||
            m_cursorYPosition != TheSequenceParser::Instance()->y_position-1)
    {
        return;
    }

    // Size of Each Char 8x16 or 16x32
    rect.w = m_cursorOnSurface->w;
    rect.h = m_cursorOnSurface->h;
    rect.x = m_cursorXPosition * m_characterWidth;
    rect.y = m_cursorYPosition * m_characterHeight;

    if(!m_globalTexture)
    {
        createTexture(GLOBAL_TEXTURE, m_screenSurface);
        if(!m_globalTexture)
        {
            SDL_Log("renderCursorOnScreen() SDL_CreateTexture globalTexture: %s",
                    SDL_GetError());
            assert(m_globalTexture);
        }
    }

    if(SDL_UpdateTexture(m_globalTexture, &rect,
                         m_cursorOnSurface->pixels,
                         m_cursorOnSurface->pitch) < 0)
    {
        SDL_Log("renderCursorOffScreen() SDL_SetTextureBlendMode globalTexture: %s",
                SDL_GetError());
    }
}

/**
 * Basically Plots each Char to Texture
 * Updates super fast now!! oh yea!!
 */
void Terminal::renderCursorOffScreen()
{
    SDL_Rect rect;

    // Check if the position has changed, if so, then skip!
    if(m_cursorXPosition != TheSequenceParser::Instance()->x_position-1 ||
            m_cursorYPosition != TheSequenceParser::Instance()->y_position-1)
    {
        return;
    }

    // Size of Each Char 8x16 or 16x32
    rect.w = m_cursorOffSurface->w;
    rect.h = m_cursorOffSurface->h;
    rect.x = m_cursorXPosition * m_characterWidth;
    rect.y = m_cursorYPosition * m_characterHeight;

    if(!m_globalTexture)
    {
        createTexture(GLOBAL_TEXTURE, m_screenSurface);
        if(!m_globalTexture)
        {
            SDL_Log("renderCursorOffScreen() SDL_CreateTexture globalTexture: %s",
                    SDL_GetError());
            assert(m_globalTexture);
        }
    }

    if(SDL_UpdateTexture(m_globalTexture, &rect,
                         m_cursorOffSurface->pixels,
                         m_cursorOffSurface->pitch) < 0)
    {
        SDL_Log("renderCursorOffScreen() SDL_UpdateTexture globalTexture: %s",
                SDL_GetError());
    }
}

/**
 * Transfers the Buffer to a Texture and
 * Renders the Screen
 */
void Terminal::drawTextureScreen()
{
    SDL_Rect rect;
    int screenWidth, screenHeight;

    if(SDL_GetRendererOutputSize(m_globalRenderer,&screenWidth,&screenHeight) < 0)
    {
        SDL_Log("drawTextureScreen() SDL_GetRendererOutputSize globalRenderer: %s",
                SDL_GetError());
    }

    // We clip off bottom 80, so that we get proper 8x16
    // Display without Extra pixel borders around the screen,
    // Texture Filter results in Pixel Bleeding.
    rect.w = m_surfaceWidth  - 40; // 680 - 640 = 40
    rect.h = m_surfaceHeight - 80; // 480 - 400 = 80
    rect.x = 0;
    rect.y = 0;

    // Destination
    m_displayRect.w = screenWidth;
    m_displayRect.h = screenHeight;
    m_displayRect.x = 0;
    m_displayRect.y = 0;

    if(SDL_RenderCopy(m_globalRenderer, m_globalTexture, &rect, &m_displayRect) < 0)
    {
        SDL_Log("drawTextureScreen() SDL_RenderCopy globalRenderer: %s",
                SDL_GetError());
    }
    SDL_RenderPresent(m_globalRenderer);
}

/**
 * Clears the Terminal Screen for Fresh Redraw.
 * Renders the Screen
 */
void Terminal::clearScreen()
{
    //Clear screen
    if(SDL_SetRenderDrawColor(m_globalRenderer, 0x00, 0x00, 0x00, 0xFF) < 0)
    {
        SDL_Log("clearScreen() SDL_SetRenderDrawColor globalRenderer: %s",
                SDL_GetError());
    }
    if(SDL_RenderClear(m_globalRenderer) < 0)
    {
        SDL_Log("clearScreen() SDL_RenderClear globalRenderer: %s",
                SDL_GetError());
    }
    SDL_RenderPresent(m_globalRenderer);
}

/**
 * Compares (2) SDL Colors to determine if they are the same
 * Without conversions.
 */
int Terminal::compareSDL_Colors(SDL_Color &src, SDL_Color &dest)
{
    return (src.r == dest.r &&
            src.g == dest.g &&
            src.b == dest.b);
}

/**
 * Replace Foreground and Background colors per character cell by pixel.
 */
void Terminal::replaceColor(SDL_Surface *src, Uint32 foreground, Uint32 background)
{
    static Uint32 fgColor = SDL_MapRGB(src->format, 255, 255, 255);  // White
    static Uint32 bgColor = SDL_MapRGB(src->format, 0,   0,   0);    // Black

    // If color already default of bitmap, then skip changing.
    if(fgColor == foreground && bgColor == background)
        return;

    Uint8 *pixel = (Uint8 *)src->pixels;
    if(SDL_MUSTLOCK(src))
    {
        if(SDL_LockSurface(src) < 0)
        {
            SDL_Log("replaceColor() SDL_LockSurface src: %s",
                    SDL_GetError());
        }
    }

    int count = src->h * src->w;
    for(int i = 0; i < count; i++, pixel += 4)
    {
        // more background in most cases check first.
        if(*(Uint32 *) pixel == bgColor)
            *(Uint32 *) pixel = background;
        else // if(*(Uint32 *) p == fgColor)
            *(Uint32 *) pixel = foreground;
    }
    if(SDL_MUSTLOCK(src))
        SDL_UnlockSurface(src);
}

/**
 * Setup The Blinking Cursor
 * This grabs the current cell of the cursor position
 * Then builder on and off character surfaces.
 */
void Terminal::setupCursorChar()
{
    m_cursorXPosition = TheSequenceParser::Instance()->x_position-1;
    m_cursorYPosition = TheSequenceParser::Instance()->y_position-1;

    SDL_Rect pick, area;

    // Transform The underscore to a Cursor.
    int asciicode = '_';

    // Quad for Char Cell in Bitmap
    // 32 Cols by 8 Rows.
    pick.x = (asciicode % 32) * m_characterWidth;
    pick.y = round(asciicode / 32) * m_characterHeight;
    pick.w = m_characterWidth;
    pick.h = m_characterHeight;

    // Quad for Char Cell to Map to Screen
    area.x = m_cursorXPosition * m_characterWidth;
    area.y = m_cursorYPosition * m_characterHeight;
    area.w = m_characterWidth;
    area.h = m_characterHeight;

    // Clear Surfaces
    fillSurface(m_tmpSurface);
    fillSurface(m_cursorOffSurface);
    fillSurface(m_cursorOnSurface);

    //First Grab Underscore from Font Surface
    if(SDL_BlitSurface(m_cachedSurface,&pick,m_tmpSurface,nullptr) < 0)
    {
        SDL_Log("setupCursorChar() SDL_BlitSurface tmpSurface: %s",
                SDL_GetError());
    }

    // Grab current Char Block where Cursor position is at on screen
    if(SDL_BlitSurface(m_screenSurface,&area,m_cursorOnSurface,nullptr) < 0)
    {
        SDL_Log("setupCursorChar() SDL_BlitSurface cursorOnSurface: %s",
                SDL_GetError());
    }

    // Grab current Char Block where Passed Cursor position is.
    if(SDL_BlitSurface(m_screenSurface,&area,m_cursorOffSurface,nullptr) < 0)
    {
        SDL_Log("setupCursorChar() SDL_BlitSurface cursorOffSurface: %s",
                SDL_GetError());
    }

    // We'll Copy over the Underscore and overall it on the cursor on
    int bpp = m_screenSurface->format->BytesPerPixel;
    Uint8 *pixelTemp = (Uint8 *)m_tmpSurface->pixels;
    Uint8 *pixelNew  = (Uint8 *)m_cursorOnSurface->pixels;

    // Transpose the underscore character to the same
    // Character cell as the current x/y position were at.

    //std::cout << "show currentFont: " << currentFont << std::endl;

    if(m_currentFont == "vga8x16.bmp")
    {
        //std::cout << "current font VGA" << std::endl;
        // VGA font starts higher
        pixelTemp += (13 * m_tmpSurface->w)      * bpp;
        pixelNew  += (15 * m_cursorOnSurface->w) * bpp;
    }
    else
    {
        //std::cout << "current font NOT VGA" << std::endl;
        // Amiga fonts already at bottom of cell.
        pixelTemp += (15 * m_tmpSurface->w)      * bpp;
        pixelNew  += (15 * m_cursorOnSurface->w) * bpp;
    }

    if(SDL_MUSTLOCK(m_cursorOnSurface))
    {
        if(SDL_LockSurface(m_cursorOnSurface) < 0)
        {
            SDL_Log("setupCursorChar() SDL_LockSurface(cursorOnSurface): %s", SDL_GetError());
        }
    }

    // Copy (2) Rows of Pixels for the Cursor
    // from TmpSurface to CursorOn Surface.
    memcpy(pixelNew, pixelTemp, (m_tmpSurface->w * 2) * bpp);

    if(SDL_MUSTLOCK(m_cursorOnSurface))
        SDL_UnlockSurface(m_cursorOnSurface);

    // Render the initial cursor only if it's exposed!
    if(TheSequenceParser::Instance()->isCursorActive())
    {
        TheTerminal::Instance()->renderCursorOnScreen();
        TheTerminal::Instance()->drawTextureScreen();
    }
}

/**
 * Draws a Char Cell to x/y location on screen surface.
 */
void Terminal::drawChar(int X, int Y, int asciicode)
{
    SDL_Rect pick, area;

    // Quad for Char Cell in Bitmap
    // 32 Cols by 8 Rows.
    pick.x = (asciicode % 32) * m_characterWidth;
    pick.y = round(asciicode / 32) * m_characterHeight;
    pick.w = m_characterWidth;
    pick.h = m_characterHeight;

    // Quad for Char Cell to Map to Screen
    area.x = X * m_characterWidth;
    area.y = Y * m_characterHeight;
    area.w = m_characterWidth;
    area.h = m_characterHeight;

    // Clear Temp Surface before blitting it.
    if(SDL_FillRect(m_tmpSurface, nullptr, SDL_MapRGB(m_tmpSurface->format, 0, 0, 0)) < 0)
    {
        SDL_Log("drawChar() SDL_FillRect tmpSurface: %s", SDL_GetError());
    }

    if(SDL_BlitSurface(m_cachedSurface,&pick,m_tmpSurface,nullptr) < 0)
    {
        SDL_Log("drawChar() SDL_BlitSurface tmpSurface: %s", SDL_GetError());
    }

    replaceColor(m_tmpSurface,
                 SDL_MapRGB(m_tmpSurface->format, m_currentFGColor.r,
                            m_currentFGColor.g, m_currentFGColor.b),
                 SDL_MapRGB(m_tmpSurface->format, m_currentBGColor.r,
                            m_currentBGColor.g, m_currentBGColor.b));

    // Write to Back Buffer for Scrolling the Screen.
    if(SDL_BlitSurface(m_tmpSurface, nullptr,
                       m_screenSurface, &area) < 0)
    {
        SDL_Log("drawChar() SDL_BlitSurface screenSurface: %s", SDL_GetError());
    }
}

/**
 * Draws a String of Character Cells to x/y location on screen surface.
 * If String is longer then screen will wrap to next line.
 * NOTE Add scrolling if hits bottom of page,
 * But this will most likely be done in the ANSI Parser.
 *
 * Not Used at this time
void Terminal::drawString(int X, int Y, char text[])
{
    SDL_Rect area;

    int i = 0;
    int asciiCode = 0;
    area.x = X;
    area.y = Y;

    for(i = 0; i < (signed)strlen(text); i++)
    {
        asciiCode = std::char_traits<unsigned char>().to_int_type(text[i]);
        drawChar(area.x / m_characterWidth, area.y / m_characterHeight, asciiCode);
        area.x += m_characterWidth;
        if(area.x >= m_surfaceWidth-5)
        {
            area.x = 0;
            area.y += m_characterHeight;
        }
    }
}
*/

/**
 * Draws Character Set of each Character Cells to x/y location on screen surface.
 * If String is longer then screen will wrap to next line.
 * Mainly for testing all characters loaded from Bitmap Image.
 *
 * Not used at this time
 *
void Terminal::drawCharSet(int X, int Y)
{
    SDL_Rect area;
    int i = 0;
    int asciiCode = 0;
    area.x = X * m_characterWidth;
    area.y = Y * m_characterHeight;

    // Loop through Each Char in Bitmap.
    for(i = 1; i < 255; i++)
    {
        asciiCode = i;
        if(asciiCode == 0)
        {
            break;
        }
        drawChar(area.x / m_characterWidth, area.y / m_characterHeight, asciiCode);
        area.x += m_characterWidth;
        // Wrap
        if(area.x >= m_surfaceWidth-5)
        {
            area.x = 0;
            area.y += m_characterHeight;
        }
    }
}
*/