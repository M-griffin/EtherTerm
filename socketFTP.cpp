
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "socketState.h"
#include "socketHandler.h"
#include "inputHandler.h"
#include "ansiParser.h"

#ifdef TARGET_OS_MAC
#include <SDL2/SDL_net.h>
#elif _WIN32
#include <SDL_net.h>
#else
#include <SDL2/SDL_net.h>
#endif

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <time.h>

/*
 * Start of FTP_Socket Derived Class (FTP)
 */
/* send a string buffer over a TCP socket with error checking */
/* returns 0 on any errors, length sent on success */
int FTP_Socket::sendSocket(unsigned char *buf, Uint32 len)
{
    Uint32 result;

    result = SDLNet_TCP_Send(controlSocket, buf, len);
    if(result < strlen((char *)buf))
    {
        if(SDLNet_GetError() && strlen(SDLNet_GetError()))
        {
            printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
            return(0);
        }
    }
    return(result);
}


/* handle Telnet better */
int FTP_Socket::recvSocket(char *message)
{
    Uint32 result;
    std::string buf;

    /* get the string buffer over the socket */
    result = SDLNet_TCP_Recv(controlSocket, message, 8192);
    // TCP Connection is broken. (because of error or closure)
    if(result <= 0)
    {
        onExit();
        return -1;
    }
    /* return the new buffer */
    return result;
}

int FTP_Socket::pollSocket()
{
    int numready = SDLNet_CheckSockets(set, 0);
    if(numready == -1)
    {
        std::cout << "SDLNet_CheckSockets: " << SDLNet_GetError() << std::endl;
        TheSocketHandler::Instance()->setActive(false);
        return numready;
    }
    if(numready && SDLNet_SocketReady(controlSocket) > 0)
    {
        numready = 1;
    }
    else
    {
        numready = 0;
    }
    return numready;
}

bool FTP_Socket::onEnter()
{
    std::cout << "FTP_Socket::onEnter()" << std::endl;
    IPaddress ip;

    set = SDLNet_AllocSocketSet(1);
    if(!set)
    {
        printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        onExit();
        return false;
    }

    /* Resolve the argument into an IP address type */
    std::cout << "Connecting to " << host.c_str() << " on port " << port << std::endl;
    if(SDLNet_ResolveHost(&ip,host.c_str(),port) == -1)
    {
        printf("SDLNet_ResolveHost: %s\n",SDLNet_GetError());
        onExit();
        return false;
    }

    //open the socket
    controlSocket = SDLNet_TCP_Open(&ip);
    if(!controlSocket)
    {
        printf("SDLNet_TCP_Open: %s\n",SDLNet_GetError());
        onExit();
        return false;
    }

    if(SDLNet_TCP_AddSocket(set,controlSocket) == -1)
    {
        printf("SDLNet_TCP_AddSocket: %s\n",SDLNet_GetError());
        onExit();
        return false;
    }

    /* reference Only
    if(SDLNet_TCP_DelSocket(set,controlSocket) == -1)
    {
        printf("SDLNet_DelSocket: %s\n",SDLNet_GetError());
        onExit();
        return false;
    } */
    return true;
}

bool FTP_Socket::onExit()
{
    std::cout << "FTP_Socket::onExit()" << std::endl;
    if(TheInputHandler::Instance()->isGlobalShutdown())
    {
        SDLNet_TCP_Close(controlSocket);
    }
    SDLNet_FreeSocketSet(set);
    return true;
}
