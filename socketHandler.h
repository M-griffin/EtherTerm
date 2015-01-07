#ifndef __SocketHandler__
#define __SocketHandler__

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include <iostream>
#include "socketState.h"

#ifdef TARGET_OS_MAC
#include <SDL.h>
#elif _WIN32
#include <windows.h>
#include <SDL.h>
#else // LINUX
#include <SDL2/SDL.h>
#endif

class SocketHandler
{
public:

    static SocketHandler* Instance()
    {
        if(globalInstance == 0)
        {
            globalInstance = new SocketHandler();
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

    // Socket Events, True if Data Available.
    int send(unsigned char *buf, Uint32 len);
    int recv(char *message);
    int update();

    bool initTelnet(std::string host, int port);
    bool initSSH(std::string host, int port, std::string username, std::string password);

    std::string getSocketType() const
    {
        return socketType;
    }
    bool isActive() const
    {
        return active;
    }
    void setActive(int _active)
    {
        active = _active;
    }
    void reset();

private:

    // Handle Blicking Cursor
    int  cursorBlink;
    bool startBlinking;
    time_t ttime, ttime2;

    SocketState *socket;
    bool active;
    std::string socketType;

    SocketHandler();
    ~SocketHandler();
    SocketHandler(const SocketHandler&);
    SocketHandler& operator=(const SocketHandler&);

    static SocketHandler* globalInstance;

};
typedef SocketHandler TheSocketHandler;

#endif
