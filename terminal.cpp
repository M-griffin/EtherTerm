// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "terminal.h"
#include "ansiParser.h"
#include "termObjectFactory.h"
#include "mainMenuState.h"
#include "inputHandler.h"

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

using namespace std;

Terminal* Terminal::globalInstance = 0;

Terminal::Terminal():
    globalWindow(nullptr),
    globalRenderer(nullptr),
    tmpSurface(nullptr),       // Char Cell
    cursorOnSurface(nullptr),  // Char Cell for the cursor.
    cursorOffSurface(nullptr), // Char Cell for the cursor.
    screenSurface(nullptr),    // Internal Screen Buffer
    bottomSurface(nullptr),
    chachedSurface(nullptr),   // Cached Font CharacterSet
    globalTexture(nullptr),    // Texture for User Screen
    globalTermStateMachine(nullptr),
    surfaceWidth(0),
    surfaceHeight(0),
    windowWidth(0),
    windowHeight(0),
    characterWidth(0),
    characterHeight(0),
    cursorXPosition(0),
    cursorYPosition(0),
    isChangingState(false),
    isRunning(false),
    isUTF8Output(false) // Default to No, if Yes Block Font Switching!

{
    // Initalize Color Masks.
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    redMask   = 0xff000000;
    greenMask = 0x00ff0000;
    blueMask  = 0x0000ff00;
    alphaMask = 0x000000ff;
#else
    redMask   = 0x000000ff;
    greenMask = 0x0000ff00;
    blueMask  = 0x00ff0000;
    alphaMask = 0xff000000;
#endif

    black        = {   0,   0,   0 };
    blue         = {   0,   0, 171 };
    green        = {   0, 171,   0 };
    cyan         = {   0, 171, 171 };
    red          = { 171,   0,   0 };
    magenta      = { 171,   0, 171 };
    brown        = { 171,  87,   0 };

    grey         = { 171, 171, 171 };
    darkGrey     = {  87,  87,  87 };
    lightBlue    = {  87,  87, 255 };
    lightGreen   = {  87, 255,  87 };
    lightCyan    = {  87, 255, 255 };
    lightRed     = { 255,  87,  87 };
    lightMagenta = { 255,  87, 255 };
    yellow       = { 255, 255,  87 };
    white        = { 255, 255, 255 };

    // Setup Default Terminal Colors
    currentFGColor = grey;
    currentBGColor = black;

    // Not yet setup!
    //globalFontFiles.push_back("vga8x16.bmp");
    //globalFontFiles.push_back("topaz2-8x16.bmp");
    //globalFontFiles.push_back("topazPlus-8x16.bmp");

    // Set defaults;
    clearSystemConnection();
    currentFont = "vga8x16.bmp";
    previousFont = "";
}

Terminal::~Terminal()
{
    // we must clean up after ourselves to prevent memory leaks
    globalRenderer = nullptr;
    globalWindow = nullptr;
    std::cout << "*** Term Released" << std::endl;
}


/*
 * Destroy and Recreate the Window
 * SDL is broken in OSX so We need to to switch
 * The resolution and Window Sizes properly.
 */
void Terminal::restartWindowSize(bool fullScreen)
{
    if (globalWindow)
    {
        //std::cout << "Destroy SDL Window" << std::endl;
        SDL_DestroyWindow(globalWindow);
        globalWindow = nullptr;
    }

    // init the window
    if (fullScreen)
    {
        globalWindow =
            SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight,
#ifndef _WIN32               // Linux / OSX FullScreen Desktop Rez Only!
                             SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
#else                        // Windows do true Fullscreen.
                             //SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
                             SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
#endif
        if(!globalWindow) // window init success
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "Terminal::restartWindowSize() SDL_CreateWindow globalWindow: %s", SDL_GetError());
            return;
        }
    }
    else
    {
        globalWindow =
            SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight,
                             SDL_WINDOW_SHOWN);
        if(!globalWindow) // window init success
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "Terminal::restartWindowSize() SDL_CreateWindow globalWindow: %s", SDL_GetError());
            return;
        }
    }
}

/*
 * Destroy and Recreate the Renderer
 * Mode is to toggle Texture Filtering On or Off.
 * SDL is broken in OSX so We need to to switch
 * The resolution and Window Sizes properly.
 */
void Terminal::restartWindowRenderer(std::string mode)
{


// Test for Windows if we can just recreate the texture.
#ifndef _WIN32

    if (globalRenderer)
    {
        //std::cout << "Destroy SDL Renderer" << std::endl;
        SDL_DestroyRenderer(globalRenderer);
        globalRenderer = nullptr;
    }

    globalRenderer =
        SDL_CreateRenderer(globalWindow, -1, SDL_RENDERER_ACCELERATED |
                           SDL_RENDERER_TARGETTEXTURE);
#endif

    // First Clear the Rexture so it will get recreated with new HINT
    if (globalTexture)
    {
        //std::cout << "SDL_DestroyTexture globalTexture" << std::endl;
        SDL_DestroyTexture(globalTexture);
        globalTexture = nullptr;
    }
    /*
     * 0 = disable vsync
     * 1 = enable vsync
     */
    if(!SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0"))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "restartWindowRenderer SDL_SetHint SDL_HINT_RENDER_VSYNC: %s", SDL_GetError());
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
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "restartWindowRenderer SDL_SetHint SDL_HINT_RENDER_SCALE_QUALITY: %s", SDL_GetError());
    }
    /*
     * 0 disable 3D acceleration
     * 1 enable 3D acceleration, using the default renderer
     * X enable 3D acceleration, using X where X is one of the valid
     *  rendering drivers. (e.g. "direct3d", "opengl", etc.)
     */
    if(!SDL_SetHint(SDL_HINT_FRAMEBUFFER_ACCELERATION, "1"))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "restartWindowRenderer SDL_SetHint SDL_HINT_RENDER_SCALE_QUALITY: %s", SDL_GetError());
    }

    // Clear and Refresh the Renderer
    if (SDL_SetRenderDrawColor(globalRenderer, 0x00, 0x00, 0x00, 0xFF) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "restartWindowRenderer SDL_SetRenderDrawColor globalRenderer: %s", SDL_GetError());
        return;
    }
    if (SDL_SetRenderTarget(globalRenderer, nullptr) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "restartWindowRenderer SDL_SetRenderTarget NULL: %s", SDL_GetError());
    }

    //int screenWidth, screenHeight;
    //SDL_GetRendererOutputSize(globalRenderer,&screenWidth,&screenHeight);
    //SDL_RenderSetLogicalSize(globalRenderer,screenWidth,screenHeight);

    if (SDL_RenderClear(globalRenderer) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "restartWindowRenderer() SDL_RenderClear globalRenderer: %s", SDL_GetError());
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
    windowTitle     = title;
    surfaceWidth    = swidth;
    surfaceHeight   = sheight;
    windowWidth     = wwidth;
    windowHeight    = wheight;
    characterWidth  = fwidth;
    characterHeight = fheight;

    // Resizing offset pixel in texture resize, shaded blocks are uneven!
    //flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
    flags = SDL_WINDOW_SHOWN; //  | SDL_WINDOW_ALLOW_HIGHDPI;

    // attempt to initialise SDL
    //if(SDL_Init(SDL_INIT_EVERYTHING) == 0)
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "Terminal::init() SDL_Init: SDL_INIT_VIDEO %s", SDL_GetError());
        return false;
    }

    std::cout << "SDL init success" << std::endl;

    /* initialize SDL_net */
    if(SDLNet_Init() < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "Terminal::init() SDLNet_Init: %s", SDL_GetError());
        return false;
    }

    std::cout << "SDLNet_Init success" << std::endl;

#ifdef _DEBBUG
     //Also need to init SDL_ttf
    if(TTF_Init() != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
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
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "Terminal::init() SDL_SetHint SDL_HINT_RENDER_VSYNC: %s", SDL_GetError());

        // Not a show stopper, continue on!
    }
    /*
     * 0 or nearest = nearest pixel sampling
     * 1 or linear  = linear filtering       (supported by OpenGL and Direct3D)
     * 2 or best    = anisotropic filtering  (supported by Direct3D)
     */
    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "Terminal::init() SDL_SetHint SDL_HINT_RENDER_SCALE_QUALITY: %s", SDL_GetError());

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
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "Terminal::init() SDL_SetHint SDL_HINT_RENDER_SCALE_QUALITY: %s", SDL_GetError());

        // Not a show stopper, continue on! but maybe!!
    }

    // init the window
    globalWindow =
        SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, flags);
    if(!globalWindow) // window init success
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "Terminal::init() SDL_CreateWindow globalWindow: %s", SDL_GetError());
        return false;
    }

    std::cout << "SDL_Window creation success" << std::endl;

    globalRenderer =
        SDL_CreateRenderer(globalWindow, -1, SDL_RENDERER_ACCELERATED |
                           SDL_RENDERER_TARGETTEXTURE);
    if(!globalRenderer) // renderer init success
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "Terminal::init() SDL_CreateRenderer globalRenderer: %s", SDL_GetError());
        return false;
    }

    cout << "SDL_Renderer creation success" << std::endl;

    if (SDL_SetRenderDrawColor(globalRenderer, 0x00, 0x00, 0x00, 0xFF) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "Terminal::init() SDL_SetRenderDrawColor globalRenderer: %s", SDL_GetError());
        return false;
    }

    // Increate the scaling to smooth without filtering.
    // Doesn't smooth anything!

    //int screenWidth, screenHeight;
    //SDL_GetRendererOutputSize(globalRenderer,&screenWidth,&screenHeight);
    //SDL_RenderSetLogicalSize(globalRenderer,screenWidth,screenHeight);
    //SDL_RenderSetScale(globalRenderer, 0, 0);

    // add some sound effects - TODO move to better place
    /*
    TheSoundManager::Instance()->load("assets/ElectroRock.ogg", "music1", SOUND_MUSIC);
    TheSoundManager::Instance()->load("assets/boom.wav", "explode", SOUND_SFX);
    TheSoundManager::Instance()->playMusic("music1", -1);
    */
    //TheInputHandler::Instance()->initialiseJoysticks();
    // start the menu state, Move this to After Surfaces are loaded.
    //globalTermStateMachine = new TermStateMachine();
    //globalTermStateMachine->changeState(new MainMenuState());
    isRunning = true; // everything inited successfully, start the main loop
    return true;
}

void Terminal::update()
{
    globalTermStateMachine->update();
}

void Terminal::clean()
{
    // Call from a private member to free properly.
    freeSurfaceTextures();

    // Shutdown SDL_Sockets.
    std::cout << "Shutting Down SDL_Net" << std::endl;

    //globalFontFiles.clear();
    //vector<std::string>().swap(globalFontFiles);

    std::cout << "Shutting Down TermStateMachine" << std::endl;
    globalTermStateMachine->clean();
    delete globalTermStateMachine;

#ifdef _DEBBUG
    std::cout << "TTF_Quit" << std::endl;
    TTF_Quit();
#endif

    std::cout << "SDL_Quit" << std::endl;
    SDL_Quit();
}

/*
 * Once we have a screen selection, we will now pull the
 * Rect radius and grab all mirrowing text from the screen
 * buffer and copy it to the clip board. Copy/Paste.
 */
void Terminal::pullSelectionBuffer(int x, int y)
{
    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(globalRenderer,&screenWidth,&screenHeight);

    // We need to Translate the Screen Width vs Rows and Width to
    // get actual the grid size of the Characters to snap everything correctly.
    double charWidth, charHeight;
    double value;

    value = (double)screenHeight / 25.0;
    charHeight = value; //round(abs(value));
    //charHeight += screenHeight % 25;
    value = (double)screenWidth / 80.0;
    charWidth = value; //round(abs(value));
    //charWidth += screenWidth % 80;

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
    if (sourceX < x)
    {
        // TOP Stationary -> Right ->> OK!
        rect.x = floor((double)sourceX / charWidth) * charWidth;
        //rect.x = sourceX;
        //rect.x -= rect.x % charWidth; // Snap to Left.
    }
    else
    {
        // Top -> Left ->> OK!
        rect.x = floor((double)x / charWidth) * charWidth;
        //rect.x = x;
        //rect.x -= rect.x % charWidth;
    }

    if (sourceY < y)
    {
        // Top Stationary ->> OK!
        rect.y = floor((double)sourceY / charHeight) * charHeight;
        //rect.y = sourceY;
        //rect.y -= rect.y % charHeight; // Snap to Top
    }
    else
    {
        // Top -> Up ->> OK!
        rect.y = floor((double)y / charHeight) * charHeight;
        //rect.y = y;
        //rect.y -= rect.y % charHeight;
    }

    // Width and height are calcuated by the different from motion to source
    // Well need to round these values to the width and height of a character!
    // Also need to swap source for the lowest value so we can select both
    // above and behind the starting points.

    // This is the bottom Region
    if (sourceX < x)
    {
        // Bottom Width RIGHT ->> OK!
        //rect.w = charWidth +
            //((x -= (x % charWidth)) - (sourceX -= sourceX % charWidth));
        rect.w = charWidth + (floor((double)x / charWidth) * charWidth) -  rect.x ;
    }
    else
    {
        // Bottom Stationary ->> OK!
        //rect.w = ((sourceX -= sourceX % charWidth)) - (rect.x);
        rect.w = charWidth + (floor((double)sourceX / charWidth) * charWidth) - rect.x; // back
    }
    if (sourceY < y)
    {
        // Bottom -> Down ->> OK!
        //rect.h = charHeight +
          //  ((y -= (y % charHeight)) - (sourceY -= sourceY % charHeight));
        rect.h = charHeight + (floor((double)y / charHeight) * charHeight) - rect.y;
    }
    else
    {
        // Bottom -> Stationary ->> OK!
        //rect.h = ((sourceY -= sourceY % charHeight)) - (rect.y);
        rect.h = charHeight + (floor((double)sourceY / charHeight) * charHeight) - rect.y; // back
    }

    // If were inbetween lines, Height = 0, then add char Height to the bottom
    // So we always have a row!
    if (rect.h == 0)
        rect.h += round(charHeight);

    if (rect.w == 0)
        rect.w += round(charWidth);

    // Now that we have the excat coordinates of the selected text.
    // We need to translate this to the screenbuffer positions
    // So that we can grab the text that was selected on the screen.

    // Get Starting Position
    int startColumn = rect.x / charWidth;
    int startRow    = rect.y / charHeight;
    // Now figure out how many character per row
    int length      = rect.w / charWidth;
    // Now figure out how many rows of characts to pull
    int numberRows  = rect.h / charHeight;

    // Use coords to pull screen text directly from screen buffer.
    TheAnsiParser::Instance()->bufferToClipboard(
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
    SDL_DestroyTexture(selectionTexture);
    selectionTexture = nullptr;
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
    SDL_GetRendererOutputSize(globalRenderer,&screenWidth,&screenHeight);

    // We clip off botom 80, so that we get proper 8x16
    // Display without Extra pixel borders around the screen,
    // Texture Filter results in Pixel Bleeding.
    rectorig.w = surfaceWidth  - 40; // 680 - 640 = 40
    rectorig.h = surfaceHeight - 80; // 480 - 400 = 80
    rectorig.x = 0,
    rectorig.y = 0;

    // Destination
    displayRect.w = screenWidth;
    displayRect.h = screenHeight;
    displayRect.x = 0;
    displayRect.y = 0;

    // Redraw the screen between selection changes so we have nothing left
    // from previous coordinates.
    if (SDL_RenderCopy(globalRenderer, globalTexture, &rectorig, &displayRect) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "renderSelectionScreen() SDL_RenderCopy globalTexture: %s", SDL_GetError());
    }

    // Next create a texture to overlay for highlighting
    if(!selectionTexture)
    {
        selectionTexture =
            SDL_CreateTexture(globalRenderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_TARGET,
                screenSurface->w, screenSurface->h);
        if (!selectionTexture)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "renderSelectionScreen() SDL_CreateTexture selectionTexture: %s", SDL_GetError());
            return;
        }
        if (SDL_SetTextureBlendMode(selectionTexture, SDL_BLENDMODE_ADD) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "renderSelectionScreen() SDL_SetTextureBlendMode selectionTexture: %s", SDL_GetError());
        }

        // On need to set this the first time!!
        // Set Render Target to the Texture.
        if (SDL_SetRenderTarget(globalRenderer, selectionTexture) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "renderSelectionScreen() SDL_SetRenderTarget selectionTexture: %s", SDL_GetError());
        }

        if (SDL_RenderClear(globalRenderer) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "renderSelectionScreen() SDL_RenderClear selectionTexture: %s", SDL_GetError());
        }

        // Fill new texture with Dark Cyan Highlight.
        if (SDL_SetRenderDrawColor(globalRenderer, 0, 50 , 50, 255) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "renderSelectionScreen() SDL_SetRenderDrawColor selectionTexture: %s", SDL_GetError());
        }

        // Can test filling to RECT to speed this up after it's working!
        if (SDL_RenderFillRect(globalRenderer, nullptr) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "renderSelectionScreen() SDL_RenderFillRect selectionTexture: %s", SDL_GetError());
        }

        //Reset back to Global Render, then push update over current texture
        if (SDL_SetRenderTarget(globalRenderer, nullptr) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "renderSelectionScreen() SDL_SetRenderTarget NULL: %s", SDL_GetError());
        }
    }

    // We need to Translate the Screen Width vs Rows and Width to
    // get actual the grid size of the Characters to snap everything correctly.
    double charWidth, charHeight;
    double value;

    value = (double)screenHeight / 25.0;
    charHeight = value; //round(abs(value));
    //charHeight += screenHeight % 25;
    value = (double)screenWidth / 80.0;
    charWidth = value; //round(abs(value));
    //charWidth += screenWidth % 80;

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
    if (sourceX < x)
    {
        // TOP Stationary -> Right ->> OK!
        rect.x = floor((double)sourceX / charWidth) * charWidth;
        //rect.x = sourceX;
        //rect.x -= rect.x % charWidth; // Snap to Left.
    }
    else
    {
        // Top -> Left ->> OK!
        rect.x = floor((double)x / charWidth) * charWidth;
        //rect.x = x;
        //rect.x -= rect.x % charWidth;
    }

    if (sourceY < y)
    {
        // Top Stationary ->> OK!
        rect.y = floor((double)sourceY / charHeight) * charHeight;
        //rect.y = sourceY;
        //rect.y -= rect.y % charHeight; // Snap to Top
    }
    else
    {
        // Top -> Up ->> OK!
        rect.y = floor((double)y / charHeight) * charHeight;
        //rect.y = y;
        //rect.y -= rect.y % charHeight;
    }

    // Width and height are calcuated by the different from motion to source
    // Well need to round these values to the width and height of a character!
    // Also need to swap source for the lowest value so we can select both
    // above and behind the starting points.

    // This is the bottom Region
    if (sourceX < x)
    {
        // Bottom Width RIGHT ->> OK!
        //rect.w = charWidth +
            //((x -= (x % charWidth)) - (sourceX -= sourceX % charWidth));
        rect.w = charWidth + (floor((double)x / charWidth) * charWidth) -  rect.x ;
    }
    else
    {
        // Bottom Stationary ->> OK!
        //rect.w = ((sourceX -= sourceX % charWidth)) - (rect.x);
        rect.w = charWidth + (floor((double)sourceX / charWidth) * charWidth) - rect.x; // back
    }
    if (sourceY < y)
    {
        // Bottom -> Down ->> OK!
        //rect.h = charHeight +
          //  ((y -= (y % charHeight)) - (sourceY -= sourceY % charHeight));
        rect.h = charHeight + (floor((double)y / charHeight) * charHeight) - rect.y;
    }
    else
    {
        // Bottom -> Stationary ->> OK!
        //rect.h = ((sourceY -= sourceY % charHeight)) - (rect.y);
        rect.h = charHeight + (floor((double)sourceY / charHeight) * charHeight) - rect.y; // back
    }

    // If were inbetween lines, Height = 0, then add char Height to the bottom
    // So we always have a row!
    if (rect.h == 0)
        rect.h += round(charHeight);

    if (rect.w == 0)
        rect.w += round(charWidth);

    // Draw First Highlight Overlay
    if (SDL_RenderCopy(globalRenderer, selectionTexture, nullptr, &rect) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "renderSelectionScreen() SDL_RenderCopy selectionTexture: %s", SDL_GetError());
    }

    // 2 Pixel expanded shaded border (I like it!)
    rect.x -= 4;
    rect.y -= 4;
    rect.w += 8;
    rect.h += 8;

    // Draw Border Overlay
    if (SDL_RenderCopy(globalRenderer, selectionTexture, nullptr, &rect) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "renderSelectionScreen() SDL_RenderCopy selectionTexture (Overlay): %s", SDL_GetError());
    }
    SDL_RenderPresent(globalRenderer);
}

/*
 * Free all Surfaces and Textures
 * Seems we have to do this from a private member
 * Otherwise the program loses the pointers.
 */
void Terminal::freeSurfaceTextures()
{
    // Clear Cached Surface.
    std::cout << std::endl << "*** Releasing Surfaces and Textures" << std::endl;


    //Free texture if it exists
    if (globalTexture)
    {
        std::cout << "SDL_DestroyTexture globalTexture" << std::endl;
        SDL_DestroyTexture(globalTexture);
        globalTexture = nullptr;
    }
    if(selectionTexture)
    {
        std::cout << "SDL_DestroyTexture selectionTexture" << std::endl;
        SDL_DestroyTexture(selectionTexture);
        selectionTexture = nullptr;
    }

    if (globalWindow)
    {
        std::cout << "Destroy SDL Window" << std::endl;
        SDL_DestroyWindow(globalWindow);
        globalWindow = nullptr;
    }
	if (globalRenderer)
    {
        std::cout << "Destroy SDL Renderer" << std::endl;
        SDL_DestroyRenderer(globalRenderer);
        globalRenderer = nullptr;
    }

    if (chachedSurface)
    {
        std::cout << "SDL_FreeSurface chachedSurface" << std::endl;
        SDL_FreeSurface(chachedSurface);     // Bitmap    -> Fonts
        chachedSurface = nullptr;
    }
    if (screenSurface)
    {
        std::cout << "SDL_FreeSurface screenSurface" << std::endl;
        SDL_FreeSurface(screenSurface);      // Screen    -> Texture
        screenSurface = nullptr;
    }
    if (tmpSurface)
    {
        std::cout << "SDL_FreeSurface tmpSurface" << std::endl;
        SDL_FreeSurface(tmpSurface);         // Char Cell -> Screen
        tmpSurface = nullptr;
    }
    if (cursorOffSurface)
    {
        std::cout << "SDL_FreeSurface cursorOffSurface" << std::endl;
        SDL_FreeSurface(cursorOffSurface);   // Cursor Off Character Cell
        cursorOffSurface = nullptr;
    }
    if (bottomSurface)
    {
        std::cout << "SDL_FreeSurface bottomSurface" << std::endl;
        SDL_FreeSurface(bottomSurface);      // Bottom Line of Scrolling Region.
        bottomSurface = nullptr;
    }
}

/*
 * Store Images later in a zip file.
 * Not in use at this time!
 */
bool Terminal::loadBitmapImageFromPak()
{
    /* Require minizip lib.
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
    */
    return chachedSurface != nullptr;
}

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
    chachedSurface = SDL_LoadBMP(path.c_str());
    if(!chachedSurface)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "loadBitmapImage() SDL_LoadBMP font: %s", currentFont.c_str());
        return false;
    }

    // Redraw Cursor with new font!
    // When no data received, this is when we want to show the cursor!
    // Setup cursor in current x/y position Cursor.
    if (TheAnsiParser::Instance()->isCursorActive())
    {
        setupCursorChar();
        renderCursorOnScreen();
        drawTextureScreen();
    }

    previousFont = currentFont; // Current to Previous
    currentFont  = fontName;    // New to Current.
    return chachedSurface != nullptr;
}

/**
 * Initialize the Render, Loads the Fonts and Create the Surfaces.
 */
bool Terminal::initSurfaceTextures()
{
    // Setup the Surface we will plot each character cell to.
    screenSurface =
        SDL_CreateRGBSurface(
            SDL_SWSURFACE, surfaceWidth, surfaceHeight, 32,
            redMask, greenMask, blueMask, alphaMask);
    if(!screenSurface)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "initSurfaceTextures() SDL_CreateRGBSurface screenSurface: %s", SDL_GetError());
        return false;
    }

    screenSurface =
        SDL_ConvertSurfaceFormat(
            screenSurface, SDL_PIXELFORMAT_ARGB8888, 0);
    if (!screenSurface)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "initSurfaceTextures() SDL_ConvertSurfaceFormat screenSurface: %s", SDL_GetError());
        return false;
    }

    // Fill the Surface with Black to initalize it.
    if (SDL_FillRect(
        screenSurface, nullptr,
        SDL_MapRGB(screenSurface->format, 0, 0, 0)) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "initSurfaceTextures() SDL_FillRect screenSurface: %s", SDL_GetError());
        return false;
    }

    // Setup Temp Surface that the size of each Character Cell.
    tmpSurface =
        SDL_CreateRGBSurface(
            SDL_SWSURFACE, characterWidth, characterHeight, 32,
            redMask, greenMask, blueMask, alphaMask);
    if(!tmpSurface)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "initSurfaceTextures() SDL_CreateRGBSurface tmpSurface: %s", SDL_GetError());
        return false;
    }
    tmpSurface =
        SDL_ConvertSurfaceFormat(
            tmpSurface, SDL_PIXELFORMAT_ARGB8888, 0);
    if(!tmpSurface)
    {
       SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "initSurfaceTextures() SDL_ConvertSurfaceFormat tmpSurface: %s", SDL_GetError());
        return false;
    }

    // Setup Cursor Surface that the size of a Character Cell.
    // Holds the original Char when we overwrite it with an blinking cursor.
    cursorOnSurface =
        SDL_CreateRGBSurface(
            SDL_SWSURFACE, characterWidth, characterHeight, 32,
            redMask, greenMask, blueMask, alphaMask);
    if(!cursorOnSurface)
    {
       SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "initSurfaceTextures() SDL_CreateRGBSurface cursorOnSurface: %s", SDL_GetError());
        return false;
    }

    cursorOnSurface =
        SDL_ConvertSurfaceFormat(
            cursorOnSurface, SDL_PIXELFORMAT_ARGB8888, 0);
    if(!cursorOnSurface)
    {
         SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "initSurfaceTextures() SDL_ConvertSurfaceFormat cursorOnSurface: %s", SDL_GetError());
        return false;
    }

    // Setup Cursor Surface that the size of a Character Cell.
    // Holds the original Char when we overwrite it with an blinking cursor.
    cursorOffSurface =
        SDL_CreateRGBSurface(
            SDL_SWSURFACE, characterWidth, characterHeight, 32,
            redMask, greenMask, blueMask, alphaMask);
    if(!cursorOffSurface)
    {
         SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "initSurfaceTextures() SDL_CreateRGBSurface cursorOffSurface: %s", SDL_GetError());
        return false;
    }

    cursorOffSurface =
        SDL_ConvertSurfaceFormat(
            cursorOffSurface, SDL_PIXELFORMAT_ARGB8888, 0);
    if (!cursorOffSurface)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "initSurfaceTextures() SDL_ConvertSurfaceFormat bottomSurface: %s", SDL_GetError());
        return false;
    }

    // Setup the Surface we will plot each character cell to.
    bottomSurface =
        SDL_CreateRGBSurface(
            SDL_SWSURFACE, screenSurface->w, characterHeight, 32,
            redMask, greenMask, blueMask, alphaMask);
    if(!bottomSurface)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "initSurfaceTextures() SDL_CreateRGBSurface bottomSurface: %s", SDL_GetError());
        return false;
    }

    bottomSurface =
        SDL_ConvertSurfaceFormat(
            bottomSurface, SDL_PIXELFORMAT_ARGB8888, 0);
    if (!bottomSurface)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "initSurfaceTextures() SDL_ConvertSurfaceFormat bottomSurface: %s", SDL_GetError());
        return false;
    }

    // Fill the Surface with Black to initalize it.
    if (SDL_FillRect(bottomSurface, nullptr,
        SDL_MapRGB(bottomSurface->format, 0, 0, 0)) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "initSurfaceTextures() SDL_FillRect bottomSurface: %s", SDL_GetError());
        return false;
    }

    // start the menu state
    globalTermStateMachine = new TermStateMachine();
    if (!globalTermStateMachine)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "initSurfaceTextures() globalTermStateMachine TermStateMachine");
        return false;
    }

    // Load into Main Menu System (Dialing Directory)
    globalTermStateMachine->changeState(new MainMenuState());
    return true;
}

/*
 * Overrides the Scrolling region of the screen
 */
void Terminal::setScrollRegion(int top, int bot, int terminalHeight)
{
    // If Region is larger then the Terminal,
    // Ignore it!
    if (top == 1 && bot >= terminalHeight)
    {
        scrollRegionActive = false;
        return;
    }
    // 0,0 is reset.
    if(top == 0 && bot == 0)
    {
        scrollRegionActive = false;
        return;
    }
    else
    {
        // Make sure Bottom Region
        // Is the same as our terminal length.
        if (bot > terminalHeight)
            bot = terminalHeight;
        scrollRegionActive = true;
    }
    // Set Scrolling Margins
    topMargin = top;
    bottomMargin = bot;
}

/*
 * Scroll a specific set region of the screen only
 */
void Terminal::scrollRegionUp()
{
    SDL_Rect area;

    // Clear Out last line after Scrolling up.
    area.x = 0;
    area.y = characterHeight * (bottomMargin-1);
    area.w = surfaceWidth;
    area.h = characterHeight;

    int bpp = screenSurface->format->BytesPerPixel;
    Uint8 *pixelNewPos = (Uint8 *)screenSurface->pixels;
    Uint8 *pixelTopPos = (Uint8 *)screenSurface->pixels;

    // Move position to start
    pixelTopPos += screenSurface->w * (characterHeight * (topMargin -1)) * bpp;

    // Start from One line below the Top Margin.
    pixelNewPos += screenSurface->w * (characterHeight * topMargin) * bpp;

    if(SDL_MUSTLOCK(screenSurface))
    {
        if(SDL_LockSurface(screenSurface) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "scrollRegionUp() SDL_LockSurface screenSurface: %s", SDL_GetError());
            return;
        }
    }

    // Move the Entire Screen Up a 1 Row of Characters.
    memmove(pixelTopPos, pixelNewPos,
        (screenSurface->w *
        (characterHeight * (bottomMargin - topMargin))) * bpp);

    if(SDL_MUSTLOCK(screenSurface))
        SDL_UnlockSurface(screenSurface);

    // Clear out very last line of surface region.
    if (SDL_FillRect(screenSurface, &area,
        SDL_MapRGB(screenSurface->format, 0, 0, 0)) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "scrollRegionUp() SDL_FillRect screenSurface: %s", SDL_GetError());
    }

    // If first Time, Crete Texture.
    if(!globalTexture)
    {
        globalTexture =
            SDL_CreateTexture(globalRenderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                screenSurface->w, screenSurface->h);
        if (!globalTexture)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "scrollRegionUp() SDL_CreateTexture globalRenderer: %s", SDL_GetError());
            return;
        }
        if (SDL_SetTextureBlendMode(globalTexture, SDL_BLENDMODE_BLEND) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "scrollRegionUp() SDL_SetTextureBlendMode globalTexture: %s", SDL_GetError());
        }
    }
    if (SDL_UpdateTexture(globalTexture, nullptr,
        screenSurface->pixels, screenSurface->pitch) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "scrollRegionUp() SDL_UpdateTexture globalTexture: %s", SDL_GetError());
    }
}

/*
 * Clips out the Top Row of the Screen,
 * Then Moves Entire Screen Up one Row to Scroll it.
 */
void Terminal::scrollScreenUp()
{
    if(scrollRegionActive)
    {
        scrollRegionUp();
        return;
    }

    int bpp = screenSurface->format->BytesPerPixel;
    Uint8 *pixelNewPos = (Uint8 *)screenSurface->pixels;
    Uint8 *pixelOldPos = (Uint8 *)screenSurface->pixels;

    // Move position to start of 2nd Line,
    // Then copying greater line down to previous.
    pixelNewPos += (screenSurface->w * characterHeight) * bpp;

    if(SDL_MUSTLOCK(screenSurface))
    {
        if(SDL_LockSurface(screenSurface) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "scrollScreenUp() SDL_LockSurface screenSurface: %s", SDL_GetError());
            return;
        }
    }

    // Move the Entire Screen Up a 1 Row of Characters.
    memmove(pixelOldPos, pixelNewPos,
            (screenSurface->w * (screenSurface->h - characterHeight)) * bpp);

    if(SDL_MUSTLOCK(screenSurface))
        SDL_UnlockSurface(screenSurface);

    if(!globalTexture)
    {
        globalTexture =
            SDL_CreateTexture(globalRenderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                screenSurface->w, screenSurface->h);

        if (!globalTexture)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "scrollScreenUp() SDL_CreateTexture globalRenderer: %s", SDL_GetError());
            return;
        }
        if (SDL_SetTextureBlendMode(globalTexture, SDL_BLENDMODE_BLEND) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "scrollScreenUp() SDL_SetTextureBlendMode globalTexture: %s", SDL_GetError());
        }
    }
    // Write Scrolled screen to Texture.
    if (SDL_UpdateTexture(globalTexture, nullptr,
        screenSurface->pixels, screenSurface->pitch) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "scrollScreenUp() SDL_UpdateTexture globalTexture: %s", SDL_GetError());
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
    if (SDL_FillRect(screenSurface, nullptr,
            SDL_MapRGB(screenSurface->format,
                currentBGColor.r, currentBGColor.g, currentBGColor.b)) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "clearScreenSurface() SDL_FillRect screenSurface: %s", SDL_GetError());
    }

    if(!globalTexture)
    {
        std::cout << "ClearScreenSurface() - globalTexture" << std::endl;
        globalTexture =
            SDL_CreateTexture(globalRenderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                screenSurface->w, screenSurface->h);
        if (!globalTexture)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "clearScreenSurface() SDL_CreateTexture globalRenderer: %s", SDL_GetError());
            return;
        }
        if (SDL_SetTextureBlendMode(globalTexture, SDL_BLENDMODE_BLEND) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "clearScreenSurface() SDL_SetTextureBlendMode globalTexture: %s", SDL_GetError());
        }
    }

    if (SDL_UpdateTexture(globalTexture, nullptr,
        screenSurface->pixels, screenSurface->pitch) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "clearScreenSurface() SDL_UpdateTexture globalRenderer: %s", SDL_GetError());
    }
    if (SDL_RenderClear(globalRenderer) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "clearScreenSurface() SDL_RenderClear globalRenderer: %s", SDL_GetError());
    }
}

/**
 * Fill entire lines with space and black foreground/Background
 * Fast update to clear a row.
 */
void Terminal::renderClearLineScreen(int y, int start, int end)
{
    // Save Background Color so we can switch back to it.
    SDL_Color originalGB = currentBGColor;
    currentBGColor = black;

    // Clear out entire line.
    for(int i = start; i < end; i++)
    {
        drawChar(i, y, 32);
        renderCharScreen(i, y);
    }
    currentBGColor = originalGB;
}

/**
 * Loops from starting y position to bottom of screen
 * Erasing all of the screen below.
 * Fill entire lines with space and black foreground/Background
 * Fast update to clear a row.
 */
void Terminal::renderClearLineAboveScreen(int y, int x)
{
    std::cout << "renderClearLineAboveScreen: " << x << ";" << y << std::endl;
    // Save Background Color so we can switch back to it.
    SDL_Color originalGB = currentBGColor;
    currentBGColor = black;

    int startPosition = x;

    // Clear out entire lines Up The Screen the screen.
    // Fix with term variables lateron.
    for(int ii = y; (ii+1) > 0; ii--)
    {
        for(int i = startPosition; (i+1) > 0; i--)
        {
            // x;y ' '
            drawChar(i, ii, 32);
            renderCharScreen(i, ii);
        }
        // Reset to starting position for following lines.
        startPosition = 79;
    }
    currentBGColor = originalGB;
}

/**
 * Loops from starting y position to bottom of screen
 * Erasing all of the screen below.
 * Fill entire lines with space and black foreground/Background
 * Fast update to clear a row.
 */
void Terminal::renderClearLineBelowScreen(int y, int x)
{
    // Save Background Color so we can switch back to it.
    SDL_Color originalGB = currentBGColor;
    currentBGColor = black;

    int startPosition = x;

    // Clear out entire lines down the screen.
    // Fix with term variables lateron.
    for(int ii = y; ii < 25; ii++)
    {
        for(int i = startPosition; i < 80; i++)
        {
            // x;y ' '
            drawChar(i, ii, 32);
            renderCharScreen(i, ii);
        }
        startPosition = 0;
    }
    currentBGColor = originalGB;
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
    pick.w = screenSurface->w - ((x + num) * characterWidth);
    pick.h = characterHeight;
    pick.x = (x + num) * characterWidth;
    pick.y = characterHeight * y;

    // destination
    dest.w = screenSurface->w - ((x + num) * characterWidth);
    dest.h = characterHeight;
    dest.x = x * characterWidth;
    dest.y = characterHeight * y;

     // Move surface to Surface
    if (SDL_BlitSurface(screenSurface ,&pick, screenSurface, &dest) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "renderDeleteCharScreen() SDL_BlitSurface bottomSurface: %s", SDL_GetError());
        return;
    }

    // Next we want to fill the hole from pull the text left with current
    // Colors background attributes as per the spec.
    int start = 80 - num;
    int end = 80;

    // Draw Char is 0 based.
    for (int i = start; i < end; i++)
        drawChar(i,y,32);

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

    if(!bottomSurface)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "renderBottomScreen() bottomSurface CreateRGBSurface failed.");
        return;
    }

    pick.w = screenSurface->w;
    pick.h = characterHeight;
    pick.x = 0;
    if(scrollRegionActive)
        pick.y = characterHeight * (bottomMargin-1);
    else
        pick.y = characterHeight * (24);

    rect.w = bottomSurface->w;
    rect.h = bottomSurface->h;
    rect.x = 0;
    if(scrollRegionActive)
        rect.y = characterHeight * (bottomMargin-1);
    else
        rect.y = characterHeight *  (24);

    if (SDL_FillRect(bottomSurface, nullptr,
            SDL_MapRGB(bottomSurface->format, 0, 0, 0)) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "renderBottomScreen() SDL_FillRect bottomSurface: %s", SDL_GetError());
        return;
    }

    if (SDL_BlitSurface(screenSurface ,&pick, bottomSurface, nullptr) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "renderBottomScreen() SDL_BlitSurface bottomSurface: %s", SDL_GetError());
        return;
    }

    if(!globalTexture)
    {
        globalTexture =
            SDL_CreateTexture(globalRenderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                screenSurface->w, screenSurface->h);
        if (!globalTexture)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "renderBottomScreen() SDL_CreateTexture globalTexture: %s", SDL_GetError());
            return;
        }
        if (SDL_SetTextureBlendMode(globalTexture, SDL_BLENDMODE_BLEND) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "renderBottomScreen() SDL_SetTextureBlendMode globalTexture: %s", SDL_GetError());
        }
    }

    if (SDL_UpdateTexture(globalTexture, &rect,
        bottomSurface->pixels, bottomSurface->pitch) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "renderBottomScreen() SDL_UpdateTexture globalTexture: %s", SDL_GetError());
    }
}


/*
void Terminal::renderBottomScreen()
{
    SDL_Rect pick;
    //SDL_Rect rect;

    if(!bottomSurface)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "renderBottomScreen() bottomSurface CreateRGBSurface failed.");
        return;
    }

    pick.w = screenSurface->w - 40;
    pick.h = characterHeight;
    pick.x = 0;
    if(scrollRegionActive)
        pick.y = characterHeight * (bottomMargin-1);
    else
        pick.y = characterHeight * (24);

    if(!globalTexture)
    {
        globalTexture =
            SDL_CreateTexture(globalRenderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                screenSurface->w, screenSurface->h);
        if (!globalTexture)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "renderBottomScreen() SDL_CreateTexture globalTexture: %s", SDL_GetError());
            return;
        }
        if (SDL_SetTextureBlendMode(globalTexture, SDL_BLENDMODE_BLEND) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "renderBottomScreen() SDL_SetTextureBlendMode globalTexture: %s", SDL_GetError());
        }
    }

    if (SDL_UpdateTexture(globalTexture, &pick,
        screenSurface->pixels, screenSurface->pitch) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "renderBottomScreen() SDL_UpdateTexture globalTexture: %s", SDL_GetError());
    }
}
*/

/**
 * Basically Plots each Char to Texture
 * Updates super fast now!! oh yea!!
 */
void Terminal::renderScreen()
{
    SDL_Rect pick;

    pick.x = 0;
    pick.y = 0;
    pick.w = surfaceWidth;
    pick.h = surfaceHeight;

    if(!globalTexture)
    {
        globalTexture =
            SDL_CreateTexture(globalRenderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                screenSurface->w, screenSurface->h);
        if (!globalTexture)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "renderScreen() SDL_CreateTexture globalTexture: %s", SDL_GetError());
            return;
        }
        if (SDL_SetTextureBlendMode(globalTexture, SDL_BLENDMODE_BLEND) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "renderScreen() SDL_SetTextureBlendMode globalTexture: %s", SDL_GetError());
        }
    }

    if (SDL_UpdateTexture(globalTexture, &pick,
        screenSurface->pixels, screenSurface->pitch) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "renderScreen() SDL_UpdateTexture globalTexture: %s", SDL_GetError());
    }
}

/**
 * Basically Plots each Char to Texture
 * Updates super fast now!! oh yea!!
 */
void Terminal::renderCharScreen(int x, int y)
{
    SDL_Rect rect;

    rect.w = tmpSurface->w;
    rect.h = tmpSurface->h;
    rect.x = x * characterWidth;
    rect.y = y * characterHeight;

    if(!globalTexture)
    {
        globalTexture =
            SDL_CreateTexture(globalRenderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                screenSurface->w, screenSurface->h);
        if (!globalTexture)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "renderCharScreen() SDL_CreateTexture globalTexture: %s", SDL_GetError());
            return;
        }

        if (SDL_SetTextureBlendMode(globalTexture, SDL_BLENDMODE_BLEND) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "renderCharScreen() SDL_SetTextureBlendMode globalTexture: %s", SDL_GetError());
        }
    }

    if (SDL_UpdateTexture(globalTexture, &rect,
        screenSurface->pixels, screenSurface->pitch) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "renderCharScreen() SDL_UpdateTexture globalTexture: %s", SDL_GetError());
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
    if(cursorXPosition != TheAnsiParser::Instance()->x_position-1 ||
        cursorYPosition != TheAnsiParser::Instance()->y_position-1)
    {
        return;
    }

    // Size of Each Char 8x16 or 16x32
    rect.w = cursorOnSurface->w;
    rect.h = cursorOnSurface->h;
    rect.x = cursorXPosition * characterWidth;
    rect.y = cursorYPosition * characterHeight;

    if(!globalTexture)
    {
        globalTexture =
            SDL_CreateTexture(globalRenderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                screenSurface->w, screenSurface->h);
        if (!globalTexture)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "renderCursorOnScreen() SDL_CreateTexture globalTexture: %s", SDL_GetError());
        }

        if (SDL_SetTextureBlendMode(globalTexture, SDL_BLENDMODE_BLEND) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "renderCursorOnScreen() SDL_SetTextureBlendMode globalTexture: %s", SDL_GetError());
        }
    }

    if (SDL_UpdateTexture(globalTexture, &rect,
        cursorOnSurface->pixels, cursorOnSurface->pitch) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "renderCursorOffScreen() SDL_SetTextureBlendMode globalTexture: %s", SDL_GetError());
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
    if(cursorXPosition != TheAnsiParser::Instance()->x_position-1 ||
        cursorYPosition != TheAnsiParser::Instance()->y_position-1)
    {
        return;
    }

    // Size of Each Char 8x16 or 16x32
    rect.w = cursorOffSurface->w;
    rect.h = cursorOffSurface->h;
    rect.x = cursorXPosition * characterWidth;
    rect.y = cursorYPosition * characterHeight;

    if(!globalTexture)
    {
        globalTexture =
            SDL_CreateTexture(globalRenderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                screenSurface->w, screenSurface->h);

        if (!globalTexture)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "renderCursorOffScreen() SDL_CreateTexture globalTexture: %s", SDL_GetError());
            return;
        }

        if (SDL_SetTextureBlendMode(globalTexture, SDL_BLENDMODE_BLEND) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "renderCursorOffScreen() SDL_SetTextureBlendMode globalTexture: %s", SDL_GetError());
        }
    }

    if (SDL_UpdateTexture(globalTexture, &rect,
        cursorOffSurface->pixels, cursorOffSurface->pitch) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "renderCursorOffScreen() SDL_UpdateTexture globalTexture: %s", SDL_GetError());
    }
}

/*
 * This hints or highlights the current screen
 * Doesn't exactly change Gama, was just testing!
 * Could be used lateron for something.
 */
void Terminal::drawGamaTextureScreen()
{
    // Next create a texture to overlay for highlighting
    // Create Initial Texture if it doesn't exist.
    if(!selectionTexture)
    {
        selectionTexture =
            SDL_CreateTexture(globalRenderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_TARGET,
                screenSurface->w, screenSurface->h);
        if (!selectionTexture)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "drawGamaTextureScreen() SDL_CreateTexture selectionTexture: %s", SDL_GetError());
            return;
        }

        if (SDL_SetTextureBlendMode(selectionTexture, SDL_BLENDMODE_MOD) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "drawGamaTextureScreen() SDL_SetTextureBlendMode selectionTexture: %s", SDL_GetError());
        }
    }

    // Set Render Target to the Texture.
    if (SDL_SetRenderTarget(globalRenderer, selectionTexture) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "drawGamaTextureScreen() SDL_SetRenderTarget selectionTexture: %s", SDL_GetError());
    }
    // Clear the Texture
    if (SDL_RenderClear(globalRenderer) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "drawGamaTextureScreen() SDL_RenderClear selectionTexture: %s", SDL_GetError());
    }
    // Fill new texture with Cyan Highlight.
    if (SDL_SetRenderDrawColor(globalRenderer, 255 , 255 , 255,  1) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "drawGamaTextureScreen() SDL_SetRenderDrawColor selectionTexture: %s", SDL_GetError());
    }
    // Can test filling to RECT to speed this up after it's working!
    if (SDL_RenderFillRect(globalRenderer, nullptr) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "drawGamaTextureScreen() SDL_RenderFillRect NULL: %s", SDL_GetError());
    }
    //Reset back to Global Render, then push update over current texture
    if (SDL_SetRenderTarget(globalRenderer, nullptr) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "drawGamaTextureScreen() SDL_SetRenderTarget NULL: %s", SDL_GetError());
    }
    // Render final update
    if (SDL_RenderCopy(globalRenderer, selectionTexture, nullptr, nullptr) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "drawGamaTextureScreen() SDL_RenderCopy globalRenderer: %s", SDL_GetError());
    }

    // Reset don't need it?
    //SDL_SetRenderDrawColor(globalRenderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderPresent(globalRenderer);

}


/**
 * Transfers the Buffer to a Texture and
 * Renders the Screen
 */
void Terminal::drawTextureScreen()
{
    SDL_Rect rect;
    int screenWidth, screenHeight;

    if (SDL_GetRendererOutputSize(globalRenderer,&screenWidth,&screenHeight) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "drawTextureScreen() SDL_GetRendererOutputSize globalRenderer: %s", SDL_GetError());
    }

    // We clip off bottom 80, so that we get proper 8x16
    // Display without Extra pixel borders around the screen,
    // Texture Filter results in Pixel Bleeding.
    rect.w = surfaceWidth  - 40; // 680 - 640 = 40
    rect.h = surfaceHeight - 80; // 480 - 400 = 80
    rect.x = 0,
    rect.y = 0;

    // Destination
    displayRect.w = screenWidth;
    displayRect.h = screenHeight;
    displayRect.x = 0;
    displayRect.y = 0;

    if (SDL_RenderCopy(globalRenderer, globalTexture, &rect, &displayRect) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "drawTextureScreen() SDL_RenderCopy globalRenderer: %s", SDL_GetError());
    }
    SDL_RenderPresent(globalRenderer);
}

/**
 * Clears the Renderer Screen for Fresh Redraw.
 * Renders the Screen
 */
void Terminal::clearScreen()
{
    //Clear screen
    if (SDL_SetRenderDrawColor(globalRenderer, 0x00, 0x00, 0x00, 0xFF) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "clearScreen() SDL_SetRenderDrawColor globalRenderer: %s", SDL_GetError());
    }
    if (SDL_RenderClear(globalRenderer) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "clearScreen() SDL_RenderClear globalRenderer: %s", SDL_GetError());
    }
    SDL_RenderPresent(globalRenderer);
}

/**
 * Compares (2) SDL Colors to determine if they are the same
 * Without conversions.
 */
int Terminal::compareSDL_Colors(SDL_Color &src, SDL_Color &dest)
{
    return (src.r == dest.r && src.g == dest.g && src.b == dest.b);
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
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "replaceColor() SDL_LockSurface src: %s", SDL_GetError());
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
    cursorXPosition = TheAnsiParser::Instance()->x_position-1;
    cursorYPosition = TheAnsiParser::Instance()->y_position-1;

    SDL_Rect pick, area;

    // Transform The underscore to a Cursor.
    int asciicode = '_';

    // Quad for Char Cell in Bitmap
    // 32 Cols by 8 Rows.
    pick.x = (asciicode % 32) * characterWidth;
    pick.y = round(asciicode / 32) * characterHeight;
    pick.w = characterWidth;
    pick.h = characterHeight;

    // Quad for Char Cell to Map to Screen
    area.x = cursorXPosition * characterWidth;
    area.y = cursorYPosition * characterHeight;
    area.w = characterWidth;
    area.h = characterHeight;

    // Clear Surfaces
    if (SDL_FillRect(tmpSurface,
        nullptr, SDL_MapRGB(tmpSurface->format, 0, 0, 0)) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "setupCursorChar() SDL_FillRect tmpSurface: %s", SDL_GetError());
    }
    if (SDL_FillRect(cursorOffSurface,
        nullptr, SDL_MapRGB(cursorOffSurface->format, 0, 0, 0)) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "setupCursorChar() SDL_FillRect cursorOffSurface: %s", SDL_GetError());
    }
    if (SDL_FillRect(cursorOnSurface,
        nullptr, SDL_MapRGB(cursorOnSurface->format, 0, 0, 0)) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "setupCursorChar() SDL_FillRect cursorOnSurface: %s", SDL_GetError());
    }

    //First Grab Underscore from Font Surface
    if (SDL_BlitSurface(chachedSurface,&pick,tmpSurface,nullptr) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "setupCursorChar() SDL_BlitSurface tmpSurface: %s", SDL_GetError());
    }

    // Grab current Char Block where Cursor position is at on screen
    if (SDL_BlitSurface(screenSurface,&area,cursorOnSurface,nullptr) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "setupCursorChar() SDL_BlitSurface cursorOnSurface: %s", SDL_GetError());
    }

    // Grab current Char Block where Passed Cursor position is.
    if (SDL_BlitSurface(screenSurface,&area,cursorOffSurface,nullptr) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "setupCursorChar() SDL_BlitSurface cursorOffSurface: %s", SDL_GetError());
    }

    // We'll Copy over the Underscore and overall it on the cursor on
    int bpp = screenSurface->format->BytesPerPixel;
    Uint8 *pixelTemp = (Uint8 *)tmpSurface->pixels;
    Uint8 *pixelNew  = (Uint8 *)cursorOnSurface->pixels;

    // Transpose the underscore character to the same
    // Character cell as the current x/y position were at.

    //std::cout << "show currentFont: " << currentFont << std::endl;

    if(currentFont == "vga8x16.bmp")
    {
        //std::cout << "current font VGA" << std::endl;
        // VGA font starts higher
        pixelTemp += (13 * tmpSurface->w)      * bpp;
        pixelNew  += (15 * cursorOnSurface->w) * bpp;
    }
    else
    {
        //std::cout << "current font NOT VGA" << std::endl;
        // Amiga fonts already at bottom of cell.
        pixelTemp += (15 * tmpSurface->w)      * bpp;
        pixelNew  += (15 * cursorOnSurface->w) * bpp;
    }

    if(SDL_MUSTLOCK(cursorOnSurface))
    {
        if(SDL_LockSurface(cursorOnSurface) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "setupCursorChar() SDL_LockSurface(cursorOnSurface): %s", SDL_GetError());
        }
    }

    // Copy (2) Rows of Pixels for the Cursor
    // from TmpSurface to CursorOn Surface.
    memcpy(pixelNew, pixelTemp, (tmpSurface->w * 2) * bpp);

    if(SDL_MUSTLOCK(cursorOnSurface))
        SDL_UnlockSurface(cursorOnSurface);

    // Render the initial cursor only if it's exposed!
    if (TheAnsiParser::Instance()->isCursorActive())
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
    pick.x = (asciicode % 32) * characterWidth;
    pick.y = round(asciicode / 32) * characterHeight;
    pick.w = characterWidth;
    pick.h = characterHeight;

    // Quad for Char Cell to Map to Screen
    area.x = X * characterWidth;
    area.y = Y * characterHeight;
    area.w = characterWidth;
    area.h = characterHeight;

    // Clear Temp Surface before blitting it.
    if (SDL_FillRect(tmpSurface, nullptr, SDL_MapRGB(tmpSurface->format, 0, 0, 0)) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "drawChar() SDL_FillRect tmpSurface: %s", SDL_GetError());
    }

    if (SDL_BlitSurface(chachedSurface,&pick,tmpSurface,nullptr) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "drawChar() SDL_BlitSurface tmpSurface: %s", SDL_GetError());
    }

    replaceColor(tmpSurface,
        SDL_MapRGB(tmpSurface->format, currentFGColor.r,
            currentFGColor.g, currentFGColor.b),
        SDL_MapRGB(tmpSurface->format, currentBGColor.r,
            currentBGColor.g, currentBGColor.b));

    // Write to Back Buffer for Scrolling the Screen.
    if (SDL_BlitSurface(tmpSurface,nullptr,screenSurface,&area) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
            "drawChar() SDL_BlitSurface screenSurface: %s", SDL_GetError());
    }
}

/**
 * Draws a String of Character Cells to x/y location on screen surface.
 * If String is longer then screen will wrap to next line.
 * NOTE Add scrolling if hits bottom of page,
 * But this will most likely be done in the ANSI Parser.
 */
void Terminal::drawString(int X, int Y, char text[])
{
    SDL_Rect area;

    int i = 0;
    int asciiCode;
    area.x = X;
    area.y = Y;

    for(i = 0; i < (signed)strlen(text); i++)
    {
        asciiCode = std::char_traits<char>().to_int_type(text[i]);
        drawChar(area.x / characterWidth, area.y / characterHeight, asciiCode);
        area.x += characterWidth;
        if(area.x >= surfaceWidth-5)
        {
            area.x = 0;
            area.y += characterHeight;
        }
    }
}

/**
 * Draws Character Set of each Character Cells to x/y location on screen surface.
 * If String is longer then screen will wrap to next line.
 * Mainly for testing all characters loaded from Bitmap Image.
 */
void Terminal::drawCharSet(int X, int Y)
{
    SDL_Rect area;
    int i = 0;
    int asciicode;
    area.x = X * characterWidth;
    area.y = Y * characterHeight;

    // Loop through Each Char in Bitmap.
    for(i = 1; i < 255; i++)
    {
        asciicode = i;
        if(asciicode == 0)
        {
            break;
        }
        drawChar(area.x / characterWidth, area.y / characterHeight, asciicode);
        area.x += characterWidth;
        // Wrap
        if(area.x >= surfaceWidth-5)
        {
            area.x = 0;
            area.y += characterHeight;
        }
    }
}
