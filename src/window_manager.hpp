#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

#ifdef TARGET_OS_MAC // OSX
#include <SDL.h>

#elif _WIN32 // Windows
#include <SDL2/SDL.h>

#else // LINUX
#include <SDL2/SDL.h>
#endif

#include <boost/enable_shared_from_this.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

/**
 * @class WindowManager
 * @author Michael Griffin
 * @date 11/16/2015
 * @file window_manager.hpp
 * @brief Manages Individual Window Interfaces with Renderers
 */
class WindowManager
    : public boost::enable_shared_from_this<WindowManager>
{
public:
    WindowManager();
    ~WindowManager();

    // Handles to Window and Renderer
    SDL_Window *window() { return m_window; }
    SDL_Renderer *renderer() { return m_renderer; }
    Uint32 getWindowId() const { return m_window_id; }

    /**
     * @brief Overide default Window Height
     * @param value
     */
    void setWindowHeight(int value);

    /**
     * @brief Overide default Window Width
     * @param value
     */
    void setWindowWidth(int value);

    /**
     * @brief Toggle Verticle Sync with GPU
     * @param value
     */
    void setHintVsync(bool value);

    /**
     * @brief Set the Texture Filtering Level for the GPU
     * @param value
     */
    void setTextureFilter(int value);

    /**
     * @brief Set Rendering Acceleration for GPU
     * @param value
     */
    void setHintAcceleration(bool value);

    /**
     * @brief Sets the Hints for Texture/Renderer
     */
    void createHints();

    /**
     * @brief Recreates the Renderer and Texture
     * @param mode
     */
    void createRenderer();

    /**
     * @brief Window Creation and recreation.
     */
    bool createWindow(bool fullScreen);

private:

    SDL_Window*    m_window;
    SDL_Renderer*  m_renderer;
    Uint32         m_window_id;
    int            m_width;
    int            m_height;

    std::string    m_hint_vSync;          // "0" or "1" Disabled or Enabled
    std::string    m_hint_textureFilter;  // "0" or "1" for none or linear
    std::string    m_hint_acceleration;   // "0" or "1" Disabled or Enabled

};

typedef boost::shared_ptr<WindowManager> window_manager_ptr;

#endif // WINDOW_MANAGER_HPP
