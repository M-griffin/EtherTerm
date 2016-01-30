#include "renderer.hpp"
#include "session.hpp"

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
                   window_manager_ptr  window,
                   input_handler_ptr   input,
                   session_ptr         session)
    : m_surface_manager(surface)
    , m_window_manager(window)
    , m_input_handler(input)
    , m_weak_session(session)
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
    , m_current_fg_color(GREY)
    , m_current_bg_color(BLACK)
    , m_term_width(80)
    , m_term_height(25)
    , m_is_scroll_region_active(false)
    , m_top_margin(0)
    , m_bottom_margin(0)
    , m_cursor_x_position(0)
    , m_cursor_y_position(0)
    , m_is_scalling_surface(false)
    , m_is_utf8_output(false)
{
    std::cout << "Renderer Created" << std::endl;
    // Startup Surface and Texture Creation
    initSurfaceTextures();
}

Renderer::~Renderer()
{
    std::cout << "~Renderer" << std::endl;
}

/**
 * @brief Startup Creation of Screen Surfaces in Memory
 * NOTE: Scaled Surface only created when needed!
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

    // Individual Character Cell Surface
    m_surface_manager->createSurface(m_surface_manager->SURFACE_CHARACTER);
    if(!m_surface_manager->surfaceExists(m_surface_manager->SURFACE_CHARACTER))
    {
        SDL_Log("initSurfaceTextures() surfaceExists SURFACE_CHARACTER: %s",
                SDL_GetError());
        assert(false);
    }

    // Individual Character Cell Cursor On
    m_surface_manager->createSurface(m_surface_manager->SURFACE_CURSOR_ON);
    if(!m_surface_manager->surfaceExists(m_surface_manager->SURFACE_CURSOR_ON))
    {
        SDL_Log("initSurfaceTextures() surfaceExists SURFACE_CURSOR_ON: %s",
                SDL_GetError());
        assert(false);
    }

    // Individual Character Cell Cursor Off
    m_surface_manager->createSurface(m_surface_manager->SURFACE_CURSOR_OFF);
    if(!m_surface_manager->surfaceExists(m_surface_manager->SURFACE_CURSOR_OFF))
    {
        SDL_Log("initSurfaceTextures() surfaceExists SURFACE_CURSOR_OFF: %s",
                SDL_GetError());
        assert(false);
    }

    // Create the Main Screen Texture, Matches the Window size.
    // Individual Character Cell Cursor Off
    m_surface_manager->createTexture(
        m_surface_manager->TEXTURE_MAIN_SCREEN,
        m_surface_manager->m_surfaceList[m_surface_manager->SURFACE_MAIN_SCREEN]->getSurface()
    );
    if(!m_surface_manager->textureExists(m_surface_manager->TEXTURE_MAIN_SCREEN))
    {
        SDL_Log("initSurfaceTextures() textureExists TEXTURE_MAIN_SCREEN: %s",
                SDL_GetError());
        assert(false);
    }
}

/**
 * @brief Calculate the Box Dimensions for Copy/Paste Selection.
 * @param rect
 */
void Renderer::calcBoxSize(SDL_Rect &rect,
                           int sourceX, int sourceY,
                           int x, int y,
                           double char_width, double char_height)
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
        rect.x = floor((double)sourceX / char_width) * char_width;
    }
    else
    {
        // Top -> Left ->> OK!
        rect.x = floor((double)x / char_width) * char_width;
    }

    if(sourceY < y)
    {
        // Top Stationary ->> OK!
        rect.y = floor((double)sourceY / char_height) * char_height;
    }
    else
    {
        // Top -> Up ->> OK!
        rect.y = floor((double)y / char_height) * char_height;
    }

    // Width and height are calculated by the different from motion to source
    // Well need to round these values to the width and height of a character!
    // Also need to swap source for the lowest value so we can select both
    // above and behind the starting points.

    // This is the bottom Region
    if(sourceX < x)
    {
        // Bottom Width RIGHT ->> OK!
        rect.w = char_width + (floor((double)x / char_width) * char_width) -  rect.x ;
    }
    else
    {
        // Bottom Stationary ->> OK!
        rect.w = char_width + (floor((double)sourceX / char_width) * char_width) - rect.x; // back
    }
    if(sourceY < y)
    {
        // Bottom -> Down ->> OK!
        rect.h = char_height + (floor((double)y / char_height) * char_height) - rect.y;
    }
    else
    {
        // Bottom -> Stationary ->> OK!
        rect.h = char_height + (floor((double)sourceY / char_height) * char_height) - rect.y; // back
    }

    // If were in between lines, Height = 0, then add char Height to the bottom
    // So we always have a row!
    if(rect.h == 0)
        rect.h += round(char_height);

    if(rect.w == 0)
        rect.w += round(char_width);
}

/**
 * @brief Translates Screen Coordinates to ScreenBuffer for Text.
 * @param x
 * @param y
 */
void Renderer::pullSelectionBuffer(int x, int y)
{
    int screen_width, screen_height;
    m_window_manager->renderOutputSize(screen_width, screen_height);

    // We need to Translate the Screen Width vs Rows and Width to
    // get actual the grid size of the Characters to snap everything correctly.
    double char_width, char_height;
    double value;

    value = (double)screen_height / (double)m_term_height;
    char_height = value; //round(abs(value));

    value = (double)screen_width / (double)m_term_width;
    char_width = value; //round(abs(value));

    // First we need to convert the current Screen size to 640x400
    // So we can calculate the actual pixel size of each resized character cell.
    SDL_Rect rect;

    // We clip off bottom 80, so that we get proper 8x16
    // Display without Extra pixel borders around the screen,
    // Texture Filter results in Pixel Bleeding.

    int sourceX = m_input_handler->getMouseSourceXPosition();
    int sourceY = m_input_handler->getMouseSourceYPosition();

    // Calculate the Box Dimensions
    calcBoxSize(rect, sourceX, sourceY, x, y, char_width, char_height);

    // Now that we have the exact coordinates of the selected text.
    // We need to translate this to the screenbuffer positions
    // So that we can grab the text that was selected on the screen.

    // Get Starting Position
    int startColumn = rect.x / char_width;
    int startRow    = rect.y / char_height;
    // Now figure out how many character per row
    int length      = rect.w / char_width;
    // Now figure out how many rows of character to pull
    int numberRows  = rect.h / char_height;

    // Use coords to pull screen text directly from screen buffer.
    session_ptr session = m_weak_session.lock();
    if(session)
    {
        // Push the Text Buffer to the Clipboard.
        session->m_sequence_parser->m_screen_buffer.bufferToClipboard(
            startColumn,
            startRow,
            length,
            numberRows
        );
    }
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
    // Redraw screen so we don't increase lighting intensity
    // As the selection keeps redrawing!  Cool effect though!!
    SDL_Rect rect_source, rect_dest;

    // Get Handles to Surface and Textures
    SDL_Surface *surface = m_surface_manager
                           ->m_surfaceList[m_surface_manager->SURFACE_MAIN_SCREEN]
                           ->getSurface();

    SDL_Texture *texture;

    // Get The Actual size of the Render/Window.
    int screen_width, screen_height;
    m_window_manager->renderOutputSize(screen_width, screen_height);

    // We clip off bottom 80, so that we get proper 8x16
    // Display without Extra pixel borders around the screen,
    // Texture Filter results in Pixel Bleeding. (NOT USED RIGHT NOW!)
    rect_source.w = surface->w; // - 40; // 680 - 640 = 40
    rect_source.h = surface->h; // - 80; // 480 - 400 = 80
    rect_source.x = 0;
    rect_source.y = 0;

    // Destination
    rect_dest.w = screen_width;
    rect_dest.h = screen_height;
    rect_dest.x = 0;
    rect_dest.y = 0;

    // Redraw the screen between selection changes so we have nothing left
    // from previous coordinates.
    m_window_manager->renderCopy(
        m_surface_manager
        ->m_textureList[m_surface_manager->TEXTURE_MAIN_SCREEN]
        ->getTexture(),
        &rect_source,
        &rect_dest
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
        // Set Render Target to the Selection Texture.
        m_window_manager->setRenderTarget(
            m_surface_manager
            ->m_textureList[m_surface_manager->TEXTURE_SELECTION]
            ->getTexture()
        );

        // Clear The Renderer
        m_window_manager->renderClear();

        // Set the Color of the Selection Texture
        m_window_manager->setRenderDrawColor(0, 50 , 50, 255);

        // Can test if filling RECT is needed to speed this up after it's working!
        m_window_manager->renderFill(nullptr);

        //Reset back to Global Render, then push update over current texture
        m_window_manager->setRenderTarget(nullptr);
    }

    texture = m_surface_manager
              ->m_textureList[m_surface_manager->TEXTURE_SELECTION]
              ->getTexture();

    // We need to Translate the Screen Width vs Rows and Width to
    // get actual the grid size of the Characters to snap everything correctly.
    double char_width, char_height, value;

    value = (double)screen_height / double(m_term_height);
    char_height = value;
    value = (double)screen_width / double(m_term_width);
    char_width = value;

    // First we need to convert the current Screen size to 640x400
    // So we can calculate the actual pixel size of each resized character cell.
    SDL_Rect rect;
    int sourceX = m_input_handler->getMouseSourceXPosition();
    int sourceY = m_input_handler->getMouseSourceYPosition();

    // Calculate the Boxsize for drawing, fill rect!
    calcBoxSize(rect, sourceX, sourceY, x, y, char_width, char_height);

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
void Renderer::setScrollRegion(int top, int bot, int term_height)
{
    // If Region is larger then the Terminal,
    // Ignore it!
    if(top == 1 && bot >= term_height)
    {
        m_is_scroll_region_active = false;
        return;
    }
    // 0,0 is reset.
    if(top == 0 && bot == 0)
    {
        m_is_scroll_region_active = false;
        return;
    }
    else
    {
        // Make sure Bottom Region
        // Is the same as our terminal length.
        if(bot > term_height)
        {
            bot = term_height;
        }
        m_is_scroll_region_active = true;
    }
    // Set Scrolling Margins
    m_top_margin = top;
    m_bottom_margin = bot;
}

/**
 * @brief Scroll a specific set region of the screen only
 */
void Renderer::scrollRegionUp()
{
    // Get handle to the Surface
    SDL_Surface *surface = m_surface_manager
                           ->m_surfaceList[m_surface_manager->SURFACE_MAIN_SCREEN]
                           ->getSurface();

    int bpp = surface->format->BytesPerPixel;
    Uint8 *pixelNewPos = (Uint8 *)surface->pixels;
    Uint8 *pixelTopPos = (Uint8 *)surface->pixels;
    Uint8 *pixelLastRowPos = (Uint8 *)surface->pixels;

    // Move position to start
    pixelTopPos += surface->w * (m_surface_manager->m_characterHeight * (m_top_margin -1)) * bpp;

    // Start from One line below the Top Margin.
    pixelNewPos += surface->w * (m_surface_manager->m_characterHeight * m_top_margin) * bpp;

    // Jump to last line, we want to clear this out after we move everything up!
    pixelLastRowPos +=  surface->w * (m_surface_manager->m_characterHeight * (m_bottom_margin -1)) * bpp;

    // Lock the Surface to Modify Pixels
    m_surface_manager->lockSurface(m_surface_manager->SURFACE_MAIN_SCREEN);

    // Move the Entire Screen Up a 1 Row of Characters.
    memmove(
        pixelTopPos,
        pixelNewPos,
        (surface->w * (m_surface_manager->m_characterHeight * (m_bottom_margin - m_top_margin))) * bpp);

    // clear the last line on the surface.
    memset(pixelLastRowPos, 0, (surface->w * m_surface_manager->m_characterHeight) * bpp);

    // Unlock when modification is done.
    m_surface_manager->unlockSurface(m_surface_manager->SURFACE_MAIN_SCREEN);

    // Update Pixels in the Texture
    m_window_manager->updateTexture(
        m_surface_manager
        ->m_textureList[m_surface_manager->TEXTURE_MAIN_SCREEN]
        ->getTexture(),
        nullptr,
        surface
    );
}

/**
 * @brief Clips out the Top Row of the Screen,
 * Then Moves Entire Screen Up one Row to Scroll it.
 */
void Renderer::scrollScreenUp()
{
    // Check for Active Scrolling Region
    if(m_is_scroll_region_active)
    {
        scrollRegionUp();
        return;
    }

    // Get handle to the Surface
    SDL_Surface *surface = m_surface_manager
                           ->m_surfaceList[m_surface_manager->SURFACE_MAIN_SCREEN]
                           ->getSurface();

    int bpp = surface->format->BytesPerPixel;
    Uint8 *pixelNewPos = (Uint8 *)surface->pixels;
    Uint8 *pixelOldPos = (Uint8 *)surface->pixels;
    Uint8 *pixelLastRowPos = (Uint8 *)surface->pixels;

    // Move position to start of 2nd Line,
    // Then copying greater line down to previous.
    pixelNewPos += (surface->w * m_surface_manager->m_characterHeight) * bpp;

    // Jump to last line, we want to clear this out after we move everything up!
    pixelLastRowPos +=  surface->w * (m_surface_manager->m_characterHeight * (m_term_height - 1)) * bpp;

    // Lock the Surface
    m_surface_manager->lockSurface(m_surface_manager->SURFACE_MAIN_SCREEN);

    // Move the Entire Screen Up a 1 Row of Characters.
    memmove(pixelOldPos, pixelNewPos,
            (surface->w * (surface->h - m_surface_manager->m_characterHeight)) * bpp);

    // clear the last line on the surface.
    memset(pixelLastRowPos, 0, (surface->w * m_surface_manager->m_characterHeight) * bpp);

    // Unlock when modification is done.
    m_surface_manager->unlockSurface(m_surface_manager->SURFACE_MAIN_SCREEN);

    // Update Pixels in the Texture
    m_window_manager->updateTexture(
        m_surface_manager
        ->m_textureList[m_surface_manager->TEXTURE_MAIN_SCREEN]
        ->getTexture(),
        nullptr,
        surface
    );
}

/**
 * @brief Clears the Surface Back Buffer, and Global Texture
 * Clears the surface to the current background color
 * This is for ESC[2J proper behavior.
 */
void Renderer::clearScreenSurface()
{
    SDL_Surface *surface = m_surface_manager
                           ->m_surfaceList[m_surface_manager->SURFACE_MAIN_SCREEN]
                           ->getSurface();

    // Fill screen with current RGB Background colors.
    m_surface_manager->fillSurfaceColor(
        m_surface_manager->SURFACE_MAIN_SCREEN,
        nullptr,
        &m_current_bg_color
    );

    // Update the Texture Pixels
    m_window_manager->updateTexture(
        m_surface_manager
        ->m_textureList[m_surface_manager->TEXTURE_MAIN_SCREEN]
        ->getTexture(),
        nullptr,
        surface
    );

    // Clear the Renderer before displaying texture.
    m_window_manager->renderClear();
}

/**
 * @brief Fill entire lines with space and BLACK foreground/Background
 * Fast update to clear a row.
 * @param y
 * @param start
 * @param end
 */
void Renderer::renderClearLineScreen(int y, int start, int end)
{
    // Save Background Color so we can switch back to it.
    SDL_Color originalGB = m_current_bg_color;
    m_current_bg_color = BLACK;

    // Clear out entire line.
    for(int i = start; i < end; i++)
    {
        drawCharacterCell(i, y, 32); // 32 Space Character
        renderCharScreen(i, y);
    }
    m_current_bg_color = originalGB;
}

/**
 * @brief Erasing all of the screen below.
 * @param y
 * @param x
 */
void Renderer::renderClearLineAboveScreen(int y, int x)
{
    // Save Background Color so we can switch back to it.
    SDL_Color originalGB = m_current_bg_color;
    m_current_bg_color = BLACK;

    // Clear out entire lines Up The Screen the screen.
    // Fix with term variables later on.
    int startPosition = x;
    for(int j = y; (j+1) > 0; j--)
    {
        for(int i = startPosition; (i+1) > 0; i--)
        {
            // x;y ' '
            drawCharacterCell(i, j, 32); // 32 space character
            renderCharScreen(i, j);
        }
        // Reset to starting position for following lines.
        startPosition = m_term_width-1;
    }
    m_current_bg_color = originalGB;
}

/**
 * @brief Erasing all of the screen below.
 * @param y
 * @param x
 */
void Renderer::renderClearLineBelowScreen(int y, int x)
{
    // Save Background Color so we can switch back to it.
    SDL_Color originalGB = m_current_bg_color;
    m_current_bg_color = BLACK;

    int startPosition = x;

    // Clear out entire lines down the screen.
    // Fix with term variables later on.
    for(int j = y; j < m_term_height; j++)
    {
        for(int i = startPosition; i < m_term_width; i++)
        {
            // x;y ' '
            drawCharacterCell(i, j, 32); // 32 space character
            renderCharScreen(i, j);
        }
        startPosition = 0;
    }
    m_current_bg_color = originalGB;
}

/**
 * @brief This will erase the current character or (s)
 * Also moves all following text left by 1.
 * @param x
 * @param y
 * @param num
 */
void Renderer::renderDeleteCharScreen(int x, int y, int num)
{
    SDL_Rect pick;
    SDL_Rect dest;

    SDL_Surface *surface = m_surface_manager
                           ->m_surfaceList[m_surface_manager->SURFACE_MAIN_SCREEN]
                           ->getSurface();

    // next we want to copy out for copy back.
    pick.w = surface->w - ((x + num) * m_surface_manager->m_characterWidth);
    pick.h = m_surface_manager->m_characterHeight;
    pick.x = (x + num) * m_surface_manager->m_characterWidth;
    pick.y = m_surface_manager->m_characterHeight * y;

    // destination
    dest.w = surface->w - ((x + num) * m_surface_manager->m_characterWidth);
    dest.h = m_surface_manager->m_characterHeight;
    dest.x = x * m_surface_manager->m_characterWidth;
    dest.y = m_surface_manager->m_characterHeight * y;

    // Move surface to Surface
    if(SDL_BlitSurface(surface ,&pick, surface, &dest) < 0)
    {
        SDL_Log("renderDeleteCharScreen() SDL_BlitSurface bottomSurface: %s",
                SDL_GetError());
        return;
    }

    // Next we want to fill the hole from pull the text left with current
    // Colors background attributes as per the spec.
    int start = m_term_width - num;
    int end = m_term_width;

    // Draw Char is 0 based.
    for(int i = start; i < end; i++)
        drawCharacterCell(i,y,32);  // 32 space character

    renderScreen();
    drawTextureScreen();
}

/**
 * @brief Render The Bottom Row of the screen (After Scrolling)
 */
void Renderer::renderBottomScreen()
{
    SDL_Rect pick;
    SDL_Rect rect;

    // Handles to the Surfaces
    SDL_Surface *surface = m_surface_manager
                           ->m_surfaceList[m_surface_manager->SURFACE_MAIN_SCREEN]
                           ->getSurface();

    SDL_Surface *bottomSurface = m_surface_manager
                                 ->m_surfaceList[m_surface_manager->SURFACE_BOTTOM_ROW]
                                 ->getSurface();

    pick.w = surface->w;
    pick.h = m_surface_manager->m_characterHeight;
    pick.x = 0;
    if(m_is_scroll_region_active)
        pick.y = m_surface_manager->m_characterHeight * (m_bottom_margin-1);
    else
        pick.y = m_surface_manager->m_characterHeight * (m_term_height-1);

    rect.w = bottomSurface->w;
    rect.h = bottomSurface->h;
    rect.x = 0;
    if(m_is_scroll_region_active)
        rect.y = m_surface_manager->m_characterHeight * (m_bottom_margin-1);
    else
        rect.y = m_surface_manager->m_characterHeight * (m_term_height-1);

    // Clear Surface
    m_surface_manager->clearSurface(m_surface_manager->SURFACE_BOTTOM_ROW);

    // Copy bottom row of the Surface
    if(SDL_BlitSurface(surface, &pick, bottomSurface, nullptr) < 0)
    {
        SDL_Log("SDL_BlitSurface() surface -> bottomSurface: %s", SDL_GetError());
        assert(false);
    }

    // Update Pixels in the Texture with the bottom row.
    m_window_manager->updateTexture(
        m_surface_manager
        ->m_textureList[m_surface_manager->TEXTURE_MAIN_SCREEN]
        ->getTexture(),
        &rect,
        bottomSurface
    );
}

/**
 * @brief Renders the MainScreen to the Texture.
 */
void Renderer::renderScreen()
{
    SDL_Surface *surface = m_surface_manager
                           ->m_surfaceList[m_surface_manager->SURFACE_MAIN_SCREEN]
                           ->getSurface();

    // Get The Actual size of the Render/Window.
    int screen_width, screen_height;
    m_window_manager->renderOutputSize(screen_width, screen_height);

    // Testing
    //SDL_RenderSetLogicalSize(m_window_manager->getRenderer(), screen_width, screen_height);

    SDL_Rect rect_dest;
    rect_dest.x = 0;
    rect_dest.y = 0;
    rect_dest.w = screen_width;
    rect_dest.h = screen_height;

    SDL_Rect rect_src;
    rect_src.x = 0;
    rect_src.y = 0;
    rect_src.w = surface->w;
    rect_src.h = surface->h;

    // Test Overide.
    m_is_scalling_surface = false;
    if(m_is_scalling_surface)
    {
        // Create Scaled
        m_surface_manager->createSurface(m_surface_manager->SURFACE_MAIN_SCALED);

        // Handle to Scaled.
        SDL_Surface *scaled = m_surface_manager
                              ->m_surfaceList[m_surface_manager->SURFACE_MAIN_SCALED]
                              ->getSurface();

        std::cout << "surface w" << surface->w << std::endl;
        std::cout << "surface h" << surface->h << std::endl;

        std::cout << "Scaled w" << scaled->w << std::endl;
        std::cout << "Scaled h" << scaled->h << std::endl;

        // Blit Main to Scaled
        SDL_BlitScaled(surface, nullptr, scaled, &rect_dest);

        //scaled = rotozoomSurfaceXY(surface, 0.0, 2, 2, 1);

        std::cout << "Scaled done" << scaled->h << std::endl;

        // Update Pixels in the Texture with the bottom row.
        m_window_manager->updateTexture(
            m_surface_manager
            ->m_textureList[m_surface_manager->TEXTURE_MAIN_SCREEN]
            ->getTexture(),
            &rect_dest,
            scaled
        );
    }
    else
    {
        // Update Pixels in the Texture with the bottom row.
        m_window_manager->updateTexture(
            m_surface_manager
            ->m_textureList[m_surface_manager->TEXTURE_MAIN_SCREEN]
            ->getTexture(),
            &rect_src,
            surface
        );
    }
}

/**
 * @brief Renderer single character cell to texture.
 * @param x
 * @param y
 */
void Renderer::renderCharScreen(int x, int y)
{
    SDL_Rect rect;

    SDL_Surface *surface = m_surface_manager
                           ->m_surfaceList[m_surface_manager->SURFACE_CHARACTER]
                           ->getSurface();

    rect.w = surface->w;
    rect.h = surface->h;
    rect.x = x * m_surface_manager->m_characterWidth;
    rect.y = y * m_surface_manager->m_characterHeight;

    // Update Pixels in the Texture with the character cell.
    m_window_manager->updateTexture(
        m_surface_manager
        ->m_textureList[m_surface_manager->TEXTURE_MAIN_SCREEN]
        ->getTexture(),
        &rect,
        m_surface_manager
        ->m_surfaceList[m_surface_manager->SURFACE_MAIN_SCREEN]
        ->getSurface()
    );
}

/**
 * @brief Setup the Cursor to Display at current position
 */
void Renderer::renderCursorOnScreen()
{
    SDL_Rect rect;

    // Grab handle from weak pointer.
    session_ptr session = m_weak_session.lock();
    if(session)
    {
        // Check if the position has changed, if so, then skip!
        if(m_cursor_x_position != session->m_sequence_parser->m_screen_buffer.m_x_position-1 ||
                m_cursor_y_position != session->m_sequence_parser->m_screen_buffer.m_y_position-1)
        {
            return;
        }
    }
    else
    {
        return;
    }

    // Handle to Cursor On Surface
    SDL_Surface *surface = m_surface_manager
                           ->m_surfaceList[m_surface_manager->SURFACE_CURSOR_ON]
                           ->getSurface();

    // Size of Each Character
    rect.w = surface->w;
    rect.h = surface->h;
    rect.x = m_cursor_x_position * m_surface_manager->m_characterWidth;
    rect.y = m_cursor_y_position * m_surface_manager->m_characterHeight;

    // Update Pixels in the Texture with the character cell.
    m_window_manager->updateTexture(
        m_surface_manager
        ->m_textureList[m_surface_manager->TEXTURE_MAIN_SCREEN]
        ->getTexture(),
        &rect,
        surface
    );
}

/**
 * @brief Setup the Cursor to hide at current position
 */
void Renderer::renderCursorOffScreen()
{
    SDL_Rect rect;

    // Grab handle from weak pointer.
    session_ptr session = m_weak_session.lock();
    if(session)
    {
        // Check if the position has changed, if so, then skip!
        if(m_cursor_x_position !=
                session->m_sequence_parser->m_screen_buffer.m_x_position-1 ||
                m_cursor_y_position !=
                session->m_sequence_parser->m_screen_buffer.m_y_position-1)
        {
            return;
        }
    }
    else
    {
        return;
    }

    // Handle to Cursor Off Surface
    SDL_Surface *surface = m_surface_manager
                           ->m_surfaceList[m_surface_manager->SURFACE_CURSOR_OFF]
                           ->getSurface();

    // Size of Each Character
    rect.w = surface->w;
    rect.h = surface->h;
    rect.x = m_cursor_x_position * m_surface_manager->m_characterWidth;
    rect.y = m_cursor_y_position * m_surface_manager->m_characterHeight;

    // Update Pixels in the Texture with the character cell.
    m_window_manager->updateTexture(
        m_surface_manager
        ->m_textureList[m_surface_manager->TEXTURE_MAIN_SCREEN]
        ->getTexture(),
        &rect,
        surface
    );
}

/**
 * @brief Pushed the Current MAIN Texture to the Renderer to Draw
 */
void Renderer::drawTextureScreen()
{
    /*
    SDL_Rect pick, rect;

    // Get The Actual size of the Render/Window.
    int screen_width, screen_height;
    m_window_manager->renderOutputSize(screen_width, screen_height);

    // Handle to Cursor Off Surface
    SDL_Surface *surface = m_surface_manager
                           ->m_surfaceList[m_surface_manager->SURFACE_MAIN_SCREEN]
                           ->getSurface();

    // Used when cliping larger screen then texture size.
    pick.w = surface->w; //  - 40; // 680 - 640 = 40
    pick.h = surface->h; // - 80; // 480 - 400 = 80
    pick.x = 0;
    pick.y = 0;

    // Destination
    rect.w = screen_width;
    rect.h = screen_height;
    rect.x = 0;
    rect.y = 0;*/

    SDL_Rect dest;
    int screen_width, screen_height;
    m_window_manager->renderOutputSize(screen_width, screen_height);

    dest.w = screen_width;
    dest.h = screen_height;
    dest.x = 0;
    dest.y = 0;

    // Copy Texture to Renderer
    m_window_manager->renderCopy(
        m_surface_manager
        ->m_textureList[m_surface_manager->TEXTURE_MAIN_SCREEN]
        ->getTexture(),
        nullptr,
        &dest
    );

    // Draw Out to Screen.
    m_window_manager->renderPresent();
}

/**
 * @brief Clears Renderer and Redraws Black Screen
 */
void Renderer::clearScreen()
{
    m_window_manager->setRenderDrawColor(0x00, 0x00, 0x00, 0xFF);
    m_window_manager->renderClear();
    m_window_manager->renderPresent();
}

/**
 * @brief Compares (2) SDL Colors to determine if they are the same
 * @param src
 * @param dest
 * @return
 */
int Renderer::compareSDL_Colors(SDL_Color &src, SDL_Color &dest)
{
    return (src.r == dest.r &&
            src.g == dest.g &&
            src.b == dest.b);
}

/**
 * @brief Recolors FG/BG colors per character cell.
 * @param foreground
 * @param background
 */
void Renderer::replaceColor(Uint32 foreground, Uint32 background)
{
    SDL_Surface *surface = m_surface_manager
                           ->m_surfaceList[m_surface_manager->SURFACE_CHARACTER]
                           ->getSurface();

    // Foreground/Background of Original Bitmap Image for Replacement.
    static Uint32 fgColor = SDL_MapRGB(surface->format, 255, 255, 255);  // White
    static Uint32 bgColor = SDL_MapRGB(surface->format, 0,   0,   0);    // Black

    // If color already default of bitmap, then skip changing.
    if(fgColor == foreground && bgColor == background)
    {
        return;
    }

    // Get a handle to the pixels.
    Uint8 *pixel = (Uint8 *)surface->pixels;
    m_surface_manager->lockSurface(m_surface_manager->SURFACE_CHARACTER);

    // Loop and replace foreground and Background colors.
    int count = surface->h * surface->w;
    for(int i = 0; i < count; i++, pixel += 4)
    {
        // more background in most cases check first.
        if(*(Uint32 *) pixel == bgColor)
        {
            *(Uint32 *) pixel = background;
        }
        else
        {
            *(Uint32 *) pixel = foreground;
        }
    }

    // Unlock the Surface Pixels
    m_surface_manager->unlockSurface(m_surface_manager->SURFACE_CHARACTER);
}

/**
 * @brief Setup the Cursor font, character and create from underscore
 * NOTE: this should be reworked, can't trust all font sets!
 * This is not the best way to handle this IMO.
 */
void Renderer::setupCursorCharacter()
{
    // Grab handle from weak pointer.
    session_ptr session = m_weak_session.lock();
    if(session && session->m_sequence_parser)
    {
        // If cursor is not active then don't waste processing.
        if(!session->m_sequence_parser->isCursorActive())
        {
            return;
        }

        m_cursor_x_position = session->m_sequence_parser->m_screen_buffer.m_x_position-1;
        m_cursor_y_position = session->m_sequence_parser->m_screen_buffer.m_y_position-1;
    }
    else
    {
        return;
    }

    //SDL_Rect pick, area;
    SDL_Rect pick, area, cursor, cursor2;

    // Transform The underscore to a Cursor.
    int asciicode = '_';
    int char_width = m_surface_manager->m_characterWidth;
    int char_height = m_surface_manager->m_characterHeight;

    // Quad for Char Cell in Bitmap
    // 32 Cols by 8 Rows.
    pick.x = (asciicode % 32) * char_width;
    pick.y = round(asciicode / 32) * char_height;
    pick.w = char_width;
    pick.h = char_height;

    // Quad for Char Cell to Map to Screen
    area.x = m_cursor_x_position * char_width;
    area.y = m_cursor_y_position * char_height;
    area.w = char_width;
    area.h = char_height;

    // Handles to Surfaces, when using more than once per method.
    SDL_Surface *surfaceFont = m_surface_manager
                               ->m_surfaceList[m_surface_manager->SURFACE_FONT]
                               ->getSurface();

    SDL_Surface *surfaceMain = m_surface_manager
                               ->m_surfaceList[m_surface_manager->SURFACE_MAIN_SCREEN]
                               ->getSurface();

    SDL_Surface *surfaceCharacter = m_surface_manager
                                    ->m_surfaceList[m_surface_manager->SURFACE_CHARACTER]
                                    ->getSurface();

    SDL_Surface *surfaceOn = m_surface_manager
                             ->m_surfaceList[m_surface_manager->SURFACE_CURSOR_ON]
                             ->getSurface();

    SDL_Surface *surfaceOff = m_surface_manager
                              ->m_surfaceList[m_surface_manager->SURFACE_CURSOR_OFF]
                              ->getSurface();

    // Clear Surfaces
    m_surface_manager->clearSurface(m_surface_manager->SURFACE_CHARACTER);
    m_surface_manager->clearSurface(m_surface_manager->SURFACE_CURSOR_ON);
    m_surface_manager->clearSurface(m_surface_manager->SURFACE_CURSOR_OFF);

    //First Grab Underscore from Font Surface
    if(SDL_BlitSurface(surfaceFont, &pick, surfaceCharacter, nullptr) < 0)
    {
        SDL_Log("setupCursorChar() SURFACE_CHARACTER: %s", SDL_GetError());
    }

    // Grab current Char Block where Cursor position is at on screen
    if(SDL_BlitSurface(surfaceMain, &area, surfaceOn, nullptr) < 0)
    {
        SDL_Log("setupCursorChar() SURFACE_CURSOR_ON: %s", SDL_GetError());
    }

    // Grab current Char Block where Passed Cursor position is.
    if(SDL_BlitSurface(surfaceMain, &area, surfaceOff, nullptr) < 0)
    {
        SDL_Log("setupCursorChar() SURFACE_CURSOR_OFF: %s", SDL_GetError());
    }

    // Grab Underscore Character from Font Serface, and overwrite the current character cell.
    if(m_surface_manager->m_currentFont == "vga8x16.bmp")
    {
        cursor.x = 0;
        cursor.y = 13;
        cursor.w = char_width;
        cursor.h = 2;

        cursor2.x = 0;
        cursor2.y = 15;
        cursor2.w = char_width;
        cursor2.h = 2;
    }
    else
    {
        cursor.x = 0;
        cursor.y = 15;
        cursor.w = char_width;
        cursor.h = 2;

        cursor2.x = 0;
        cursor2.y = 15;
        cursor2.w = char_width;
        cursor2.h = 2;
    }

    // Add the Underscore to the surfaceOn
    if(SDL_BlitSurface(surfaceCharacter, &cursor, surfaceOn, &cursor2) < 0)
    {
        SDL_Log("setupCursorChar() SURFACE_CURSOR_OFF: %s", SDL_GetError());
    }
}

/**
 * @brief Draws/Recolors a Char Cell to x/y location on screen surface.
 * @param X
 * @param Y
 * @param ascii_code
 */
void Renderer::drawCharacterCell(int X, int Y, int ascii_code)
{
    SDL_Rect pick, area;

    // Quad for Char Cell in Bitmap
    // 32 Cols by 8 Rows.
    pick.x = (ascii_code % 32) * m_surface_manager->m_characterWidth;
    pick.y = round(ascii_code / 32) * m_surface_manager->m_characterHeight;
    pick.w = m_surface_manager->m_characterWidth;
    pick.h = m_surface_manager->m_characterHeight;

    // Quad for Char Cell to Map to Screen
    area.x = X * m_surface_manager->m_characterWidth;
    area.y = Y * m_surface_manager->m_characterHeight;
    area.w = m_surface_manager->m_characterWidth;
    area.h = m_surface_manager->m_characterHeight;

    SDL_Surface *surface = m_surface_manager
                           ->m_surfaceList[m_surface_manager->SURFACE_CHARACTER]
                           ->getSurface();

    m_surface_manager->clearSurface(m_surface_manager->SURFACE_CHARACTER);

    // Copy from font surface to character surface.
    if(SDL_BlitSurface(
                m_surface_manager
                ->m_surfaceList[m_surface_manager->SURFACE_FONT]
                ->getSurface(),
                &pick,
                surface,
                nullptr) < 0)
    {
        SDL_Log("drawChar() SDL_BlitSurface tmpSurface: %s", SDL_GetError());
    }

    // Replace the Character Cell Color
    replaceColor(
        SDL_MapRGB(surface->format, m_current_fg_color.r,
                   m_current_fg_color.g, m_current_fg_color.b),
        SDL_MapRGB(surface->format, m_current_bg_color.r,
                   m_current_bg_color.g, m_current_bg_color.b));

    // Write to Back Buffer for Scrolling the Screen.
    if(SDL_BlitSurface(
                surface,
                nullptr,
                m_surface_manager
                ->m_surfaceList[m_surface_manager->SURFACE_MAIN_SCREEN]
                ->getSurface(),
                &area) < 0)
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
 */
void Renderer::drawString(int X, int Y, char text[])
{
    SDL_Rect area;

    int i = 0;
    int asciiCode = 0;
    area.x = X;
    area.y = Y;

    int surface_width = m_surface_manager
                        ->m_surfaceList[m_surface_manager->SURFACE_MAIN_SCREEN]
                        ->getSurface()->w;

    for(i = 0; i < (signed)strlen(text); i++)
    {
        asciiCode = std::char_traits<unsigned char>().to_int_type(text[i]);
        drawCharacterCell(
            area.x / m_surface_manager->m_characterWidth,
            area.y / m_surface_manager->m_characterHeight,
            asciiCode
        );

        area.x += m_surface_manager->m_characterWidth;
        if(area.x >= surface_width-5)
        {
            area.x = 0;
            area.y += m_surface_manager->m_characterHeight;
        }
    }
}


/**
 * Draws Character Set of each Character Cells to x/y location on screen surface.
 * If String is longer then screen will wrap to next line.
 * Mainly for testing all characters loaded from Bitmap Image.
 *
 * Not used at this time
 */
void Renderer::drawCharSet(int X, int Y)
{
    SDL_Rect area;
    int i = 0;
    int asciiCode = 0;
    area.x = X * m_surface_manager->m_characterWidth;
    area.y = Y * m_surface_manager->m_characterHeight;

    int surface_width = m_surface_manager
                        ->m_surfaceList[m_surface_manager->SURFACE_MAIN_SCREEN]
                        ->getSurface()->w;


    // Loop through Each Char in Bitmap.
    for(i = 1; i < 255; i++)
    {
        asciiCode = i;
        if(asciiCode == 0)
        {
            break;
        }
        drawCharacterCell(
            area.x / m_surface_manager->m_characterWidth,
            area.y / m_surface_manager->m_characterHeight,
            asciiCode
        );
        area.x += m_surface_manager->m_characterWidth;
        // Wrap
        if(area.x >= surface_width-5)
        {
            area.x = 0;
            area.y += m_surface_manager->m_characterHeight;
        }
    }
}
