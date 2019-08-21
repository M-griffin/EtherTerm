#ifndef IRC_MANAGER_HPP
#define IRC_MANAGER_HPP

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <vector>

// Forward Declaration
class Session;
typedef std::shared_ptr<Session> session_ptr;
typedef std::weak_ptr<Session> session_weak_ptr;

class SessionIO;
typedef std::shared_ptr<SessionIO> session_io_ptr;

/**
 * @class IrcManager
 * @author Michael Griffin
 * @date 26/12/2017
 * @file irc_manager.hpp
 * @brief IRC Manager for Client Service Commands
 */
class IrcManager
{
public:
    explicit IrcManager(session_ptr session);
    ~IrcManager();

    // Handle To session for sending responses back to server.
    session_weak_ptr m_weak_session;
    session_io_ptr   m_session_io;

    typedef std::vector<std::string> vec_string;
    
    typedef struct ControlMessage {
        std::string m_control;
        std::string m_message;
    } ControlMessage;

    typedef std::unique_ptr<ControlMessage> ctrl_message_ptr;

    /**
     * @class IrcChannel
     * @author Michael Griffin
     * @date 31/12/2017
     * @file irc_manager.hpp
     * @brief Internal IRC Channel Class
     */
    class IrcChannel
    {
    public:

        IrcChannel(std::string channel_name, std::string channel_topic, vec_string user_names)
            : m_channel_name(channel_name)
            , m_channel_topic(channel_topic)
            , m_user_names(user_names)
            , m_channel_text()
            , m_user_history_text()
            , m_is_active(true)
        { }
        ~IrcChannel()
        {
            vec_string().swap(m_user_names);
            vec_string().swap(m_channel_text);
            vec_string().swap(m_user_history_text);
        }


        std::string         m_channel_name;
        std::string         m_channel_topic;
        vec_string          m_user_names;
        vec_string          m_channel_text;
        vec_string          m_user_history_text;
        bool                m_is_active;
    };
    
    typedef std::shared_ptr<IrcChannel> channel_ptr;

    std::vector<channel_ptr> m_channels;
    std::string              m_incoming_buffer;

    /**
     * @brief General Output for Text Box
     * @param message
     */
    void prepareOut(std::string &message);

    /**
     * @brief Handles Notices
     * @param message
     */
    void onNotice(std::string &message);


    std::string getTimeStamp();
    void handleLineRead(std::string &data);    
    void parseControlMessage(ctrl_message_ptr &ctrl_message);
    
    /**
     * @brief Irc Manager Setup
     */
    void startUp();

    std::string setScrollRegion();
    std::string drawStatusBar();
    std::string moveToUserPrompt();
    std::string drawUserPrompt();
    
    void clearUserPrompt();        
    void handleUserInput(const std::string &input);

};


typedef std::shared_ptr<IrcManager> irc_manager_ptr;

#endif // IRC_MANAGER_HPP
