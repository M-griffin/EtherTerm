#include "io_service.hpp"
#include "socket_handler.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <system_error>

IOService::IOService()
    : m_is_active(false)
{
}

IOService::~IOService()
{
    std::cout << "~IOService" << std::endl;
}


/**
 * @brief Main looping method
 */
void IOService::run()
{

    char msg_buffer[MAX_BUFFER_SIZE];

    m_is_active = true;
    while(m_is_active)
    {
        // This will wait for another job to be inserted on next call
        // Do we want to insert the job back, if poll is empty or
        // move to vector then look polls..  i think #2.
        for(unsigned int i = 0; i < m_service_list.size(); i++)
        {
            service_base_ptr job_work = m_service_list.get(i);

            /**
             * Handle Read Service if Data is Available.
             */
            if (job_work->getServiceType() == SERVICE_TYPE_READ)
            {
                // If Data Available, read, then populate buffer
                // Otherwise keep polling till data is available.
                int result = job_work->getSocket()->poll();
                if (result > 0)
                {
                    memset(&msg_buffer, 0, MAX_BUFFER_SIZE);
                    int length = job_work->getSocket()->recvSocket(msg_buffer);
                    if(length < 0)
                    {
                        // Error - Lost Connection
                        std::cout << "async_read - lost connection!: " << length << std::endl;
                        job_work->getSocket()->setInactive();
                        callback_function_handler run_callback(job_work->getCallback());
                        std::error_code lost_connect_error_code (1, std::system_category());
                        run_callback(lost_connect_error_code);
                        m_service_list.remove(i);
                    }
                    else
                    {
                        job_work->setBuffer((unsigned char *)msg_buffer);
                        callback_function_handler run_callback(job_work->getCallback());
                        std::error_code success_code (0, std::generic_category());
                        run_callback(success_code);
                        m_service_list.remove(i);
                    }
                }
                else if (result == -1)
                {
                    std::cout << "async_poll - lost connection" << std::endl;
                    callback_function_handler run_callback(job_work->getCallback());
                    std::error_code lost_connect_error_code (1, std::system_category());
                    run_callback(lost_connect_error_code);
                    m_service_list.remove(i);
                }
            }

            /**
             * Handle Write Service if Data is Available.
             */
            else if (job_work->getServiceType() == SERVICE_TYPE_WRITE)
            {
                // std::cout << "* SERVICE_TYPE_WRITE" << std::endl;
                int result = job_work->getSocket()->sendSocket(
                                 (unsigned char*)job_work->getStringSequence().c_str(),
                                 job_work->getStringSequence().size());

                if (result <= 0)
                {
                    // Error - Lost Connection
                    std::cout << "async_write - lost connection!" << std::endl;
                    job_work->getSocket()->setInactive();
                    callback_function_handler run_callback(job_work->getCallback());
                    std::error_code lost_connect_error_code (1, std::system_category());
                    run_callback(lost_connect_error_code);
                    m_service_list.remove(i);
                }
                else
                {
                    callback_function_handler run_callback(job_work->getCallback());
                    std::error_code success_code (0, std::generic_category());
                    run_callback(success_code);
                    m_service_list.remove(i);
                }
            }

            else if (job_work->getServiceType() == SERVICE_TYPE_CONNECT_TELNET)
            {
                // Get host and port from string.
                // This would better as a vector<std::string> for the sequqnce,
                // More versitile.!
                std::vector<std::string> ip_address = split(job_work->getStringSequence(), ':');
                bool is_success = false;
                if (ip_address.size() > 1)
                {
                    is_success = job_work->getSocket()->connectTelnetSocket(
                                     ip_address.at(0),
                                     std::atoi(ip_address.at(1).c_str())
                                 );
                }
                else
                {
                    is_success = job_work->getSocket()->connectTelnetSocket(
                                     ip_address.at(0),
                                     23
                                 );
                }

                if (is_success)
                {
                    callback_function_handler run_callback(job_work->getCallback());
                    std::error_code success_code (0, std::generic_category());
                    run_callback(success_code);
                    m_service_list.remove(i);
                }
                else
                {
                    // Error - Unable to connect
                    std::cout << "async_connection - unable to connect" << std::endl;
                    job_work->getSocket()->setInactive();
                    callback_function_handler run_callback(job_work->getCallback());
                    std::error_code not_connected_error_code (1, std::system_category());
                    run_callback(not_connected_error_code);
                    m_service_list.remove(i);
                }
            }

            else if (job_work->getServiceType() == SERVICE_TYPE_CONNECT_SSH)
            {
                // Get host and port from string.
                // This would better as a vector<std::string> for the sequqnce,
                // More versitile.!
                std::vector<std::string> ip_address = split(job_work->getStringSequence(), ':');
                std::cout << "ip_address: " << ip_address.size();
                bool is_success = false;
                if (ip_address.size() >= 4)
                {
                    
                    std::cout << "1. " << ip_address.at(0) << std::endl;
                    std::cout << "2. " << ip_address.at(1) << std::endl;
                    std::cout << "3. " << ip_address.at(2) << std::endl;
                    std::cout << "4. " << ip_address.at(3) << std::endl;
                    
                    is_success = job_work->getSocket()->connectSshSocket(
                                     ip_address.at(0),
                                     std::atoi(ip_address.at(1).c_str()),
                                     ip_address.at(2),
                                     ip_address.at(3)
                                 );
                }

                if (is_success)
                {
                    callback_function_handler run_callback(job_work->getCallback());
                    std::error_code success_code (0, std::generic_category());
                    run_callback(success_code);
                    m_service_list.remove(i);
                }
                else
                {
                    // Error - Unable to connect
                    std::cout << "async_connection - unable to connect" << std::endl;
                    job_work->getSocket()->setInactive();
                    callback_function_handler run_callback(job_work->getCallback());
                    std::error_code not_connected_error_code (1, std::system_category());
                    run_callback(not_connected_error_code);
                    m_service_list.remove(i);
                }
            }
            
            // SERVICE_TYPE_CONNECT_IRC
            else if (job_work->getServiceType() == SERVICE_TYPE_CONNECT_IRC)
            {
                // Get host and port from string.
                // This would better as a vector<std::string> for the sequqnce,
                // More versitile.!
                std::vector<std::string> ip_address = split(job_work->getStringSequence(), ':');
                bool is_success = false;
                if (ip_address.size() > 1)
                {
                    is_success = job_work->getSocket()->connectIrcSocket(
                                     ip_address.at(0),
                                     std::atoi(ip_address.at(1).c_str())
                                 );
                }
                else
                {
                    is_success = job_work->getSocket()->connectIrcSocket(
                                     ip_address.at(0),
                                     6667
                                 );
                }

                if (is_success)
                {                    
                    // Send Initial Connection Information
                    std::string nick = "mercyful1";
                    std::string ident = "mercyful1";
                    std::string read_name = "michael";
                    std::string host = "localhost";
                    
                    std::stringstream ss;
                    ss  << "NICK " << nick << "\r\n" 
                        << "USER " << ident << " " << host << " bla : " << read_name << "\r\n";
                    
                    std::string output = ss.str();
                    job_work->getSocket()->sendSocket((unsigned char *)output.c_str(), output.size());
                                        
                    callback_function_handler run_callback(job_work->getCallback());
                    std::error_code success_code (0, std::generic_category());
                    run_callback(success_code);
                    m_service_list.remove(i);
                }
                else
                {
                    // Error - Unable to connect
                    std::cout << "async_connection - unable to connect" << std::endl;
                    job_work->getSocket()->setInactive();
                    callback_function_handler run_callback(job_work->getCallback());
                    std::error_code not_connected_error_code (1, std::system_category());
                    run_callback(not_connected_error_code);
                    m_service_list.remove(i);
                }
            }
            
        }

        // Temp timer, change to 10/20 miliseconds for cpu useage
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

}

/**
 * @brief Main looping method
 */
void IOService::stop()
{
    m_is_active = false;
    m_service_list.clear();
}
