#ifndef SESSION_BASE_HPP
#define SESSION_BASE_HPP

#include "window_manager.hpp"
#include "surface_manager.hpp"
#include "sequence_parser.hpp"
#include "sequence_decoder.hpp"
#include "renderer.hpp"
#include "input_handler.hpp"
#include "tcp_connection.hpp"
#include "safe_queue.hpp"
#include "message_queue.hpp"
#include "menu_manager.hpp"

#include <boost/enable_shared_from_this.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/weak_ptr.hpp>

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
typedef boost::weak_ptr<Session> session_weak_ptr;


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

    Session(
        //boost::asio::io_service& io_service,
        connection_ptr           connection,
        std::set<session_ptr>&   sessions,
        std::string              program_path
    )
        : m_is_dial_directory(false)
        , m_program_path(program_path)
        , m_window_manager(new WindowManager())
        , m_surface_manager(new SurfaceManager(m_window_manager, program_path))
        , m_input_handler(new InputHandler(m_surface_manager))
        , m_connection(connection)
        //, m_io_service(io_service)
        , m_session_list(sessions)
    {
        std::cout << "Session Created!" << std::endl;
        // NOTES Reallocate by using reset! When rending Term Height/Width Change, need to recreate.
        // m_sequence_parser.reset(new SequenceParser(m_renderer, connection));
    }

    ~Session()
    {
        std::cout << "~Session" << std::endl;
    }

    /**
     * @brief Starts up the Rendering and Sequence Decoder which needs a handle back to the session.
     */
    void startup()
    {
        // Can't Pass Shared_From_This in itializer list, must wait till object is constructed.
        m_renderer.reset(new Renderer(m_surface_manager, m_window_manager, m_input_handler, shared_from_this()));
        m_sequence_parser.reset(new SequenceParser(m_renderer, m_connection));
        m_sequence_decoder.reset(new SequenceDecoder(shared_from_this()));
    }

    /**
     * @brief Active Connections etc are send to new sessions on creation.
     * @param io_service
     * @param connection
     * @return
     */
    static session_ptr create(//boost::asio::io_service& io_service,
                              connection_ptr           connection,
                              std::set<session_ptr>&   sessions,
                              std::string              program_path)

    {
        // Create and Pass back the new Session Instance.
        //session_ptr new_session(new Session(io_service, connection, sessions, program_path));
        session_ptr new_session(new Session(connection, sessions, program_path));

        // Call Startup outside of constrctor for shared_from_this() handle.
        new_session->startup();
        return new_session;
    }

    /**
     * @brief Setup the initial Async Socket Data Calls for Managing the connection.
     */
    void wait_for_data()
    {

        // Add code to for call back on data received from Server!


    }

    /**
     * @brief Callback from The Broadcaster to write data to the active sessions.
     * @param msg
     */
    void deliver(const std::string &msg)
    {
        std::cout << "SESSION DELIVER" << std::endl;

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
     * @brief Callback after Writing Data, Check Errors.
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
     * @brief Handle to launch the dialing directory for local sessions!
     */
    void start_menu_instance()
    {
        // Allocate a new Dialing Directory Instance on existing placeholder.
        m_menu_manager.reset(
            new MenuManager(
                m_program_path,
                m_renderer,
                m_sequence_decoder
            )
        );
        m_is_dial_directory = true;
    }

    /**
     * @brief Cleanups Up Session by popping pointer off stack when done.
     */
    void close_session()
    {
        // Remove our session from the list close the session.
        m_session_list.erase(shared_from_this());
    }


    // Is this a local session used for the Dialing Directory.
    bool                     m_is_dial_directory;

    // Program Path for File locations.
    std::string              m_program_path;

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
    sequence_decoder_ptr     m_sequence_decoder;

    // Socket_Manager handles sockets and incoming data from the server.
    connection_ptr           m_connection;
   
    // Place Holder if we need it later on
//    boost::asio::io_service& m_io_service;

    // Decoded Data Queue for Sequence Parser.
    SafeQueue<MessageQueue>  m_data_queue;

    // Handle to session pointers so we can pop our own session off the stack
    std::set<session_ptr>    m_session_list;

    // Dialing Directory for Local Session.
    menu_manager_ptr         m_menu_manager;

};

#endif // SESSION_BASE_HPP
