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
#include "session_manager.hpp"

#include <boost/enable_shared_from_this.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/weak_ptr.hpp>
#include <boost/make_shared.hpp>

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
        connection_ptr           connection,
        std::string              program_path,
        session_manager_ptr      session_manager,
        system_connection_ptr    system_connection
    )
        : m_is_dial_directory(false)
        , m_program_path(program_path)
        , m_weak_session_manager(session_manager)
        , m_window_manager(new WindowManager())
        , m_surface_manager(new SurfaceManager(m_window_manager, program_path))
        , m_connection(connection)
        , m_system_connection(system_connection)
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
     * We create on Startup becasue the session much be created before it can be passed.
     */
    void startup()
    {
        // Can't Pass Shared_From_This in itializer list, must wait till object is constructed.
        m_input_handler.reset(new InputHandler(m_surface_manager, shared_from_this()));
        m_renderer.reset(new Renderer(m_surface_manager, m_window_manager, m_input_handler, shared_from_this()));
        m_sequence_parser.reset(new SequenceParser(m_renderer, m_connection));
        m_sequence_decoder.reset(new SequenceDecoder(shared_from_this()));

        // On creation, load a list of Font Sets XML
        bool is_loaded = m_surface_manager->readFontSets();
        if (!is_loaded)
        {
            std::cout << "Error: m_surface_manager->readFontSets()" << std::endl;
            assert(false);
        }
        else
        {
            std::cout << "Fontset Created!" << std::endl;
        }

        // Interface Testing USE ONLY
        // Don't Startup menu_managers for other sessions, this is one off!
        //m_menu_manager.reset(new MenuManager(m_program_path, m_renderer, m_sequence_decoder));

        // Load the Font set and set the default colors for testing!
        /*
        m_surface_manager->setCurrentFont("topazPlus-8x16.bmp");
        m_surface_manager->loadBitmapImage();

        m_renderer->m_current_fg_color = m_renderer->WHITE;
        m_renderer->m_current_bg_color = m_renderer->BLACK;

        std::cout << "display ET2.ans" << std::endl;
        m_menu_manager->m_menu_function.m_menu_io.displayAnsiFile("et2.ans");

        std::cout << "message queue" << std::endl;
        MessageQueue msgQueue;
        while(!m_data_queue.is_empty())
        {
            std::cout << "message dequeue" << std::endl;
            msgQueue = std::move(m_data_queue.dequeue());
            if(msgQueue.m_text.empty())
            {
                // Make Sure Vector is not Empty!
                if(msgQueue.m_queueParams.size() > 0)
                {
                    std::cout << "m_sequence_ Input" << std::endl;
                    m_sequence_parser->sequenceInput(msgQueue.m_queueParams);
                }
            }
            else
            {
                std::cout << "m_sequence_ textInput" << std::endl;
                m_sequence_parser->textInput(msgQueue.m_text);
            }
            msgQueue.clear();
        }

        m_renderer->renderScreen();
        m_renderer->drawTextureScreen();

        std::cout << "Done!" << std::endl;
        */

        // Test
        //m_sequence_decoder->decodeEscSequenceData("Testing");

        // Test initial Graphics with writting out the Character set.
        /*
        m_surface_manager->loadBitmapImage();

        m_renderer->m_current_fg_color = m_renderer->WHITE;
        m_renderer->m_current_bg_color = m_renderer->BLACK;

        m_renderer->drawCharSet(0, 0);
        m_renderer->renderScreen();
        m_renderer->drawTextureScreen();*/
    }

    /**
     * @brief Active Connections etc are send to new sessions on creation.
     * @param io_service
     * @param connection
     * @return
     */
    static session_ptr create(//boost::asio::io_service& io_service,
        connection_ptr           connection,
        session_manager_ptr      session_manager,
        std::string              program_path,
        system_connection_ptr    system_connection)

    {
        // Create and Pass back the new Session Instance.
        session_ptr new_session(new Session(connection, program_path, session_manager, system_connection));
        return new_session;
    }

    /**
     * @brief Setup the initial Async Socket Data Calls for Managing the connection.
     */
    void waitForSocketData()
    {

        // Add code to for call back on data received from Server!
        // Handle Socket Data

    }

    /**
     * @brief Callback from The SessionManager to write data to the active sessions.
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
     * @brief Callback after Writing Data, Check For Errors.
     * @param error
     */
    void handleWrite(const boost::system::error_code& error)
    {
        if(error)
        {
            std::cout << "async_write error: " << error.message() << std::endl;
            std::cout << "Session Closed()" << std::endl;

            if(m_connection->is_open())
            {
                m_connection->socket().shutdown(tcp::socket::shutdown_both);
                m_connection->socket().close();
            }
        }
    }

    /**
     * @brief Handle to launching the dialing directory for local sessions!
     */
    void startMenuInstance()
    {
        // Allocate a new Dialing Directory Instance on existing placeholder.
        std::cout << "Starting Menu Instatnce:" << std::endl;
        m_menu_manager.reset(
            new MenuManager(
                m_program_path,
                m_renderer,
                m_sequence_decoder
            )
        );
        m_is_dial_directory = true;

        // Startup the Dialing Directory Sequence, and display initial screen
        m_menu_manager->setupDialDirectory();

        // Fill in Data Layer to Dialing Directory.
        m_menu_manager->updateDialDirectory();
    }

    /**
     * @brief Process Waiting Data Buffer and pass to rendering
     * @return
     */
    void update()
    {
        // If Menu System, Check for Input to passthrough to the Menu System
        if (m_is_dial_directory)
        {
            std::string input_sequence;
            m_input_handler->getInputSequence(input_sequence);
            if (input_sequence.size() > 0)
            {
                m_menu_manager->handleMenuUpdates(input_sequence);
                m_menu_manager->updateDialDirectory();
            }
        }
        else
        {
            // We have Socket Connection, pass input back to the server.
            std::string input_sequence;
            m_input_handler->getInputSequence(input_sequence);
            if (input_sequence.size() > 0)
            {
                // Pass to the Socket once we have that setup,
                deliver(input_sequence);
            }
        }

        // Check for Processed/Received Session Data waiting to display.
        MessageQueue msgQueue;
        bool was_data_received = !m_data_queue.is_empty();
        while(!m_data_queue.is_empty())
        {
            //std::cout << "Message dequeue in progress" << std::endl;
            msgQueue = std::move(m_data_queue.dequeue());
            if(msgQueue.m_text.empty())
            {
                // Make Sure Vector is not Empty!
                if(msgQueue.m_queueParams.size() > 0)
                {
                    //std::cout << "m_sequence_ Input" << std::endl;
                    m_sequence_parser->sequenceInput(msgQueue.m_queueParams);
                }
            }
            else
            {
                //std::cout << "m_sequence_ textInput" << std::endl;
                m_sequence_parser->textInput(msgQueue.m_text);
            }
            msgQueue.clear();
        }

        // Render Changes on updates only, save CPU usage!
        if (was_data_received)
        {
            m_renderer->renderScreen();
            m_renderer->drawTextureScreen();
        }
    }

    /**
     * @brief Cleanups Up Current Session by popping pointer off SessionManager
     */
    void closeThisSession()
    {
        // Remove our session from the list close the session.
        session_manager_ptr session_mgr = m_weak_session_manager.lock();
        if (session_mgr)
        {
            // If dialing directory is closed, then shutdown all windows/sessions
            if (m_is_dial_directory)
            {
                session_mgr->shutdown();
            }
            else
            {
                session_mgr->leave(shared_from_this());
            }
        }
    }


    // Is this a local session used for the Dialing Directory.
    bool                     m_is_dial_directory;

    // Program Path for File locations.
    std::string              m_program_path;

    // Handle to List of Sessions to Keep Active.
    session_manager_wptr     m_weak_session_manager;

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

    // Decoded Data Queue for Sequence Parser.
    SafeQueue<MessageQueue>  m_data_queue;

    // Dialing Directory for Local Session.
    menu_manager_ptr         m_menu_manager;

    // Holds System Connection Information (struct resides/passed from MenuManager)
    system_connection_ptr    m_system_connection;

};

#endif // SESSION_BASE_HPP
