#ifndef ASYNC_CONNECTION_HPP
#define ASYNC_CONNECTION_HPP

#include "io_service.hpp"
#include "socket_handler.hpp"

#include <memory>

/**
 * @class async_connection
 * @author Michael Griffin
 * @date 12/09/2017
 * @file async_connection.hpp
 * @brief Handles Shared Sockets Async Requests to IOService.
 */
class async_connection
{
public:

    explicit async_connection(IOService& io_service)
        : m_socket_handle(new SocketHandler())
        , m_io_service(io_service)
    {
        std::cout << "async_connection Created" << std::endl;
    }

    ~async_connection()
    {
        std::cout << "~async_connection" << std::endl;
    }

    /**
     * @brief Is the Socket Created and Open
     * @return
     */
    bool is_open()
    {
        return m_socket_handle->isActive();
    }

    /**
     * @brief Handle to Socket
     * @return
     */
    socket_handler_ptr socket()
    {
        return m_socket_handle;
    }

    /**
     * @brief Shutdowns an Open Connection
     */
    void shutdown()
    {
        try
        {
            if (m_socket_handle->isActive())
            {
                m_socket_handle->close();
            }
        }
        catch (std::exception &ex)
        {
            std::cout << "tcp_connection shutdown() - Caught exception: " << ex.what();
        }
    }

    /**
     * @brief Closes an Open (Maybe Not Connected Socket)
     */
    void close()
    {
        try
        {
            if (m_socket_handle->isActive())
            {
                m_socket_handle->close();
            }
        }
        catch (std::exception &ex)
        {
            std::cout << "async_connection close() - Caught exception: " << ex.what();
        }
    }

    /**
     * @brief Async Read Callback for IOService Work
     * @param BufferSequence - populates will data retrieved
     * @param Callback - returns error code
     */
    template <typename BufferSequence, typename Callback>
    void async_read(BufferSequence &buffer, const Callback &callback)
    {
        m_io_service.addAsyncJob(buffer, nullptr, m_socket_handle, callback, SERVICE_TYPE_READ);
    }

    /**
     * @brief Async Write Callback for IOService Work
     * @param StringSequence - Data to Pass to Socket
     * @param Callback - returns error code
     */
    template <typename StringSequence, typename Callback>
    void async_write(StringSequence string_seq, const Callback &callback)
    {
        // Place Holder is used for template parmeters, string_seq is used in writes
        // Where the Buffer Place Holder in the above method is used for reads.
        // nullptr can't be passed as reference for vector
        std::vector<unsigned char> place_holder;
        m_io_service.addAsyncJob(place_holder, string_seq, m_socket_handle, callback, SERVICE_TYPE_WRITE);
    }

    /**
     * @brief Async Connection Callback for IOService Work
     * @param StringSequence - Host:Port
     * @param Callback - return error code and handles to new session
     */
    template <typename StringSequence, typename Protocol, typename Callback>
    void async_connect(StringSequence string_seq, Protocol protocol, const Callback &callback)
    {
        // Place Holder is used for template parmeters, string_seq is used in writes
        // Where the Buffer Place Holder in the above method is used for reads.
        // nullptr can't be passed as reference for vector
        std::vector<unsigned char> place_holder;
        int service_type = SERVICE_TYPE_NONE;

        if(protocol == "TELNET")
        {
            service_type = SERVICE_TYPE_CONNECT_TELNET;
        }
        else if (protocol == "SSH")
        {
            service_type = SERVICE_TYPE_CONNECT_SSH;
        }
        else if (protocol == "IRC")
        {
            service_type = SERVICE_TYPE_CONNECT_IRC;
        }


        m_io_service.addAsyncJob(place_holder, string_seq, m_socket_handle, callback, service_type);
    }

    socket_handler_ptr  m_socket_handle;
    IOService          &m_io_service;
};

typedef std::shared_ptr<async_connection> connection_ptr;

#endif // ASYNC_CONNECTION_HPP
