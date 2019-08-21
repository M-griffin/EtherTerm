#ifndef SESSION_MANAGER_HPP
#define SESSION_MANAGER_HPP

#include "safe_queue.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <set>

// Forward Declarations
class Session;
typedef std::shared_ptr<Session> session_ptr;

class SystemEntry;
typedef std::shared_ptr<SystemEntry> system_entry_ptr;

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
     * @brief Window Session is Killed we want to switch focus to the first available window.
     */
    void grabNewWindowFocus();

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

    // Detect anytime a system is removed, is so, we need to reset focus
    // To the first available window from the main thread!
    bool m_is_system_disconnected;

    // Queue of new connections to spawn.
    SafeQueue<system_entry_ptr> m_new_connections;

    // Handle to Sessions
    std::set<session_ptr> m_sessions;
};

typedef std::shared_ptr<SessionManager>  session_manager_ptr;
typedef std::weak_ptr<SessionManager>    session_manager_wptr;

#endif // SESSION_MANAGER_HPP
