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

            // Handle Read Service if Data is Available.
            if (job_work->getServiceType() == SERVICE_TYPE_READ)
            {
                std::error_code ec (0, std::generic_category());
                std::cout << "Poll: " << job_work->getSocket()->poll() << std::endl;
                // If Data Available, read, then ppulate buffer, and remove
                // Otherwise keep polling till data is available.
                int result = job_work->getSocket()->poll();
                if (result > 0)
                {
                    std::cout << "tcp_connection async_read " << std::endl;

                    
                    // TEST DATA, read socket and setBuffer()
                    std::string str = "Test Data for you!";
                    std::copy(str.begin(), str.end(), std::back_inserter(
                                job_work->getBuffer()
                            ));
                                       
                    
                    // Setup and execute the Callback.
                    function_handler result(job_work->getCallback());
                    result(ec);
                }
                else if (result == -1) 
                {
                    // error
                }
            }            
        }

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
