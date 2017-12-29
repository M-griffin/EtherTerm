
#ifndef __MY_SOCKET__H
#define __MY_SOCKET__H

#include <SDL2_net/SDL_net.hpp>

#ifdef _WIN32
#include <winsock2.h>
#endif

#include <libssh/libssh.h>

#include <iostream>
#include <memory>
#include <string>

/**
 * @class SocketState
 * @author Michael Griffin
 * @date 11/12/2017
 * @file socket_state.hpp
 * @brief Base Socket State Template
 */
class SocketState
{
public:

    SocketState(const std::string &host, const int &port)
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


/**
 * @class SDL_Socket
 * @author Blue
 * @date 11/12/2017
 * @file socket_state.hpp
 * @brief SDL / TCP Telnet Socket Template
 */
class SDL_Socket : public SocketState
{
public:

    SDL_Socket(const std::string &host, const int &port)
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


/**
 * @class IRC_Socket
 * @author Blue
 * @date 12/24/2017
 * @file socket_state.hpp
 * @brief SDL / TCP IRC Socket Template
 */
class IRC_Socket : public SocketState
{
public:

    IRC_Socket(const std::string &host, const int &port)
        : SocketState(host, port)
        , m_tcp_socket(nullptr)
        , m_socket_set(nullptr)
    { }

    virtual ~IRC_Socket()
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


/**
 * @class FTP_Socket
 * @author Michael Griffin
 * @date 11/12/2017
 * @file socket_state.hpp
 * @brief FTP Socket Template
 */
class FTP_Socket : public SocketState
{
public:

    FTP_Socket(const std::string &host, const int &port,
               const std::string &user, const std::string &pass)
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


/**
 * @class SSH_Socket
 * @author Michael Griffin
 * @date 11/12/2017
 * @file socket_state.hpp
 * @brief SSH Socket Template
 */
class SSH_Socket : public SocketState
{
public:

    SSH_Socket(const std::string &host, const int &port,
               const std::string &user, const std::string &pass)
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
