#ifndef __MENU_STATE__
#define __MENU_STATE__

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "termState.hpp"
#include "socketState.hpp"

class MenuState :
    public TermState
{
public:

    virtual ~MenuState() {}

protected:

/*
    typedef void(*Callback)();
    virtual void setCallbacks(const std::vector<Callback>& callbacks) = 0;

    std::vector<Callback> callbacks;
*/
};


#endif
