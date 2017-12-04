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

    /**
     * @brief handles pending sequences waiting for reply responses
     * @param option
     * @return
     */
    bool checkReply(unsigned char option)
    {
        return checkSequence(reply_sequence, option);
    }
    void deleteReply(unsigned char option)
    {
        deleteSequence(reply_sequence, option);
    }
    void addReply(unsigned char option)
    {
        addSequence(reply_sequence, option);
    }

    /**
     * @brief handles active sequences negotiated.
     * @param option
     * @return
     */
    bool checkActive(unsigned char option)
    {
        return checkSequence(active_sequence, option);
    }
    void deleteActive(unsigned char option)
    {
        deleteSequence(active_sequence, option);
    }
    void addActive(unsigned char option)
    {
        addSequence(active_sequence, option);
    }

    /**
     * @class FindFirst
     * @brief Comparatior for options in vectors.
     */
    struct FindFirst
    {
        FindFirst(unsigned char i) : to_find(i) { }
        unsigned char to_find;
        bool operator()
        (const unsigned char &p)
        {
            return p == to_find;
        }
    };


    /**
     * @brief Templates for Handling Sequence Vector Operations
     * @param t
     * @param option
     * @return
     */
    template <typename T>
    bool checkSequence(T &t, unsigned char option)
    {
        typename T::iterator it =
            find_if(t.begin(), t.end(), FindFirst(option));

        if(it == t.end())
        {
            //std::cout << option << " not found" << std::endl;
            return false;
        }
        else
        {
            //std::cout << option << " found" << std::endl;
        }
        return true;
    }

    template <typename T>
    void addSequence(T &t, unsigned char option)
    {
        typename T::iterator it =
            find_if(t.begin(), t.end(), FindFirst(option));

        // Sequence Not Found, add new sequence.
        if(it == t.end())
            t.push_back(option);
    }

    template <typename T>
    void deleteSequence(T &t, unsigned char option)
    {
        typename T::iterator it =
            find_if(t.begin(), t.end(), FindFirst(option));

        typename T::iterator it_back = t.end();

        if(t.size() == 0)
        {
            return;
        }
        else if(t.size() > 1)
        {
            // If Sequence Found, Swap to Back and Remove Back.
            if(it != t.end())
            {
                iter_swap(it, it_back);
                t.pop_back();
            }
            else
            {
                // Clear all.
                std::vector<unsigned char>().swap(t);
            }
        }
    }

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

    // Holds Sequences Waiting for Reply and Active.
    std::vector<unsigned char> reply_sequence;
    std::vector<unsigned char> active_sequence;

};

typedef boost::shared_ptr<TelnetManager> telnet_manager_ptr;

#endif
