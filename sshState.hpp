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
#include "menuState.hpp"
//#include "termObject.h"
#include "socketState.hpp"

/*
 * Telnet Class handles Tel-opt Negotiation
 * Parsing Socket Data and Sending Through to ANSI Parser.
 */
class SSHState :
    public MenuState
{
public:

    SSHState() :
        b_isShutdown(false)
    { }
    virtual ~SSHState()
    {
        std::cout << "Shutting Down SSHState" << std::endl;
    }
    virtual void update();
    virtual bool onEnter();
    virtual bool onExit();

    virtual std::string getStateID() const
    {
        return sshID;
    }

private:

    void handleSession();

    bool b_isShutdown;
    static const std::string sshID;
    std::string m_inputSequence;


    /*
    virtual void setCallbacks(const std::vector<Callback>& callbacks);
    */

};

#endif
