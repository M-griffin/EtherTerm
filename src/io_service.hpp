#ifndef IO_SERVICE_HPP
#define IO_SERVICE_HPP

#include "safe_vector.hpp"

#include <functional>
#include <sstream>
#include <vector>
#include <memory>

class SocketHandler;
typedef std::shared_ptr<SocketHandler> socket_handler_ptr;

class Session;
typedef std::shared_ptr<Session> session_ptr;

const int SERVICE_TYPE_NONE           = 0;
const int SERVICE_TYPE_READ           = 1;
const int SERVICE_TYPE_WRITE          = 2;
const int SERVICE_TYPE_CONNECT_TELNET = 3;
const int SERVICE_TYPE_CONNECT_SSH    = 4;
const int SERVICE_TYPE_CONNECT_IRC    = 5;

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

    /**
     * Handles Call Back Functions, execute at the end of Asyn Job.
     */
    typedef std::function<void(const std::error_code& error)> callback_function_handler;
    static const int MAX_BUFFER_SIZE = 8193;

    /**
     * @class ServiceBase
     * @author Michael Griffin
     * @date 06/12/2017
     * @file io_service.hpp
     * @brief IO Service Job Base Template, needed to access virtual methods on ServiceJob
     */
    class ServiceBase
    {
    public:
        virtual void setBuffer(unsigned char *buffer) = 0;
        virtual std::vector<unsigned char> &getBuffer() = 0;
        virtual std::string getStringSequence() = 0;
        virtual socket_handler_ptr getSocket() = 0;
        virtual callback_function_handler getCallback() = 0;
        virtual int getServiceType() = 0;
    };
    typedef std::shared_ptr<ServiceBase> service_base_ptr;

    /**
     * @class ServiceJob
     * @author Michael Griffin
     * @date 06/12/2017
     * @file io_service.hpp
     * @brief IO Service Job Template
     */
    template <class MutableBufferSequence, class StringSequence, class SocketHandle, class Callback, class ServiceType>
    class ServiceJob : public ServiceBase
    {
    public:
        virtual void setBuffer(unsigned char *buffer)
        {
            for(int i = 0; i < MAX_BUFFER_SIZE; i++)
            {
                m_buffer.push_back(buffer[i]);
            }
        }
        virtual std::vector<unsigned char> &getBuffer()
        {
            return m_buffer;
        }
        virtual std::string getStringSequence()
        {
            return m_string_sequence;
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

        ServiceJob(MutableBufferSequence &buffer, StringSequence string_sequence, SocketHandle socket_handle, Callback callback,
                   ServiceType service_type)
            : m_buffer(buffer)
            , m_string_sequence(string_sequence)
            , m_socket_handle(socket_handle)
            , m_callback(callback)
            , m_service_type(service_type)
        { }

        MutableBufferSequence &m_buffer;
        StringSequence         m_string_sequence;
        SocketHandle           m_socket_handle;
        Callback               m_callback;
        ServiceType            m_service_type;
    };

    /**
     * @brief Add Async Jobs to the Vector Queue
     * @param buffer
     * @param write_sequence
     * @param socket_handle
     * @param callback
     * @param service_type
     */
    template <typename MutableBufferSequence, typename StringSequence, typename SocketHandle, typename Callback, typename ServiceType>
    void addAsyncJob(MutableBufferSequence &buffer, StringSequence write_sequence, SocketHandle socket_handle,
                     Callback &callback, ServiceType service_type)
    {
        ServiceJob<MutableBufferSequence, StringSequence, SocketHandle, Callback, ServiceType> *job
            = new ServiceJob <MutableBufferSequence, StringSequence, SocketHandle, Callback, ServiceType>
        (buffer, write_sequence, socket_handle, callback, service_type);

        m_service_list.push_back(std::shared_ptr<ServiceBase>(job));
    }

    void run();
    void stop();

    /**
     * @brief String Split for Hostname : Port on Async Connects
     * @param s
     * @param delimiter
     * @return
     */
    std::vector<std::string> split(const std::string& s, char delimiter)
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter))
        {
            tokens.push_back(token);
        }
        return tokens;
    }

    SafeVector<service_base_ptr>  m_service_list;
    bool                          m_is_active;

};

#endif // IO_SERVICE_HPP
