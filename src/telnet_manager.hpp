#ifndef __TELNET_MANAGER_H_
#define __TELNET_MANAGER_H_

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/weak_ptr.hpp>
#include <vector>

// Forward Declaration
class Session;
typedef boost::shared_ptr<Session> session_ptr;
typedef boost::weak_ptr<Session> session_weak_ptr;

/**
 * @class TelnetManager
 * @author Michael Griffin
 * @date 1/18/2016
 * @file telnet_manager.hpp
 * @brief Handles Parsing Telnet Options and Responses
 */
class TelnetManager
{
public:

    TelnetManager(session_ptr session);
    ~TelnetManager();

    // Handle To session for sending responses back to server.
    session_weak_ptr m_weak_session;

    /**
     * @brief Handles Telnet Options and Replies, Returns text data
     * @param buffer
     * @return
     */
    std::string parseIncomingData(const std::vector<unsigned char> &msg_buffer);

private:

    // Global Option State for Telnet Options Parsing.
    int m_teloptStage;
    int m_teloptCommand;

    // NOTE FIXME
    // Should probably set this as started
    bool m_isSGACompleted,
         m_isBINCompleted,
         m_isECHOCompleted;

    // Then set these as Completed!
    bool m_is_active_sga,
         m_is_active_ttype,
         m_is_active_naws,
         m_is_active_bin,
         m_is_active_echo;

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
};

typedef boost::shared_ptr<TelnetManager> telnet_manager_ptr;

#endif

