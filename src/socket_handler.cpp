
#include "socket_handler.hpp"
#include "socket_state.hpp"

#include <iostream>
#include <exception>

int SocketHandler::sendSocket(unsigned char *buffer, Uint32 length)
{
    return m_socket.back()->sendSocket(buffer, length);
}

/**
 * @brief Receive Waiting Data
 * @param message
 * @return
 */
int SocketHandler::recvSocket(char *message)
{
    return m_socket.back()->recvSocket(message);
}

int SocketHandler::poll()
{
    int ret;
    if(m_is_active)
    {
        ret = m_socket.back()->pollSocket();
        if(ret == -1)
        {
            SDL_Log("Socket Closed by host, disconnecting.");

            // Shutdown Socket.
            m_socket.back()->onExit();
            m_is_active = false;
        }
        else if(ret == 0)  // No Data!
        {

        }
        else
        {
        }
    }
    else
    {
        // Inactive Connection
        SDL_Log("Showdown received, Socket Closed.");
        ret = -1;
    }
    return ret;
}

/**
 * @brief Connect Telnet Socket
 * @param host
 * @param port
 * @return
 */
bool SocketHandler::connectTelnetSocket(std::string host, int port)
{
    std::cout << "SocketHandler::initTelnet" << std::endl;
    if(!m_is_active)
    {
        try
        {
            m_socket_type = SOCKET_TYPE_TELNET;
            socket_state_ptr sdl_socket(new SDL_Socket(host, port));
            m_socket.push_back(sdl_socket);
            if(m_socket.back()->onEnter())
            {
                m_is_active = true;
            }
            else
            {
                SDL_Log("Unable to initialize Telnet Socket.");
                close();
                return false;
            }
        }
        catch(std::exception& e)
        {
            close();
            std::cerr << "exception creating new SDL_Socket: "
                      << e.what() << std::endl;
            return false;
        }
    }
    else
    {
        SDL_Log("Telnet Socket already Active!");
        return false;
    }
    return true;
}

/**
 * @brief Create SSH Socket
 * @param host
 * @param port
 * @param username
 * @param password
 * @return
 */
bool SocketHandler::connectSshSocket(std::string host, int port,
                                     std::string username, std::string password)
{
    if(!m_is_active)
    {
        try
        {
            m_socket_type = SOCKET_TYPE_SSH;
            socket_state_ptr ssh_socket(new SSH_Socket(host, port, username, password));
            m_socket.push_back(ssh_socket);
            if(m_socket.back()->onEnter())
            {
                m_is_active = true;
            }
            else
            {
                SDL_Log("Unable to initialize SSH Socket.");
                close();
                return false;
            }
        }
        catch(std::exception& e)
        {
            std::cerr << "exception creating SSH_Socket: "
                      << e.what() << std::endl;

            m_is_active = false;
            close();
            return false;
        }
    }
    else
    {
        SDL_Log("SSH Socket already Active!");
        return false;
    }
    return true;
}

/**
 * @brief Type of Socket
 * @return
 */
std::string SocketHandler::getSocketType() const
{
    return m_socket_type;
}

/**
 * @brief Check if Socket is Active
 * @return
 */
bool SocketHandler::isActive() const
{
    return m_is_active;
}

/**
 * @brief On Errors, set Inactive
 */
void SocketHandler::setInactive()
{
    m_is_active = false;
}


/**
 * @brief Socket Reset
 */
void SocketHandler::close()
{
    try
    {
        // Deactivate Socket, then Clean it.
        if(m_socket.size() > 0 && m_socket.back())
        {
            m_socket.back()->onExit();
        }
        m_is_active = false;
        m_socket_type.erase();
        m_socket.pop_back();
        std::vector<socket_state_ptr>().swap(m_socket);
    }
    catch(std::exception& e)
    {
        std::cerr << "exception caught: " << e.what() << '\n';
    }
}
