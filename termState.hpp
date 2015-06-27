// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#ifndef __TERM_STATE__
#define __TERM_STATE__

#include <iostream>
#include <string>
#include <vector>

class TermState
{
public:

    virtual ~TermState() { std::cout << "~TermState." << std::endl; }
    virtual void update() = 0;
    virtual bool onEnter() = 0;
    virtual bool onExit() = 0;
    virtual void resume() {}
    virtual std::string getStateID() const = 0;

protected:

    TermState() : isLoadingComplete(false), isActive(false)
    { }
    bool isLoadingComplete;
    bool isActive;

};

#endif
