#ifndef IO_SERVICE_HPP
#define IO_SERVICE_HPP

#include "safe_queue.hpp"

#include <vector>
#include <memory>

class SocketHandler;
typedef std::shared_ptr<SocketHandler> socket_handler_ptr;

/**
 * @class IOService
 * @author Michael Griffin
 * @date 06/12/2017
 * @file io_service.hpp
 * @brief Handle Asyn looping and funcation callback on Socket Data
 */
class IOService
{
public:
    IOService();
    ~IOService();

    typedef std::function<void(const std::error_code& error)> function_handler;

    class service_work 
    {
    public:
        virtual std::vector<unsigned char> &getBuffer() = 0;
        virtual socket_handler_ptr getSocket() = 0;
        virtual function_handler getCallback() = 0;
    };
    typedef std::shared_ptr<service_work> service_work_ptr;

    template <class ConstBufferSequence, class  SocketHandle, class  Callback>
    class service_job : public service_work
    {                
    public:
        virtual std::vector<unsigned char> &getBuffer() { return m_buffer; }
        virtual socket_handler_ptr getSocket() { return m_socket_handle; }
        virtual function_handler getCallback() { return m_callback; }

        service_job(ConstBufferSequence &buffer, SocketHandle socket_handle, Callback &callback)
            : m_buffer(buffer)
            , m_socket_handle(socket_handle)
            , m_callback(callback)
        {
            std::cout << " * addAsyncRead Job Created!" << std::endl;
        }
        
        ConstBufferSequence &m_buffer;
        SocketHandle         m_socket_handle;
        Callback            &m_callback;
    };


    template <typename ConstBufferSequence, typename SocketHandle, typename Callback>
    void addAsyncRead(ConstBufferSequence &buffer, SocketHandle socket_handle, Callback &callback)
    {
        std::cout << " * addAsyncRead" << std::endl;

        service_job<ConstBufferSequence, SocketHandle, Callback> *job
            = new service_job <ConstBufferSequence, SocketHandle, Callback>
        (buffer, socket_handle, callback);

        m_queue.enqueue(std::shared_ptr<service_work>(job));
    }

    void addAsyncWrite();

    void run();
    void stop();

    SafeQueue<service_work_ptr>   m_queue;
    std::vector<service_work_ptr> m_work;

    bool m_is_active;

};

#endif // IO_SERVICE_HPP
