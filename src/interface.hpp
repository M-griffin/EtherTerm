#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "session.hpp"
#include "broad_caster.hpp"

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>

#include <iostream>
#include <thread>
#include <set>

using boost::asio::ip::tcp;

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
        : m_work(m_io_service)
        , m_program_path(program_path)
        , m_session_list(new BroadCaster())
        , m_is_global_shutdown(false)
    {
        std::cout << "Interface Created" << std::endl;
        // Startup worker thread of ASIO. We want socket communications in a separate thread.
        // We only spawn a single thread for IO_Service on startup.
        m_thread = create_thread();
    }

    ~Interface()
    {
        //std::cout << "~Interface" << std::endl;
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

        // Test 2nd Local Session to test multiple windows.
        spawnLocalSession();

        SDL_Event event;

        // Send the event along for each session
        if (m_session_list->numberOfSessions() == 0)
        {
            std::cout << "Startup Unsuccessful, Shutting Down!" << std::endl;
            m_is_global_shutdown = true;
            return;
        }
        else
        {
            std::cout << "Startup Successful!" << std::endl;
        }

        // Main Loop SDL Event Polling must always be done in main thread.
        // Along with All Rendering. This is how SDL works. Events are therefore
        // Read, then passed through to each session to handle per instance.
        while (!m_is_global_shutdown)
        {

            // Send the event along for each session
            if (m_session_list->numberOfSessions() == 0)
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
            if (m_session_list->numberOfSessions() == 0)
            {
                std::cout << "Shutting Down!" << std::endl;
                m_is_global_shutdown = true;
                break;
            }

            // Process Data Queue for Each Active Session.
            m_session_list->update();
            SDL_Delay(10);
        }

    }

    /**
     * @brief Spawns Local Session For Dialing Directory Interface
     */
    void spawnLocalSession()
    {
        //Mock up connection which will not be used.
        // connection_ptr new_connection(new tcp_connection(m_io_service));
        //connection_ptr new_connection;
        //session_ptr new_session = Session::create(m_io_service, new_connection, m_session_list, m_program_path);
        session_ptr new_session = Session::create(nullptr, m_session_list, m_program_path);

        // Join the Broadcaster to keep instance properly active!
        m_session_list->join(new_session);

        // Call Startup outside of constrctor for shared_from_this() handle.
        new_session->startup();

        // Start the Dialing Directory on the local instance
        new_session->start_menu_instance();                
    }

    /**
     * @brief Spawn Connection Sessions
     *        Connection Sessions handle their own IO
     */
    void spawnConnectionSession()
    {

        // Connection Mock up, add passing for this later on!
        std::string hostname;
        std::string port;

        tcp::resolver resolver(m_io_service);
        tcp::resolver::query query(hostname, port);

        // Use all end pointer for IPV4 & IPV6 support
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        // Connection handles TCP_Socket
        connection_ptr new_connection(new tcp_connection(m_io_service));

        // Start Async Connection
        boost::asio::async_connect(new_connection->socket(),
                                   endpoint_iterator,
                                   [this, new_connection]
                                   (boost::system::error_code ec, tcp::resolver::iterator)
        {
            if(!ec)
            {
                std::cout << "Connected" << std::endl;

                // Were good, spawn the session
                //session_ptr new_session = Session::create(m_io_service, new_connection, m_session_list, m_program_path);
                session_ptr new_session = Session::create(new_connection, m_session_list, m_program_path);

                m_session_list->join(new_session);
                // Start Async Read/Writes for Session Socket Data.
                // Required only for Sessions that make connections, Local sessions do not use this.
                new_session->wait_for_data();                

                //  Or just pass in msession_list to the session!  so it can pop itself off.
            }
            else
            {
                std::cout << "Unable to Connect" << std::endl;
            }
        });

    }

    /**
     * @brief Process Input Events per session
     * @param event
     */
    void process_event(SDL_Event &event)
    {
        int num_sesisons = m_session_list->numberOfSessions();
        if(num_sesisons > 0 && !m_is_global_shutdown)
        {
            std::set<session_ptr>::iterator itEnd = end(m_session_list->m_sessions);
            for(auto it = begin(m_session_list->m_sessions); it != itEnd; ++it)
            {
                // If number of sessions changed, (Window was Closed) restart loop.
                if (m_session_list->numberOfSessions() == num_sesisons)
                {
                    // If the window id matches the current event, then
                    // send through the event to the specific session for processing.
                    if((*it)->m_window_manager->getWindowId() == event.window.windowID)
                    {
                        // Pass the Events to the specific Session for Specific Window and Input Events.
                        (*it)->m_input_handler->update(event);
                    }
                }
                else
                {
                    // Restart the Loop so we don't miss any events on Window Closes.
                    if (m_session_list->numberOfSessions() > 0)
                    {
                        it = begin(m_session_list->m_sessions);
                        if((*it)->m_window_manager->getWindowId() == event.window.windowID)
                        {
                            // Pass the Events to the specific Session for Specific Window and Input Events.
                            (*it)->m_input_handler->update(event);
                        }
                    }
                    else
                    {
                        // No more sessions exist, exit
                        std::cout << "No more sesisons exist, Complete Shutdown!" << std::endl;
                        return;
                    }
                }
            }
        }
    }

    // Client ASIO Service Specific
    boost::asio::io_service        m_io_service;
    boost::asio::io_service::work  m_work;
    std::string                    m_program_path;

    // Spawned Session List
    broad_caster_ptr               m_session_list;

    std::thread                    m_thread;
    bool                           m_is_global_shutdown;

};

#endif // INTERFACE_HPP
