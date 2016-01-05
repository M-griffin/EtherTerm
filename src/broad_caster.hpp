#ifndef BROAD_CASTER_HPP
#define BROAD_CASTER_HPP

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/weak_ptr.hpp>

#include <iostream>
#include <string>
#include <set>

class Session;
typedef boost::shared_ptr<Session> session_ptr;


/**
 * @class BroadCaster
 * @author Michael Griffin
 * @date 15/08/2015
 * @file boad_caster.hpp
 * @brief Keep's Sessions Active, good for adding and removing.
 */
class BroadCaster
{
public:
    BroadCaster();
    ~BroadCaster();

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
    void deliver(std::string msg);

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

    // Handle to Sessions
    std::set<session_ptr> m_sessions;
};

typedef boost::shared_ptr<BroadCaster>	broad_caster_ptr;
typedef boost::weak_ptr<BroadCaster>	broad_caster_wptr;

#endif // BROAD_CASTER_HPP
