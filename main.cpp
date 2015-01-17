
/*
 *  EtherTerm (c) 2014-2015 Michael Griffin <mrmisticismo@hotmail.com>
 *  A full Telnet / SSH emulated terminal clinet in SDL 2.0
 *  With with CP437 Character set and ANSI Graphic Support.
 *
 *  Linker Options: -mwindows to remove Console Debug Window.
 */

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "socketState.h"
#include "telnetState.h"
#include "terminal.h"
// Needed for Clean Shutdown
#include "ansiParser.h"
#include "sequenceParser.h"
#include "inputHandler.h"
#include "socketHandler.h"

#include <unistd.h>
#include <sys/types.h>

#ifdef TARGET_OS_MAC
#include <SDL.h>
#include <SDL_Main.h>
#include <SDL_net.h>
#elif _WIN32
#include <windows.h>
#include <SDL.h>
#include <SDL_Main.h>
#include <SDL_net.h>
#else // LINUX
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_net.h>

const  int  TRUE  = 1;
const  int  FALSE = 0;
#endif

#include <libssh/libssh.h>

#include <cstdio>
#include <cstring>
#include <string>
#include <cstdlib>
#include <sstream>
#include <fstream>

#include <iostream>
#include <exception>

using namespace std;

/**
 * Reads in Ansi file into Buffer Only
 * For Testing the Ansi Parser.
 */
void readinAnsi(std::string FileName, std::string &buff)
{

    std::string path;
#ifdef _WIN32
//    std::string path = "assets\\";
#else
//    std::string path = "assets/";
#endif
    path += FileName;
    FILE *fp;
    std::string::size_type id1 = 0;
    int c = 0;
    if((fp = fopen(path.c_str(), "r+")) ==  NULL)
    {
        printf("\r\n*** ANSI not found\r\n");
        return;
    }
    do
    {
        c = getc(fp);
        if(c != EOF)
            buff += c;
    }
    while(c != EOF);
    fclose(fp);
}

/*
 * Shutdown Procedures
 */
void cleanup()
{
    // Clearnup Surfaces and Textures
    TheTerminal::Instance()->clean();

    //Release Instances
    TheAnsiParser::ReleaseInstance();
    TheSequenceParser::ReleaseInstance();
    TheSocketHandler::ReleaseInstance();
    TheInputHandler::ReleaseInstance();
    TheTerminal::ReleaseInstance();
    std::cout << "Shutdown complete." << std::endl;
    // Pause to check for messages
    //char ch;
    //std::cin.get(ch);
}

/*
 *  Main Program Entrance
 */
int main(int argc, char* argv[])
{
    // Run cleanup on exit of program
    atexit(cleanup);
    std::string temp;

    // Initalize Renderer and Window with default sizes.
    // We define 680 instead of 640 becasue we clip the extract off
    // in Screen->Texure.  We do this becasue when Texture filtering is used
    // Pixels on the last line tend to bleed from bledning. This clips off the
    // Bleeding Leaving a nice screen.
    if(TheTerminal::Instance()->init("EtherTerm v1.0.0a", 680, 480, 1280, 800, 8, 16))
    {
        // Setup the Surfaces
        if(TheTerminal::Instance()->initSurfaceTextures())
        {
            std::cout << "Surface & Textures Initalized. " << std::endl;
            // Load Font Texture to Surface
            if(TheTerminal::Instance()->loadBitmapImage(TheTerminal::Instance()->getCurrentFont()))
            {
                std::cout << "Term init success!\n";
                // Display intro ANSI Screen
                TheTerminal::Instance()->clearScreenSurface();
                TheAnsiParser::Instance()->reset();

                while(TheTerminal::Instance()->running())
                {
                    //frameStart = SDL_GetTicks();
                    // If the font changed, then load the new image.
                    if(TheTerminal::Instance()->didFontChange())
                        TheTerminal::Instance()->loadBitmapImage(
                            TheTerminal::Instance()->getCurrentFont());

                    // Test Ansi Screens
                    //readinAnsi("test.ans",temp);
                    //TheSequenceParser::Instance()->processSequence(temp);
                    //break;

                    // Main Loop
                    TheTerminal::Instance()->update();

                    // Not a FPS, check for Rendering Flag.
                    // Save CPU usage, only switch this on when
                    // We have data to draw to the screen, once
                    // Everything is drawn, then toggle this off
                    // And only check Events and Input.
                    if(TheTerminal::Instance()->getRenderReady())
                    {
                        TheTerminal::Instance()->render();
                    }
                }                
            }
            else
            {
                std::cout << "Error Loading Font " << SDL_GetError() << "\n";
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                    "Closed Session", "User has closed the program.", NULL);
                return -1;
            }
        }
        else
        {
            std::cout << "Term init failure 2. " << SDL_GetError() << "\n";
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                "Closed Session", "Terminal Error on Init 2.", NULL);
            return -1;
        }
    }
    else
    {
        std::cout << "Term init failure 1. " << SDL_GetError() << "\n";
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
            "Closed Session", "Terminal Error on Init 1.", NULL);
        return -1;
    }
    
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
        "Closed Session", "User has closed the program.", NULL);

    return 0;
}
