#ifndef __InputHandler__
#define __InputHandler__

#include <SDL2/SDL.h>

#include <iostream>
#include <string>
#include <memory>

class SurfaceManager;
typedef std::shared_ptr<SurfaceManager> surface_manager_ptr;
typedef std::weak_ptr<SurfaceManager> surface_manager_weak_ptr;


class Session;
typedef std::shared_ptr<Session> session_ptr;
typedef std::weak_ptr<Session> session_weak_ptr;

/**
 * @class InputHandler
 * @author Michael Griffin
 * @date 11/17/2015
 * @file input_handler.hpp
 * @brief Handing Local SDL Input Event Processing.
 */
class InputHandler
{
public:

    InputHandler(surface_manager_ptr &surface_manager, session_ptr session);
    ~InputHandler();

    /**
     * @brief Used to Restart the SDL_GetText Event on changeing focus.
     */
    void resetTextInput();

    // keyboard events, True if Data Available.
    bool update(SDL_Event &event);
    void reset()
    {
        m_inputSequence.erase();
    }

    // Input Suequence Appends untill it's ready to be pulled
    // Then it's moved and cleared.
    bool getInputSequence(std::string &sequence)
    {
        if(!m_inputSequence.empty())
        {
            sequence = std::move(m_inputSequence);
            m_inputSequence.erase();
            return true;
        }

        return false;
    }

    void globalShutdown()
    {
        m_globalShutdown = true;
    }

    bool isGlobalShutdown() const
    {
        return m_globalShutdown;
    }

    // mouse events
    bool isMouseSelection() const
    {
        return m_isMouseSelected;
    }

    // mouse positions
    int getMouseSourceXPosition() const
    {
        return m_mouseSourceXPosition;
    }
    int getMouseSourceYPosition() const
    {
        return m_mouseSourceYPosition;
    }

private:

    // Append Input Events until data is read to be pulled.
    void setInputSequence(const std::string &sequence)
    {
        m_inputSequence += sequence;
    }

    // Handles Access to Surfaces and Windows.
    surface_manager_weak_ptr  m_weak_surface_manager;
    session_weak_ptr          m_weak_session;

    bool m_globalShutdown;
    bool m_isWindowMode;
    bool m_isMouseSelected;
    int m_mouseSourceXPosition;
    int m_mouseSourceYPosition;
    int m_mouseReleaseXPosition;
    int m_mouseReleaseYPosition;
    int m_fullScreenWindowSize;

    std::string m_keyMap;
    std::string m_inputSequence; // Keyboard Input

    // Control Keys.
    enum
    {
        CTRLA = 0x01,
        CTRLB = 0x02,
        CTRLC = 0x03,
        CTRLD = 0x04,
        CTRLE = 0x05,
        CTRLF = 0x06,
        CTRLG = 0x07,
        CTRLH = 0x08,
        CTRLI = 0x09,
        CTRLJ = 0x0a,
        CTRLK = 0x0b,
        CTRLL = 0x0c,
        CTRLM = 0x0d,
        CTRLN = 0x0e,
        CTRLO = 0x0f,
        CTRLP = 0x10,
        CTRLQ = 0x11,
        CTRLR = 0x12,
        CTRLS = 0x13,
        CTRLT = 0x14,
        CTRLU = 0x15,
        CTRLV = 0x16,
        CTRLW = 0x17,
        CTRLX = 0x18,
        CTRLY = 0x19,
        CTRLZ = 0x1a
    };

    const
    unsigned char CTRL_KEY_TABLE[26] =
    {
        CTRLA, CTRLB, CTRLC, CTRLD, CTRLE,
        CTRLF, CTRLG, CTRLH, CTRLI, CTRLJ,
        CTRLK, CTRLL, CTRLM, CTRLN, CTRLO,
        CTRLP, CTRLQ, CTRLR, CTRLS, CTRLT,
        CTRLU, CTRLV, CTRLW, CTRLX, CTRLY,
        CTRLZ
    };

    // First Break Up into Separate Functions,
    // Later Map Enum and and setup commands for the following.
    void handleWindowEvents(SDL_Event &event);
    bool handleTextInputEvent(SDL_Event &event);
    void handleMouseButtonUpEvent(SDL_Event &event);
    void handleMouseMotionEvent(SDL_Event &event);
    bool handleMouseButtonDownEvent(SDL_Event &event);
    bool handleShiftControlKeys(SDL_Event &event);
    bool handleControlKeys(SDL_Event &event);
    bool handleAlternateKeys(SDL_Event &event);
    bool handleKeyPadAndFunctionKeys(SDL_Event &event);
    bool handleANSIKeyMapFunctionKeys(SDL_Event &event);
    bool handleVT100KeyMapFunctionKeys(SDL_Event &event);
    bool handleLINUXKeyMapFunctionKeys(SDL_Event &event);
    bool handleSCOKeyMapFunctionKeys(SDL_Event &event);
    bool handleKeyDownEvents(SDL_Event &event);
};

typedef std::shared_ptr<InputHandler> input_handler_ptr;
typedef std::weak_ptr<InputHandler> input_handler_weak_ptr;


#endif
