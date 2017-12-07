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
        // std::cout << "IO_SERVICE RUNNING" << std::endl;
        // std::cout << "Hello waiter" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        service_work_ptr job_work = m_queue.dequeue();

        // Test running job for reading..
        std::error_code ec (0, std::generic_category());

        std::cout << "Poll: " << job_work->getSocket()->poll() << std::endl;


        std::cout << "tcp_connection async_read " << std::endl;

        std::string str = "Test Data for you!";
        std::copy(str.begin(), str.end(), std::back_inserter(
                      job_work->getBuffer()
                  ));
        function_handler result(job_work->getCallback());
        result(ec);

    }

}

/**
 * @brief Main looping method
 */
void IOService::stop()
{

    m_queue.clear();

}
