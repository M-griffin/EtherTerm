#ifndef SESSION_HPP
#define SESSION_HPP

#include "window_manager.hpp"
#include "surface_manager.hpp"
#include "sequence_parser.hpp"
#include "sequence_decoder.hpp"
#include "renderer.hpp"
#include "input_handler.hpp"
#include "async_connection.hpp"
#include "safe_queue.hpp"
#include "message_queue.hpp"
#include "menu_manager.hpp"
#include "session_manager.hpp"
#include "telnet_manager.hpp"
#include "irc_manager.hpp"
#include "safe_queue.hpp"
#include "protocol.hpp"

//#include <chrono>
#include <fstream>
#include <memory>
#include <thread>
#include <deque>
#include <set>

class Session;
typedef std::shared_ptr<Session> session_ptr;
typedef std::weak_ptr<Session> session_weak_ptr;


/**
 * @class Session
 * @author Michael Griffin
 * @date 11/16/2015
 * @file session.hpp
 * @brief Sessions manage each instance of a window / renderer
 */
class Session
    : public std::enable_shared_from_this<Session>
{
public:

    // Used for Unit Testing.
    explicit Session() { }

    Session(
        connection_ptr           &connection,
        std::string              &program_path,
        session_manager_ptr      &session_manager,
        system_connection_ptr    &system_connection,
        int                      &term_height,
        int                      &term_width
    )
        : m_is_transfer(false)
        , m_is_dial_directory(false)
        , m_term_height(term_height)
        , m_term_width(term_width)
        , m_char_height(16)
        , m_char_width(8)
        , m_program_path(program_path)
        , m_weak_session_manager(session_manager)
        , m_window_manager(
              new WindowManager(
                  (m_term_height * m_char_height),
                  (m_term_width * m_char_width)
              )
          )
        , m_surface_manager(new SurfaceManager(m_window_manager, program_path))
        , m_connection(connection)
        , m_system_connection(system_connection)
        , m_is_connected(false)
        , m_is_leaving(false)
        , m_is_shutdown(false)
        , m_cursor_blink(2)
        , m_start_blinking(false)
        , m_ttime(0)
        , m_ttime2(0)
    {
        std::cout << "Session Created!" << std::endl;

        m_in_data_vector.reserve(0);
        m_in_data_vector.resize(0);

        // NOTES Reallocate by using reset! When rending Term Height/Width Change, need to recreate.
        // m_sequence_parser.reset(new SequenceParser(m_renderer, connection));
    }

    ~Session()
    {
        std::cout << "~Session" << std::endl;

        // Clear any Data left over in the Buffer.
        std::vector<unsigned char>().swap(m_in_data_vector);
    }

    /**
     * @brief Starts up the Rendering and Sequence Decoder which needs a handle back to the session.
     * We create on Startup becasue the session much be created before it can be passed.
     */
    void startup()
    {
        // Can't Pass Shared_From_This() in initializer list, must wait till object is constructed.
        m_input_handler.reset(new InputHandler(m_surface_manager, shared_from_this()));
        m_renderer.reset(
            new Renderer(
                m_surface_manager,
                m_window_manager,
                m_input_handler,
                shared_from_this(),
                m_term_height,
                m_term_width
            )
        );
        m_sequence_parser.reset(new SequenceParser(m_renderer, m_connection));
        m_sequence_decoder.reset(new SequenceDecoder(shared_from_this()));

        m_protocol.reset(new Protocol(shared_from_this(), m_connection, m_program_path));
        // On creation, load a list of Available Font Sets XML
        bool is_loaded = m_surface_manager->readFontSets();
        if(!is_loaded)
        {
            std::cout << "Error: m_surface_manager->readFontSets()" << std::endl;
            assert(false);
        }
        else
        {
            std::cout << "Fontset Created!" << std::endl;
        }
    }

    /**
     * @brief Active Connections etc are send to new sessions on creation.
     * @param io_service
     * @param connection
     * @return
     */
    static session_ptr create(
        connection_ptr           connection,
        session_manager_ptr      &session_manager,
        std::string              &program_path,
        system_connection_ptr    system_connection,
        int                      &term_height,
        int                      &term_width)

    {
        // Create and Pass back the new Session Instance.
        session_ptr new_session(
            new Session(
                connection,
                program_path,
                session_manager,
                system_connection,
                term_height,
                term_width
            )
        );
        return new_session;
    }

    /**
     * @brief Create Async Connection
     * @param connection_string
     * @param protocol
     */
    void createConnection(std::string connection_string, std::string protocol)
    {
        m_connection->async_connect(connection_string,
                                    protocol,
                                    std::bind(
                                        &Session::handleConnection,
                                        shared_from_this(),
                                        std::placeholders::_1));
    }

    /**
     * @brief Async Connection Handler
     * @param error
     * @param new_session
     */
    void handleConnection(const std::error_code& error)
    {
        if (error)
        {
            std::cout << "Unable to Connect, closing down session." << std::endl;
            // Close the Socket here so shutdown doesn't call both close() and shutdown() on socket.
            if(m_connection->socket()->isActive())
            {
                m_connection->socket()->close();
            }
            
            m_is_leaving = true;
            std::cout << "Connection Closed, Leaving Session! " << std::endl;

            // Shutdown the Input handler.
            if(!m_input_handler->isGlobalShutdown())
            {
                session_manager_ptr session_mgr = m_weak_session_manager.lock();
                if(session_mgr)
                {
                    session_mgr->leave(shared_from_this());
                }
            }
            return;
        }

        m_is_connected = true;
        waitForSocketData();
    }

    /**
     * @brief Setup the initial Async Socket Data Calls for Managing the connection.
     */
    void waitForSocketData()
    {
        // Important, clear out buffer before each read.
        // A reference is passed by Session through the IO Service.
        std::vector<unsigned char>().swap(m_in_data_vector);
        if(m_connection->socket()->isActive() && !m_is_shutdown)
        {
            m_connection->async_read(m_in_data_vector,
                                     std::bind(
                                         &Session::handleRead,
                                         shared_from_this(),
                                         std::placeholders::_1));
        }
        else
        {
            std::cout << "waitForSocketData: Lost Connection." << std::endl;
        }
    }

    /**
     * @brief Callback after data received. handles telnet options
     * Then parses out normal text data from client to server.
     * @param error
     * @param bytes_transferred
     */
    void handleRead(const std::error_code& error) //, size_t bytes_transferred)
    {
        //std::cout << "* handleRead: " << error << std::endl;
        if(error)
        {
            std::cout << "* Async Read Error!" << error << std::endl;
        }

        session_manager_ptr session_mgr = m_weak_session_manager.lock();
        if(session_mgr && !m_is_shutdown)
        {
            if(!error)
            {
                // Parse incoming data, send through decoder
                //std::string incoming_data = m_raw_data;

                if(m_system_connection->protocol == "TELNET")
                {
                    // Handle Telnet option parsing from incoming data.
                    // Returns Test and ESC Sequence, All Options are Responsed to in the Manager.
                    if(m_telnet_manager)
                    {
                        std::string parsed_data = "";
                        if(!m_is_transfer)
                        {
                        	parsed_data = m_telnet_manager->parseIncomingData(m_in_data_vector);
                        }
                        else
                        {
                            // Were in a file transfer, forward
                            parsed_data = std::string(m_in_data_vector.begin(), m_in_data_vector.end());
                            
                            // need some detection when transfer is completed!
                            return;
                        }

                        // Debugging ESC Sequences and data incoming from server.
                        
                        std::ofstream ostrm("stream_output.txt", std::ios_base::app);
                        if (ostrm.is_open())
                        {
                            ostrm << parsed_data << std::flush;
                        }

                        // Process Only if data in the buffer.
                        if(parsed_data.size() > 0)
                        {
                            m_sequence_decoder->decodeEscSequenceData(parsed_data);
                        }

                        ostrm.close();
                    }
                    else
                    {
                        std::cout << "Error: m_telnet_manager inaccessable!" << std::endl;
                    }
                }
                else if(m_system_connection->protocol == "IRC")
                {
                    std::string incoming_data = "";
                    for(auto c : m_in_data_vector)
                    {
                        // Ignore null's
                        if(c == '\0')
                        {
                            break;
                        }
                        incoming_data += c;
                    }

                    m_irc_manager->handleLineRead(incoming_data);

                }
                else
                {
                    /** Not in use yet, when time comes change to support m_raw_data_vector
                     *  as needed.  Still deciding best way to handle this.
                     */
                    if(m_in_data_vector.size() != 0)
                    {
                        std::string incoming_data = "";
                        for(auto c : m_in_data_vector)
                        {
                            // Ignore null's
                            if(c == '\0')
                            {
                                break;
                            }
                            incoming_data += c;
                        }
                        m_sequence_decoder->decodeEscSequenceData(incoming_data);
                    }
                }

                // Restart Callback to wait for more data.
                // If this step is skipped, then the node will exit
                // since io_service will have no more work!
                if(m_connection->socket()->isActive())
                {
                    waitForSocketData();
                }
                else
                {
                    // TODO, new io service, might need extra shutdown on session here?
                    std::cout << "Error: Session Socket Closed!" << std::endl;
                }

            }
            else
            {
                // First error, we mark leaving, if we loop again, ignore
                if(!m_is_leaving)
                {
                    if(session_mgr && error)
                    {
                        m_is_leaving = true;
                        // Disconenct the session.
                        std::cout << "Connection Closed, Leaving Session! " << std::endl;

                        // Shutdown the Input handler.
                        if(!m_input_handler->isGlobalShutdown())
                        {
                            session_mgr->leave(shared_from_this());
                        }
                    }
                }
            }
        }
        else
        {
            std::cout << "Error, Not connected to the SessionManager!" << std::endl;
            if(m_connection->socket()->isActive())
            {
                // if the socket is still active, shut it down
                m_connection->shutdown();
            }
        }
    }

    /**
     * @brief Callback from The SessionManager to write data to the active sessions.
     * @param msg
     */
    void deliver(const std::string &string_msg)
    {
        if(string_msg.size() == 0 || string_msg[0] == '\0')
        {
            return;
        }

        if(m_connection->socket()->isActive() && !m_is_shutdown)
        {
            m_connection->async_write(string_msg,
                                      std::bind(
                                          &Session::handleWrite,
                                          shared_from_this(),
                                          std::placeholders::_1));
        }
    }

    /**
     * @brief Queues ESC[6n Response Sequences
     * @param string_msg
     */
    void deliverEscResponseSequence(const std::string &string_msg)
    {
        m_write_queue.enqueue(string_msg);
    }

    /**
     * @brief Callback after Writing Data, Check For Errors.
     * @param error
     */
    void handleWrite(const std::error_code& error)
    {
        //std::cout << "* handleWrite" << error << std::endl;
        if(error)
        {
            std::cout << "Async_write Error: " << error.message() << std::endl;
            std::cout << "Session Closed()" << std::endl;

            try
            {
                if(m_connection->is_open())
                {
                    // Only Shutdown when Connected!
                    if(m_is_connected)
                    {
                        m_connection->shutdown();
                    }

                }

            }
            catch (std::exception &ex)
            {
                std::cout << "handleWrite() - Caught Exception on shutdown: " << ex.what();
            }
        }
    }

    /**
     * @brief Handle to launching the dialing directory for local sessions!
     */
    void startMenuInstance()
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

        // Startup the Dialing Directory Sequence, and display initial screen
        m_menu_manager->setupDialDirectory();

        // Fill in Data Layer to Dialing Directory.
        m_menu_manager->updateDialDirectory();
    }

    /**
     * @brief Startup Telnet Option/Sequence Parser
     */
    void createTelnetManager()
    {
        // Setup and Load Font from Dialing Directory for Selected System
        if(m_system_connection->font != "")
        {
            m_surface_manager->setCurrentFont(m_system_connection->font);
            m_surface_manager->loadBitmapFontImage();
        }

        // Make Sure Ansi Parser/Render is Setup for For connection with Black Screen.
        m_sequence_parser->reset();
        m_renderer->renderScreen();
        m_renderer->drawTextureScreen();

        // Startup the Telnet Instatnce and pass the current session along.
        m_telnet_manager.reset(
            new TelnetManager(shared_from_this())
        );
    }

    /**
     * @brief Create Irc Manager when session is starting.
     */
    void createIrcManager()
    {
        m_irc_manager.reset (
            new IrcManager(shared_from_this())
        );
    }

    /**
     * @brief Toggles off the Blinking of the cursor while data is displayed.
     */
    void toggleBlinkingCursor(bool blink)
    {
        m_ttime = SDL_GetTicks();
        m_start_blinking = blink;
        m_cursor_blink = 2;
    }

    /**
     * @brief Process Waiting Data Buffer and pass to rendering
     * @return
     */
    void update()
    {
        // If Menu System, Check for Input to passthrough to the Menu System
        if(m_is_dial_directory)
        {
            std::string input_sequence;
            m_input_handler->getInputSequence(input_sequence);

            if(input_sequence.size() > 0)
            {
                // Menu Key returned EOF for System Exit.
                // Handle Closing Windows and Program.
                if (m_menu_manager->handleMenuUpdates(input_sequence) == EOF)
                {
                    // This is mirror clicking X on the Window and proceed with
                    // Proper shutdown and de-allocation.
                    SDL_Event event;
                    event.type = SDL_WINDOWEVENT;
                    event.window.event = SDL_WINDOWEVENT_CLOSE;
                    event.window.windowID = m_window_manager->getWindowId();
                    SDL_PushEvent(&event);
                    return;
                }
                m_menu_manager->updateDialDirectory();
            }
        }
        else
        {
            // We have Socket Connection, pass input back to the server.
            std::string input_sequence;
            m_input_handler->getInputSequence(input_sequence);
            if(input_sequence.size() > 0)
            {
                if(m_system_connection->protocol == "IRC")
                {
                    m_irc_manager->handleUserInput(input_sequence);
                }
                else
                {
                    // Telnet / SSH etc.. pass through to Server.
                    deliver(input_sequence);
                }
            }
        }

        // If were in the middle of a text selection then
        // Exit and wait till next loop to process data,
        // We want to avoid blinking cursor and rendering while
        // were in the middle of capturing text!
        if(m_input_handler->isMouseSelection())
        {
            return;
        }

        // Check for Processed/Received Session Data waiting to display.
        MessageQueue msgQueue;
        bool was_data_received = !m_data_queue.is_empty();

        // If we received data, turn off blinking while displaying screens.
        if(was_data_received)
        {
            // Turn off the cursor while we process data.
            //m_sequence_parser->setCursorActive(false);
            toggleBlinkingCursor(false);
        }

        while(!m_data_queue.is_empty() && !m_is_shutdown)
        {
            msgQueue = std::move(m_data_queue.dequeue());
            if(msgQueue.m_text.empty())
            {
                // Make Sure Vector is not Empty!
                if(msgQueue.m_queueParams.size() > 0)
                {
                    m_sequence_parser->sequenceInput(msgQueue.m_queueParams);
                }
            }
            else
            {
                m_sequence_parser->textInput(msgQueue.m_text);
            }
            msgQueue.clear();
        }

        // Handle Immediate ESC Position Response Sequences
        // Queue all responses and write out as single output.
        std::string esc_response_sequence = "";
        while (m_write_queue.size() > 0 && !m_is_shutdown)
        {
            esc_response_sequence += m_write_queue.dequeue();
        }

        if (esc_response_sequence.size() > 0)
        {
            deliver(esc_response_sequence);
        }

        // Render Changes on updates only, save CPU usage!
        if(was_data_received)
        {
            // Render all screen data waiting to be seen.
            m_renderer->renderScreen();

            // Setup and render the cursor after rending new screen.
            if(m_sequence_parser->isCursorActive())
            {
                if(m_is_connected)
                {
                    m_renderer->setupCursorCharacter();
                    m_renderer->renderCursorOnScreen();
                }
            }

            // Render Entire Screen now.
            m_renderer->drawTextureScreen();

            // Don't reset while it's active, only flip from inactive to active!
            if(!m_start_blinking)
            {
                toggleBlinkingCursor(true);
            }
        }
        else
        {
            // Setup and render the cursor after rending new screen.
            if(m_sequence_parser->isCursorActive())
            {
                // Loop here to blink the cursor.
                if(m_is_connected)
                {
                    // Setup Timer for Blinking Cursor
                    // Initial State = On, then Switch to off in next loop.
                    if(m_cursor_blink % 2 == 0)
                    {
                        m_ttime2 = SDL_GetTicks();
                        if(m_start_blinking && (m_ttime2 - m_ttime) > 400)
                        {
                            m_renderer->renderCursorOffScreen();
                            m_renderer->drawTextureScreen();
                            --m_cursor_blink;
                            m_ttime = SDL_GetTicks();
                        }
                    }
                    else
                    {
                        m_ttime2 = SDL_GetTicks();
                        if(m_start_blinking && (m_ttime2 - m_ttime) > 400)
                        {
                            m_renderer->renderCursorOnScreen();
                            m_renderer->drawTextureScreen();
                            ++m_cursor_blink;
                            m_ttime = SDL_GetTicks();
                        }
                    }
                }
            }
        }
    }

    /**
     * @brief Cleanups Up Current Session by popping pointer off SessionManager
     */
    void closeThisSession()
    {
        // Remove our session from the list close the session.
        session_manager_ptr session_mgr = m_weak_session_manager.lock();
        if(session_mgr)
        {
            // If dialing directory is closed, then shutdown all windows/sessions
            if(m_is_dial_directory)
            {
                session_mgr->shutdown();
            }
            else
            {
                session_mgr->leave(shared_from_this());
            }
        }
    }

    // Are are transfering files.
    bool                     m_is_transfer;

    // Is this a local session used for the Dialing Directory.
    bool                     m_is_dial_directory;

    // Setup the initial terminal window size by characters
    // This will used to calculate the generated surface and texture sizes
    // ie 80x25, 80x50 etc..
    int                      m_term_height;
    int                      m_term_width;
    int                      m_char_height;
    int                      m_char_width;

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

    // Handle to external protocols (file transfers)
    protocol_ptr             m_protocol;
    // Handle Telnet Option Negoiation and Responses.
    telnet_manager_ptr       m_telnet_manager;

    // IRC Manager for Line and Command Parsing
    irc_manager_ptr          m_irc_manager;

    bool                     m_is_connected;
    bool                     m_is_leaving;
    bool                     m_is_shutdown;

    // Handle Blinking cursor
    int                      m_cursor_blink;
    bool                     m_start_blinking;
    time_t                   m_ttime, m_ttime2;

    // Input Raw Data Buffer.
    std::vector<unsigned char> m_in_data_vector;
    SafeQueue<std::string>     m_write_queue;

};

#endif // SESSION_BASE_HPP
