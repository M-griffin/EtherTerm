
#ifndef __MY_SOCKET__H
#define __MY_SOCKET__H

#ifdef TARGET_OS_MAC
#include <SDL2/SDL_net.h>
#elif _WIN32
#include <SDL_net.h>
#include <winsock2.h>
#else
#include <SDL2/SDL_net.h>
#endif

#include <libssh/libssh.h>

#include <iostream>
#include <memory>
#include <string>

/*
 * Virtual Class Container for Common access to different
 * Socket And connection types.
 */
class SocketState
{
public:

    SocketState(std::string host, int port)
        : m_host(host)
        , m_port(port)
        , m_is_socket_active(false)
    { }

    virtual ~SocketState()
    {
        std::cout << "Shutting Down SocketState" << std::endl;
    }

    virtual int sendSocket(unsigned char *message, Uint32 len) = 0;
    virtual int recvSocket(char *message)= 0;
    virtual int pollSocket() = 0;
    virtual bool onEnter() = 0;
    virtual bool onExit() = 0;

    std::string m_host;
    int         m_port;
    bool        m_is_socket_active;
};

typedef std::shared_ptr<SocketState> socket_state_ptr;

/*
 * Class for TCP Telnet Sockets
 */
class SDL_Socket : public SocketState
{
public:

    SDL_Socket(std::string host, int port)
        : SocketState(host, port)
        , m_tcp_socket(nullptr)
        , m_socket_set(nullptr)
    { }

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

    TCPsocket        m_tcp_socket;
    SDLNet_SocketSet m_socket_set;

};


/*
 * Class for TCP FTP Sockets
 */
class FTP_Socket : public SocketState
{
public:

    FTP_Socket(std::string host, int port, std::string user, std::string pass)
        : SocketState(host, port)
        , m_user_id(user)
        , m_password(pass)
        , m_control_socket(nullptr)
        , m_listen_socket(nullptr)
        , m_data_socket(nullptr)
        , m_socket_set(nullptr)
    { }

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

    std::string      m_user_id;
    std::string      m_password;

    TCPsocket        m_control_socket;
    TCPsocket        m_listen_socket;
    TCPsocket        m_data_socket;
    SDLNet_SocketSet m_socket_set;
};


/*
 * Class for TCP SSH Connections
 */
class SSH_Socket : public SocketState
{
public:

    SSH_Socket(std::string host, int port, std::string user, std::string pass)
        : SocketState(host, port)
        , m_user_id(user)
        , m_password(pass)
        , m_ssh_channel(nullptr)
        , m_ssh_session(nullptr)
    { }

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
                  << ssh_get_error(m_ssh_session) << std::endl;
    }

private:

    std::string m_user_id;
    std::string m_password;

    ssh_channel m_ssh_channel;
    ssh_session m_ssh_session;

};



#endif
