#include "io_service.hpp"
#include "socket_handler.hpp"

#include <iostream>
#include <thread>
#include <chrono>

IOService::IOService()
    : m_is_active(false)
{
}

IOService::~IOService()
{
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
                std::error_code ec (0, std::generic_category());
                // If Data Available, read, then ppulate buffer, and remove
                // Otherwise keep polling till data is available.
                int result = job_work->getSocket()->poll();
                if (result > 0)
                {
                    memset(&msg_buffer, 0, sizeof(msg_buffer));
                    int length = job_work->getSocket()->recvSocket(msg_buffer);
                    if(length < 0)
                    {
                        // Error - Lost Connection
                        std::cout << "tcp_connection async_read - lost connection!: " << length << std::endl;
                        job_work->getSocket()->setInactive();
                        m_service_list.remove(i);
                    }
                    else
                    {
                        job_work->setBuffer((unsigned char *)msg_buffer);
                        callback_function_handler run_callback(job_work->getCallback());
                        run_callback(ec);
                        m_service_list.remove(i);
                    }
                }
                else if (result == -1)
                {
                    std::cout << "SDL Poll Error!" << std::endl;
                    m_service_list.remove(i);
                }
            }

            /**
             * Handle Write Service if Data is Available.
             */
            else if (job_work->getServiceType() == SERVICE_TYPE_WRITE)
            {
               // std::cout << "* SERVICE_TYPE_WRITE" << std::endl;
                std::error_code ec (0, std::generic_category());
                int result = job_work->getSocket()->sendSocket(
                                 (unsigned char*)job_work->getWriteSequence().c_str(),
                                 job_work->getWriteSequence().size());

                if (result <= 0)
                {
                    // Error - Lost Connection
                    std::cout << "tcp_connection async_write - lost connection!" << std::endl;
                    job_work->getSocket()->setInactive();
                    m_service_list.remove(i);
                }
                else
                {
                    callback_function_handler run_callback(job_work->getCallback());
                    run_callback(ec);
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
    m_service_list.clear();
}
