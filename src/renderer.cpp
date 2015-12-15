#include "renderer.hpp"

#ifdef TARGET_OS_MAC
#include <SDL2/SDL.h>
#ifdef _DEBBUG
#include <SDL2/SDL_ttf.h>
#endif
#elif _WIN32
#include <SDL2/SDL.h>
#ifdef _DEBBUG
#include <SDL2/SDL_ttf.h>
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


Renderer::Renderer(surface_manager_ptr surface,
                   window_manager_ptr window,
                   input_handler_ptr input)
    : m_surface_manager(surface)
    , m_window_manager(window)
    , m_input_handler(input)
    , BLACK( {   0,   0,   0,   0 })
, BLUE( {   0,   0, 171,   0 })
, GREEN( {   0, 171,   0,   0 })
, CYAN( {   0, 171, 171,   0 })
, RED( { 171,   0,   0,   0 })
, MAGENTA( { 171,   0, 171,   0 })
, BROWN( { 171,  87,   0,   0 })
, GREY( { 171, 171, 171,   0 })
, DARK_GREY( {  87,  87,  87,   0 })
, LIGHT_BLUE( {  87,  87, 255,   0 })
, LIGHT_GREEN( {  87, 255,  87,   0 })
, LIGHT_CYAN( {  87, 255, 255,   0 })
, LIGHT_RED( { 255,  87,  87,   0 })
, LIGHT_MAGENTA( { 255,  87, 255,   0 })
, YELLOW( { 255, 255,  87,   0 })
, WHITE( { 255, 255, 255,   0 })
, m_currentFGColor(GREY)
, m_currentBGColor(BLACK)
, m_termWidth(80)
, m_termHeight(25)
, m_scrollRegionActive(false)
, m_topMargin(0)
, m_bottomMargin(0)
, m_cursorXPosition(0)
, m_cursorYPosition(0)
, m_isUTF8Output(false)
{
    // Startup Surface and Texture Creation
    initSurfaceTextures();
}

Renderer::~Renderer()
{
    std::cout << "~Renderer" << std::endl;
}

/**
 * @brief Startup Creation of Screen Surfaces in Memory
 * NOTE: Scaled Surface only screated when needed!
 */
void Renderer::initSurfaceTextures()
{
    // Main Screen Surface
    m_surface_manager->createSurface(m_surface_manager->SURFACE_MAIN_SCREEN);
    if(!m_surface_manager->surfaceExists(m_surface_manager->SURFACE_MAIN_SCREEN))
    {
        SDL_Log("initSurfaceTextures() surfaceExists SURFACE_MAIN_SCREEN: %s",
                SDL_GetError());
        assert(false);
    }

    // Bottom Row of Main Screen Surface (Scrolling)
    m_surface_manager->createSurface(m_surface_manager->SURFACE_BOTTOM_ROW);
    if(!m_surface_manager->surfaceExists(m_surface_manager->SURFACE_BOTTOM_ROW))
    {
        SDL_Log("initSurfaceTextures() surfaceExists SURFACE_BOTTOM_ROW: %s",
                SDL_GetError());
        assert(false);
    }

    // Indivdiual Character Cell Surface
    m_surface_manager->createSurface(m_surface_manager->SURFACE_CHARACTER);
    if(!m_surface_manager->surfaceExists(m_surface_manager->SURFACE_CHARACTER))
    {
        SDL_Log("initSurfaceTextures() surfaceExists SURFACE_CHARACTER: %s",
                SDL_GetError());
        assert(false);
    }

    // Indivdiual Character Cell Cursor On
    m_surface_manager->createSurface(m_surface_manager->SURFACE_CURSOR_ON);
    if(!m_surface_manager->surfaceExists(m_surface_manager->SURFACE_CURSOR_ON))
    {
        SDL_Log("initSurfaceTextures() surfaceExists SURFACE_CURSOR_ON: %s",
                SDL_GetError());
        assert(false);
    }

    // Indivdiual Character Cell Cursor Off
    m_surface_manager->createSurface(m_surface_manager->SURFACE_CURSOR_OFF);
    if(!m_surface_manager->surfaceExists(m_surface_manager->SURFACE_CURSOR_OFF))
    {
        SDL_Log("initSurfaceTextures() surfaceExists SURFACE_CURSOR_OFF: %s",
                SDL_GetError());
        assert(false);
    }
}

/**
 * @brief Calcuate the Box Dimensions for Copy/Paste Selection.
 * @param rect
 */
void Renderer::calcBoxSize(SDL_Rect &rect,
                           int sourceX, int sourceY,
                           int x, int y,
                           double charWidth, double charHeight)
{
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
}

/**
 * @brief Translates Screen Coordinates to ScreenBuffer for Text.
 * @param x
 * @param y
 */
void Renderer::pullSelectionBuffer(int x, int y)
{
    int screenWidth, screenHeight;
    m_window_manager->renderOutputSize(screenWidth, screenHeight);

    // We need to Translate the Screen Width vs Rows and Width to
    // get actual the grid size of the Characters to snap everything correctly.
    double charWidth, charHeight;
    double value;

    value = (double)screenHeight / (double)m_termHeight;
    charHeight = value; //round(abs(value));

    value = (double)screenWidth / (double)m_termWidth;
    charWidth = value; //round(abs(value));

    // First we need to convert the current Screen size to 640x400
    // So we can calcuate the actual pixel size of each resized character cell.
    SDL_Rect rect;

    // We clip off botom 80, so that we get proper 8x16
    // Display without Extra pixel borders around the screen,
    // Texture Filter results in Pixel Bleeding.

    int sourceX = m_input_handler->getMouseSourceXPosition();
    int sourceY = m_input_handler->getMouseSourceYPosition();

    // Calcuate the Box Dimensions
    calcBoxSize(rect, sourceX, sourceY, x, y, charWidth, charHeight);

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
    /*
     * Rework once the ansi parser is reworked!
     *
    TheSequenceParser::Instance()->bufferToClipboard(
        startColumn, startRow, length, numberRows);*/
}

/**
 * @brief Clear so texture is refreshed each selection
 */
void Renderer::clearSelectionTexture()
{
    if(m_surface_manager->textureExists(m_surface_manager->TEXTURE_SELECTION))
    {
        m_surface_manager->delTexture(m_surface_manager->TEXTURE_SELECTION);
    }
}

/**
 * @brief On selected mouse movement area! We want to overlay a blended texture
 * @param x
 * @param y
 */
void Renderer::renderSelectionScreen(int x, int y)
{
    // Redraw screen so we don't increase lighting intensitity
    // As the selection keeps redrawing!  Cool effect though!!
    SDL_Rect rectorig;

    // Get Handles to Surface and Textures
    SDL_Surface *surface = m_surface_manager
                           ->m_surfaceList[m_surface_manager->SURFACE_MAIN_SCREEN]
                           ->getSurface();

    SDL_Texture *texture = m_surface_manager
                           ->m_textureList[m_surface_manager->TEXTURE_MAIN_SCREEN]
                           ->getTexture();

    // Get The Actual size of the Render/Window.
    int screenWidth, screenHeight;
    m_window_manager->renderOutputSize(screenWidth, screenHeight);

    // We clip off botom 80, so that we get proper 8x16
    // Display without Extra pixel borders around the screen,
    // Texture Filter results in Pixel Bleeding.
    rectorig.w = surface->w; // - 40; // 680 - 640 = 40
    rectorig.h = surface->h; // - 80; // 480 - 400 = 80
    rectorig.x = 0;
    rectorig.y = 0;

    // Destination
    m_displayRect.w = screenWidth;
    m_displayRect.h = screenHeight;
    m_displayRect.x = 0;
    m_displayRect.y = 0;

    // Redraw the screen between selection changes so we have nothing left
    // from previous coordinates.
    m_window_manager->renderCopy(
        texture,
        &rectorig,
        &m_displayRect
    );

    // Next create a texture to overlay for highlighting
    if(!m_surface_manager->textureExists(m_surface_manager->TEXTURE_SELECTION))
    {
        // Create A New Texture using the Main Screen Surface.
        m_surface_manager->createTexture(
            m_surface_manager->TEXTURE_SELECTION,
            surface
        );

        // On need to set this the first time!!
        // Set Render Target to the Texture.
        m_window_manager->setRenderTarget(texture);

        // Clear The Renderer
        m_window_manager->clearRender();

        // Set the Color of the Selection Texture
        m_window_manager->setRenderDrawColor(0, 50 , 50, 255);

        // Can test if filling RECT is needed to speed this up after it's working!
        m_window_manager->renderFill(nullptr);

        //Reset back to Global Render, then push update over current texture
        m_window_manager->setRenderTarget(nullptr);
    }

    // We need to Translate the Screen Width vs Rows and Width to
    // get actual the grid size of the Characters to snap everything correctly.
    double charWidth, charHeight, value;

    value = (double)screenHeight / double(m_termHeight);
    charHeight = value;
    value = (double)screenWidth / double(m_termWidth);
    charWidth = value;

    // First we need to convert the current Screen size to 640x400
    // So we can calcuate the actual pixel size of each resized character cell.
    SDL_Rect rect;
    int sourceX = m_input_handler->getMouseSourceXPosition();
    int sourceY = m_input_handler->getMouseSourceYPosition();

    // Calculate the Boxsize for drawing, fill rect!
    calcBoxSize(rect, sourceX, sourceY, x, y, charWidth, charHeight);

    // Draw First Highlight Overlay
    m_window_manager->renderCopy(
        texture,
        nullptr,
        &rect
    );

    // Now draw over and add a
    // 2 Pixel expanded shaded border (I like it!)
    rect.x -= 4;
    rect.y -= 4;
    rect.w += 8;
    rect.h += 8;

    // Draw Border Overlay
    // Draw First Highlight Overlay
    m_window_manager->renderCopy(
        texture,
        nullptr,
        &rect
    );

    // Draw Out Renderer.
    m_window_manager->renderPresent();
}

/**
 * @Brief Overrides the Scrolling region of the screen
 */
void Renderer::setScrollRegion(int top, int bot, int terminalHeight)
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

/**
 * @brief Scroll a specific set region of the screen only
 */
void Renderer::scrollRegionUp()
{
    SDL_Rect area;

    // Clear Out last line after Scrolling up.
    area.x = 0;
    area.y = m_surface_manager->m_characterHeight * (m_bottomMargin-1);
    area.w = m_surface_manager->m_surfaceWidth;
    area.h = m_surface_manager->m_characterHeight;

    // Get handle to the Surface
    SDL_Surface *surface = m_surface_manager
                           ->m_surfaceList[m_surface_manager->SURFACE_MAIN_SCREEN]
                           ->getSurface();

    int bpp = surface->format->BytesPerPixel;
    Uint8 *pixelNewPos = (Uint8 *)surface->pixels;
    Uint8 *pixelTopPos = (Uint8 *)surface->pixels;

    // Move position to start
    pixelTopPos += surface->w * (m_surface_manager->m_characterHeight * (m_topMargin -1)) * bpp;

    // Start from One line below the Top Margin.
    pixelNewPos += surface->w * (m_surface_manager->m_characterHeight * m_topMargin) * bpp;

    // Lock the Surface to Modify Pixels
    m_surface_manager->lockSurface(m_surface_manager->SURFACE_MAIN_SCREEN);

    // Move the Entire Screen Up a 1 Row of Characters.
    memmove(pixelTopPos, pixelNewPos,
            (surface->w * (m_surface_manager->m_characterHeight * (m_bottomMargin - m_topMargin))) * bpp);

    // Unlock when modification is done.
    m_surface_manager->unlockSurface(m_surface_manager->SURFACE_MAIN_SCREEN);

    // Clear out very last line of surface region.
    m_window_manager->renderFill(&area);
    
    // Update Pixels in the Texture
    m_window_manager->updateTexture(
        m_surface_manager
        ->m_textureList[m_surface_manager->TEXTURE_MAIN_SCREEN]
        ->getTexture(),
        nullptr,
        surface
    );    
}

/*
 * Clips out the Top Row of the Screen,
 * Then Moves Entire Screen Up one Row to Scroll it.
 */
void Renderer::scrollScreenUp()
{
    if(!m_screenSurface)
    {
        SDL_Log("scrollScreenUp() m_screenSurface: %s",
                SDL_GetError());
        return;
    }

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
void Renderer::clearScreenSurface()
{
    if(!m_screenSurface)
    {
        SDL_Log("clearScreenSurface() m_screenSurface: %s",
                SDL_GetError());
    }

    // Fill screen with current RGB Background colors.
    if(SDL_FillRect(m_screenSurface, nullptr,
                    SDL_MapRGBA(m_screenSurface->format,
                                m_currentBGColor.r, m_currentBGColor.g, m_currentBGColor.b, 0)) < 0)
        //SDL_MapRGB(m_screenSurface->format,
        //           m_currentBGColor.r, m_currentBGColor.g, m_currentBGColor.b)) < 0)
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
void Renderer::renderClearLineScreen(int y, int start, int end)
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
void Renderer::renderClearLineAboveScreen(int y, int x)
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
void Renderer::renderClearLineBelowScreen(int y, int x)
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
void Renderer::renderDeleteCharScreen(int x, int y, int num)
{
    SDL_Rect pick;
    SDL_Rect dest;

    if(!m_screenSurface)
    {
        SDL_Log("renderDeleteCharScreen() m_screenSurface: %s",
                SDL_GetError());
    }

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
void Renderer::renderBottomScreen()
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
                    SDL_MapRGBA(m_bottomSurface->format, 0, 0, 0, 0)) < 0)
        //SDL_MapRGB(m_bottomSurface->format, 0, 0, 0)) < 0)
    {
        SDL_Log("renderBottomScreen() SDL_FillRect bottomSurface: %s",
                SDL_GetError());
        return;
    }

    if(!m_screenSurface)
    {
        SDL_Log("renderBottomScreen() m_screenSurface: %s",
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
void Renderer::renderScreen()
{
    SDL_Rect pick;

    pick.x = 0;
    pick.y = 0;
    pick.w = m_surfaceWidth;
    pick.h = m_surfaceHeight;

    if(!m_screenSurface)
    {
        SDL_Log("renderBottomScreen() m_screenSurface: %s",
                SDL_GetError());
        return;
    }

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
void Renderer::renderCharScreen(int x, int y)
{
    SDL_Rect rect;

    rect.w = m_tmpSurface->w;
    rect.h = m_tmpSurface->h;
    rect.x = x * m_characterWidth;
    rect.y = y * m_characterHeight;

    if(!m_screenSurface)
    {
        SDL_Log("renderCharScreen() m_screenSurface: %s",
                SDL_GetError());
        return;
    }

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
void Renderer::renderCursorOnScreen()
{
    SDL_Rect rect;

    // Check if the position has changed, if so, then skip!
    if(m_cursorXPosition != TheSequenceParser::Instance()->x_position-1 ||
            m_cursorYPosition != TheSequenceParser::Instance()->y_position-1)
    {
        return;
    }

    if(!m_screenSurface || !m_cursorOnSurface)
    {
        SDL_Log("renderCursorOnScreen() surface error: %s",
                SDL_GetError());
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
void Renderer::renderCursorOffScreen()
{
    SDL_Rect rect;

    // Check if the position has changed, if so, then skip!
    if(m_cursorXPosition != TheSequenceParser::Instance()->x_position-1 ||
            m_cursorYPosition != TheSequenceParser::Instance()->y_position-1)
    {
        return;
    }

    if(!m_screenSurface || !m_cursorOnSurface)
    {
        SDL_Log("renderCursorOffScreen() surface error: %s",
                SDL_GetError());
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
void Renderer::drawTextureScreen()
{
    SDL_Rect rect;
    int screenWidth, screenHeight;

    if(m_globalRenderer)
    {
        if(SDL_GetRendererOutputSize(m_globalRenderer,&screenWidth,&screenHeight) < 0)
        {
            SDL_Log("drawTextureScreen() SDL_GetRendererOutputSize globalRenderer: %s",
                    SDL_GetError());
        }

        if(!m_globalTexture)
        {
            SDL_Log("drawTextureScreen() m_globalTexture error: %s",
                    SDL_GetError());
            return;
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
    else
    {
        SDL_Log("drawTextureScreen() globalRenderer: %s",
                SDL_GetError());
    }
}

/**
 * Clears the Terminal Screen for Fresh Redraw.
 * Renders the Screen
 */
void Renderer::clearScreen()
{
    if(m_globalRenderer)
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
    else
    {
        SDL_Log("clearScreen() globalRenderer: %s",
                SDL_GetError());
    }
}

/**
 * Compares (2) SDL Colors to determine if they are the same
 * Without conversions.
 */
int Renderer::compareSDL_Colors(SDL_Color &src, SDL_Color &dest)
{
    return (src.r == dest.r &&
            src.g == dest.g &&
            src.b == dest.b);
}

/**
 * Replace Foreground and Background colors per character cell by pixel.
 */
void Renderer::replaceColor(SDL_Surface *src, Uint32 foreground, Uint32 background)
{
    if(!src)
    {
        SDL_Log("replaceColor() SDL_Surface error: %s",
                SDL_GetError());
        return;
    }

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
void Renderer::setupCursorChar()
{
    if(!m_tmpSurface || !m_cachedSurface)
    {
        SDL_Log("setupCursorChar() m_tmpSurface error: %s",
                SDL_GetError());
        return;
    }

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
    if(SDL_BlitSurface(m_cachedSurface, &pick, m_tmpSurface, nullptr) < 0)
    {
        SDL_Log("setupCursorChar() SDL_BlitSurface tmpSurface: %s",
                SDL_GetError());
    }

    if(!m_screenSurface)
    {
        SDL_Log("setupCursorChar() m_screenSurface error: %s",
                SDL_GetError());
        return;
    }

    // Grab current Char Block where Cursor position is at on screen
    if(SDL_BlitSurface(m_screenSurface, &area, m_cursorOnSurface, nullptr) < 0)
    {
        SDL_Log("setupCursorChar() SDL_BlitSurface cursorOnSurface: %s",
                SDL_GetError());
    }

    // Grab current Char Block where Passed Cursor position is.
    if(SDL_BlitSurface(m_screenSurface, &area, m_cursorOffSurface, nullptr) < 0)
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
        TheRenderer::Instance()->renderCursorOnScreen();
        TheRenderer::Instance()->drawTextureScreen();
    }
}

/**
 * Draws a Char Cell to x/y location on screen surface.
 */
void Renderer::drawChar(int X, int Y, int asciicode)
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

    if(!m_tmpSurface)
    {
        SDL_Log("drawChar() m_tmpSurface error: %s",
                SDL_GetError());

        // Create it, shouldn't be free!
        createSurface(TEMP_SURFACE);
        return;
    }

    // Clear Temp Surface before blitting it.
    if(SDL_FillRect(m_tmpSurface, nullptr, SDL_MapRGBA(m_tmpSurface->format, 0, 0, 0, 0)) < 0)
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
