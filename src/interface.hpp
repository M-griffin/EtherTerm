#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "session.hpp"

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>

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
    Interface()
        : m_work(m_io_service)
    {
        // Startup worker thread of ASIO. We want socket communications in a seperate thread.
        // We only spawn a single thread for IO_Service on startup.
        m_thread = create_thread();
    }

    ~Interface()
    {
    }

    /**
     * @brief Shutdown the socket and commication thread.
     */
    void shutdown()
    {
        m_io_service.stop();
        m_thread.join();
    }

    /**
     * @brief Starts up the Sessions
     */
    void startup()
    {
        // Startup is called once, we need to setup the menu session
        // Here to handle Graphics and i/o processing

        // Then determine the best way to swap connection sessions.

        /***** WE NEED A WAY TO DETERMINE SESSION TYPE AND LINK THE SESSIONS!*/


    }


    /**
     * @brief Spawns Local Sessions Menu System etc..
     *        Return the Session Pointer to link it to the class using it.
     */
    void spawnLocalSession()
    {
        //Mock up connection which will not be used.
        connection_ptr new_connection(new tcp_connection(m_io_service));

        // Spawn the session
        session_ptr new_session = Session::create(m_io_service, new_connection, m_session_list);

        // Start Async Read/Writes for Session Socket Data.
        // Required only for Sessions that make connections, Local sessions do not use this.
        new_session->wait_for_data();

        //  Or just poass in msession_list to the session!  so it can pop itself off.
        m_session_list.insert(new_session);
    }

    /**
     * @brief Spawn Connection Sessions
     *        Connection Sessions handle their own IO
     */
    void spawnConnectionSession()
    {

        // Connection Mock up, add passing for this lateron!
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
                session_ptr new_session = Session::create(m_io_service, new_connection, m_session_list);

                // Start Async Read/Writes for Session Socket Data.
                // Required only for Sessions that make connections, Local sessions do not use this.
                new_session->wait_for_data();

                //  Or just poass in msession_list to the session!  so it can pop itself off.
                m_session_list.insert(new_session);
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
        if(m_session_list.size() > 0)
        {
            std::set<session_ptr>::iterator it;
            for(it = m_session_list.begin(); it != m_session_list.end(); ++it)
            {
                // If the window id matches the current event, then
                // send through the event to the specific session for processing.
                if((*it)->m_window_manager->getWindowId() == event.window.windowID)
                {

                    // Now pass the events for LocalInput/Mouse and Windows to the Session.

                }
            }
        }
    }

    // Spawned Session List
    std::set<session_ptr>          m_session_list;

    // MenuSystem here,  pass the m_session_List, and io_service
    // To the menu system so it can spawn new connections that link back here.

    // Client ASIO Service Specific
    boost::asio::io_service        m_io_service;
    boost::asio::io_service::work  m_work;
    std::thread                    m_thread;

};

#endif // INTERFACE_HPP
