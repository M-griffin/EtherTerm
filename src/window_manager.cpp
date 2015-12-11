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

WindowManager::WindowManager()
    : m_window(nullptr)
    , m_renderer(nullptr)
    , m_window_id(0)
    , m_width(0)
    , m_height(0)
    , m_hint_vSync("0")          // "0" or "1" Disabled or Enabled
    , m_hint_textureFilter("1")  // "0" or "1", "2" for none or linear, bilinear
    , m_hint_acceleration("1")   // "0" or "1" Disabled or Enabled
{
}

WindowManager::~WindowManager()
{
    // Cleanup Deallocate Renderer and Window.
    std::cout << "Closing Window - Title " << m_window_id << std::endl;

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
 * @brief Overide default Window Height
 * @param value
 */
void WindowManager::setWindowHeight(int value)
{
    m_height = value;
}

/**
 * @brief Overide default Window Width
 * @param value
 */
void WindowManager::setWindowWidth(int value)
{
    m_width = value;
}

/**
 * @brief Toggle Verticle Sync with GPU
 * @param value
 */
void WindowManager::setHintVsync(bool value)
{
    if (value)
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
        case 2: // BiLinear
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
    if (value)
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
        SDL_Log("restartWindowRenderer globalRenderer: %s",
                SDL_GetError());
        assert(m_renderer);
    }

    // Clear and Refresh the Terminal
    if(SDL_SetRenderDrawColor(m_renderer, 0x00, 0x00, 0x00, 0xFF) < 0)
    {
        SDL_Log("restartWindowRenderer SDL_SetRenderDrawColor globalRenderer: %s",
                SDL_GetError());
        return;
    }

    if(SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE) < 0)
    {
        SDL_Log("restartWindowRenderer SDL_SetRenderDrawBlendMode globalRenderer: %s",
                SDL_GetError());
        return;
    }

    if(SDL_SetRenderTarget(m_renderer, nullptr) < 0)
    {
        SDL_Log("restartWindowRenderer SDL_SetRenderTarget NULL: %s",
                SDL_GetError());
    }


    if(SDL_RenderClear(m_renderer) < 0)
    {
        SDL_Log("restartWindowRenderer() SDL_RenderClear globalRenderer: %s",
                SDL_GetError());
    }
}

/**
 * @brief Window Creation and recreation.
 */
bool WindowManager::createWindow(bool fullScreen)
{

    // Set new Texture Hints.
    createHints();

    // Create the Renderer
    createRenderer();

    // If Window Already Exists, destory so we can recreate it.
    if(m_window)
    {
        //std::cout << "Destroy SDL Window" << std::endl;
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }

    // create window in in full screen or windowed mode.
    if(fullScreen)
    {
        m_window = SDL_CreateWindow("EtherTerm 3.0 Demo - Fullscreen",
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
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
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
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

    // Make Sure the window was created
    if(!m_window)
    {
        //Create renderer for window
        m_renderer = SDL_CreateRenderer(m_window,
                                        -1,
                                        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if(!m_renderer)
        {
            std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
            assert(m_renderer);
        }
        else
        {
            //Initialize renderer color RGBA
            SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

            //Grab window identifier
            m_window_id = SDL_GetWindowID(m_window);
        }
    }
    else
    {
        std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Display and move focus to the new window.
    SDL_ShowWindow(m_window);
    SDL_RaiseWindow(m_window);
    return true;
}


