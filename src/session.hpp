#ifndef SESSION_BASE_HPP
#define SESSION_BASE_HPP

#include "window_manager.hpp"
#include "surface_manager.hpp"
#include "sequence_parser.hpp"
#include "renderer.hpp"
#include "input_handler.hpp"
#include "tcp_connection.hpp"

#include <boost/enable_shared_from_this.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>


// For IO service!
#include <thread>
#include <deque>
#include <set>

using boost::asio::ip::tcp;

class Session;
typedef boost::shared_ptr<Session> session_ptr;


/**
 * @class Session
 * @author Michael Griffin
 * @date 11/16/2015
 * @file session.hpp
 * @brief Sessions manage each instance of a window / renderer
 */
class Session
    : public boost::enable_shared_from_this<Session>
{
public:

    // Still early build up on session class!
    Session(boost::asio::io_service& io_service, connection_ptr connection, std::set<session_ptr>& sessions, std::string program_path)
        : m_window_manager(new WindowManager())
        , m_surface_manager(new SurfaceManager(m_window_manager, program_path))
        , m_input_handler(new InputHandler(m_surface_manager))
        , m_renderer(new Renderer(m_surface_manager, m_window_manager, m_input_handler, shared_from_this()))
        , m_sequence_parser(new SequenceParser(m_renderer, connection))
        , m_connection(connection)
        , m_io_service(io_service)
        , m_session_list(sessions)
    {

        // NOTES Reallocate by using reset! When rending Term Height/Width Change, need to recrete.
        // m_sequence_parser.reset(new SequenceParser(m_renderer, connection));
    }

    ~Session();

    /**
     * @brief Active Connections etc are send to new sessions on creation.
     * @param io_service
     * @param connection
     * @return
     */
    static session_ptr create(boost::asio::io_service& io_service,
                              connection_ptr connection,
                              std::set<session_ptr>& sessions,
                              std::string program_path)

    {
        session_ptr new_session(new Session(io_service, connection, sessions, program_path));
        return new_session;
    }

    /**
     * @brief Setup the initial Async Socket Data Calls for Managing the connetion.
     */
    void wait_for_data()
    {

    }

    /**
     * @brief Callback from The Broadcaster to write data to the active sessions.
     * @param msg
     */
    void deliver(const std::string &msg)
    {
        if(msg.size() == 0 || msg[0] == '\0')
        {
            return;
        }

        if(m_connection->is_open())
        {
            boost::asio::async_write(m_connection->socket(),
                                     boost::asio::buffer(msg, msg.size()),
                                     boost::bind(
                                         &Session::handleWrite,
                                         shared_from_this(),
                                         boost::asio::placeholders::error
                                     ));

        }
    }


    /**
     * @brief Callback after Writing Data, If error/hangup notifies
     * everything this person has left.
     * @param error
     */
    void handleWrite(const boost::system::error_code& error)
    {
        if(error)
        {
            std::cout << "async_write error: " << error.message() << std::endl;
            std::cout << "Session Closed()" << std::endl;

            if (m_connection->is_open())
            {
                m_connection->socket().shutdown(tcp::socket::shutdown_both);
                m_connection->socket().close();
            }
        }
    }

    /**
     * @brief Cleanups Up Session by poping pointer off stack when done.
     */
    void close_session()
    {
        // Remove our session from the list close the session.
        m_session_list.erase(shared_from_this());
    }

    /*
     * Bottom up hirarchy, Socket -> Decorder -> Ansi Parser -> Surfaces -> Renderer / Window.
     * feel like socket will feed to session class at this moment.
     */

    // Window Managers Creates and Handles Input/Mouse Events Rendering per active instance.
    window_manager_ptr       m_window_manager;

    // Surface and Texture Manager, Handles creating surface/textures to display
    surface_manager_ptr      m_surface_manager;

    // Handles specific input per window session
    input_handler_ptr        m_input_handler;

    // Handle to Rendering.
    renderer_ptr             m_renderer;

    // Handles parsing data to create surfaces and screens
    sequence_parser_ptr      m_sequence_parser;

    // Decodes incoming data and ESC sequences into arrays for the ansi parser
//    data_decorder_ptr      m_data_decorder;

    // Socket_Manager handles sockets and incomming data from the server.
    connection_ptr           m_connection;

    // Place Holder if we need it lateron
    boost::asio::io_service& m_io_service;

    // Handle to session pointers so we can pop our own session off the stack
    std::set<session_ptr>&   m_session_list;

};

#endif // SESSION_BASE_HPP
