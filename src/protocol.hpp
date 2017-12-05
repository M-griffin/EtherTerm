#ifndef PROTOCOLS_HPP
#define PROTOCOLS_HPP

#include <iostream>
#include <memory>
#include <string>

class Session;
typedef std::shared_ptr<Session> session_ptr;
typedef std::weak_ptr<Session> session_weak_ptr;

class tcp_connection;
typedef std::shared_ptr<tcp_connection> connection_ptr;
typedef std::weak_ptr<tcp_connection> connection_wptr;

class Protocol
{
public:
    Protocol(session_ptr session, connection_ptr connection, std::string program_path)
        : m_weak_session(session)
        , m_weak_connection(connection)
        , m_program_path(program_path)
        , m_socket_duplicate(0)
    {
        std::cout << "Protocol" << std::endl;
    }

    ~Protocol()
    {
        std::cout << "~Protocol" << std::endl;
    }


    /*
     * Setup Assets Path
     */
    std::string getProgramPath();

    /*
     * Setup Downloads Path
     */
    std::string getDownloadPath();

    /**
     * @brief Helper to append protocol path
     */
    std::string getProtocolPath();

    /**
     * @brief Check for Windows NT Compitable System
     * @return
     */
    bool isWinNT();

    /**
     * @brief Start Call for passing socket to external protocols.
     */
    void executeProtocols();

    /**
     * @brief Call an Extenral process
     * @param socket_descriptor
     */
    void executeProcess(int socket_descriptor);


    session_weak_ptr     m_weak_session;
    connection_wptr      m_weak_connection;
    std::string          m_program_path;
    
    // Duplicate Socket Handle
    int                  m_socket_duplicate;
};

typedef std::shared_ptr<Protocol> protocol_ptr;

#endif // PROTOCOLS_HPP
