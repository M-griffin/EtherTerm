#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "session.hpp"
#include "session_manager.hpp"
#include "socket_handler.hpp"
#include "io_service.hpp"

#include <iostream>
#include <thread>
#include <set>

/**
 * @class Interface
 * @author Michael Griffin
 * @date 11/17/2015
 * @file interface.hpp
 * @brief This Interfaces the Main System with Sessions and Async Client Connections.
 *        Must Call Shutdown() on close, this class is not RAII and is shutdown
 *        Only by the MenuSession being closed down or all connections (headless)
 *        Sessions are still used from Main Thread, while the IO_SERVICE handles
 *        Async IO in separate threads for Sockets.
 */
class Interface
{
public:

    /**
     * @brief Create IO_Service Working thread for socket communications.
     * @return
     */
    std::thread create_thread()
    {
        return std::thread([&] { m_io_service.run(); });
    }

    // Setup IO_Service polling for connections. Use Work to keep io_service active.
    // while waiting for new connections.  Only need single thread for all sockets.
    Interface(std::string program_path)
        : m_program_path(program_path)
        , m_session_manager(new SessionManager())
        , m_is_global_shutdown(false)
    {
        std::cout << "Interface Created" << std::endl;
        // Start up worker thread of ASIO. We want socket communications in a separate thread.
        // We only spawn a single thread for IO_Service on start up
        m_thread = create_thread();
    }

    ~Interface()
    {
        std::cout << "~Interface" << std::endl;
        m_io_service.stop();
        m_thread.join();
    }

    /**
     * @brief Starts up Main Loop and Sessions.
     */
    void startup()
    {
        // Startup the initial Menu/Dialing Directory Sessions
        spawnLocalSession();

        // Send the event along for each session
        if(m_session_manager->numberOfSessions() == 0)
        {
            std::cout << "Startup Unsuccessful, Shutting Down!" << std::endl;
            m_is_global_shutdown = true;
            return;
        }
        else
        {
            std::cout << "Startup Successful!" << std::endl;
        }

        SDL_Event event;

        // Main Loop SDL Event Polling must always be done in main thread.
        // Along with All Rendering. This is how SDL works. Events are therefore
        // Read, then passed through to each session to handle per instance.
        while(!m_is_global_shutdown)
        {
            try
            {
                // Send the event along for each session
                if(m_session_manager->numberOfSessions() == 0)
                {
                    std::cout << "Shutting Down!" << std::endl;
                    m_is_global_shutdown = true;
                    return;
                }

                // Process SDL Events for Window and Key Input
                while(SDL_PollEvent(&event) != 0 && !m_is_global_shutdown)
                {
                    process_event(event);
                }

                // All sessions Closed, then exit.
                if(m_session_manager->numberOfSessions() == 0)
                {
                    std::cout << "Shutting Down!" << std::endl;
                    m_is_global_shutdown = true;
                    break;
                }

                // Process Each Event and Data Queue of Sessions in the Manager.
                m_session_manager->update();

                // Check for Spawned Connection here. System connection container will be populated
                // With connection info and data to be passed and spawned.
                if(m_session_manager)
                {
                    // First Check for any Disconnected systems, If so, regrab window focus
                    m_session_manager->grabNewWindowFocus();

                    // Now test for any waiting connection spawns
                    while(!m_session_manager->m_new_connections.is_empty())
                    {
                        system_connection_ptr new_system;
                        new_system = std::move(m_session_manager->m_new_connections.dequeue());

                        std::cout << "Swaping a new system connection from Interface!" << std::endl;
                        spawnConnectionSession(new_system);
                    }
                }

                // Delay for CPU Usage, 10 ms is pretty good.
                SDL_Delay(10);

            }
            catch(std::exception &e)
            {
                std::cerr << e.what();
            }
        }
    }

    /**
     * @brief Spawns Local Session For Dialing Directory Interface
     */
    void spawnLocalSession()
    {

        // Hard Coded for testing, move this to direcotry.ini!
        int height = 25;
        int width = 80;

        // Pass Nullptr on Connection and System Connection since this is a local session.
        session_ptr new_session = Session::create(
                                      nullptr,
                                      m_session_manager,
                                      m_program_path,
                                      nullptr,
                                      height,
                                      width
                                  );

        // Join the SessionManager to keep instance properly active!
        m_session_manager->join(new_session);

        // Call Startup outside of constructor for shared_from_this() handle.
        new_session->startup();

        // Start the Dialing Directory on the local instance
        new_session->startMenuInstance();
    }

    /**
     * @brief Spawn Connection Sessions
     *        Connection Sessions handle their own IO
     */
    void spawnConnectionSession(system_connection_ptr system_connection)
    {
        if(!system_connection)
        {
            return;
        }

        // Basic Debugging.
        std::cout << "Connecting to: "
                  << system_connection->protocol
                  << " "
                  << system_connection->name
                  << " "
                  << system_connection->ip
                  << " "
                  << system_connection->port
                  << std::endl;

        // Connection handles TCP_Socket
        connection_ptr new_connection(new async_connection(m_io_service));

        // Hard Coded for testing, move this to direcotry.ini!
        int height = 25;
        int width = 80;

        // Create the Session before passing to the Connection (Async Thread)
        session_ptr new_session = Session::create(
                                      new_connection,
                                      m_session_manager,
                                      m_program_path,
                                      system_connection,
                                      height,
                                      width
                                  );
        std::cout << "Connected Session Created" << std::endl;

        // Call Startup outside of constructor for shared_from_this() handle.
        new_session->startup();

        // If Telnet Session spawn it, Add Other Instances later on, SSH, IRC, FTP etc..
        // TODO, think this sets up the ansi parser might need to break this out.
        if(system_connection->protocol == "TELNET" || system_connection->protocol == "SSH")
        {
            std::cout << "Starting TelnetManager in Interface" << std::endl;
            new_session->createTelnetManager();
        }

        // Start Async Read/Writes Loop for Session Socket Data.
        // Required only for Sessions that make connections, Local sessions do not use this.
        // Add Session to the SessionManager
        m_session_manager->join(new_session);
        std::cout << "New Connection Session pre-added to Session Manager" << std::endl;

        // Start Blinking Cursor for Active connection session.
        new_session->m_sequence_parser->setCursorActive(true);

        // Setup the Connection String
        std::string connection_string = system_connection->ip;
        connection_string.append(":");
        connection_string.append(std::to_string(system_connection->port));
        
        // TODO Change this to WriteSequence and use vector, incase passwords or others has colons
        // That could casue issues on string split!
        if(system_connection->protocol == "SSH")
        {
            connection_string.append(":");
            connection_string.append(system_connection->login);
            connection_string.append(":");
            connection_string.append(system_connection->password);
        }

        new_session->createConnection(connection_string, system_connection->protocol);
    }

    /**
     * @brief Process SDL_TextInput and SDL_Window Events per session
     * I beleive this should be a Single Event at a time passed through!
     * @param event
     */
    void process_event(SDL_Event &event)
    {
        int num_sesisons = m_session_manager->numberOfSessions();
        if(num_sesisons > 0 && !m_is_global_shutdown)
        {
            std::set<session_ptr>::iterator itEnd = end(m_session_manager->m_sessions);
            for(auto it = begin(m_session_manager->m_sessions); it != itEnd; ++it)
            {
                // If number of sessions changed, (Window was Closed) restart loop.
                if(m_session_manager->numberOfSessions() == num_sesisons)
                {
                    // If the window id matches the current event, then
                    // send through the event to the specific session for processing.
                    if((*it)->m_window_manager->getWindowId() == event.window.windowID)
                    {
                        // Pass the Events to the specific Session for Specific Window and Input Events.
                        (*it)->m_input_handler->update(event);
                        return;
                    }
                }
                else
                {
                    // Restart the Loop so we don't miss any events on Window Closes.
                    // The number or sessions will change on a close, so we need to re-initialize the Iterators.
                    if(m_session_manager->numberOfSessions() > 0)
                    {
                        it = begin(m_session_manager->m_sessions);
                        if((*it)->m_window_manager->getWindowId() == event.window.windowID)
                        {
                            // Pass the Events to the specific Session for Specific Window and Input Events.
                            (*it)->m_input_handler->update(event);
                            return;
                        }
                    }
                    else
                    {
                        // No more sessions exist, exit
                        std::cout << "No more sessions exist, Complete Shutdown!" << std::endl;
                        return;
                    }
                }
            }
        }
    }

    // Custom Async IO Service.
    IOService                      m_io_service;
    std::string                    m_program_path;

    // Spawned Session List
    session_manager_ptr            m_session_manager;

    std::thread                    m_thread;
    bool                           m_is_global_shutdown;

};

#endif // INTERFACE_HPP
