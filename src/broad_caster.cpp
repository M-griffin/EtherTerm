#include "broad_caster.hpp"
#include "session.hpp"

#include <algorithm>
#include <boost/bind.hpp>

BroadCaster::~BroadCaster()
{
    std::cout << "~BroadCaster" << std::endl;
}

/**
 * @brief Notifies that a user has joined the room
 * @param participant
 */
void BroadCaster::join(session_ptr session)
{
    std::cout << "joined BroadCaster" << std::endl;
    m_sessions.insert(session);
}


/**
 * @brief Notifies that a user has left the room
 * @param participant
 */
void BroadCaster::leave(session_ptr session)
{
    std::cout << "left BroadCaster" << std::endl;

    // Clear Session
    m_sessions.erase(session);
}

/**
 * @brief Sends message to all users in the current room.
 * @param participant
 */
void BroadCaster::deliver(std::string msg)
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
int BroadCaster::numberOfSessions()
{
    int count = 0;
    for(auto it = begin(m_sessions); it != end(m_sessions); ++it)
    {
        ++count;
    }
    return count;
}

/**
 * @brief Broacaster Anchors the Sessions, we loop to shutdown all connections.
 * @return
 */
void BroadCaster::shutdown()
{
    for(auto it = begin(m_sessions); it != end(m_sessions); ++it)
    {
        /*
        if ((*it)->m_connection)
        {
            (*it)->m_connection->m_normal_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
            (*it)->m_connection->m_normal_socket.close();
        }
        m_sessions.erase(it);
        */
    }
}
