// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "termStateMachine.h"
#include <iostream>

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

void TermStateMachine::render()
{
    if(!TermStates.empty())
    {
        TermStates.back()->render();
    }
}

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

void TermStateMachine::changeState(TermState *theState)
{
    if(!TermStates.empty())
    {
        if(TermStates.back()->getStateID() == theState->getStateID())
        {
            return; // do nothing
        }

        TermStates.back()->onExit();
        TermStates.pop_back();
    }

    // initialise it
    theState->onEnter();

    // push back our new state
    TermStates.push_back(theState);
}