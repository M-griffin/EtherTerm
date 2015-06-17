
#ifndef __MY_SOCKET__H
#define __MY_SOCKET__H

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "terminal.h"

#ifdef TARGET_OS_MAC
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#elif _WIN32
#include <SDL.h>
#include <SDL_net.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#endif

#include <libssh/libssh.h>

#include <iostream>
#include <string>

/*
 * Virtual Class Container for Common access to different
 * Socket And connection types.
 */
class SocketState
{
public:

    virtual ~SocketState()
    {
        std::cout << "Shutting Down SocketState" << std::endl;
    }
    virtual int sendSocket(unsigned char *message, Uint32 len) = 0;
    virtual int recvSocket(char *message)= 0;
    virtual int pollSocket() = 0;
    virtual bool onEnter() = 0;
    virtual bool onExit() = 0;
};

/*
 * Class for TCP Telnet Sockets
 */
class SDL_Socket : public SocketState
{
public:

    SDL_Socket(std::string _host, int _port) :
        sock(nullptr),
        set(nullptr)
    {
        host = _host;
        port = _port;
    }

    virtual ~SDL_Socket()
    {
        std::cout << "Shutting Down SDL_SocketState" << std::endl;
    }
    virtual int sendSocket(unsigned char *message, Uint32 len);
    virtual int recvSocket(char *message);
    virtual int pollSocket();
    virtual bool onEnter();
    virtual bool onExit();

private:

    std::string host;
    int port;
    TCPsocket sock;
    SDLNet_SocketSet set;

};

/*
 * Class for TCP FTP Sockets
 */
class FTP_Socket : public SocketState
{
public:

    FTP_Socket(std::string _host, int _port, std::string _user, std::string _pass)
    :
        controlSocket(nullptr),
        listenSocket(nullptr),
        dataSocket(nullptr),
        set(nullptr)
    {
        host = _host;
        port = _port;
        userId = _user;
        password = _pass;
    }

    virtual ~FTP_Socket()
    {
        std::cout << "Shutting Down FTP SDL_SocketState" << std::endl;
    }
    virtual int sendSocket(unsigned char *message, Uint32 len);
    virtual int recvSocket(char *message);
    virtual int pollSocket();
    virtual bool onEnter();
    virtual bool onExit();

private:

    std::string host;
    int port;
    std::string userId;
    std::string password;

    TCPsocket controlSocket;
    TCPsocket listenSocket;
    TCPsocket dataSocket;
    SDLNet_SocketSet set;

};

/*
 * Class for TCP SSH Connections
 */
class SSH_Socket : public SocketState
{
public:

    SSH_Socket(std::string _host, int _port, std::string _user, std::string _pass)
    :
        sshChannel(nullptr),
        session(nullptr)
    {
        host = _host;
        port = _port;
        userId = _user;
        password = _pass;
    }

    virtual  ~SSH_Socket()
    {
        std::cout << "Shutting Down SSH_SocketState" << std::endl;
    }
    virtual int sendSocket(unsigned char *message, Uint32 len);
    virtual int recvSocket(char *message);
    virtual int pollSocket();
    virtual bool onEnter();
    virtual bool onExit();

    // Specific Functions for SSH
    int verify_knownhost();
    int authenticate_console();
    int authenticate_kbdint();
    void error()
    {
        std::cout << "Authentication failed: "
            << ssh_get_error(session) << std::endl;
    }


private:

    ssh_channel sshChannel;
    ssh_session session;

    std::string host;
    int port;
    std::string userId;
    std::string password;

};

#endif
