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

                std::cout << "* SERVICE_TYPE_READ" << std::endl;

                std::error_code ec (0, std::generic_category());
                // If Data Available, read, then ppulate buffer, and remove
                // Otherwise keep polling till data is available.
                int result = job_work->getSocket()->poll();
                std::cout << "Poll: " << result << std::endl;
                if (result > 0)
                {
                    std::cout << "io_service data available" << std::endl;

                    memset(&msg_buffer, 0, sizeof(msg_buffer));
                    int length = job_work->getSocket()->recvSocket(msg_buffer);
                    if(length < 0)
                    {
                        // Error - Lost Connection
                        std::cout << "tcp_connection async_read - lost connection!: " << length << std::endl;

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
                    std::cout << "SDL Poll Error!" << std::endl;
                    std::cout << "Remove Job (i) before: " << m_service_list.size() << std::endl;
                    m_service_list.remove(i);
                    std::cout << "Remove Job (i) after: " << m_service_list.size() << std::endl;
                }
            }

            /**
             * Handle Write Service if Data is Available.
             */
            else if (job_work->getServiceType() == SERVICE_TYPE_WRITE)
            {

                std::cout << "* SERVICE_TYPE_WRITE" << std::endl;

                std::error_code ec (0, std::generic_category());
                // If Data Available, read, then ppulate buffer, and remove
                // Otherwise keep polling till data is available.


                //std::cout << "data: " << job_work->getBuffer() << std::endl;
                std::basic_string<unsigned char> str(job_work->getBuffer().data());

                int result = job_work->getSocket()->sendSocket((unsigned char *)str.c_str(), str.size());


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

            // Temp timer, change to 10/20 miliseconds for cpu useage
            // possiably as lock wait() check for jobs added to vector queue.
            std::this_thread::sleep_for(std::chrono::milliseconds(40));

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
