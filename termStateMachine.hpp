#ifndef __TERM_StateMachine__
#define __TERM_StateMachine__

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include <iostream>
#include <vector>
#include "termState.hpp"

class TermStateMachine
{
public:

//  Instantiate the Singleton
    static TermStateMachine* Instance()
    {
        if(!m_globalInstance)
        {
            m_globalInstance = new TermStateMachine();
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

    void update();
    //void pushState(TermState* theState);
    void changeState(TermState* theState);
    //void popState();
    void clean();
    std::vector<TermState*>& getTermStates() { return TermStates; }

private:

    static TermStateMachine* m_globalInstance;

    TermStateMachine();
    ~TermStateMachine();

    TermStateMachine(const TermStateMachine&);
    TermStateMachine& operator=(const TermStateMachine&);

    std::vector<TermState*> TermStates;
};

//Setup the Class Type
typedef TermStateMachine TheStateMachine;

#endif
