#include "socket_state.hpp"
#include "socket_handler.hpp"

#include <SDL2_net/SDL_net.hpp>

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <time.h>

/**
 * @class _TCPsocket
 * @author Michael Griffin
 * @date 07/04/2018
 * @file socket_telnet.cpp
 * @brief Pulled from SDLnetTCP needed to access socket channel.
 */
struct _TCPsocket
{
    int ready;
    SOCKET channel;
    IPaddress remoteAddress;
    IPaddress localAddress;
    int sflag;
};

/**
 * Start of IRC_Socket Derived Class
 *
 * send a string buffer over a TCP socket with error checking
 * returns 0 on any errors, length sent on success
 */
int IRC_Socket::sendSocket(unsigned char *buffer, Uint32 length)
{
    int result = 0;

    if(m_is_socket_active)
    {
        result = SDLNet_TCP_Send(m_tcp_socket, buffer, length);

        if(result < (signed)strlen((char *)buffer))
        {
            if(SDLNet_GetError() && strlen(SDLNet_GetError()))
            {
                std::cout << "SDLNet_TCP_Send: " << SDLNet_GetError() << std::endl;
                return(0);
            }
        }
    }

    return(result);
}

int IRC_Socket::recvSocket(char *message)
{
    int result = -1;

    if(m_is_socket_active)
    {
        result = SDLNet_TCP_Recv(m_tcp_socket, message, 8192);

        if(result <= 0)
        {
            // -1 is Error 0 is Server Closed Connection
            return -1;
        }

        message[result] = 0;
    }

    return result;
}

int IRC_Socket::pollSocket()
{
    int num_ready = -1;

    if(m_is_socket_active)
    {
        num_ready = SDLNet_CheckSockets(m_socket_set, 0);

        if(num_ready == -1)
        {
            m_is_socket_active = false;
            return num_ready;
        }

        if(num_ready && SDLNet_SocketReady(m_tcp_socket) > 0)
        {
            num_ready = 1;
        }
        else
        {
            num_ready = 0;
        }
    }

    return num_ready;
}

bool IRC_Socket::onEnter()
{
    IPaddress ip;

    m_socket_set = SDLNet_AllocSocketSet(1);

    if(!m_socket_set)
    {
        std::cout << "SDLNet_AllocSocketSet: " << SDLNet_GetError() << std::endl;
        onExit();
        return false;
    }

    /* Resolve the argument into an IP address type */
    std::cout << "Connecting to " << m_host.c_str() << " on port " << m_port << std::endl;

    if(SDLNet_ResolveHost(&ip, m_host.c_str(), m_port) == -1)
    {
        std::cout << "SDLNet_ResolveHost: " << SDLNet_GetError() << std::endl;
        m_tcp_socket = nullptr;
        onExit();
        return false;
    }

    m_tcp_socket = SDLNet_TCP_Open(&ip);

    if(!m_tcp_socket)
    {
        std::cout << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
        onExit();
        return false;
    }

    if(SDLNet_TCP_AddSocket(m_socket_set, m_tcp_socket) == -1)
    {
        std::cout << "SDLNet_TCP_AddSocket: " << SDLNet_GetError() << std::endl;
        onExit();
        return false;
    }

    // Successful Startup
    m_is_socket_active = true;
    std::cout << "Connection Successful" << std::endl;

    return true;
}

bool IRC_Socket::onExit()
{
    std::cout << "IRC_Socket::onExit()" << std::endl;
    /*  double check global shutdown?!?
    if(TheInputHandler::Instance()->isGlobalShutdown())
     */
    {
        if(m_tcp_socket)
        {
            SDLNet_TCP_Close(m_tcp_socket);
        }

        m_tcp_socket = nullptr;
    }

    if(m_socket_set)
    {
        SDLNet_FreeSocketSet(m_socket_set);
    }

    m_socket_set = nullptr;
    return true;
}

/**
 * @brief Return Socket Instance for Duplication
 * @return
 */
int IRC_Socket::getSocketInstance()
{
    return m_tcp_socket->channel;
}
