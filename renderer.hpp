#ifndef __TERM_H_
#define __TERM_H_

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#ifdef TARGET_OS_MAC
#include <SDL2/SDL.h>
#ifdef _DEBBUG
#include <SDL2/SDL_ttf.h>
#endif
#elif _WIN32
#include <SDL.h>
#ifdef _DEBBUG
#include <SDL_ttf.h>
#endif
#else
#include <SDL2/SDL.h>
#ifdef _DEBBUG
#include <SDL2/SDL_ttf.h>
#endif
#endif

#include <iostream>
#include <string>
#include <vector>

class Renderer
{
public:

    static Renderer* Instance()
    {
        if(!m_globalInstance)
        {
            m_globalInstance = new Renderer();
            return m_globalInstance;
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

    // Pass Starting Screen and Font Values.
    bool init(const char* title,
              int swidth, int sheight,
              int wwidth, int wheight,
              int fwidth, int fheight);

    void clean();

    void setProgramPath(std::string _programPath)
    {
        m_programPath = _programPath;
    }
    std::string getProgramPath() const
    {
        return m_programPath;
    }
    SDL_Renderer* getRenderer() const
    {
        return m_globalRenderer;
    }
    SDL_Window* getWindow()   const
    {
        return m_globalWindow;
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
        return m_systemConnection;
    }
    void setSystemConnection(SystemConnection systemConnection)
    {
        m_systemConnection = systemConnection;
    }
    void clearSystemConnection()
    {
        m_systemConnection.name.erase();
        m_systemConnection.ip.erase();
        m_systemConnection.port=0;
        m_systemConnection.protocol.erase();
        m_systemConnection.login.erase();
        m_systemConnection.password.erase();
        m_systemConnection.font.erase();
        m_systemConnection.keyMap.erase();
    }

    bool running()
    {
        return m_isRunning;
    }
    void quit()
    {
        m_isRunning = false;
    }
    int getSurfaceWidth() const
    {
        return m_surfaceWidth;
    }
    int getSurfaceHeight() const
    {
        return m_surfaceHeight;
    }
    void setWindowWidth(int width)
    {
        m_windowWidth = width;
    }
    void setWindowHeight(int height)
    {
        m_windowHeight = height;
    }
    int getWindowWidth() const
    {
        return m_windowWidth;
    }
    int getWindowHeight() const
    {
        return m_windowHeight;
    }
    bool changingState()
    {
        return m_isChangingState;
    }
    void changingState(bool cs)
    {
        m_isChangingState = cs;
    }
    void setCurrentFont(std::string font)
    {
        std::cout << "*** setCurrentFont: " << font << std::endl;
        m_currentFont = font;
    }
    std::string getCurrentFont() const
    {
        return m_currentFont;
    }
    bool didFontChange() const
    {
        return (m_currentFont != m_previousFont);
    }


    // Now for Rendering Code
    void restartWindowSize(bool fullScreen);
    void restartWindowRenderer(std::string mode);
    void pullSelectionBuffer(int x, int y);
    void clearSelectionTexture();
    void renderSelectionScreen(int x, int y);
    void freeSurfaceTextures();
    //bool loadBitmapImageFromPak();
    bool loadBitmapImage(std::string fontName);

    void createTexture(int textureType, SDL_Surface *surface);
    void fillSurface(SDL_Surface *surface);
    void convertSurface(int surfaceType);
    void createSurface(int surfaceType);

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
    void drawTextureScreen();
    void clearScreen();
    int  compareSDL_Colors(SDL_Color &src, SDL_Color &dest);
    void replaceColor(SDL_Surface *src, Uint32 foreground, Uint32 background);
    void setupCursorChar();
    void drawChar(int X, int Y, int asciicode);
    //void drawString(int X, int Y, char text[]);
    //void drawCharSet(int X, int Y);

/*
    std::vector<std::string> getFontFiles()
    {
        return m_globalFontFiles;
    }*/

    // Matched Colors with Pablo Draw.
    SDL_Color BLACK;
    SDL_Color BLUE;
    SDL_Color GREEN;
    SDL_Color CYAN;
    SDL_Color RED;
    SDL_Color MAGENTA;
    SDL_Color BROWN;

    SDL_Color GREY;
    SDL_Color DARK_GREY;
    SDL_Color LIGHT_BLUE;
    SDL_Color LIGHT_GREEN;
    SDL_Color LIGHT_CYAN;
    SDL_Color LIGHT_RED;
    SDL_Color LIGHT_MAGENTA;
    SDL_Color YELLOW;
    SDL_Color WHITE;

    SDL_Color m_currentFGColor;
    SDL_Color m_currentBGColor;

    // Scrolling Region
    bool m_scrollRegionActive;
    int  m_topMargin;
    int  m_bottomMargin;

private:

    SystemConnection m_systemConnection;
    std::string    m_programPath;
    std::string    m_windowTitle;
    std::string    m_currentFont;
    std::string    m_previousFont;

    SDL_Window*    m_globalWindow;
    SDL_Renderer*  m_globalRenderer;

    // Surfaces
    SDL_Surface*   m_tmpSurface;        // Char Cell
    SDL_Surface*   m_cursorOnSurface;   // Char Cell for the cursor.
    SDL_Surface*   m_cursorOffSurface;  // Char Cell for the cursor.
    SDL_Surface*   m_screenSurface;     // Internal Screen Buffer
    SDL_Surface*   m_bottomSurface;     // Last Line Buffer
    SDL_Surface*   m_cachedSurface;    // Cached Font Surface

    // Textures
    SDL_Texture*   m_globalTexture;     // Texture for User Screen
    SDL_Texture*   m_selectionTexture;  // For Copy Text Selection

    std::vector<SDL_Surface *> m_surfaceList;

    // Handle Surface Alias
    enum
    {
        TEMP_SURFACE = 0,
        CURSOR_ON_SURFACE,
        CURSOR_OFF_SURFACE,
        SCREEN_SURFACE,
        BOTTOM_SURFACE,
        CACHED_SURFACE
    };

    // Handle Texture Alias
    enum
    {
        GLOBAL_TEXTURE = 0,
        SELECTION_TEXTURE
    };

#ifdef _DEBBUG
    TTF_Font*      trueTypeFont;      // UTF-8 Fonts.
#endif

    Uint32 m_redMask,
           m_greenMask,
           m_blueMask,
           m_alphaMask;

    int m_surfaceWidth;
    int m_surfaceHeight;
    int m_windowWidth;
    int m_windowHeight;
    int m_surfaceBits;

    SDL_Rect m_displayRect;
    SDL_Rect m_rectBackground;

    int  m_characterWidth;
    int  m_characterHeight;
    int  m_cursorXPosition;
    int  m_cursorYPosition;

    bool m_isChangingState;
    bool m_isRunning;
    bool m_isUTF8Output;

    static Renderer* m_globalInstance;
    //std::vector<std::string> m_globalFontFiles;

    Renderer();
    ~Renderer();
    Renderer(const Renderer&);
    Renderer& operator=(const Renderer&);
};

typedef Renderer TheRenderer;

#endif
