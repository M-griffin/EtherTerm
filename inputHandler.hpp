#ifndef __InputHandler__
#define __InputHandler__

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

//#include "safeQueue.hpp"

#ifdef TARGET_OS_MAC
#include <SDL2/SDL.h>
#elif _WIN32
#include <windows.h>
#include <SDL.h>
#else // LINUX
#include <SDL2/SDL.h>
#endif

#include <iostream>
#include <string>

/*
 * WIP - Were going to make a SafeQueue of SDL_Events
 * On Pool, we'll loop all events per frame,
 * Save them into the queue, then cycle them so
 * we don't look any on actions.  Might need to revisit
 * how actions are taken, if they should be queued instead!
 */

class InputHandler
{
public:
    static InputHandler* Instance()
    {
        if(!m_globalInstance)
        {
            m_globalInstance = new InputHandler();
        }
        return m_globalInstance;
    }

// Release And Clear the Singleton
    static void ReleaseInstance()
    {
        if(m_globalInstance)
        {
            delete m_globalInstance;
            m_globalInstance = nullptr;
        }
        return;
    }

// keyboard events, True if Data Available.
    bool update();
    void reset()
    {
        m_inputSequence.erase();
    }

    // Input Suequence Appends untill it's ready to be pulled
    // Then it's moved and cleared.
    bool getInputSequence(std::string &sequence)
    {
        if (!m_inputSequence.empty())
        {
            sequence = std::move(m_inputSequence);
            m_inputSequence.erase();
            return true;
        }
        return false;
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
    void setInputSequence(std::string sequence)
    {
        m_inputSequence += sequence;
    }

    bool m_globalShutdown;
    bool m_isWindowMode;
    bool m_isMouseSelected;
    int m_mouseSourceXPosition;
    int m_mouseSourceYPosition;
    int m_mouseReleaseXPosition;
    int m_mouseReleaseYPosition;
    int m_fullScreenWindowSize;

    std::string m_inputSequence; // Keyboard Input

    InputHandler();
    ~InputHandler();
    InputHandler(const InputHandler&);
    InputHandler& operator=(const InputHandler&);

// singleton
    static InputHandler* m_globalInstance;

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

// First Break Up into Seperate Functions,
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

typedef InputHandler TheInputHandler;


#endif
