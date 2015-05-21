#ifndef __SSH_STATE_H_
#define __SSH_STATE_H_

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#ifdef TARGET_OS_MAC
#include <SDL2/SDL_net.h>
#elif _WIN32
#include <SDL_net.h>
#else
#include <SDL2/SDL_net.h>
#endif

// Temp
#define NUM_LINES 25

#include <vector>
#include "menuState.h"
#include "termObject.h"
#include "socketState.h"

/*
 * Telnet Class handles Tel-opt Negotiation
 * Parsing Socket Data and Sending Through to ANSI Parser.
 */
class SSHState : public MenuState
{
public:

    virtual ~SSHState()
        { std::cout << "Shutting Down SSHState" << std::endl; }
    virtual void update();
    virtual bool onEnter();
    virtual bool onExit();

    virtual std::string getStateID() const
    {
        return sshID;
    }

private:

    void handleSession();

    // Virtual Classes.
    static const std::string sshID;
    virtual void setCallbacks(const std::vector<Callback>& callbacks);
    std::string inputSequence;
    bool shutdown;

    std::vector<TermObject*> termObjects;
};

#endif
