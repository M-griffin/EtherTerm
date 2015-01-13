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
#include <SDL.h>
#elif _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <iostream>
#include <cmath>
#include <cstdio>

using namespace std;

Terminal* Terminal::globalInstance = 0;

Terminal::Terminal():
    globalWindow(0),
    globalRenderer(0),
    tmpSurface(0),       // Char Cell
    cursorOnSurface(0),  // Char Cell for the cursor.
    cursorOffSurface(0), // Char Cell for the cursor.
    screenSurface(0),    // Internal Screen Buffer
    bottomSurface(0),
    chachedSurface(0),  // Cached Font CharacterSet
    globalTexture(0),   // Texture for User Screen
    globalTermStateMachine(0),
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
    isRenderReady(0)

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
    previousFont = "vga8x16.bmp";
}

Terminal::~Terminal()
{
    // we must clean up after ourselves to prevent memory leaks
    globalRenderer = 0;
    globalWindow = 0;
    std::cout << "Term Released" << std::endl;
}

bool Terminal::init(const char* title,
                int swidth, int sheight,
                int wwidth, int wheight,
                int fwidth, int fheight)
{
    int flags = 0;
    // store the Term width and height
    surfaceWidth    = swidth;
    surfaceHeight   = sheight;
    windowWidth     = wwidth;
    windowHeight    = wheight;
    characterWidth  = fwidth;
    characterHeight = fheight;

    // Resizing offset pixel in texture resize, shaded blocks are uneven!
    //flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
    flags = SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;

    // attempt to initialise SDL
    //if(SDL_Init(SDL_INIT_EVERYTHING) == 0)
    if(SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        std::cout << "SDL init success" << std::endl;
        /* initialize SDL_net */
        if(SDLNet_Init()==-1)
        {
            std::cout << "SDLNet_Init: " << SDLNet_GetError() << std::endl;
            return false;
        }
        else
        {
            std::cout << "SDLNet_Init success" << std::endl;
        }
        /*
         * 0 = disable vsync
         * 1 = enable vsync
         */
        if(!SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0"))
        {
            std::cout << "Warning: VSync not enabled!\n";
        }
        /*
         * 0 or nearest = nearest pixel sampling
         * 1 or linear  = linear filtering       (supported by OpenGL and Direct3D)
         * 2 or best    = anisotropic filtering  (supported by Direct3D)
         */
        if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
        {
            std::cout << "Warning: Linear texture filtering not enabled!\n";
        }
        /*
         * 0 disable 3D acceleration
         * 1 enable 3D acceleration, using the default renderer
         * X enable 3D acceleration, using X where X is one of the valid
         *  rendering drivers. (e.g. "direct3d", "opengl", etc.)
         */
        if(!SDL_SetHint(SDL_HINT_FRAMEBUFFER_ACCELERATION, "1"))
        {
            std::cout << "Warning: Framebuffer Acceleration not enabled!\n";
        }

        // init the window
        globalWindow =
            SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, flags);
        if(globalWindow != 0) // window init success
        {
            std::cout << "window creation success\n";
            globalRenderer =
                SDL_CreateRenderer(globalWindow, -1, SDL_RENDERER_ACCELERATED |
                                   SDL_RENDERER_TARGETTEXTURE);
            if(globalRenderer != 0) // renderer init success
            {
                cout << "renderer creation success\n";
                SDL_SetRenderDrawColor(globalRenderer, 0x00, 0x00, 0x00, 0xFF);
            }
            else
            {
                std::cout << "renderer init fail\n";
                return false; // renderer init fail
            }

            // Increate the scaling to smooth without filtering.
            // Doesn't smooth anything!
            //SDL_RenderSetLogicalSize(globalRenderer,640*2,400*2);
            //SDL_RenderSetScale(globalRenderer, 0, 0);
        }
        else
        {
            std::cout << "window init fail\n";
            return false; // window init fail
        }
    }
    else
    {
        std::cout << "SDL init fail\n";
        return false; // SDL init fail
    }
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

void Terminal::render()
{
    globalTermStateMachine->render();
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

    std::cout << "SDL_Quit" << std::endl;
    SDL_Quit();
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

    SDL_RenderCopy(globalRenderer, globalTexture, &rectorig, &displayRect);

    // Next create a texture to overlay for highlighting
    if(!selectionTexture)
    {
        std::cout << "RenderCharScreen() - globalTexture" << std::endl;
        selectionTexture =
            SDL_CreateTexture(globalRenderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_TARGET,
                screenSurface->w, screenSurface->h);
        SDL_SetTextureBlendMode(selectionTexture, SDL_BLENDMODE_ADD);       
    }

    // Set Render Target to the Texture.
    SDL_SetRenderTarget(globalRenderer, selectionTexture);
    SDL_RenderClear(globalRenderer);

    // Fill new texture with Cyan Highlight.
    SDL_SetRenderDrawColor(globalRenderer, 0, 100 , 100, 255);

    // Can test filling to RECT to speed this up after it's working!
    SDL_RenderFillRect(globalRenderer, NULL);

    //Reset back to Global Render, then push update over current texture
    SDL_SetRenderTarget(globalRenderer, NULL);

    // We need to Translate the Screen Width and Height and translate to
    // get actual size of the Characters to snap everyone correctly.
    int charWidth, charHeight;
    charHeight = screenHeight / 25;
    charHeight += screenHeight % 25;
    charWidth = screenWidth / 80;
    charWidth += screenWidth % 80;

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
    // This is the Top Region
    if (sourceX < x)
    {
        // TOP Stationary -> Right ->> OK!
        rect.x = sourceX;
        rect.x -= rect.x % charWidth; // Snap to Left.
    }
    else
    {
        // Top -> Left ->> OK!
        rect.x = x;
        rect.x -= rect.x % charWidth;
    }

    if (sourceY < y)
    {
        // Top Stationary ->> OK!
        rect.y = sourceY;
        rect.y -= rect.y % charHeight; // Snap to Top
    }
    else
    {
        // Top -> Up ->> OK!
        rect.y = y;
        rect.y -= rect.y % charHeight;
    }

    // Width and height are calcuated by the different from source
    // Well need to round these values to the width and height of a character!
    // Also need to swap source for the lowest value so we can select both
    // above and behind the starting points.

    // This is the bottom Region
    if (sourceX < x)
    {
        // Bottom Width RIGHT ->> OK!
        rect.w = charWidth +
            ((x -= (x % charWidth)) - (sourceX -= sourceX % charWidth));
    }
    else
    {
        // Bottom Stationary ->> OK!
        rect.w = ((sourceX -= sourceX % charWidth)) - (rect.x);
    }
    if (sourceY < y)
    {
        // Bottom -> Down ->> OK!
        rect.h = charHeight +
            ((y -= (y % charHeight)) - (sourceY -= sourceY % charHeight));
    }
    else
    {
        // Bottom -> Stationary ->> OK!
        rect.h = ((sourceY -= sourceY % charWidth)) - (rect.y);
    }

    SDL_RenderCopy(globalRenderer, selectionTexture, NULL, &rect);

    // Reset dont need it?
    //SDL_SetRenderDrawColor(globalRenderer, 0x00, 0x00, 0x00, 0xFF);
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
    std::cout << std::endl << "*** Releaseing Surfaces and Textures" << std::endl;

    if (globalWindow)
    {
        std::cout << "Destroy SDL Window" << std::endl;
        SDL_DestroyWindow(globalWindow);
    }
    if (globalRenderer)
    {
        std::cout << "Destroy SDL Renderer" << std::endl;
        SDL_DestroyRenderer(globalRenderer);
    }

    if (chachedSurface)
    {
        std::cout << "SDL_FreeSurface chachedSurface" << std::endl;
        SDL_FreeSurface(chachedSurface);     // Bitmap    -> Fonts
    }
    if (screenSurface)
    {
        std::cout << "SDL_FreeSurface screenSurface" << std::endl;
        SDL_FreeSurface(screenSurface);      // Screen    -> Texture
    }
    if (tmpSurface)
    {
        std::cout << "SDL_FreeSurface tmpSurface" << std::endl;
        SDL_FreeSurface(tmpSurface);         // Char Cell -> Screen
    }
    if (cursorOffSurface)
    {
        std::cout << "SDL_FreeSurface cursorOffSurface" << std::endl;
        SDL_FreeSurface(cursorOffSurface);   // Cursor Off Character Cell
    }
    if (bottomSurface)
    {
        std::cout << "SDL_FreeSurface bottomSurface" << std::endl;
        SDL_FreeSurface(bottomSurface);      // Bottom Line of Scrolling Region.
    }
    //Free texture if it exists
    if (globalTexture)
    {
        std::cout << "SDL_DestroyTexture globalTexture" << std::endl;
        SDL_DestroyTexture(globalTexture);
    }
    if(selectionTexture)
    {
        std::cout << "SDL_DestroyTexture selectionTexture" << std::endl;
        SDL_DestroyTexture(selectionTexture);
    }

}
/*
 * Store Images later in a zip file.
 */
bool Terminal::loadBitmapImageFromPak()
{
    /*
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
    return chachedSurface != NULL;
}

/**
* Loads the Bitmap Font Image
* Default fonts are White on Black, inital setup for 32 cols by 8 rows
* of Characters Cells at 9x16.
*/
bool Terminal::loadBitmapImage(std::string path)
{
    //Load image at specified path
#ifdef _WIN32
    std::string realPath = "assets\\";
#else
    std::string realPath = "assets/";
#endif
    realPath += path;
    chachedSurface = SDL_LoadBMP(realPath.c_str());
    if(!chachedSurface)
    {
        std::cout << "Unable to SDL_LoadBMP: " << SDL_GetError() << std::endl;
        return false;
    }
    previousFont = currentFont;
    return chachedSurface != NULL;
}

/**
 * Initalize the Render, Loads the Fonts and Create the Surfaces.
 */
bool Terminal::initSurfaceTextures()
{
    //Initialization flag
    bool success = true;

    // Setup the Surface we will plot each character cell to.
    screenSurface =
        SDL_CreateRGBSurface(
            SDL_SWSURFACE, surfaceWidth, surfaceHeight, 32,
            redMask, greenMask, blueMask, alphaMask);
    if(!screenSurface)
    {
        std::cout << "screenSurface - CreateRGBSurface failed: "
                  <<  SDL_GetError() << std::endl;
        success = false;
    }
    screenSurface =
        SDL_ConvertSurfaceFormat(
            screenSurface, SDL_PIXELFORMAT_ARGB8888, 0);

    // Fill the Surface with Black to initalize it.
    SDL_FillRect(
        screenSurface, NULL,
        SDL_MapRGB(screenSurface->format, 0, 0, 0));

    // Setup Temp Surface that the size of each Character Cell.
    tmpSurface =
        SDL_CreateRGBSurface(
            SDL_SWSURFACE, characterWidth, characterHeight, 32,
            redMask, greenMask, blueMask, alphaMask);
    if(!tmpSurface)
    {
        std::cout << "tmpSurface - CreateRGBSurface failed: "
                  <<  SDL_GetError() << std::endl;
        success = false;
    }
    tmpSurface =
        SDL_ConvertSurfaceFormat(
            tmpSurface, SDL_PIXELFORMAT_ARGB8888, 0);

    // Setup Cursor Surface that the size of a Character Cell.
    // Holds the original Char when we overwrite it with an blinking cursor.
    cursorOnSurface =
        SDL_CreateRGBSurface(
            SDL_SWSURFACE, characterWidth, characterHeight, 32,
            redMask, greenMask, blueMask, alphaMask);
    if(!cursorOnSurface)
    {
        std::cout << "cursorOnSurface - CreateRGBSurface failed: "
                  << SDL_GetError() << std::endl;
        success = false;
    }
    cursorOnSurface =
        SDL_ConvertSurfaceFormat(
            cursorOnSurface, SDL_PIXELFORMAT_ARGB8888, 0);

    // Setup Cursor Surface that the size of a Character Cell.
    // Holds the original Char when we overwrite it with an blinking cursor.
    cursorOffSurface =
        SDL_CreateRGBSurface(
            SDL_SWSURFACE, characterWidth, characterHeight, 32,
            redMask, greenMask, blueMask, alphaMask);
    if(!cursorOffSurface)
    {
        std::cout << "cursorOffSurface - CreateRGBSurface failed: " <<
                  SDL_GetError() << std::endl;
        success = false;
    }
    cursorOffSurface =
        SDL_ConvertSurfaceFormat(
            cursorOffSurface, SDL_PIXELFORMAT_ARGB8888, 0);

    // Setup the Surface we will plot each character cell to.
    bottomSurface =
        SDL_CreateRGBSurface(
            SDL_SWSURFACE, screenSurface->w, characterHeight, 32,
            redMask, greenMask, blueMask, alphaMask);
    if(!bottomSurface)
    {
        std::cout << "bottomSurface - CreateRGBSurface failed: " <<
                  SDL_GetError() << std::endl;
        success = false;
    }
    bottomSurface =
        SDL_ConvertSurfaceFormat(
            bottomSurface, SDL_PIXELFORMAT_ARGB8888, 0);

    // Fill the Surface with Black to initalize it.
    SDL_FillRect(bottomSurface, NULL,
        SDL_MapRGB(bottomSurface->format, 0, 0, 0));

    // start the menu state
    globalTermStateMachine = new TermStateMachine();
    globalTermStateMachine->changeState(new MainMenuState());
    return success;
}

/*
 * Overides the Scrolling region of the screen
 */
void Terminal::setScrollRegion(int top, int bot)
{
    if(top == 0 && bot == 0)
        scrollRegionActive = FALSE;
    else
        scrollRegionActive = TRUE;
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
            return;
    }

    // Move the Entire Screen Up a 1 Row of Characters.
    memmove(pixelTopPos, pixelNewPos,
        (screenSurface->w *
        (characterHeight * (bottomMargin - topMargin))) * bpp);

    if(SDL_MUSTLOCK(screenSurface))
        SDL_UnlockSurface(screenSurface);

    // Clear out very last line
    SDL_FillRect(screenSurface, &area,
        SDL_MapRGB(screenSurface->format, 0, 0, 0));

    // If first Time, Crete Texture.
    if(!globalTexture)
    {
        globalTexture =
            SDL_CreateTexture(globalRenderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                screenSurface->w, screenSurface->h);
        SDL_SetTextureBlendMode(globalTexture, SDL_BLENDMODE_BLEND);
    }
    SDL_UpdateTexture(globalTexture, NULL,
        screenSurface->pixels, screenSurface->pitch);
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
            return;
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
        SDL_SetTextureBlendMode(globalTexture, SDL_BLENDMODE_BLEND);
    }
    // Write Scrolled screen to Texture.
    SDL_UpdateTexture(globalTexture, NULL,
        screenSurface->pixels, screenSurface->pitch);
}

/**
 * Clears the Surface Back Buffer, and Global Texture
 * Renderes the Screen Blank for new set of data.
 */
void Terminal::clearScreenSurface()
{
    SDL_FillRect(screenSurface, NULL,
                 SDL_MapRGB(screenSurface->format, 0, 0, 0));

    if(!globalTexture)
    {
        std::cout << "ClearScreenSurface() - globalTexture" << std::endl;
        globalTexture =
            SDL_CreateTexture(globalRenderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                screenSurface->w, screenSurface->h);
    }
    SDL_UpdateTexture(globalTexture, NULL,
        screenSurface->pixels, screenSurface->pitch);
    SDL_RenderClear(globalRenderer);
}


/**
 * Fill entire lines with space and black foregroud/Background
 * Updates super fast now!! oh yea!!
 */
void Terminal::renderClearLineScreen(int y, int start, int end)
{
    // Save Current color so we can switch back to it.
    SDL_Color originalGB = currentBGColor;

    // Clear out entire line.
    for(int i = start; i < end; i++)
    {
        drawChar(i, y, 32);
        renderCharScreen(i, y);
    }
    currentBGColor = originalGB;
}

/**
 * Bascially Plots each Char to a Texture
 * Updates super fast now!!
 * So were only writting the to the bottom line of the screen only
 * And not redrawing the entire screen each time
 */
void Terminal::renderBottomScreen()
{
    SDL_Rect pick;
    SDL_Rect rect;

    if(!bottomSurface)
    {
        std::cout << "bottomSurface - CreateRGBSurface failed: " <<
            SDL_GetError() << std::endl;
    }

    pick.w = screenSurface->w;
    pick.h = characterHeight;
    pick.x = 0;
    if(scrollRegionActive)
        pick.y = characterHeight * (bottomMargin-1);
    else
        pick.y = characterHeight * 24;

    rect.w = bottomSurface->w;
    rect.h = bottomSurface->h;
    rect.x = 0;
    if(scrollRegionActive)
        rect.y = characterHeight * (bottomMargin-1);
    else
        rect.y = characterHeight * 24;

    SDL_FillRect(bottomSurface, NULL,
                 SDL_MapRGB(bottomSurface->format, 0, 0, 0));
    SDL_BlitSurface(screenSurface ,&pick, bottomSurface, NULL);
    if(!globalTexture)
    {
        std::cout << "RenderBottomScreen() - globalTexture" << std::endl;
        globalTexture =
            SDL_CreateTexture(globalRenderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                screenSurface->w, screenSurface->h);
        SDL_SetTextureBlendMode(globalTexture, SDL_BLENDMODE_BLEND);
    }
    SDL_UpdateTexture(globalTexture, &rect,
        bottomSurface->pixels, bottomSurface->pitch);
}

/**
 * Bascially Plots each Char to Texture
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
        std::cout << "RenderScreen() - Creating globalTexture" << std::endl;
        globalTexture =
            SDL_CreateTexture(globalRenderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                screenSurface->w, screenSurface->h);
        SDL_SetTextureBlendMode(globalTexture, SDL_BLENDMODE_BLEND);
    }
    SDL_UpdateTexture(globalTexture, &pick,
        screenSurface->pixels, screenSurface->pitch);
}

/**
 * Bascially Plots each Char to Texture
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
        std::cout << "RenderCharScreen() - globalTexture" << std::endl;
        globalTexture =
            SDL_CreateTexture(globalRenderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                screenSurface->w, screenSurface->h);
        SDL_SetTextureBlendMode(globalTexture, SDL_BLENDMODE_BLEND);
    }
    SDL_UpdateTexture(globalTexture, &rect,
        screenSurface->pixels, screenSurface->pitch);
}

/**
 * Bascially Plots each Char to Texture
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
        std::cout << "RenderCursorOnScreen() - globalTexture" << std::endl;
        globalTexture =
            SDL_CreateTexture(globalRenderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                screenSurface->w, screenSurface->h);
        SDL_SetTextureBlendMode(globalTexture, SDL_BLENDMODE_BLEND);
    }
    SDL_UpdateTexture(globalTexture, &rect,
        cursorOnSurface->pixels, cursorOnSurface->pitch);
}

/**
 * Bascially Plots each Char to Texture
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
        std::cout << "RenderCursorOffScreen() - globalTexture" << std::endl;
        globalTexture =
            SDL_CreateTexture(globalRenderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                screenSurface->w, screenSurface->h);
        SDL_SetTextureBlendMode(globalTexture, SDL_BLENDMODE_BLEND);
    }
    SDL_UpdateTexture(globalTexture, &rect,
        cursorOffSurface->pixels, cursorOffSurface->pitch);
}

/*
 * This is a proof of concept that will brighten
 * Screen displayed after Texturefiltering is applied
 * since texture filtering slightly dims colors to leave
 * a blurred feeling
 */
void Terminal::drawGamaTextureScreen()
{

    // Next create a texture to overlay for highlighting
    if(!selectionTexture)
    {
        std::cout << "drawGamaTextureScreen() - globalTexture" << std::endl;
        selectionTexture =
            SDL_CreateTexture(globalRenderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_TARGET,
                screenSurface->w, screenSurface->h);
        SDL_SetTextureBlendMode(selectionTexture, SDL_BLENDMODE_MOD);
    }

    // Set Render Target to the Texture.
    SDL_SetRenderTarget(globalRenderer, selectionTexture);
    SDL_RenderClear(globalRenderer);

    // Fill new texture with Cyan Highlight.
    SDL_SetRenderDrawColor(globalRenderer, 255 , 255 , 255,  1);

    // Can test filling to RECT to speed this up after it's working!
    SDL_RenderFillRect(globalRenderer, NULL);

    //Reset back to Global Render, then push update over current texture
    SDL_SetRenderTarget(globalRenderer, NULL);


    SDL_RenderCopy(globalRenderer, selectionTexture, NULL, NULL);

    // Reset dont need it?
    //SDL_SetRenderDrawColor(globalRenderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderPresent(globalRenderer);

}


/**
 * Transfers the Buffer to a Texture and
 * Renderes the Screen
 */
void Terminal::drawTextureScreen()
{
    SDL_Rect rect;
    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(globalRenderer,&screenWidth,&screenHeight);

    // We clip off botom 80, so that we get proper 8x16
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

    SDL_RenderCopy(globalRenderer, globalTexture, &rect, &displayRect);
    SDL_RenderPresent(globalRenderer);

    // Testing only for increasing brightness!
    //drawGamaTextureScreen();
}

/**
 * Clears the Renderer Screen for Fresh Redraw.
 * Renderes the Screen
 */
void Terminal::clearScreen()
{
    //Clear screen
    SDL_SetRenderDrawColor(globalRenderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(globalRenderer);
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
 * Replace Foreground and Backgroud colors per character cell by pixel.
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
            return;
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
 * This grabs the current cell of the cusor position
 * Then builder on and off character surfaces.
 */
void Terminal::setupCursorChar()
{
    // Check if the position has changed.
    if(cursorXPosition != TheAnsiParser::Instance()->x_position-1 ||
            cursorYPosition != TheAnsiParser::Instance()->y_position-1)
    {
        cursorXPosition = TheAnsiParser::Instance()->x_position-1;
        cursorYPosition = TheAnsiParser::Instance()->y_position-1;
    }
    else
        return;

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
    SDL_FillRect(tmpSurface,
                 NULL, SDL_MapRGB(tmpSurface->format, 0, 0, 0));
    SDL_FillRect(cursorOffSurface,
                 NULL, SDL_MapRGB(cursorOffSurface->format, 0, 0, 0));
    SDL_FillRect(cursorOnSurface,
                 NULL, SDL_MapRGB(cursorOnSurface->format, 0, 0, 0));

    //First Grab UnderScore from Font Surface
    SDL_BlitSurface(chachedSurface,&pick,tmpSurface,NULL);

    // Grab current Char Block where Cursor position is at on screen
    SDL_BlitSurface(screenSurface,&area,cursorOnSurface,NULL);

    // Grab curent Char Block where Passed Cursor position is.
    SDL_BlitSurface(screenSurface,&area,cursorOffSurface,NULL);

    // We'll Copy over the Underscore and overall it on the cursor on
    int bpp = screenSurface->format->BytesPerPixel;
    Uint8 *pixelTemp = (Uint8 *)tmpSurface->pixels;
    Uint8 *pixelNew  = (Uint8 *)cursorOnSurface->pixels;

    // Transpose the underscore character to the same
    // Character cell as the current x/y position were at.
    if(currentFont == "vga8x16.bmp")
    {
        // VGA font starts higher
        pixelTemp += (13 * tmpSurface->w)      * bpp;
        pixelNew  += (15 * cursorOnSurface->w) * bpp;
    }
    else
    {
        // Amiga fonts already at bottom of cell.
        pixelTemp += (15 * tmpSurface->w)      * bpp;
        pixelNew  += (15 * cursorOnSurface->w) * bpp;
    }

    if(SDL_MUSTLOCK(cursorOnSurface))
    {
        if(SDL_LockSurface(cursorOnSurface) < 0)
            return;
    }

    // Copy (2) Rows of Pixels for the Cursor
    // from TmpSurface to CursorOn Surface.
    memcpy(pixelNew, pixelTemp, (tmpSurface->w * 2) * bpp);

    if(SDL_MUSTLOCK(cursorOnSurface))
        SDL_UnlockSurface(cursorOnSurface);

    // Draw the inital cursor on the screen, this is needed
    // Right away for Echo'ed input.
    TheTerminal::Instance()->renderCursorOnScreen();
    TheTerminal::Instance()->drawTextureScreen();
}

/**
 * Draws a Char Cell to x/y loction on screen surface.
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
    SDL_FillRect(tmpSurface, NULL, SDL_MapRGB(tmpSurface->format, 0, 0, 0));
    SDL_BlitSurface(chachedSurface,&pick,tmpSurface,NULL);

    replaceColor(tmpSurface,
        SDL_MapRGB(tmpSurface->format, currentFGColor.r,
            currentFGColor.g, currentFGColor.b),
        SDL_MapRGB(tmpSurface->format, currentBGColor.r,
            currentBGColor.g, currentBGColor.b));

    // Write to Back Buffer for Scrolling the Screen.
    SDL_BlitSurface(tmpSurface,NULL,screenSurface,&area);
}

/**
 * Draws a String of Character Cells to x/y loction on screen surface.
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
        area.x=area.x+characterWidth;
        if(area.x >= surfaceWidth-5)
        {
            area.x = 0;
            area.y += characterHeight;
        }
    }
}

/**
 * Draws Character Set of each Character Cells to x/y loction on screen surface.
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
        area.x = area.x + characterWidth;
        // Wrap
        if(area.x >= surfaceWidth-5)
        {
            area.x = 0;
            area.y += characterHeight;
        }
    }
}
