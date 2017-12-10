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
                // If Data Available, read, then ppulate buffer, and remove
                // Otherwise keep polling till data is available.
                int result = job_work->getSocket()->poll();
                if (result > 0)
                {
                    memset(&msg_buffer, 0, MAX_BUFFER_SIZE);
                    int length = job_work->getSocket()->recvSocket(msg_buffer);
                    if(length < 0)
                    {
                        // Error - Lost Connection
                        std::cout << "tcp_connection async_read - lost connection!: " << length << std::endl;
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
                    std::cout << "SDL Poll Error!" << std::endl;
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
                                 (unsigned char*)job_work->getWriteSequence().c_str(),
                                 job_work->getWriteSequence().size());

                if (result <= 0)
                {
                    // Error - Lost Connection
                    std::cout << "tcp_connection async_write - lost connection!" << std::endl;
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
        }

        // Temp timer, change to 10/20 miliseconds for cpu useage
        // possiably as lock wait() check for jobs added to vector queue.
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
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