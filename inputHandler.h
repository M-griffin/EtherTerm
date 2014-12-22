#ifndef __InputHandler__
#define __InputHandler__

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include <iostream>

#ifdef _WIN32
    #include <SDL.h>
#else
    #include <SDL2/SDL.h>
#endif

# define CTRLA           0x01
# define CTRLB           0x02
# define CTRLC           0x03
# define CTRLD           0x04
# define CTRLE           0x05
# define CTRLF           0x06
# define CTRLG           0x07
# define CTRLH           0x08
# define CTRLI           0x09
# define CTRLJ           0x0a
# define CTRLK           0x0b
# define CTRLL           0x0c
# define CTRLM           0x0d
# define CTRLN           0x0e
# define CTRLO           0x0f
# define CTRLP           0x10
# define CTRLQ           0x11
# define CTRLR           0x12
# define CTRLS           0x13
# define CTRLT           0x14
# define CTRLU           0x15
# define CTRLV           0x16
# define CTRLW           0x17
# define CTRLX           0x18
# define CTRLY           0x19
# define CTRLZ           0x1a


class InputHandler
{
public:

    static InputHandler* Instance()
    {
        if(globalInstance == 0)
        {
            globalInstance = new InputHandler();
        }

        return globalInstance;
    }

    // keyboard events, True if Data Available.
    bool update();
    void reset()   { inputSequence.erase(); }

    std::string getInputSequence() const { return inputSequence; }
    bool isGlobalShutdown() const { return globalShutdown; }

    // mouse events
    //bool getMouseButtonState(int buttonNumber) const;

private:

    void setInputSequence(std::string sequence) { inputSequence = sequence; }

    bool globalShutdown;
    bool fullScreen;
    int fullScreenWindowSize;
    std::string inputSequence; // Keyboard Input
    std::string inputText;     // Copy/Paste Input

    InputHandler();
    ~InputHandler();

    InputHandler(const InputHandler&);
    InputHandler& operator=(const InputHandler&);

    // singleton
    static InputHandler* globalInstance;

    const unsigned char CTRLKEYTABLE[26] = {
    CTRLA, CTRLB, CTRLC, CTRLD, CTRLE,
    CTRLF, CTRLG, CTRLH, CTRLI, CTRLJ,
    CTRLK, CTRLL, CTRLM, CTRLN, CTRLO,
    CTRLP, CTRLQ, CTRLR, CTRLS, CTRLT,
    CTRLU, CTRLV, CTRLW, CTRLX, CTRLY,
    CTRLZ };
};
typedef InputHandler TheInputHandler;


#endif
