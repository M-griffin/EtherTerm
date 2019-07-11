
/*
 *  EtherTerm (c) 2014-2019 Michael Griffin <mrmisticismo@hotmail.com>
 *  An Emulated Client Terminal in SDL 2.0
 *  With CP437 Character and ANSI Graphics Support.
 *
 *  Linker Options: -mwindows to remove Console Debug Window (WIN32)
 *
 */

#include "interface.hpp"
#include "dialing_manager.hpp"

#include <unistd.h>
#include <sys/types.h>

#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>

#ifdef TARGET_OS_MAC // OSX
#include <mach-o/dyld.h>

#elif _WIN32 // Windows
#include <SDL2/SDL_Main.h>
#include <winsock2.h>
#include <windows.h>
#else
#endif

#include <SDL2_net/SDL_net.hpp>

#include <cstdio>
#include <cstring>
#include <climits>
#include <unistd.h>

#include <string>
#include <iostream>
#include <memory>

/**
 * @brief Program Initilization
 * @return
 */
bool startup(std::string &program_path)
{
    bool success = true;

    // Initialize SDL w/ Video, Skip Sound and Controllers for now.
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        success = false;
    }

    if(SDLNet_Init()==-1)
    {
        std::cout << "SDL_Net could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        success = false;
    }

    // Turn off, might make a toggle for this later on.
    SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");

    // Validate Dialing Directory here, if doesn't exist create default with an entry.
    dial_manager_ptr dial_manager(new DialingManager(program_path));

    if(!dial_manager->validateFile())
    {
        std::cout << "Dialing Directory Validation has failed. " << std::endl;
        success = false;
    }

    return success;
}

/**
 * @brief Main Program Entrance
 * @param argc
 * @param argv
 * @return
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
    std::string real_path;
#ifdef TARGET_OS_MAC
    char current_path[PATH_MAX];
    uint32_t size = sizeof(current_path);

    if(_NSGetExecutablePath(current_path, &size) != 0)
    {
        std::cout << "Unable to get Program Path!" << std::endl;
    }

    // Remove Executable
    real_path = current_path;
    std::string::size_type position;
    position = real_path.rfind("/EtherTerm");

    if(position != std::string::npos)
        real_path.erase(position+1,real_path.size()-1);

    // remove extra './'
    position = real_path.rfind("./");

    if(position != std::string::npos)
        real_path.erase(position,2);

#elif _WIN32
    // Get the Current Program path.
    char current_path[PATH_MAX];
    int result = GetModuleFileName(NULL, current_path, PATH_MAX-1);

    if(result == 0)
    {
        std::cout << "Unable to get Program Path!" << std::endl;
        exit(1);
    }

    real_path = current_path;
    std::string::size_type position = real_path.rfind("\\",real_path.size()-1);

    if(position != std::string::npos)
        real_path.erase(position+1);

#else

    char exe_path[PATH_MAX] = {0};
#ifdef __FreeBSD__
    ssize_t result = readlink("/proc/curproc/file", exe_path, PATH_MAX);
#else
    ssize_t result = readlink("/proc/self/exe", exe_path, PATH_MAX);
#endif

    if(result < 0)
    {
        std::cout << "Unable to get Program Path!" << std::endl;
        exit(1);
    }

    const char* t = " \t\n\r\f\v";
    real_path = exe_path;
    real_path = real_path.erase(real_path.find_last_not_of(t) + 1);
    real_path += "/";

    // Remove Executable
    std::string::size_type position;
    position = real_path.rfind("/EtherTerm");

    if(position != std::string::npos)
        real_path.erase(position+1,real_path.size()-1);

#endif
    std::cout << "EtherTerm Working directory is: " << real_path << std::endl;

    /*
     * We want to be able to run the program headless without SDL loaded
     * For automation and testing
     */

    // Setup the interface for spawning session windows.
    // Using Smart Pointer will clear final allocation prior to full exit of system.
    {
        interface_ptr interface_spawn(new Interface(real_path));

        // Don't loaded menu system waiting for user input, just handle
        // Scripts and connections or parsing functions for testing.
        if(is_headless)
        {
            // Execute scripts / connections via command line
            // Don't startup SDL since were not using the video
            // or Waiting for Keyboard input from the window events.

            // Nothing to do right now.
            std::cout << "is_headless not implemented - exiting" << std::endl;
            interface_spawn->m_is_global_shutdown = true;
            SDL_Delay(2000);
            exit(0);
        }
        else
        {
            // Startup the Window System and load initial menu
            // Window with Initialization with window event processing.
            if(!startup(real_path))
            {
                std::cout << "Startup issues - exiting" << std::endl;
                interface_spawn->m_is_global_shutdown = true;
                SDL_Delay(2000);
                exit(-1);
            }

            // Lead into Interface spawn for session startup and management.
            interface_spawn->startup();
        }
    }

    // SDL is done.
    std::cout << "EtherTerm Shutdown completed." << std::endl;
    SDL_Quit();

    // Closing Message Box.
    SDL_ShowSimpleMessageBox(
        SDL_MESSAGEBOX_INFORMATION,
        "Closed Session",
        "User has closed the program.",
        nullptr
    );

    exit(0);
}
