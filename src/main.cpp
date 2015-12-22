
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

#ifdef TARGET_OS_MAC // OSX
#include <SDL.h>
#include <SDL_Main.h>
#include <mach-o/dyld.h>

#elif _WIN32 // Windows
#include <SDL2/SDL.h>
#include <winsock2.h>
#include <windows.h>
#include <SDL2/SDL_Main.h>

#else // LINUX
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
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
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
		success = false;
	}

    // Turn off, might make a toggle for this later on.
    SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
	return success;
}

/*
 *  Main Program Entrance
 */
int main(int argc, char* argv[])
{
    bool is_headless = false;
    SDL_SetMainReady();

    int c = 0;

    /*
     * Interfaces to be added later on
     * h = headless
     * t = telnet
     * s = secure ssl/ssh
     * f = ftp
     * i = irc
     */
    while ((c = getopt (argc, argv, "h")) != -1)
    {
        switch(c) {
            case 'h':
                // Headless Operation
                is_headless = true;
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
    if (is_headless)
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
        if (!SDLStartUp())
        {
             return -1;
        }

        // Were good to go, lets get the window and initial session going.
        std::cout << "EtherTerm Startup successful." << std::endl;

        bool is_global_shutdown = false;
        SDL_Event event;

        // we need to get the window_id from the main window (menu)
        // if this window closes, then we are done!
        interface_spawn.startup();

        // Main Loop SDL Event Polling must always be done in main thread.
        // Along with All Rendering. This is how SDL works. Events are therefore
        // Read, then passed through to each session to handle per instance.
        while (!is_global_shutdown)
        {
            while(SDL_PollEvent(&event) != 0 && !is_global_shutdown)
            {
                // Send the event along for each session
                interface_spawn.process_event(event);


                switch(event.type)
                {
                    case SDL_QUIT:
                        is_global_shutdown = true;
                        break;
                }
            }
        }

        // Setup the First Window for the Startup Splash Screen and Window.

        // SDL is done.
        std::cout << "EtherTerm Shutdown completed." << std::endl;
        SDL_Quit();
    }

/*
 * SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");

//    TheRenderer::Instance()->setProgramPath(realPath);

    // Initialize Renderer and Window with default sizes.
    // We define 680 instead of 640 because we clip the extract off
    // in Screen->Texture.  We do this because when Texture filtering is used
    // Pixels on the last line tend to bleed from blending. This clips off the
    // Bleeding Leaving a nice screen.

    if(TheRenderer::Instance()->init("EtherTerm v3.0 alpha preview", 680, 480, 1280, 800, 8, 16))
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

                    // Process Any incoming Data from the State
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
    } */

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                             "Closed Session", "User has closed the program.", nullptr);

    return 0;
}
