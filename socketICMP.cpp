
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

// WIP, not in use at the moment.
// Was an idea to ping servers in dialing directory
// for idea of connection lag before connecting to system.

#include "socketState.h"
#include "inputHandler.h"

#ifdef TARGET_OS_MAC
#include <SDL.h>
#include <SDL_net.h>
#elif _WIN32
#include <windows.h>
#include <SDL.h>
#include <SDL_net.h>
#else // LINUX
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#endif
#include <libssh/libssh.h>

#include <cstdio>
#include <cstring>
#include <cerrno>


/*
 * Start of ICMP_Socket Dervived Class (PING)
 *
 *
 */


/* send a string buffer over a TCP socket with error checking */
/* returns 0 on any errors, length sent on success */
int ICMP_Socket::sendSocket(unsigned char *buf, Uint32 len)
{
    Uint32 result = 0;
    /*

    // send the buffer, with the NULL as well
    result = SDLNet_TCP_Send(sock, buf, len);
    //printf("\r\n SOCKET SEND Len: %d, result: %u, str: %s", len, result, buf);

    if(result < strlen((char *)buf))
    {
        if(SDLNet_GetError() && strlen(SDLNet_GetError()))
        {
            printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
            return(0);
        }
    }

    // return the length sent
     */
    return(result);
}


/* handle Telnet better */
int ICMP_Socket::recvSocket(char *message)
{

    Uint32 result = 0;
    /*
    std::string buf;

    // get the string buffer over the socket
    result = SDLNet_TCP_Recv(sock, message, 4096);
    //printf("\r\n SOCKET RECV Len: %d, result: %u \r\n", strlen((char *)message), result);

    // TCP Connection is broken. (because of error or closure)
    if(result <= 0)
    {
        onExit();
        return -1;
    }

    // return the new buffer
     */
    return result;
}

int ICMP_Socket::pollSocket()
{

    int numready = SDLNet_CheckSockets(set, 0);
    if(numready == -1)
    {
        std::cout << "SDLNet_CheckSockets: " << SDLNet_GetError() << std::endl;
        return numready;
    }

    if (numready && SDLNet_SocketReady(sock) == TRUE)
        numready = 1;
    else
        numready = 0;

    return numready;

}

bool ICMP_Socket::onEnter()
{

    /*
    std::cout << "ICMP_Socket::onEnter()" << std::endl;
    IPaddress ip;

    set=SDLNet_AllocSocketSet(1);
    if(!set)
    {
        std::cout << "SDLNet_AllocSocketSet: " << SDLNet_GetError() << std::endl;
        onExit();
        return false;
    }

    // Resolve the argument into an IPaddress type
    std::cout << "Connecting to " << host.c_str() << " on port " << port << std::endl;

    if(SDLNet_ResolveHost(&ip,host.c_str(),port) == -1)
    {
        std::cout << "SDLNet_ResolveHost: " << SDLNet_GetError() << std::endl;
        onExit();
        return false;
    }

 *
 * This needs to be repoaces, always sets datagram with port,
 * Need to rewrite a new one for ICMP
 *
    // open the server socket
    sock=SDLNet_UDP_Open(0); // 0 random port for now.
    if(!sock)
    {
        std::cout << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
        onExit();
        return false;
    }


    if(SDLNet_UDP_AddSocket(set,sock)==-1)
    {
        std::cout << "SDLNet_UDP_AddSocket: " << SDLNet_GetError() << std::endl;
        onExit();
        return false;
    }



    // Allocate memory for the packet
    if (!(packet = SDLNet_AllocPacket(512)))
    {
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }

    // bind server address to channel 0
    if(SDLNet_UDP_Bind(sock, 0, &ip) == -1)
    {
        std::cout << "SDLNet_UDP_Bind: " << SDLNet_GetError() << std::endl;
        onExit();
        return false;
    }
*/
    return true;
}

bool ICMP_Socket::onExit()
{
    std::cout << "ICMP_Socket::onExit()" << std::endl;

    //if (TheInputHandler::Instance()->isGlobalShutdown())
        //SDLNet_FreePacket(packet);
        //SDLNet_TCP_Close(sock);

    return true;
}