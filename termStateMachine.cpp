// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "queueManager.hpp"
#include "termStateMachine.hpp"
#include <iostream>

// Init the Global Instance
TermStateMachine* TermStateMachine::m_globalInstance = nullptr;

TermStateMachine::TermStateMachine()
{ }

TermStateMachine::~TermStateMachine()
{
    std::cout << "TermStateMachine Released." << std::endl;
    if(!TermStates.empty())
    {
        TermStates.back()->onExit();
        delete TermStates.back();
        TermStates.clear();
    }
}


void TermStateMachine::clean()
{
    if(!TermStates.empty())
    {
        TermStates.back()->onExit();
        delete TermStates.back();
        TermStates.clear();
    }
}

void TermStateMachine::update()
{
    if(!TermStates.empty())
    {
        TermStates.back()->update();
    }
}

/*
 * Not Used at This Time
 *
void TermStateMachine::pushState(TermState *theState)
{
    TermStates.push_back(theState);
    TermStates.back()->onEnter();
}

void TermStateMachine::popState()
{
    if(!TermStates.empty())
    {
        TermStates.back()->onExit();
        TermStates.pop_back();
    }
    TermStates.back()->resume();
}
*/

void TermStateMachine::changeState(TermState *theState)
{
    if(!TermStates.empty())
    {
        if(TermStates.back()->getStateID() == theState->getStateID())
        {
            return; // do nothing
        }
        TermStates.back()->onExit();

        std::cout << "Deleteing Current StateMachine!: " << TermStates.size() << std::endl;
        // This is fucking everything up!!
        delete TermStates.back();  // Memory not freed!  needs rewrite!
        TermStates.pop_back();

        // Clear the Memory!
        std::cout << "Clearing Memory of StateMachine!: " << TermStates.size() << std::endl;
        std::vector<TermState*>().swap(TermStates);
    }

    // Refresh the Queue
    TheQueueManager::ReleaseInstance();

    // initialize it
    theState->onEnter();
        
    // push back our new state
    TermStates.push_back(theState);
}