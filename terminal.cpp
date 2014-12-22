// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "terminal.h"
#include "termObjectFactory.h"
#include "mainMenuState.h"

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

Term* Term::globalInstance = 0;

Term::Term():
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

    // Initalize Defaults
    CHAR_WIDTH  = 8;
    CHAR_HEIGHT = 16;

    // Not yet setup!
    globalFontFiles.push_back("vga8x16.bmp");
    globalFontFiles.push_back("topaz2-8x16.bmp");
    globalFontFiles.push_back("topazPlus-8x16.bmp");

    // Set defaults;
    clearSystemConnection();
    currentFont = "vga8x16.bmp";
    previousFont = "vga8x16.bmp";

}

Term::~Term()
{
    // we must clean up after ourselves to prevent memory leaks
    globalRenderer = 0;
    globalWindow = 0;
}


bool Term::init(const char* title, int swidth, int sheight, int wwidth, int wheight)
{
    int flags = 0;

    // store the Term width and height
    surfaceWidth = swidth;
    surfaceHeight = sheight;

    windowWidth = wwidth;
    windowHeight = wheight;

    flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;

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
        if( !SDL_SetHint( SDL_HINT_RENDER_VSYNC, "0" ) )
        {
            std::cout << "Warning: VSync not enabled!\n";
        }

        /*
         * 0 or nearest = nearest pixel sampling
         * 1 or linear  = linear filtering       (supported by OpenGL and Direct3D)
          * 2 or best    = anisotropic filtering  (supported by Direct3D)
         */
        if( !SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2") )
        {
            std::cout << "Warning: Linear texture filtering not enabled!\n";
        }

        /*
         * 0 disable 3D acceleration
         * 1 enable 3D acceleration, using the default renderer
         * X enable 3D acceleration, using X where X is one of the valid rendering drivers. (e.g. "direct3d", "opengl", etc.)
         */
        if( !SDL_SetHint(SDL_HINT_FRAMEBUFFER_ACCELERATION, "1") )
        {
            std::cout << "Warning: Framebuffer Acceleration not enabled!\n";
        }

        // init the window
        globalWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, flags);

        if(globalWindow != 0) // window init success
        {
            std::cout << "window creation success\n";
            globalRenderer = SDL_CreateRenderer(globalWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE );

            if(globalRenderer != 0) // renderer init success
            {
                cout << "renderer creation success\n";
                SDL_SetRenderDrawColor( globalRenderer, 0x00, 0x00, 0x00, 0xFF );
            }
            else
            {
                std::cout << "renderer init fail\n";
                return false; // renderer init fail
            }
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

void Term::render()
{
    globalTermStateMachine->render();
}

void Term::update()
{
    globalTermStateMachine->update();
}

void Term::handleEvents()
{
    //Handle Keyboard Input,
    //Also Check Sockets here.
    //TheInputHandler::Instance()->update()
}

void Term::clean()
{
    cout << "cleaning Term\n";

    // Clear Surfaces and Textures.
    Close();

    // Shutdown SDL_Sockets.
    SDLNet_Quit();

    /*
    TheInputHandler::Instance()->clean();
    */

    globalFontFiles.clear();
    vector<std::string>().swap(globalFontFiles);

    globalTermStateMachine->clean();

    globalTermStateMachine = 0;
    delete globalTermStateMachine;

    SDL_DestroyWindow(globalWindow);
    SDL_DestroyRenderer(globalRenderer);
    SDL_Quit();
}

/*
 *
 * Now for Rendering Code
 *
 */
bool Term::loadBitmapImageFromPak()
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
        //printf("unzReadCurrentFile() Error (%d != %d)\n", result, info.uncompressed_size);
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
bool Term::loadBitmapImage( std::string path )
{
    //Load image at specified path
#ifdef _WIN32
    std::string realPath = "assets\\";
#else
    std::string realPath = "assets/";
#endif
    realPath += path;
    chachedSurface = SDL_LoadBMP(realPath.c_str());
    if (!chachedSurface)
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
bool Term::InitSurfaceTextures()
{
    //Initialization flag
    bool success = true;

    // Setup the Surface we will plot each character cell to.
    screenSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, surfaceWidth, surfaceHeight, 32,
                               redMask, greenMask, blueMask, alphaMask);

    screenSurface = SDL_ConvertSurfaceFormat( screenSurface, SDL_PIXELFORMAT_ARGB8888, 0 );

    // Fill the Surface with Black to initalize it.
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0, 0, 0));


    // Setup Temp Surface that the size of each Character Cell.
    tmpSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, CHAR_WIDTH, CHAR_HEIGHT, 32,
                               redMask, greenMask, blueMask, alphaMask);
    if(!tmpSurface)
    {
        std::cout << "tmpSurface - CreateRGBSurface failed: " <<  SDL_GetError() << std::endl;
        success = false;
    }

    tmpSurface = SDL_ConvertSurfaceFormat( tmpSurface, SDL_PIXELFORMAT_ARGB8888, 0 );

    // Setup Cursor Surface that the size of a Character Cell.
    // Holds the original Char when we overwrite it with an blinking cursor.
    cursorOnSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, CHAR_WIDTH, CHAR_HEIGHT, 32,
                               redMask, greenMask, blueMask, alphaMask);
    if(!cursorOnSurface)
    {
        std::cout << "cursorOnSurface - CreateRGBSurface failed: " << SDL_GetError() << std::endl;
        success = false;
    }

    cursorOnSurface = SDL_ConvertSurfaceFormat( cursorOnSurface, SDL_PIXELFORMAT_ARGB8888, 0 );

    // Setup Cursor Surface that the size of a Character Cell.
    // Holds the original Char when we overwrite it with an blinking cursor.
    cursorOffSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, CHAR_WIDTH, CHAR_HEIGHT, 32,
                               redMask, greenMask, blueMask, alphaMask);
    if(!cursorOffSurface)
    {
        std::cout << "cursorOffSurface - CreateRGBSurface failed: " <<  SDL_GetError() << std::endl;
        success = false;
    }

    cursorOffSurface = SDL_ConvertSurfaceFormat( cursorOffSurface, SDL_PIXELFORMAT_ARGB8888, 0 );

    // Setup the Surface we will plot each character cell to.
    bottomSurface = SDL_CreateRGBSurface(SDL_SWSURFACE,screenSurface->w, CHAR_HEIGHT, 32,
                               redMask, greenMask, blueMask, alphaMask);

    bottomSurface = SDL_ConvertSurfaceFormat( bottomSurface, SDL_PIXELFORMAT_ARGB8888, 0 );

    // Fill the Surface with Black to initalize it.
    SDL_FillRect(bottomSurface, NULL, SDL_MapRGB(bottomSurface->format, 0, 0, 0));

    // start the menu state
    globalTermStateMachine = new TermStateMachine();
    globalTermStateMachine->changeState(new MainMenuState());

    return success;
}

//
void Term::Close()
{

    // Clear Cached Surface.
    SDL_FreeSurface( chachedSurface );   // Bitmap      -> Fonts
    SDL_FreeSurface( screenSurface );    // Screen    -> Texture
    SDL_FreeSurface( tmpSurface );       // Char Cell -> Screen
    SDL_FreeSurface( cursorOnSurface );  // Cursor On Character Cell
    SDL_FreeSurface( cursorOffSurface ); // Cursor Off Character Cell
    SDL_FreeSurface( bottomSurface );    // Bottom Line of Scrolling Region.

    //Free texture if it exists
    SDL_DestroyTexture( globalTexture );

}

/*
 * Overides the Scrolling region of the screen
 */
void Term::SetScrollRegion(int top, int bot)
{
    if (top == 0 && bot == 0)
        scrollRegionActive = FALSE;
    else
        scrollRegionActive = TRUE;

    topMargin = top;
    botMargin = bot;

    //printf("\r\nScroll Region TOP: %i, BOT: %i",top, bot);
}

/*
 * Scroll a specific set region of the screen only
 */
void Term::ScrollRegionUp ()
{

    //SDL_Rect pick, pick2, pick3;
    SDL_Rect pick3;

/*
    // Source, Grab One Row Down Though Bottom
    pick.x = 0;
    pick.y = CHAR_HEIGHT * (topMargin);
    pick.w = surfaceWidth;
    pick.h = CHAR_HEIGHT * (botMargin - topMargin);

    // ---------------------------------------
    // Destination at top of Screen
    pick2.x = 0;
    pick2.y = CHAR_HEIGHT * (topMargin-1);
    pick2.w = surfaceWidth;
    pick2.h = CHAR_HEIGHT * (botMargin - topMargin);
    */

    // Clear Out last line after Scrolling up.
    pick3.x = 0;
    pick3.y = CHAR_HEIGHT * (botMargin-1);
    pick3.w = surfaceWidth;
    pick3.h = CHAR_HEIGHT;

    // Not needed Anymore.
    //SDL_BlitSurface(screenSurface,&pick,screenSurface,&pick2);
    //SDL_FillRect(screenSurface, &pick3, SDL_MapRGB(screenSurface->format, 0, 0, 0));

    int bpp = screenSurface->format->BytesPerPixel;
    Uint8 *pNewPos = (Uint8 *)screenSurface->pixels + 0 * screenSurface->pitch + 0 * bpp;
    Uint8 *pTopPos = (Uint8 *)screenSurface->pixels + 0 * screenSurface->pitch + 0 * bpp;

    // Move position to start
    pTopPos += screenSurface->w * (CHAR_HEIGHT * (topMargin -1)) * bpp;

    // Start from One line below the Top Margin.
    pNewPos += screenSurface->w * (CHAR_HEIGHT * topMargin) * bpp;

    if (SDL_MUSTLOCK (screenSurface))
    {
        if (SDL_LockSurface (screenSurface) < 0)
            return;
    }

    // Move the Entire Screen Up a 1 Row of Characters.
    memmove(pTopPos, pNewPos, (screenSurface->w * (CHAR_HEIGHT * (botMargin - topMargin))) * bpp);

    if (SDL_MUSTLOCK (screenSurface))
        SDL_UnlockSurface (screenSurface);

    // Clear out very last line
    SDL_FillRect(screenSurface, &pick3, SDL_MapRGB(screenSurface->format, 0, 0, 0));

    // If first Time, Crete Texture.
    if (!globalTexture)
    {
        globalTexture = SDL_CreateTexture( globalRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, screenSurface->w, screenSurface->h );
        SDL_SetTextureBlendMode( globalTexture, SDL_BLENDMODE_BLEND );
    }

    //SDL_UpdateTexture(globalTexture, &pick2, screenSurface->pixels, screenSurface->pitch);
    SDL_UpdateTexture(globalTexture, NULL, screenSurface->pixels, screenSurface->pitch);

}


/*
 * Clips out the Top Row of the Screen,
 * Then Moves Entire Screen Up one Row to Scroll it.
 */
void Term::ScrollScreenUp ()
{
    if (scrollRegionActive)
    {
        ScrollRegionUp();
        return;
    }

    int bpp = screenSurface->format->BytesPerPixel;
    Uint8 *pNewPos = (Uint8 *)screenSurface->pixels + 0 * screenSurface->pitch + 0 * bpp;
    Uint8 *pOldPos = (Uint8 *)screenSurface->pixels + 0 * screenSurface->pitch + 0 * bpp;

    // Move position to start of 2nd Line, Then copying greater line down to previous.
    pNewPos += (screenSurface->w * CHAR_HEIGHT) * bpp;

    if (SDL_MUSTLOCK (screenSurface))
    {
        if (SDL_LockSurface (screenSurface) < 0)
            return;
    }

    // Move the Entire Screen Up a 1 Row of Characters.
    memmove(pOldPos, pNewPos, (screenSurface->w * (screenSurface->h - CHAR_HEIGHT)) * bpp);

    if (SDL_MUSTLOCK (screenSurface))
        SDL_UnlockSurface (screenSurface);

    if (!globalTexture)
    {
        globalTexture = SDL_CreateTexture( globalRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, screenSurface->w, screenSurface->h );
        SDL_SetTextureBlendMode( globalTexture, SDL_BLENDMODE_BLEND );
    }

    // Write Scrolled screen to Texture.
    SDL_UpdateTexture(globalTexture, NULL, screenSurface->pixels, screenSurface->pitch);
}



/**
 * Clears the Surface Back Buffer, and Global Texture
 * Renderes the Screen Blank for new set of data.
 */
void Term::ClearScreenSurface()
{
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0, 0, 0));

    if (!globalTexture)
    {
        std::cout << "ClearScreenSurface() - Creating globalTexture" << std::endl;
        globalTexture = SDL_CreateTexture( globalRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, screenSurface->w, screenSurface->h );
    }

    SDL_UpdateTexture(globalTexture, NULL, screenSurface->pixels, screenSurface->pitch);
    SDL_RenderClear( globalRenderer );
}


/**
 * Fill entire lines with space and black foregroud/Background
 * Updates super fast now!! oh yea!!
 */
void Term::RenderClearLineScreen(int y, int start, int end)
{
    // Save Current color so we can switch back to it.
    SDL_Color originalGB = currentBGColor;

    // Clear out entire line.
    for (int i = start; i < end; i++)
    {
        DrawChar(i, y, 32);
        RenderCharScreen(i, y);
    }

    currentBGColor = originalGB;
}



/**
 * Bascially Plots each Char to Texture
 * Updates super fast now!! oh yea!!
 * So were only writting the currnet lines to the scrren
 * And now redrawing the entire screen or since char at a time.
 */
void Term::RenderBottomScreen()
{

    SDL_Rect pick; SDL_Rect rect;

    pick.w = screenSurface->w;
    pick.h = CHAR_HEIGHT;
    pick.x = 0;

    if (scrollRegionActive)
        pick.y = CHAR_HEIGHT * (botMargin-1);
    else
        pick.y = CHAR_HEIGHT * 24;

    if(!bottomSurface)
    {
        std::cout << "bottomSurface - CreateRGBSurface failed: " << SDL_GetError() << std::endl;
    }

    rect.w = bottomSurface->w;
    rect.h = bottomSurface->h;
    rect.x = 0;

    if (scrollRegionActive)
        rect.y = CHAR_HEIGHT * (botMargin-1);
    else
        rect.y = CHAR_HEIGHT * 24;

    SDL_FillRect(bottomSurface, NULL, SDL_MapRGB(bottomSurface->format, 0, 0, 0));
    SDL_BlitSurface(screenSurface ,&pick, bottomSurface, NULL);

    if (!globalTexture)
    {
        std::cout << "RenderBottomScreen() - Creating globalTexture" << std::endl;
        globalTexture = SDL_CreateTexture( globalRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, screenSurface->w, screenSurface->h );
        SDL_SetTextureBlendMode( globalTexture, SDL_BLENDMODE_BLEND );
    }

    SDL_UpdateTexture(globalTexture, &rect, bottomSurface->pixels, bottomSurface->pitch);
}



/**
 * Bascially Plots each Char to Texture
 * Updates super fast now!! oh yea!!
 */
void Term::RenderScreen()
{
    SDL_Rect pick;

    pick.x = 0;
    pick.y = 0;
    pick.w = surfaceWidth;
    pick.h = surfaceHeight; // - 80;

    if (!globalTexture)
    {
        std::cout << "RenderScreen() - Creating globalTexture" << std::endl;
        globalTexture = SDL_CreateTexture( globalRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, screenSurface->w, screenSurface->h );
        SDL_SetTextureBlendMode( globalTexture, SDL_BLENDMODE_BLEND );
    }

    SDL_UpdateTexture(globalTexture, &pick, screenSurface->pixels, screenSurface->pitch);
}


/**
 * Bascially Plots each Char to Texture
 * Updates super fast now!! oh yea!!
 */
void Term::RenderCharScreen(int x, int y)
{
    SDL_Rect rect;

    // Size of Each Char 8x16
    rect.w = tmpSurface->w;
    rect.h = tmpSurface->h;
    rect.x = x * CHAR_WIDTH;
    rect.y = y * CHAR_HEIGHT;

    if (!globalTexture)
    {
        std::cout << "RenderCharScreen() - Creating globalTexture" << std::endl;
        globalTexture = SDL_CreateTexture( globalRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, screenSurface->w, screenSurface->h );
        SDL_SetTextureBlendMode( globalTexture, SDL_BLENDMODE_BLEND );
    }

    SDL_UpdateTexture(globalTexture, &rect, screenSurface->pixels, screenSurface->pitch);

}

/**
 * Bascially Plots each Char to Texture
 * Updates super fast now!! oh yea!!
 */
void Term::RenderCursorOnScreen(int x, int y)
{
    SDL_Rect rect;

    // Size of Each Char 8x16
    rect.w = cursorOnSurface->w;
    rect.h = cursorOnSurface->h;
    rect.x = x * CHAR_WIDTH;
    rect.y = y * CHAR_HEIGHT;

    if (!globalTexture)
    {
        std::cout << "RenderCursorOnScreen() - Creating globalTexture" << std::endl;
        globalTexture = SDL_CreateTexture( globalRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, screenSurface->w, screenSurface->h );
        SDL_SetTextureBlendMode( globalTexture, SDL_BLENDMODE_BLEND );
    }

    SDL_UpdateTexture(globalTexture, &rect, cursorOnSurface->pixels, cursorOnSurface->pitch);
}

/**
 * Bascially Plots each Char to Texture
 * Updates super fast now!! oh yea!!
 */
void Term::RenderCursorOffScreen(int x, int y)
{
    SDL_Rect rect;

    // Size of Each Char 8x16
    rect.w = cursorOffSurface->w;
    rect.h = cursorOffSurface->h;
    rect.x = x * CHAR_WIDTH;
    rect.y = y * CHAR_HEIGHT;

    if (!globalTexture)
    {
        std::cout << "RenderCursorOffScreen() - Creating globalTexture" << std::endl;
        globalTexture = SDL_CreateTexture( globalRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, screenSurface->w, screenSurface->h );
        SDL_SetTextureBlendMode( globalTexture, SDL_BLENDMODE_BLEND );
    }

    SDL_UpdateTexture(globalTexture, &rect, cursorOffSurface->pixels, cursorOffSurface->pitch);
}

/**
 * Transfers the Buffer to a Texture and
 * Renderes the Screen
 */
void Term::DrawTextureScreen()
{
    SDL_Rect rect;

    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(globalRenderer,&screenWidth,&screenHeight);

    // Source
    // We clip off botom 80, so that we get proper 8x16
    // Display without Extra pixel borders around the screen,
    rect.w = 640;
    rect.h = 400;
    rect.x = 0,
    rect.y = 0;

    // Destination
    displayRect.w = screenWidth;
    displayRect.h = screenHeight;
    displayRect.x = 0;
    displayRect.y = 0;

    SDL_RenderCopy(globalRenderer, globalTexture, &rect, &displayRect);
    SDL_RenderPresent(globalRenderer);
}

/**
 * Clears the Renderer Screen for Fresh Redraw.
 * Renderes the Screen
 */
void Term::ClearScreen()
{
    //Clear screen
    SDL_SetRenderDrawColor( globalRenderer, 0x00, 0x00, 0x00, 0xFF );
    SDL_RenderClear( globalRenderer );
    SDL_RenderPresent( globalRenderer );
}

/**
 * Compares (2) SDL Colors to determine if they are the same
 * Without conversions.
 */
int Term::CompareSDL_Colors(SDL_Color &src, SDL_Color &dest)
{
    return (src.r == dest.r && src.g == dest.g && src.b == dest.b);
}

/**
 * Replace Foreground and Backgroud colors per character cell by pixel.
 */
void Term::ReplaceColor (SDL_Surface *src, Uint32 foreground, Uint32 background)
{

    static Uint32 fgColor = SDL_MapRGB (src->format, 255, 255, 255); // White
    static Uint32 bgColor = SDL_MapRGB (src->format, 0,   0,   0);   // Black

    if (fgColor == foreground && bgColor == background)
        return;

    int bpp = src->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)src->pixels + 0 * src->pitch + 0 * bpp;

    if (SDL_MUSTLOCK (src))
    {
        if (SDL_LockSurface (src) < 0)
            return;
    }

    for (int x = 0; x < src->w; x ++)
    {
        for (int y = 0; y < src->h; y ++)
        {
            if (*(Uint32 *) p == fgColor)
                *(Uint32 *) p = foreground;
            else
            if (*(Uint32 *) p == bgColor)
                *(Uint32 *) p = background;

            // Incriment 4 bits. (BPP)
            p += 4;

            // Update Pixel Color as long as it's not already the current
            // Color of the Bitmap Image
            //if (GetPixel (src, x, y) == fgColor && foreground != fgColor)
                //PutPixel (src, x, y, foreground);
            //else if (GetPixel (src, x, y) == bgcolor && background !=  bgcolor)
                //PutPixel (src, x, y, background);
        }
    }

    if (SDL_MUSTLOCK (src))
        SDL_UnlockSurface (src);

}

/**
 * Draws a Char Cell to x/y loction on screen surface.
 */
void Term::SetupCursorChar(int X, int Y)
{
    SDL_Rect pick, area, cursor, cursor2;
    int asciicode = '_';

    // Quad for Char Cell in Bitmap
    // 32 Cols by 8 Rows.
    pick.x = (asciicode % 32) * CHAR_WIDTH;
    pick.y = round(asciicode / 32) * CHAR_HEIGHT;

    pick.w = CHAR_WIDTH;
    pick.h = CHAR_HEIGHT;

    // Quad for Char Cell to Map to Screen
    area.x = X * CHAR_WIDTH;
    area.y = Y * CHAR_HEIGHT;
    area.w = CHAR_WIDTH;
    area.h = CHAR_HEIGHT;

    // One pixel row of Underscore
    // Underscore character it too high
    // So we clip it, and lower it
    // After we get the oringal Character block
    // then draw the cursor on top of it.

    cursor.x = 0;
    cursor.y = 13;
    cursor.w = CHAR_WIDTH;
    cursor.h = 2;

    cursor2.x = 0;
    cursor2.y = 15;
    cursor2.w = CHAR_WIDTH;
    cursor2.h = 2;

    // Clear Surfaces
    SDL_FillRect(tmpSurface,       NULL, SDL_MapRGB(tmpSurface->format, 0, 0, 0));
    SDL_FillRect(cursorOffSurface, NULL, SDL_MapRGB(cursorOffSurface->format, 0, 0, 0));
    SDL_FillRect(cursorOnSurface,  NULL, SDL_MapRGB(cursorOnSurface->format, 0, 0, 0));

    // Setup and Copy CURSOR Character to Temp Surface
    SDL_BlitSurface(chachedSurface,&pick,tmpSurface,NULL);
    SDL_BlitSurface(screenSurface,&area,cursorOnSurface,NULL);
    SDL_BlitSurface(screenSurface,&area,cursorOffSurface,NULL);
    SDL_BlitSurface(tmpSurface,&cursor,cursorOnSurface,&cursor2);
}

/**
 * Draws a Char Cell to x/y loction on screen surface.
 */
void Term::DrawChar(int X, int Y, int asciicode)
{
    SDL_Rect pick, area;

    // Quad for Char Cell in Bitmap
    // 32 Cols by 8 Rows.
    pick.x = (asciicode % 32) * CHAR_WIDTH;
    pick.y = round(asciicode / 32) * CHAR_HEIGHT;

    pick.w = CHAR_WIDTH;
    pick.h = CHAR_HEIGHT;

    // Quad for Char Cell to Map to Screen
    area.x = X * CHAR_WIDTH;
    area.y = Y * CHAR_HEIGHT;
    area.w = CHAR_WIDTH;
    area.h = CHAR_HEIGHT;

    // Clear Temp Surface before blitting it.
    SDL_FillRect(tmpSurface, NULL, SDL_MapRGB(tmpSurface->format, 0, 0, 0));
    SDL_BlitSurface(chachedSurface,&pick,tmpSurface,NULL);

    ReplaceColor (tmpSurface,
            SDL_MapRGB (tmpSurface->format, currentFGColor.r, currentFGColor.g, currentFGColor.b),
            SDL_MapRGB (tmpSurface->format, currentBGColor.r, currentBGColor.g, currentBGColor.b));


    // Write to Back Buffer for Scrolling the Screen.
    SDL_BlitSurface(tmpSurface,NULL,screenSurface,&area);
}

/**
 * Draws a String of Character Cells to x/y loction on screen surface.
 * If String is longer then screen will wrap to next line.
 * NOTE Add scrolling if hits bottom of page,
 * But this will most likely be done in the ANSI Parser.
 */
void Term::DrawString(int X, int Y, char text[])
{
    SDL_Rect area;

    int i = 0;
    int asciiCode;
    area.x = X;
    area.y = Y;

    //Per la lunghezza della stringa
    for (i=0; i < (signed)strlen(text); i++)
    {
        asciiCode = std::char_traits<char>().to_int_type(text[i]);

        // 9x16 Dimension
        DrawChar(area.x / CHAR_WIDTH, area.y / CHAR_HEIGHT, asciiCode);
        area.x=area.x+CHAR_WIDTH;

        if (area.x >= surfaceWidth-5)
        {
            area.x = 0; area.y += CHAR_HEIGHT;
        }
    }
}

/**
 * Draws Character Set of each Character Cells to x/y loction on screen surface.
 * If String is longer then screen will wrap to next line.
 * Mainly for testing all characters loaded from Bitmap Image.
 */
void Term::DrawCharSet(int X, int Y)
{
    SDL_Rect area;

    int i=0;
    int asciicode;
    area.x = X * CHAR_WIDTH;
    area.y = Y * CHAR_HEIGHT;

    // Loop through Each Char in Bitmap.
    for (i=1; i < 255; i++)
    {
        asciicode = i; //std::char_traits<char>().to_int_type(text[i]);
        if (asciicode == 0 )
        {
            break;
        }

        // 9x16 Dimensions.
        DrawChar(area.x / CHAR_WIDTH, area.y / CHAR_HEIGHT, asciicode);
        area.x = area.x + CHAR_WIDTH;

        // Wrap
        if (area.x >= surfaceWidth-5)
        {
            area.x = 0;
            area.y += CHAR_HEIGHT;
        }
    }
}

