#ifndef __TERM_StateMachine__
#define __TERM_StateMachine__

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include <vector>
#include "termState.h"

class TermStateMachine
{
public:

    TermStateMachine() {}
    ~TermStateMachine() {}
    void update();
    void render();
    void pushState(TermState* theState);
    void changeState(TermState* theState);
    void popState();
    void clean();
    std::vector<TermState*>& getTermStates() { return TermStates; }

private:
    std::vector<TermState*> TermStates;
};

#endif
