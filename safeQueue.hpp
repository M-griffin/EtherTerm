#ifndef SAFEQUEUE_HPP
#define SAFEQUEUE_HPP

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>

/*
 * A simple Thread Safe Queue Template
 * This needs to be tested more with mutex's and threads!
 */
template <class T>
class SafeQueue
{
public:
    explicit SafeQueue(void) :
        q(),
        m(),
        c()
    {}

    // Copy Constructor (Overridden with MOVE)
    SafeQueue& operator=(SafeQueue& other)
    {
        if (this != &other)
        {
            std::lock_guard<std::mutex> lock(m);
            //std::cout << "Copy construct" << std::endl;
            if (!&other.isEmpty())
            {
                q = other.q;
                m = other.m;
                c = other.c;
            }
        }
        return *this;
    }

    // Move Constructor.
    SafeQueue& operator=(SafeQueue&& other)
    {
        if (this != &other)
        {
            std::lock_guard<std::mutex> lock(m);
            //std::cout << "Move construct" << std::endl;
            if (!other.isEmpty())
            {
                q.swap(other.q);
            }
        }
        return *this;
    }

    ~SafeQueue(void)
    {
        // Probably not needed!
        std::queue<T>().swap(q);
    }

    // Add Item to Queue.
    //void enqueue(const T& t)
    void enqueue(T t)
    {
        std::lock_guard<std::mutex> lock(m);
        q.push(t);
        c.notify_one();
    }

    // Get first Item in Queue. FIFO
    T dequeue(void)
    {
        std::unique_lock<std::mutex> lock(m);
        while(q.empty())
        {
            c.wait(lock);
        }
        T val = q.front();
        q.pop();
        return val;
    }

    // Clear out the Entire Queue.
    void clear(void)
    {
        std::lock_guard<std::mutex> lock(m);
        std::queue<T>().swap(q);
    }

    // Check for Non-Blocking Wait.
    bool isEmpty(void) const
    {
        return q.empty();
    }

    // Check for Non-Blocking Wait.
    unsigned long size(void) const
    {
        return q.size();
    }

private:
    std::queue<T> q;
    mutable std::mutex m;
    std::condition_variable c;
};


#endif // SAFEQUEUE_HPP
