
/*
 *  EtherTerm (c) 2014-2016 Michael Griffin <mrmisticismo@hotmail.com>
 *  An Emulated Client Terminal in SDL 2.0
 *  With CP437 Character and ANSI Graphics Support.
 *
 *  Linker Options: -mwindows to remove Console Debug Window (WIN32)
 *
 */

#include "interface.hpp"

#include <unistd.h>
#include <sys/types.h>

#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_Main.h>

#ifdef TARGET_OS_MAC // OSX
#include <mach-o/dyld.h>

#elif _WIN32 // Windows
#include <winsock2.h>
#include <windows.h>
#endif

#include <cstdio>
#include <cstring>
#include <climits>
#include <unistd.h>

#include <string>
#include <iostream>

/**
 * @brief Initial Startup for SDL with Video
 * @return
 */
bool SDLStartUp()
{
    bool success = true;

    // Initialize SDL w/ Video, Skip Sound and Controllers for now.
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        success = false;
    }
    
    return success;
}

/*
 *  Main Program Entrance
 */
int main(int argc, char* argv[])
{
    bool is_headless = false;
    SDL_SetMainReady();
    /*
     * Interfaces to be added later on
     * h = headless
     * t = telnet
     * s = secure ssl/ssh
     * f = ftp
     * i = irc
     */
    int c = 0;
    while((c = getopt(argc, argv, "h")) != -1)
    {
        switch(c)
        {
            case 'h':
                // Headless Operation
                is_headless = true;
                break;

            default:
                break;
        }
    }

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
    ssize_t result = readlink("/proc/self/exe", exePath, PATH_MAX);
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
    std::cout << "EtherTerm Working directory is: " << realPath << std::endl;

    /*
     * We want to be able to run the program headless without SDL loaded
     * For automation and testing
     */

    // Setup the interface for spawning session windows.
    Interface interface_spawn(realPath);

    // Don't loaded menu system waiting for user input, just handle
    // Scripts and connections or parsing functions for testing.
    if(is_headless)
    {
        // Execute scripts / connections via command line
        // Don't startup SDL since were not using the video
        // or Waiting for Keyboard input from the window events.

        // Nothing to do right now.
        return 0;
    }
    else
    {
        // Startup the Window System and load initial menu
        // Window with Initialization with window event processing.
        if(!SDLStartUp())
        {
            return -1;
        }

        // Lead into Interface spawn for session startup and management.
        interface_spawn.startup();

        // SDL is done.
        std::cout << "EtherTerm Shutdown completed." << std::endl;
        SDL_Quit();
    }

    // Closing Message Box.
    SDL_ShowSimpleMessageBox(
        SDL_MESSAGEBOX_INFORMATION,
        "Closed Session",
        "User has closed the program.",
        nullptr
    );

    return 0;
}
