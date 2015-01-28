#ifndef __TERM_H_
#define __TERM_H_

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#ifdef TARGET_OS_MAC
#include <SDL2/SDL.h>
#elif _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "termStateMachine.h"
#include <vector>

class Terminal
{
public:

    static Terminal* Instance()
    {
        if(globalInstance == 0)
        {
            globalInstance = new Terminal();
            return globalInstance;
        }
        return globalInstance;
    }

    // Release And Clear the Singleton
    static void ReleaseInstance()
    {
        if(globalInstance != 0)
        {
            delete globalInstance;
            globalInstance = 0;
        }
        return;
    }

    // Pass Starting Screen and Font Values.
    bool init(const char* title,
            int swidth, int sheight,
            int wwidth, int wheight,
            int fwidth, int fheight);

    void update();
    void clean();

    SDL_Renderer* getRenderer() const
    {
        return globalRenderer;
    }
    SDL_Window* getWindow()   const
    {
        return globalWindow;
    }
    TermStateMachine* getStateMachine()
    {
        return globalTermStateMachine;
    }

    // Holds current connection
    typedef struct SystemConnection
    {
        std::string name;
        std::string ip;
        int         port;
        std::string protocol;
        std::string login;
        std::string password;  // Make this encrypted with more time.
        std::string font;
        std::string keyMap;    // ANSI, VT100
    } SystemConnection;

    SystemConnection getSystemConnection() const
    {
        return systemConnection;
    }
    void setSystemConnection(SystemConnection _systemConnection)
    {
        systemConnection = _systemConnection;
    }
    void clearSystemConnection()
    {
        systemConnection.name.erase();
        systemConnection.ip.erase();
        systemConnection.port=0;
        systemConnection.protocol.erase();
        systemConnection.login.erase();
        systemConnection.password.erase();
        systemConnection.font.erase();
        systemConnection.keyMap.erase();
    }

    bool running()
    {
        return isRunning;
    }
    void quit()
    {
        isRunning = false;
    }
    int getSurfaceWidth() const
    {
        return surfaceWidth;
    }
    int getSurfaceHeight() const
    {
        return surfaceHeight;
    }
    void setWindowWidth(int width)
    {
        windowWidth = width;
    }
    void setWindowHeight(int height)
    {
        windowHeight = height;
    }
    int getWindowWidth() const
    {
        return windowWidth;
    }
    int getWindowHeight() const
    {
        return windowHeight;
    }
    bool changingState()
    {
        return isChangingState;
    }
    void changingState(bool cs)
    {
        isChangingState = cs;
    }
    void setRenderReady(bool rr)
    {
        isRenderReady = rr;
    }
    bool getRenderReady() const
    {
        return isRenderReady;
    }
    void setCurrentFont(std::string font)
    {
        currentFont = font;
    }
    std::string getCurrentFont() const
    {
        return currentFont;
    }
    bool didFontChange() const
    {
        return (currentFont != previousFont);
    }


    // Now for Rendering Code
    void restartWindowSize(bool fullScreen);
    void restartWindowRenderer(std::string mode);
    void pullSelectionBuffer(int x, int y);
    void clearSelectionTexture();
    void renderSelectionScreen(int x, int y);
    void freeSurfaceTextures();
    bool loadBitmapImageFromPak();
    bool loadBitmapImage(std::string path);
    bool initSurfaceTextures();
    void setScrollRegion(int top, int bot, int terminalHeight);
    void scrollRegionUp();
    void scrollScreenUp();
    void clearScreenSurface();
    void renderDeleteCharScreen(int x, int y, int num);
    void renderClearLineAboveScreen(int y, int x);
    void renderClearLineBelowScreen(int y, int x);
    void renderClearLineScreen(int y, int start, int end);
    void renderBottomScreen();
    void renderScreen();
    void renderCharScreen(int x, int y);
    void renderCursorOnScreen();
    void renderCursorOffScreen();
    void drawGamaTextureScreen();
    void drawTextureScreen();
    void clearScreen();
    int  compareSDL_Colors(SDL_Color &src, SDL_Color &dest);
    void replaceColor(SDL_Surface *src, Uint32 foreground, Uint32 background);
    void setupCursorChar();
    void drawChar(int X, int Y, int asciicode);
    void drawString(int X, int Y, char text[]);
    void drawCharSet(int X, int Y);

    std::vector<std::string> getFontFiles()
    {
        return globalFontFiles;
    }

    // Matched Colors with Pablo Draw.
    SDL_Color black;
    SDL_Color blue;
    SDL_Color green;
    SDL_Color cyan;
    SDL_Color red;
    SDL_Color magenta;
    SDL_Color brown;

    SDL_Color grey;
    SDL_Color darkGrey;
    SDL_Color lightBlue;
    SDL_Color lightGreen;
    SDL_Color lightCyan;
    SDL_Color lightRed;
    SDL_Color lightMagenta;
    SDL_Color yellow;
    SDL_Color white;

    SDL_Color currentFGColor;
    SDL_Color currentBGColor;

    // Scrolling Region
    bool scrollRegionActive;
    int  topMargin;
    int  bottomMargin;

private:

    SystemConnection systemConnection;
    std::string    windowTitle;
    std::string    currentFont;
    std::string    previousFont;

    SDL_Window*    globalWindow;
    SDL_Renderer*  globalRenderer;

    //Loaded Surface
    SDL_Surface*   tmpSurface;        // Char Cell
    SDL_Surface*   cursorOnSurface;   // Char Cell for the cursor.
    SDL_Surface*   cursorOffSurface;  // Char Cell for the cursor.
    SDL_Surface*   screenSurface;     // Internal Screen Buffer
    SDL_Surface*   bottomSurface;
    SDL_Surface*   chachedSurface;    // Cached Font CharacterSet
    SDL_Texture*   globalTexture;     // Texture for User Screen

    SDL_Texture*   selectionTexture;  // WIP

    TermStateMachine* globalTermStateMachine;
    Uint32 redMask, greenMask, blueMask, alphaMask;

    int surfaceWidth;
    int surfaceHeight;
    int windowWidth;
    int windowHeight;

    SDL_Rect displayRect;
    SDL_Rect rectBackground;

    int  characterWidth;
    int  characterHeight;
    int  cursorXPosition;
    int  cursorYPosition;

    bool isChangingState;
    bool isRunning;
    bool isRenderReady;

    static Terminal* globalInstance;
    std::vector<std::string> globalFontFiles;

    Terminal();
    ~Terminal();
    Terminal(const Terminal&);
    Terminal& operator=(const Terminal&);
};

typedef Terminal TheTerminal;

#endif
