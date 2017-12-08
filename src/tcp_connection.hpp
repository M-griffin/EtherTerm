#ifndef TCP_CONNECTION_HPP
#define TCP_CONNECTION_HPP

// #include <boost/asio.hpp>
#include "io_service.hpp"
#include "socket_handler.hpp"

#include <memory>

/**
 * @class tcp_connection
 * @author Michael Griffin
 * @date 15/08/2015
 * @file tcp_connection.hpp
 * @brief Handles a shared socket per individual session
 */
class tcp_connection
{
public:

    tcp_connection(IOService& io_service)
        : m_socket_handle(new SocketHandler())
        , m_io_service(io_service)
    {
        std::cout << "tcp_connection Created" << std::endl;
    }

    ~tcp_connection()
    {
        std::cout << "~tcp_connection" << std::endl;
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
        catch (std::exception ex)
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
        catch (std::exception ex)
        {
            std::cout << "tcp_connection close() - Caught exception: " << ex.what();
        }
    }

    /**
     * @brief Async Read Callback for IOService Work
     */
    template <typename BufferSequence, typename Callback>
    void async_read(BufferSequence &buffer, const Callback &callback)
    {
        m_io_service.addAsyncRead(buffer, nullptr, m_socket_handle, callback, SERVICE_TYPE_READ);
    }

    /**
     * @brief Async Write Callback for IOService Work
     */
    template <typename StringSequence, typename Callback>
    void async_write(StringSequence string, const Callback &callback)
    {
        // Place Holder is used for template parmeters, buffer is used in write
        // Where the Place Holder in the above method is used for reads.
        std::vector<unsigned char> place_holder;
        m_io_service.addAsyncWrite(place_holder, string, m_socket_handle, callback, SERVICE_TYPE_WRITE);
    }

    socket_handler_ptr m_socket_handle;
    IOService          &m_io_service;
};

typedef std::shared_ptr<tcp_connection> connection_ptr;

#endif // TCP_CONNECTION_HPP
