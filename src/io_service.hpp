#ifndef IO_SERVICE_HPP
#define IO_SERVICE_HPP

#include "safe_vector.hpp"

#include <vector>
#include <memory>

class SocketHandler;
typedef std::shared_ptr<SocketHandler> socket_handler_ptr;

    const int SERVICE_TYPE_NONE  = 0;
    const int SERVICE_TYPE_READ  = 1;
    const int SERVICE_TYPE_WRITE = 2;

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

    class service_base
    {
    public:
        virtual void setBuffer(char *buffer) = 0;
        virtual std::vector<unsigned char> &getBuffer() = 0;        
        virtual socket_handler_ptr getSocket() = 0;
        virtual function_handler getCallback() = 0;
        virtual int getServiceType() = 0;
    };
    typedef std::shared_ptr<service_base> service_base_ptr;

    template <class ConstBufferSequence, class  SocketHandle, class Callback, class ServiceType>
    class service_job : public service_base
    {
    public:
    
        // Virtual Methods to pickup Data from Base Classes
        virtual void setBuffer(char *buffer) 
        {
            std::vector<unsigned char> conversion(buffer, buffer+sizeof(buffer));
            m_buffer.swap(conversion);
        }        
        virtual std::vector<unsigned char> &getBuffer()
        {
            return m_buffer;
        }
        virtual socket_handler_ptr getSocket()
        {
            return m_socket_handle;
        }
        virtual function_handler getCallback()
        {
            return m_callback;
        }
        virtual int getServiceType()
        {
            return static_cast<int>(m_service_type);
        }

        service_job(ConstBufferSequence &buffer, SocketHandle socket_handle, Callback &callback, 
                ServiceType service_type)
            : m_buffer(buffer)
            , m_socket_handle(socket_handle)
            , m_callback(callback)
            , m_service_type(service_type)
        {
            std::cout << " * addAsyncRead Job Created!" << std::endl;
        }

        ConstBufferSequence &m_buffer;
        SocketHandle         m_socket_handle;
        Callback            &m_callback;
        ServiceType          m_service_type;
    };


    template <typename ConstBufferSequence, typename SocketHandle, typename Callback, typename ServiceType>
    void addAsyncRead(ConstBufferSequence &buffer, SocketHandle socket_handle, Callback &callback, ServiceType serviceType)
    {
        std::cout << " * addAsyncRead" << std::endl;        
        service_job<ConstBufferSequence, SocketHandle, Callback, ServiceType> *job
            = new service_job <ConstBufferSequence, SocketHandle, Callback, ServiceType>
        (buffer, socket_handle, callback, serviceType);

        m_service_list.push_back(std::shared_ptr<service_base>(job));
    }

    void addAsyncWrite();

    void run();
    void stop();

    SafeVector<service_base_ptr>  m_service_list;

    bool m_is_active;

};

#endif // IO_SERVICE_HPP
