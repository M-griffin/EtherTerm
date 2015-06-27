#ifndef SEQUENCEMANAGER_HPP
#define SEQUENCEMANAGER_HPP

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "safeQueue.hpp"
#include "sequenceDecoder.hpp"
#include <iostream>

class SequenceManager :
    private SequenceDecoder
{
public:

    //  Instantiate the Singleton
    static SequenceManager* Instance()
    {
        if(!m_globalInstance)
        {
            m_globalInstance = new SequenceManager();
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

    // Parse Queue and Waiting Messages
    void update();

    // Decode Input Received.
    void decode(std::string input);

private:

    static SequenceManager* m_globalInstance;

    SequenceManager();
    ~SequenceManager();
    SequenceManager(const SequenceManager&);
    SequenceManager& operator=(const SequenceManager&);
};

//Setup the Class Type
typedef SequenceManager TheSequenceManager;

#endif // SEQUENCEMANAGER_HPP
