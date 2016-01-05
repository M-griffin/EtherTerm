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
    BroadCaster()
    {
        std::cout << "Global BroadCaster Created." << std::endl;
    }
    ~BroadCaster();

    void join(session_ptr session);
    void leave(session_ptr session);
    void deliver(std::string msg);

    int  numberOfSessions();
    void shutdown();

private:

    std::set<session_ptr> m_sessions;
};

typedef boost::shared_ptr<BroadCaster>	broad_caster_ptr;
typedef boost::weak_ptr<BroadCaster>	broad_caster_wptr;

#endif // BROAD_CASTER_HPP
