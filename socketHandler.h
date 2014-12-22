#ifndef __SocketHandler__
#define __SocketHandler__

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include <iostream>
#include "socketState.h"

#ifdef _WIN32
    #include <SDL.h>
#else
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

    // Socket Events, True if Data Available.
    int send(unsigned char *buf, Uint32 len);
    int recv(char *message);
    int update();

    bool initTelnet(std::string host, int port);
    bool initSSH(std::string host, int port, std::string username, std::string password);

    std::string getSocketType() const { return socketType; }
    bool isActive() const { return active; }
    void setActive(int _active) { active = _active; }
    void reset();

private:

    SocketState *socket;

    bool active;
    std::string socketType;

    SocketHandler();
    ~SocketHandler();

    SocketHandler(const SocketHandler&);
    SocketHandler& operator=(const SocketHandler&);

    // singleton
    static SocketHandler* globalInstance;

};
typedef SocketHandler TheSocketHandler;


#endif
