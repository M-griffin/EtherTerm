#ifndef __SocketHandler__
#define __SocketHandler__

#include "socket_state.hpp"
#include "io_service.hpp"

#ifdef TARGET_OS_MAC
#include <SDL2/SDL.h>
#elif _WIN32
#include <winsock2.h>
#include <windows.h>
#include <SDL.h>
#else // LINUX
#include <SDL2/SDL.h>
#endif

#include <iostream>
#include <vector>
#include <memory>

class SocketHandler
{
public:

    SocketHandler(IOService& io_service) 
        : m_io_service(io_service)
        , m_socket()    
        , m_socket_type("")
        , m_is_active(false)
    {
    }
    
    ~SocketHandler()
    {
        std::vector<socket_state_ptr>().swap(m_socket);
    }
    
    std::string const SOCKET_TYPE_TELNET = "TELNET";
    std::string const SOCKET_TYPE_SSH = "SSH";
    std::string const SOCKET_TYPE_FTP = "FTP";

    // Socket Events, True if Data Available.
    int sendSocket(unsigned char *buf, Uint32 len);
    int recvSocket(char *message);
    int poll();

    bool createTelnetSocket(std::string host, int port);
    bool createSshSocket(std::string host, int port, std::string username, std::string password);

    std::string getSocketType() const;
    bool isActive() const;
    void close();

    IOService &getIOService() const 
    {
        return m_io_service;
    }
    
private:

    IOService                      &m_io_service;
    std::vector<socket_state_ptr>   m_socket;    
    std::string                     m_socket_type;
    bool                            m_is_active;

};

typedef std::shared_ptr<SocketHandler> socket_handler_ptr;

#endif
