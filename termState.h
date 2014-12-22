// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#ifndef __TERM_STATE__
#define __TERM_STATE__

#include <string>
#include <vector>

class TermState
{
public:
    
    virtual ~TermState() {}
    
    virtual void update() = 0;
    virtual void render() = 0;
    
    virtual bool onEnter() = 0;
    virtual bool onExit() = 0;
    
    virtual void resume() {}
    
    virtual std::string getStateID() const = 0;
    
protected:
    
    TermState() : isLoadingComplete(false), isActive(false)
    {
        
    }
    
    bool isLoadingComplete;
    bool isActive;
    
};

#endif
