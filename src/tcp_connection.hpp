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
    }

    boost::asio::ip::tcp::socket& socket()
    {
        return m_socket;
    }

    boost::asio::ip::tcp::socket m_socket;
};

typedef boost::shared_ptr<tcp_connection> connection_ptr;

#endif // TCP_CONNECTION_HPP
