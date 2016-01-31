#include "window_manager.hpp"

#include <iostream>

#ifdef TARGET_OS_MAC // OSX
#include <SDL.h>

#elif _WIN32 // Windows
#include <SDL2/SDL.h>

#else // LINUX
#include <SDL2/SDL.h>
#endif

#include <iostream>

WindowManager::WindowManager(int window_height, int window_width)
    : m_window(nullptr)
    , m_renderer(nullptr)
    , m_window_id(0)
    , m_width(window_width)
    , m_height(window_height)
    , m_hint_vSync("0")          // "0" or "1" Disabled or Enabled
    , m_hint_textureFilter("0")  // "0" or "1", "2" for none or linear, bi-linear
    , m_hint_acceleration("1")   // "0" or "1" Disabled or Enabled
    , m_is_active_window(false)
{
    std::cout << "WindowManager Created!" << std::endl;

    static long position_placement = 0;
    m_position_placement = position_placement;
    ++position_placement;

    // Startup create the Window NOT Fullscreen.
    if(createWindow(false))
    {
        std::cout << "Creating Window and Renderer!" << std::endl;
        // Set new Render Hints.
        createHints();

        // Create the Renderer
        createRenderer();
    }
}

WindowManager::~WindowManager()
{
    // Cleanup Deallocate Renderer and Window.
    std::cout << "~WindowManager: " << m_window_id << std::endl;

    if(m_renderer)
    {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }

    if(m_window)
    {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
}

/**
 * @brief Set Current Window to Focus
 */
void WindowManager::grabWindow()
{
    SDL_RaiseWindow(m_window);
}

/**
 * @brief Override default Window Height
 * @param value
 */
void WindowManager::setWindowHeight(int value)
{
    m_height = value;
}

/**
 * @brief Override default Window Width
 * @param value
 */
void WindowManager::setWindowWidth(int value)
{
    m_width = value;
}

/**
 * @brief Toggle Vertical Sync with GPU
 * @param value
 */
void WindowManager::setHintVsync(bool value)
{
    if(value)
        m_hint_vSync = "1"; // on
    else
        m_hint_vSync = "0"; // off
}

/**
 * @brief Set the Texture Filtering Level for the GPU
 * @param value
 */
void WindowManager::setTextureFilter(int value)
{
    switch(value)
    {
        case 0: // None
            m_hint_vSync = "0";
            break;
        case 1: // Linear
            m_hint_vSync = "1";
            break;
        case 2: // Bi-linear
            m_hint_vSync = "2";
            break;
        default:
            m_hint_vSync = "0";
            break;
    }
}

/**
 * @brief Set Rendering Acceleration for GPU
 * @param value
 */
void WindowManager::setHintAcceleration(bool value)
{
    if(value)
        m_hint_acceleration = "1"; // on
    else
        m_hint_acceleration = "0"; // off
}

/**
 * @brief Sets the Hints for Texture/Renderer
 */
void WindowManager::createHints()
{
    /*
     * 0 = disable vsync
     * 1 = enable vsync
     */
    if(!SDL_SetHint(SDL_HINT_RENDER_VSYNC, m_hint_vSync.c_str()))
    {
        SDL_Log("SurfaceManager::init() SDL_SetHint SDL_HINT_RENDER_VSYNC: %s",
                SDL_GetError());
    }
    /*
     * 0 or nearest = nearest pixel sampling
     * 1 or linear  = linear filtering       (supported by OpenGL and Direct3D)
     * 2 or best    = anisotropic filtering  (supported by Direct3D)
     */
    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, m_hint_textureFilter.c_str()))
    {
        SDL_Log("SurfaceManager::init() SDL_SetHint SDL_HINT_RENDER_SCALE_QUALITY: %s",
                SDL_GetError());
    }
    /*
     * 0 disable 3D acceleration
     * 1 enable 3D acceleration, using the default renderer
     * X enable 3D acceleration, using X where X is one of the valid
     *  rendering drivers. (e.g. "direct3d", "opengl", etc.)
     */
    if(!SDL_SetHint(SDL_HINT_FRAMEBUFFER_ACCELERATION, m_hint_acceleration.c_str()))
    {
        SDL_Log("SurfaceManager::init() SDL_SetHint SDL_HINT_RENDER_SCALE_QUALITY: %s",
                SDL_GetError());
    }
}

/**
 * @brief Recreates the Renderer and Texture
 * @param mode
 */
void WindowManager::createRenderer()
{
    if(m_renderer)
    {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }

    m_renderer = SDL_CreateRenderer(
                     m_window,
                     -1,
                     SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    if(!m_renderer)
    {
        SDL_Log("createRenderer globalRenderer: %s",
                SDL_GetError());
        assert(m_renderer);
    }

    // Clear and Refresh the Terminal
    if(SDL_SetRenderDrawColor(m_renderer, 0x00, 0x00, 0x00, 0xFF) < 0)
    {
        SDL_Log("createRenderer SDL_SetRenderDrawColor globalRenderer: %s",
                SDL_GetError());
        return;
    }

    if(SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE) < 0)
    {
        SDL_Log("createRenderer SDL_SetRenderDrawBlendMode globalRenderer: %s",
                SDL_GetError());
        return;
    }

    if(SDL_SetRenderTarget(m_renderer, nullptr) < 0)
    {
        SDL_Log("createRenderer SDL_SetRenderTarget NULL: %s",
                SDL_GetError());
    }

    if(SDL_RenderClear(m_renderer) < 0)
    {
        SDL_Log("createRenderer() SDL_RenderClear globalRenderer: %s",
                SDL_GetError());
    }
}

/**
 * @brief Window Creation and recreation.
 */
bool WindowManager::createWindow(bool fullScreen)
{
    // If Window Already Exists, destory so we can recreate it.
    if(m_window)
    {
        //std::cout << "Destroy SDL Window" << std::endl;
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }

    int display_height = (m_height / 2);
    int display_width  = (m_width / 2);

    // create window in in full screen or windowed mode.
    if(fullScreen)
    {
        m_window = SDL_CreateWindow("EtherTerm 3.0 Demo - Full Screen",
                                    (m_position_placement % 2 == 0) ? SDL_WINDOWPOS_CENTERED : display_width,
                                    (m_position_placement % 2 == 0) ? SDL_WINDOWPOS_CENTERED : display_height,
                                    m_width,
                                    m_height,
                                    SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);

        if(!m_window) // window init success
        {
            SDL_Log("Terminal::restartWindowSize() SDL_CreateWindow globalWindow: %s",
                    SDL_GetError());
            assert(m_window);
        }
    }
    else
    {
        m_window = SDL_CreateWindow("EtherTerm 3.0 Demo - Windowed",
                                    (m_position_placement % 2 == 0) ? SDL_WINDOWPOS_CENTERED : display_width,
                                    (m_position_placement % 2 == 0) ? SDL_WINDOWPOS_CENTERED : display_height,
                                    m_width,
                                    m_height,
                                    SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        if(!m_window) // window init success
        {
            SDL_Log("Terminal::restartWindowSize() SDL_CreateWindow globalWindow: %s",
                    SDL_GetError());
            assert(m_window);
        }
    }

    // Set the Window ID
    m_window_id = SDL_GetWindowID(m_window);

    std::string title = "EtherTerm 3.x Demo w/ Multiple Windows - Window ID: ";
    title.append(std::to_string(m_window_id));
    SDL_SetWindowTitle(m_window, title.c_str());

    // Display and move focus to the new window.
    SDL_ShowWindow(m_window);
    SDL_RaiseWindow(m_window);
    return true;
}

/**
 * @brief Set the Texture Renderer
 * @param texture
 */
void WindowManager::setRenderTarget(SDL_Texture *texture)
{
    if(SDL_SetRenderTarget(m_renderer, texture) < 0)
    {
        SDL_Log("setRenderTarget(): %s", SDL_GetError());
    }
}

/**
 * @brief Clears the Renderer of all Data.
 */
void WindowManager::renderClear()
{
    if(SDL_RenderClear(m_renderer) < 0)
    {
        SDL_Log("clearRenderer(): %s", SDL_GetError());
    }
}

/**
 * @brief Set the Renderer Drawing Color (4) Bits.
 * @param R
 * @param G
 * @param B
 * @param A
 */
void WindowManager::setRenderDrawColor(Uint32 R, Uint32 G, Uint32 B, Uint32 A)
{
    if(SDL_SetRenderDrawColor(m_renderer, R, G , B, A) < 0)
    {
        SDL_Log("setRendererDrawColor(): %s", SDL_GetError());
    }
}

/**
 * @brief Fill Rect Area of Renderer Display on Screen
 * @param rect
 */
void WindowManager::renderFill(SDL_Rect *rect)
{
    if(SDL_RenderFillRect(m_renderer, rect) < 0)
    {
        SDL_Log("rendererFill(): %s", SDL_GetError());
    }
}

/**
 * @brief Copy From Texture to Renderer
 * @param texture
 * @param orig
 * @param dest
 */
void WindowManager::renderCopy(SDL_Texture *texture, SDL_Rect *orig, SDL_Rect *dest)
{
    if(SDL_RenderCopy(m_renderer, texture, orig, dest) < 0)
    {
        SDL_Log("renderCopy(): %s", SDL_GetError());
    }
}

/**
 * @brief Draw Renderer to Screen
 */
void WindowManager::renderPresent()
{
    SDL_RenderPresent(m_renderer);
}

/**
 * @brief Get the Window Dimensions of the Renderer
 * @param screenWidth
 * @param screenHeight
 */
void WindowManager::renderOutputSize(int &screenWidth, int &screenHeight)
{
    SDL_GetRendererOutputSize(
        m_renderer,
        &screenWidth,
        &screenHeight
    );
}


/**
 * @brief Update The Texture (Streaming)
 * @param texture
 * @param rect
 * @param surface
 */
void WindowManager::updateTexture(SDL_Texture *texture,
                                  SDL_Rect *rect,
                                  SDL_Surface *surface)
{
    if(SDL_UpdateTexture(texture,
                         rect,
                         surface->pixels,
                         surface->pitch) < 0)
    {
        SDL_Log("updateTexture(): %s", SDL_GetError());
    }
}
