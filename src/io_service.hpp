#ifndef IO_SERVICE_HPP
#define IO_SERVICE_HPP

#include "safe_vector.hpp"

//#include <functional>
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
 * @brief Handle Async Socket Read/Write and function Callback on results.
 */
class IOService
{

public:
    IOService();
    ~IOService();

    typedef std::function<void(const std::error_code& error)> callback_function_handler;
    static const int MAX_BUFFER_SIZE = 8193;

    class service_base
    {
    public:
        virtual void setBuffer(unsigned char *buffer) = 0;
        virtual std::vector<unsigned char> &getBuffer() = 0;  
        virtual std::string getWriteSequence() = 0;
        virtual socket_handler_ptr getSocket() = 0;
        virtual callback_function_handler getCallback() = 0;
        virtual int getServiceType() = 0;
    };
    typedef std::shared_ptr<service_base> service_base_ptr;

    template <class ConstBufferSequence, class StringSequence, class SocketHandle, class Callback, class ServiceType>
    class service_job : public service_base
    {
    public:
    
        // Virtual Methods to pickup Data from Base Classes
        virtual void setBuffer(unsigned char *buffer) 
        {
            for(int i = 0; i < MAX_BUFFER_SIZE; i++)
            {
                
                //std::string buff = static_cast<char *>(buffer);
                //std::copy(buff.begin(), buff.end(), std::back_inserter(m_buffer));
                m_buffer.push_back(buffer[i]);
            }
        }        
        virtual std::vector<unsigned char> &getBuffer()
        {
            return m_buffer;
        }
        virtual std::string getWriteSequence()
        {
            return m_write_sequence;
        }
        virtual socket_handler_ptr getSocket()
        {
            return m_socket_handle;
        }
        virtual callback_function_handler getCallback()
        {
            return m_callback;
        }        
        virtual int getServiceType()
        {
            return m_service_type;
        }

        service_job(ConstBufferSequence &buffer, StringSequence write_sequence, SocketHandle socket_handle, Callback callback, 
                ServiceType service_type)
            : m_buffer(buffer)
            , m_write_sequence(write_sequence)
            , m_socket_handle(socket_handle)
            , m_callback(callback)
            , m_service_type(service_type)
        {
            //std::cout << " * addAsync Job Created!" << std::endl;
        }

        ConstBufferSequence &m_buffer;
        StringSequence       m_write_sequence;
        SocketHandle         m_socket_handle;
        Callback             m_callback;
        ServiceType          m_service_type;
    };

    template <typename ConstBufferSequence, typename StringSequence, typename SocketHandle, typename Callback, typename ServiceType>
    void addAsyncRead(ConstBufferSequence &buffer, StringSequence write_sequence, SocketHandle socket_handle, Callback &callback, ServiceType service_type)
    {
        //std::cout << " * addAsyncRead Job" << std::endl;        
        service_job<ConstBufferSequence, StringSequence, SocketHandle, Callback, ServiceType> *job
            = new service_job <ConstBufferSequence, StringSequence, SocketHandle, Callback, ServiceType>
        (buffer, write_sequence, socket_handle, callback, service_type);

        m_service_list.push_back(std::shared_ptr<service_base>(job));
    }

    template <typename ConstBufferSequence, typename StringSequence, typename SocketHandle, typename Callback, typename ServiceType>
    void addAsyncWrite(ConstBufferSequence &buffer, StringSequence write_sequence, SocketHandle socket_handle, Callback &callback, ServiceType service_type)
    {
        //std::cout << " * addAsyncWrite Job" << std::endl;        
        service_job<ConstBufferSequence, StringSequence, SocketHandle, Callback, ServiceType> *job
            = new service_job <ConstBufferSequence, StringSequence, SocketHandle, Callback, ServiceType>
        (buffer, write_sequence, socket_handle, callback, service_type);

        m_service_list.push_back(std::shared_ptr<service_base>(job));
    }

    void run();
    void stop();

    SafeVector<service_base_ptr>  m_service_list;

    bool m_is_active;

};

#endif // IO_SERVICE_HPP
