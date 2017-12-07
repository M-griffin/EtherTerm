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
                std::cout << "Poll: " << result << std::endl;
                if (result > 0)
                {
                    std::cout << "io_service data available" << std::endl;

                    char msg_buffer[8193] = {'\0'};
                    int length = job_work->getSocket()->recvSocket(msg_buffer);
                    if(length < 0)
                    {
                        // Error - Lost Connection
                        std::cout << "tcp_connection async_read - lost connection!" << std::endl;

                        job_work->getSocket()->setInactive();
                        std::cout << "Remove Job (i) before: " << m_service_list.size() << std::endl;
                        m_service_list.remove(i);
                        std::cout << "Remove Job (i) after: " << m_service_list.size() << std::endl;
                    }
                    else
                    {
                        std::cout << "tcp_connection async_read " << std::endl;

                        // Success
                        // Setup and execute the Callback.
                        job_work->setBuffer(msg_buffer);

                        /*
                        // TEST DATA, for CallBack
                        std::string str = "Test Data for you!";
                        std::copy(str.begin(), str.end(), std::back_inserter(
                                job_work->getBuffer()
                            ));
                        */

                        std::cout << "callback_function_handler setup: " << msg_buffer << std::endl;
                        callback_function_handler run_callback(job_work->getCallback());
                        run_callback(ec);

                        std::cout << "Remove Job (i) before: " << m_service_list.size() << std::endl;
                        m_service_list.remove(i);
                        std::cout << "Remove Job (i) after: " << m_service_list.size() << std::endl;
                    }
                }
                else if (result == -1)
                {
                    // error
                }
            }
            
            /**
             * Handle Write Service if Data is Available.
             */
            else if (job_work->getServiceType() == SERVICE_TYPE_WRITE)
            {
                std::error_code ec (0, std::generic_category());
                // If Data Available, read, then ppulate buffer, and remove
                // Otherwise keep polling till data is available.

                int result = job_work->getSocket()->sendSocket(
                                 reinterpret_cast<unsigned char*>(job_work->getBuffer().data()),
                                 (Uint32)job_work->getBuffer().size());

                if (result <= 0)
                {
                    std::cout << "io_service data sent" << std::endl;

                    // Error - Lost Connection
                    std::cout << "tcp_connection async_write - lost connection!" << std::endl;

                    job_work->getSocket()->setInactive();
                    std::cout << "Remove Job (i) before: " << m_service_list.size() << std::endl;
                    m_service_list.remove(i);
                    std::cout << "Remove Job (i) after: " << m_service_list.size() << std::endl;
                }
                else
                {
                    std::cout << "tcp_connection async_write " << std::endl;

                    std::cout << "callback_function_handler setup: " << std::endl;
                    callback_function_handler run_callback(job_work->getCallback());
                    run_callback(ec);

                    std::cout << "Remove Job (i) before: " << m_service_list.size() << std::endl;
                    m_service_list.remove(i);
                    std::cout << "Remove Job (i) after: " << m_service_list.size() << std::endl;
                }
            }

        }

        // Temp timer, change to 10/20 miliseconds for cpu useage
        // possiably as lock wait() check for jobs added to vector queue.
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

}

/**
 * @brief Main looping method
 */
void IOService::stop()
{
    m_service_list.clear();
}
