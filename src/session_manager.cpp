#include "session_manager.hpp"
#include "session.hpp"

#include <algorithm>
//#include <std/bind.hpp>


SessionManager::SessionManager()
    : m_is_system_disconnected(false)
{
    std::cout << "SessionManager Created" << std::endl;
}

SessionManager::~SessionManager()
{
    std::cout << "~SessionManager" << std::endl;
    // Clear Any new Waiting connections on shutdown.
    m_new_connections.clear();
}

/**
 * @brief Notifies that a user has joined the room
 * @param participant
 */
void SessionManager::join(session_ptr session)
{
    std::cout << "Joined SessionManager" << std::endl;
    m_sessions.insert(session);
}

/**
 * @brief Notifies that a user has left the room
 * @param participant
 */
void SessionManager::leave(session_ptr session)
{
    std::cout << "Left SessionManager" << std::endl;

    try
    {
        // If session is connected, we need to disconnect it first
        if(session->m_connection)
        {
            // Shutdown the Connection before closing
            session->m_connection->shutdown();
            // Force no longer being connected now.
            session->m_is_connected = false;
            session->m_connection->close();
        }

        // Remove the Session
        m_sessions.erase(session);

        // Mark that a system has disconnect to reset window focus
        m_is_system_disconnected = true;
    }
    catch (std::exception ex)
    {
        std::cout << "SessionManager::leave() - Caught Exception on shutdown: " << ex.what();
    }
}

/**
 * @brief Window Session is Killed we want to switch focus to the first available window.
 */
void SessionManager::grabNewWindowFocus()
{
    // If a system hasn't diconnected then return;
    if (!m_is_system_disconnected)
    {
        return;
    }

    // Only reset focus if there are no active windows!
    bool found_active_window = false;
    for (auto it = m_sessions.begin(); it != m_sessions.end(); ++it)
    {
        if ((*it)->m_window_manager->isActiveWindow())
        {
            found_active_window = true;
        }
    }

    // A Window closed, and there is no active window.
    // Reset to first session as long as it exists.
    if (!found_active_window && m_sessions.begin() != m_sessions.end())
    {
        std::cout << "No Active Window, resetting Focus to first available window" << std::endl;
        auto session = m_sessions.begin();
        (*session)->m_window_manager->grabWindow();
    }
  
    // Reset for next system detected.
    m_is_system_disconnected = false;
}

/**
 * @brief Sends message to all users in the current room.
 * @param participant
 *
void SessionManager::deliver(std::string msg)
{
    if(msg.size() == 0)
        return;

    // This isn't used at this time!
    std::cout << "Deliver Global SessionManager notices: " << msg << std::endl;
    std::for_each(m_sessions.begin(), m_sessions.end(),
                  std::bind(&Session::deliver, _1, std::ref(msg)));
}*/


/**
 * @brief Retrieve Number of users connected
 * Also helpful for determinging next node number.
 * @return
 */
int SessionManager::numberOfSessions()
{
    return m_sessions.size();
}

/**
 * @brief Update, Hits Session Updates for processing Data Queue.
 * @return
 */
void SessionManager::update()
{
    // Becasue disconnects can happen in a thread, we need to double check
    // If a session was poped off in the middle of the Iteration
    std::string::size_type num_sessions = m_sessions.size();
    for(auto it = m_sessions.begin(); it != m_sessions.end(); ++it)
    {
        // Make sure handle is valid once we call it.
        if (num_sessions != m_sessions.size())
        {
            return;
        }

        // Run Session Update for Session and Event Processing.
        (*it)->update();        
    }
}

/**
 * @brief Broacaster Anchors the Sessions, shutdown all session.
 * @return
 */
void SessionManager::shutdown()
{
    // Swap to pop all enteries off the stack.
    std::set<session_ptr>().swap(m_sessions);
}
