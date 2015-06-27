#ifndef __TELNET_STATE_H_
#define __TELNET_STATE_H_

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#ifdef TARGET_OS_MAC
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#elif _WIN32
#include <SDL.h>
#include <SDL_net.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#endif

#include <vector>
#include "termState.hpp"
#include "socketState.hpp"

/*
 * Telnet Class handles Tel-opt Negotiation
 * Parsing Socket Data and Sending Through to ANSI Parser.
 */

class TelnetState :
    public TermState
{
public:

    TelnetState() :
        m_teloptStage(0),
        m_teloptCommand(0),
        m_isSGACompleted(false),
        m_isBINCompleted(false),
        m_isECHOCompleted(true), // Default to True for MUD'S etc.
        m_finishedSGA(false),
        m_finishedTTYPE(false),
        m_finishedNAWS(false),
        m_finishedBIN(false),
        m_finishedECHO(false),
        m_isShutdown(false),
        m_currentOption(0)
    { }
    virtual ~TelnetState()
    {
        std::cout << "Shutting Down ~TelnetState" << std::endl;
    }
    virtual void update();
    virtual bool onEnter();
    virtual bool onExit();
    virtual std::string getStateID() const
    {
        return telnetID;
    }

private:

    void handleSession();

    // Global Option State for Telnet Options Parsing.
    int m_teloptStage;
    int m_teloptCommand;

    // NOTE FIXME
    // Should probably set this as started
    bool m_isSGACompleted,
         m_isBINCompleted,
         m_isECHOCompleted;

    // Then set these as Completed!
    bool m_finishedSGA,
         m_finishedTTYPE,
         m_finishedNAWS,
         m_finishedBIN,
         m_finishedECHO;

    bool m_isShutdown;

    unsigned
    char m_currentOption;

    // Telnet Protocol Functions.
    unsigned
    char telnetOptionAcknowledge(unsigned char m_teloptCommand);
    unsigned
    char telnetOptionDeny(unsigned char m_teloptCommand);

    void telnetOptionNawsReply();
    void telnetOptionTerminalTypeReply();
    void telnetSendIAC(unsigned char command, unsigned char option);
    unsigned
    char telnetOptionParse(unsigned char c);

    static const std::string telnetID;
    std::string inputSequence;

    // Virtual Classes.
    /*
    virtual void setCallbacks(const std::vector<Callback>& callbacks);
    */
};

#endif
