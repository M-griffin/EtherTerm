#ifndef TCP_CONNECTION_HPP
#define TCP_CONNECTION_HPP

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

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

    tcp_connection(boost::asio::io_service& io_service)
        : m_socket(io_service)
    {
        std::cout << "tcp_connection Created" << std::endl;
    }
    ~tcp_connection()
    {
        std::cout << "~tcp_connection" << std::endl;
        if (m_socket.is_open())
        {
            // Only Shutdown When Conencted, if were get here
            // Connection is already killed.
            //m_socket.shutdown(tcp::socket::shutdown_both);
            m_socket.close();
        }
    }

    /**
     * @brief Is the Socket Created and Open
     * @return
     */
    bool is_open()
    {
        return m_socket.is_open();
    }

    /**
     * @brief Handle to Socket
     * @return
     */
    boost::asio::ip::tcp::socket& socket()
    {
        return m_socket;
    }

    /**
     * @brief Shutdowns an Open Connection
     */
    void shutdown()
    {
        if (m_socket.is_open())
        {
            m_socket.shutdown(tcp::socket::shutdown_both);
        }
    }

    /**
     * @brief Closes an Open (Maybe Not Connected Socket)
     */
    void close()
    {
        if (m_socket.is_open())
        {
            m_socket.close();
        }
    }

    boost::asio::ip::tcp::socket m_socket;
};

typedef boost::shared_ptr<tcp_connection> connection_ptr;

#endif // TCP_CONNECTION_HPP
