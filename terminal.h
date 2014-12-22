#ifndef __TERM_H_
#define    __TERM_H_

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#ifdef _WIN32
    #include <SDL.h>
#else
    #include <SDL2/SDL.h>
#endif

#include "termStateMachine.h"
#include <vector>

class Term
{
public:

    static Term* Instance()
    {
        if(globalInstance == 0)
        {
            globalInstance = new Term();
            return globalInstance;
        }

        return globalInstance;
    }

    bool init(const char* title, int swidth, int sheight, int wwidth,  int wheight);

    void render();
    void update();
    void handleEvents();
    void clean();

    SDL_Renderer*     getRenderer() const { return globalRenderer; }
    SDL_Window*       getWindow()   const { return globalWindow; }
    TermStateMachine* getStateMachine()   { return globalTermStateMachine; }

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
    } SystemConnection;


    SystemConnection getSystemConnection() const { return systemConnection; }
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
    }

    bool running() { return isRunning; }
    void quit() {isRunning = false; }

    int getSurfaceWidth() const { return surfaceWidth; }
    int getSurfaceHeight() const { return surfaceHeight; }

    int getWindowWidth() const { return windowWidth; }
    int getWindowHeight() const { return windowHeight; }

    bool changingState() { return isChangingState; }
    void changingState(bool cs) { isChangingState = cs; }

    void setRenderReady(bool rr) { isRenderReady = rr; }
    bool getRenderReady() const { return isRenderReady; }

    void setCurrentFont(std::string font) { currentFont = font; }
    std::string getCurrentFont() const { return currentFont; }

    bool didFontChange() const { return (currentFont != previousFont); }

    // Now for Rendering Code
    bool loadBitmapImageFromPak();
    bool loadBitmapImage( std::string path );
    bool InitSurfaceTextures();
    void Close();
    void SetScrollRegion(int top, int bot);
    void ScrollRegionUp ();
    void ScrollScreenUp ();
    void ClearScreenSurface();
    void RenderClearLineScreen(int y, int start, int end);
    void RenderBottomScreen();
    void RenderScreen();
    void RenderCharScreen(int x, int y);
    void RenderCursorOnScreen(int x, int y);
    void RenderCursorOffScreen(int x, int y);
    void DrawTextureScreen();
    void ClearScreen();
    int  CompareSDL_Colors(SDL_Color &src, SDL_Color &dest);
    void ReplaceColor (SDL_Surface *src, Uint32 foreground, Uint32 background);
    void SetupCursorChar(int X, int Y);
    void DrawChar(int X, int Y, int asciicode);
    void DrawString(int X, int Y, char text[]);
    void DrawCharSet(int X, int Y);

    std::vector<std::string> getFontFiles() { return globalFontFiles; }

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
    int scrollRegionActive;
    int topMargin;
    int botMargin;

private:

    SystemConnection systemConnection;
    std::string    currentFont;
    std::string    previousFont;

    SDL_Window*    globalWindow;
    SDL_Renderer*  globalRenderer;

    //Loaded Surface
    SDL_Surface*   tmpSurface;       // Char Cell
    SDL_Surface*   cursorOnSurface;  // Char Cell for the cursor.
    SDL_Surface*   cursorOffSurface; // Char Cell for the cursor.
    SDL_Surface*   screenSurface;    // Internal Screen Buffer
    SDL_Surface*   bottomSurface;
    SDL_Surface*   chachedSurface;  // Cached Font CharacterSet

    SDL_Texture*   globalTexture;   // Texture for User Screen

    TermStateMachine* globalTermStateMachine;

    Uint32 redMask, greenMask, blueMask, alphaMask;

    int surfaceWidth;
    int surfaceHeight;
    int windowWidth;
    int windowHeight;

    SDL_Rect displayRect;
    SDL_Rect rectBackground;

    int  CHAR_WIDTH;
    int  CHAR_HEIGHT;

    bool isChangingState;
    bool isRunning;
    bool isRenderReady;

    static Term* globalInstance;
    std::vector<std::string> globalFontFiles;

    Term();
    ~Term();

    Term(const Term&);
    Term& operator=(const Term&);
};

typedef Term TheTerm;

#endif
