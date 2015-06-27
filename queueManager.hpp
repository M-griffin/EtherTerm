#ifndef QUEUEMANAGER_HPP
#define QUEUEMANAGER_HPP

#include "messageQueue.hpp"
#include "safeQueue.hpp"
#include <iostream>

class QueueManager
{
public:
    //  Instantiate the Singleton
    static QueueManager* Instance()
    {
        if(!m_globalInstance)
        {
            m_globalInstance = new QueueManager();
            return m_globalInstance;
        }
        return m_globalInstance;
    }

    // Release And Clear the Singleton
    static void ReleaseInstance()
    {
        if(m_globalInstance)
        {
            delete m_globalInstance;
            m_globalInstance = nullptr;
        }
        return;
    }

    SafeQueue<MessageQueue> m_queue;

private:

    static QueueManager* m_globalInstance;

    QueueManager();
    ~QueueManager();
    QueueManager(const QueueManager&);
    QueueManager& operator=(const QueueManager&);

};

//Setup the Class Type
typedef QueueManager TheQueueManager;

#endif // QUEUEMANAGER_HPP
