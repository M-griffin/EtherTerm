#ifndef MESSAGEQUEUE_HPP
#define MESSAGEQUEUE_HPP

#include <iostream>
#include <vector>
#include <string>

/**
 * @class MessageQueue
 * @author Michael Griffin
 * @date 12/20/2015
 * @file message_queue.hpp
 * @brief Storage for Text/ESC sequences.
 */
class MessageQueue
{
public:
    MessageQueue() {}
    ~MessageQueue()
    {
        std::vector<int>().swap(m_queueParams);
        m_text.erase();
    }

    std::vector<int> m_queueParams;
    std::string m_text;

    void clear()
    {
        std::vector<int>().swap(m_queueParams);
        m_text.erase();
    }
};

#endif // MESSAGEQUEUE_HPP
