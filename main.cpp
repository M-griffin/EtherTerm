
/*
 *  EtherTerm (c) 2014-2015 Michael Griffin <mrmisticismo@hotmail.com>
 *  A full Telnet / SSH emulated terminal client in SDL 2.0
 *  With with CP437 Character set and ANSI Graphic Support.
 *
 *  Linker Options: -mwindows to remove Console Debug Window.
 */

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "renderer.hpp"
// Needed for Clean Shutdown
#include "sequenceParser.hpp"
#include "sequenceManager.hpp"
#include "inputHandler.hpp"
#include "socketHandler.hpp"
#include "mainMenuState.hpp"
#include "termStateMachine.hpp"
#include "queueManager.hpp"

#include <unistd.h>
#include <sys/types.h>

#ifdef TARGET_OS_MAC
#include <SDL.h>
#include <SDL_Main.h>
//#include <SDL_net.h>
#include <mach-o/dyld.h>
#elif _WIN32
#include <windows.h>
#include <SDL.h>
#include <SDL_Main.h>
//#include <SDL_net.h>
#else // LINUX
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
//#include <SDL2/SDL_net.h>
#endif

#include <libssh/libssh.h>

#include <cstdio>
#include <cstring>
#include <climits>
#include <string>
#include <iostream>
#include <unistd.h>
/*
 * Shutdown Procedures
 */
void cleanup()
{
    // cleanup theState.
    TheStateMachine::ReleaseInstance();

    // Cleanup Surfaces and Textures
    TheRenderer::Instance()->clean();

    TheSocketHandler::ReleaseInstance();
    TheInputHandler::ReleaseInstance();

    //Release Instances
    TheSequenceParser::ReleaseInstance();
    TheSequenceManager::ReleaseInstance();
    TheQueueManager::ReleaseInstance();

    TheRenderer::ReleaseInstance();
    std::cout << "Shutdown complete." << std::endl;
}

/*
 *  Main Program Entrance
 */
int main(int argc, char* argv[])
{
    // Not used at this time!!
    int c = 0;
    while ((c = getopt (argc, argv, "c:b:a:")) != -1)
    {
        switch(c) {
            default:
                break;
        }
    }


    // Run cleanup on exit of program
    atexit(cleanup);

    // Get the Folder the Executable runs in.
    std::string realPath;
#ifdef TARGET_OS_MAC
    char currentPath[PATH_MAX];
    uint32_t size = sizeof(currentPath);
    if(_NSGetExecutablePath(currentPath, &size) != 0)
    {
        std::cout << "Unable to get Program Path!" << std::endl;
    }

    // Remove Executable
    realPath = currentPath;
    std::string::size_type position;
    position = realPath.rfind("/EtherTerm");
    if(position != std::string::npos)
        realPath.erase(position+1,realPath.size()-1);

    // remove extra './'
    position = realPath.rfind("./");
    if(position != std::string::npos)
        realPath.erase(position,2);

#elif _WIN32
    // Get the Current Program path.
    char currentPath[PATH_MAX];
    int result = GetModuleFileName(NULL, currentPath, PATH_MAX-1);
    if(result == 0)
    {
        std::cout << "Unable to get Program Path!" << std::endl;
        return 1;
    }

    realPath = currentPath;
    std::string::size_type position = realPath.rfind("\\",realPath.size()-1);
    if(position != std::string::npos)
        realPath.erase(position+1);
#else

    char exePath[PATH_MAX] = {0};
#ifdef __FreeBSD__
    ssize_t result = readlink("/proc/curproc/file", exePath, PATH_MAX);
#else
    ssize_t result = readlink("/proc/self/exe", exePath, PATH_MAX);
#endif
    if(result < 0)
    {
        std::cout << "Unable to get Program Path!" << std::endl;
        return 1;
    }

    const char* t = " \t\n\r\f\v";
    realPath = exePath;
    realPath = realPath.erase(realPath.find_last_not_of(t) + 1);
    realPath += "/";

    // Remove Executable
    std::string::size_type position;
    position = realPath.rfind("/EtherTerm");
    if(position != std::string::npos)
        realPath.erase(position+1,realPath.size()-1);

#endif
    std::cout << "Working directory is: " << realPath << std::endl;
    TheRenderer::Instance()->setProgramPath(realPath);

    // Initialize Renderer and Window with default sizes.
    // We define 680 instead of 640 because we clip the extract off
    // in Screen->Texture.  We do this because when Texture filtering is used
    // Pixels on the last line tend to bleed from blending. This clips off the
    // Bleeding Leaving a nice screen.
    if(TheRenderer::Instance()->init("EtherTerm v2.8 alpha preview", 680, 480, 1280, 800, 8, 16))
    {
        // Setup the Surfaces
        if(TheRenderer::Instance()->initSurfaceTextures())
        {


            std::cout << "Surface & Textures Initialized. " << std::endl;
            // Load Font Texture to Surface
            if( TheRenderer::Instance()->loadBitmapImage(TheRenderer::Instance()->getCurrentFont()) )
            {
                std::cout << "Term init success!\n";
                TheRenderer::Instance()->clearScreen();

                // Display intro ANSI Screen
                TheRenderer::Instance()->clearScreenSurface();
                TheSequenceParser::Instance()->reset();

                // Setup main menu state
                // Load into Main Menu System (Dialing Directory)
               //std::shared_ptr<MainMenuState> ms(new MainMenuState());
               //TheTerminal::Instance()->getStateMachine()->changeState(ms);
               TheStateMachine::Instance()->changeState(new MainMenuState());

                while(TheRenderer::Instance()->running())
                {
                    // If the font changed, then load the new image.
                    if(TheRenderer::Instance()->didFontChange())
                        TheRenderer::Instance()->loadBitmapImage(
                            TheRenderer::Instance()->getCurrentFont());

                    // Main Loop, Handle update per State Machine
                    TheStateMachine::Instance()->update();

                    // Proces Any incoming Data from the State
                    // Process and send to Screen for Updates.
                    TheSequenceManager::Instance()->update();
                }
            }
            else
            {
                std::cout << "Error Loading Font." << SDL_GetError() << "\n";
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                                         "Closed Session", "Error Loading Font.", nullptr);
                return -1;
            }
        }
        else
        {
            std::cout << "Term init failure: allocating surfaces." << SDL_GetError() << "\n";
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                                     "Closed Session", "Term init failure: allocating surfaces.", nullptr);
            return -1;
        }
    }
    else
    {
        std::cout << "Term init failure: SDL Init()." << SDL_GetError() << "\n";
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                                 "Closed Session", "Term init failure: SDL Init().", nullptr);
        return -1;
    }

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                             "Closed Session", "User has closed the program.", nullptr);

    return 0;
}
