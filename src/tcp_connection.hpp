#ifndef TCP_CONNECTION_HPP
#define TCP_CONNECTION_HPP

// #include <boost/asio.hpp>

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
/*
    tcp_connection(boost::asio::io_service& io_service)
        : m_socket(io_service)
    {
        std::cout << "tcp_connection Created" << std::endl;
    }
    ~tcp_connection()
    {
        std::cout << "~tcp_connection" << std::endl;
    }

    **
     * @brief Is the Socket Created and Open
     * @return
     *
    bool is_open()
    {
        return m_socket.is_open();
    }

    **
     * @brief Handle to Socket
     * @return
     *
    boost::asio::ip::tcp::socket& socket()
    {
        return m_socket;
    }

    **
     * @brief Shutdowns an Open Connection
     *
    void shutdown()
    {
        try
        {
            if (m_socket.is_open())
            {
                m_socket.shutdown(tcp::socket::shutdown_both);
            }
        }
        catch (std::exception ex)
        {
            std::cout << "tcp_connection shutdown() - Caught exception: " << ex.what();
        }
    }

    **
     * @brief Closes an Open (Maybe Not Connected Socket)
     *
    void close()
    {
        try
        {
            if (m_socket.is_open())
            {
                m_socket.close();
            }
        }
        catch (std::exception ex)
        {
            std::cout << "tcp_connection close() - Caught exception: " << ex.what();
        }
    }*/

   // boost::asio::ip::tcp::socket m_socket;
};

typedef std::shared_ptr<tcp_connection> connection_ptr;

#endif // TCP_CONNECTION_HPP
