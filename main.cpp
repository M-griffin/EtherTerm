
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

//#include "socketState.hpp"
//#include "telnetState.hpp"
#include "terminal.hpp"
// Needed for Clean Shutdown
#include "sequenceParser.hpp"
#include "sequenceManager.hpp"
#include "inputHandler.hpp"
#include "socketHandler.hpp"

#include <unistd.h>
#include <sys/types.h>

#ifdef TARGET_OS_MAC
#include <SDL.h>
#include <SDL_Main.h>
#include <SDL_net.h>
#include <mach-o/dyld.h>
#elif _WIN32
#include <windows.h>
#include <SDL.h>
#include <SDL_Main.h>
#include <SDL_net.h>
#else // LINUX
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_net.h>
#endif

#include <libssh/libssh.h>

#include <cstdio>
#include <cstring>
#include <climits>
#include <string>
#include <cstdlib>
#include <sstream>
#include <fstream>

#include <iostream>
#include <exception>

/**
 * Reads in ANSI file into Buffer Only
 * For Testing the ANSI Parser.
 */
void readinAnsi(std::string FileName, std::string &buff)
{
#ifdef _WIN32
    std::string path = "assets\\";
#else
    std::string path = "assets/";
#endif
    path += FileName;
    FILE *fp;

    int c = 0;
    if((fp = fopen(path.c_str(), "r+")) ==  nullptr)
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
    // Cleanup Surfaces and Textures
    TheTerminal::Instance()->clean();

    TheSocketHandler::ReleaseInstance();
    TheInputHandler::ReleaseInstance();

    //Release Instances
    TheSequenceParser::ReleaseInstance();
    TheSequenceManager::ReleaseInstance();

    TheTerminal::ReleaseInstance();
    std::cout << "Shutdown complete." << std::endl;
    // Pause to check shutdown messages before window closes.
    //char ch;
    //std::cin.get(ch);
}

/*
 * Screen Test Cases to make sure
 * EOL and Screen Erasing functions are working properly.
 */
void runTestScreenCases()
{

    /*
    // OK!
    // Insert * at 11y 40x.
    std::string seq("\x1b[11;40H*");
    TheSequenceParser::Instance()->processSequence(seq);

    // OK!
    // Line 11 Erase All Text After *, then All Text Before. Leaving only *
    seq = "\x1b[K\x1b[2D\x1b[1K";
    TheSequenceParser::Instance()->processSequence(seq);

    // OK!
    // Line 13 Erase Entire line.
    seq = "\x1b[13;40H*\x1b[2K";
    TheSequenceParser::Instance()->processSequence(seq);

    // OK!
    //Delete all text after and below.
    seq = "\x1b[23;40H*\x1b[J";
    TheSequenceParser::Instance()->processSequence(seq);

    // OK!
    // Delete all text before and above.
    seq = "\x1b[2;40H*\x1b[2D\x1b[1J";
    TheSequenceParser::Instance()->processSequence(seq);

    // OK - Works correct, might not be the correct behavior through!
    // Move to Line 17, position 1, and newline erase after newline.
    seq = "\x1b[17;10H\r\n";
    TheSequenceParser::Instance()->processSequence(seq);
    */
}

class argv_range
{
public:
    argv_range(int argc, const char *const argv[])
        : argc_(argc), argv_(argv)
    { }
    const char *const *begin() const
    {
        return argv_;
    }
    const char *const *end() const
    {
        return argv_ + argc_;
    }

private:
    const int argc_;
    const char *const *argv_;
};

/*
 *  Main Program Entrance
 */
int main(int argc, char* argv[])
{
    std::cout << argc << " arguments" << std::endl;
    for(auto *arg: argv_range(argc, argv))
    {
        // Do something.
        std::cout << arg << " Parameters" << std::endl;
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
    std::cout << "Working directory is: " << realPath << std::endl;
    TheTerminal::Instance()->setProgramPath(realPath);

    // Initialize Renderer and Window with default sizes.
    // We define 680 instead of 640 because we clip the extract off
    // in Screen->Texture.  We do this because when Texture filtering is used
    // Pixels on the last line tend to bleed from blending. This clips off the
    // Bleeding Leaving a nice screen.
    if(TheTerminal::Instance()->init("EtherTerm v2.7 alpha preview", 680, 480, 1280, 800, 8, 16))
    {
        // Setup the Surfaces
        if(TheTerminal::Instance()->initSurfaceTextures())
        {
            std::cout << "Surface & Textures Initialized. " << std::endl;
            // Load Font Texture to Surface
            if(TheTerminal::Instance()->loadBitmapImage(TheTerminal::Instance()->getCurrentFont()))
            {
                std::cout << "Term init success!\n";
                TheTerminal::Instance()->clearScreen();

                // Display intro ANSI Screen
                TheTerminal::Instance()->clearScreenSurface();
                TheSequenceParser::Instance()->reset();

                while(TheTerminal::Instance()->running())
                {
                    // If the font changed, then load the new image.
                    if(TheTerminal::Instance()->didFontChange())
                        TheTerminal::Instance()->loadBitmapImage(
                            TheTerminal::Instance()->getCurrentFont());

#ifdef _DEBUG
                    // Test-Case ANSI Cursor Functions
                    /*
                    std::string temp;
                    readinAnsi("testcase.ans", temp);
                    TheSequenceParser::Instance()->processSequence(temp);
                    runTestScreenCases();
                    break;
                    */
#endif

                    // Main Loop, Handle update per State Machine
                    TheTerminal::Instance()->update();

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
