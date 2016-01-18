#ifndef SESSION_MANAGER_HPP
#define SESSION_MANAGER_HPP

#include "safe_queue.hpp"

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/weak_ptr.hpp>

#include <iostream>
#include <string>
#include <set>

// Forward Declarations
class Session;
typedef boost::shared_ptr<Session> session_ptr;

class SystemConnection;
typedef boost::shared_ptr<SystemConnection> system_connection_ptr;

/**
 * @class SessionManager
 * @author Michael Griffin
 * @date 15/08/2015
 * @file session_manager.hpp
 * @brief Keep's Sessions Active, good for adding and removing.
 */
class SessionManager
{
public:
    SessionManager();
    ~SessionManager();

    /**
     * @brief Notifies that a user has joined the room
     * @param participant
     */
    void join(session_ptr session);

    /**
     * @brief Notifies that a user has left the room
     * @param participant
     */
    void leave(session_ptr session);

    /**
     * @brief Sends message to all users in the current room.
     * @param participant
     */
    // void deliver(std::string msg);

    /**
     * @brief Retrieve Number of users connected
     * Also helpful for determinging next node number.
     * @return
     */
    int numberOfSessions();

    /**
     * @brief Update, Hits Session Updates for processing Data Queue.
     * @return
     */
    void update();

    /**
     * @brief Broacaster Anchors the Sessions, shutdown all session.
     * @return
     */
    void shutdown();

    /**
     * @brief Check if there are any new connections to Spawn
     * @return
     */
    bool hasNewConnections();

    // Queue of new connections to spawn.
    SafeQueue<system_connection_ptr> m_new_connections;

    // Handle to Sessions
    std::set<session_ptr> m_sessions;
};

typedef boost::shared_ptr<SessionManager>  session_manager_ptr;
typedef boost::weak_ptr<SessionManager>	   session_manager_wptr;

#endif // BROAD_CASTER_HPP
