#include "session_manager.hpp"
#include "session.hpp"

#include <algorithm>
#include <boost/bind.hpp>


SessionManager::SessionManager()
{
    std::cout << "BroadCaster Created" << std::endl;
}

SessionManager::~SessionManager()
{
    std::cout << "~BroadCaster" << std::endl;

    // Clear Any new Waiting connections on shutdown.
    m_new_connections.clear();
}

/**
 * @brief Notifies that a user has joined the room
 * @param participant
 */
void SessionManager::join(session_ptr session)
{
    std::cout << "joined session BroadCaster" << std::endl;
    m_sessions.insert(session);
}

/**
 * @brief Notifies that a user has left the room
 * @param participant
 */
void SessionManager::leave(session_ptr session)
{
    std::cout << "left session BroadCaster" << std::endl;

    // Clear Session
    m_sessions.erase(session);
}

/**
 * @brief Sends message to all users in the current room.
 * @param participant
 */
void SessionManager::deliver(std::string msg)
{
    if(msg.size() == 0)
        return;

    std::cout << "deliver BroadCaster notices: " << msg << std::endl;
    std::for_each(m_sessions.begin(), m_sessions.end(),
                  boost::bind(&Session::deliver, _1, boost::ref(msg)));
}

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
    for(auto it = m_sessions.begin(); it != m_sessions.end(); ++it)
    {
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
