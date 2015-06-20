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

    TermStateMachine() {}
    ~TermStateMachine() { std::cout << "TermStateMachine Released" << std::endl; }
    void update();
    //void pushState(TermState* theState);
    void changeState(TermState* theState);
    //void popState();
    void clean();
    std::vector<TermState*>& getTermStates() { return TermStates; }

private:
    std::vector<TermState*> TermStates;
};

#endif
