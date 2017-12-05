
#include "protocol.hpp"

#include "tcp_connection.hpp"
#include "message_queue.hpp"
#include "session.hpp"

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#endif

#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <thread>


/*
 * Setup Assets Path
 */
std::string Protocol::getProgramPath()
{
    std::string path = m_program_path;
#ifdef _WIN32
    path += "assets\\";
#else
    path += "assets/";
#endif
    return path;
}


/*
 * Setup Downloads Path
 */
std::string Protocol::getDownloadPath()
{
    std::string path = m_program_path;
#ifdef _WIN32
    path += "downloads\\";
#else
    path += "downloads/";
#endif
    return path;
}


/**
 * @brief Helper to append protocol path
 */
std::string Protocol::getProtocolPath()
{
    // Create Default Phone Book.
    std::string path = getProgramPath();
#ifdef _WIN32
    path.append("protocol\\");
#else
    path.append("protocol/");
#endif
    return path;
}


/**
 * @brief Check for Windows NT Compitable System
 * @return
 */
bool Protocol::isWinNT()
{
#ifdef _WIN32
    OSVERSIONINFO osv;
    osv.dwOSVersionInfoSize = sizeof(osv);
    GetVersionEx(&osv);
    return (osv.dwPlatformId == VER_PLATFORM_WIN32_NT);
#else
    return false;
#endif
}


/**
 * @brief Start Call for passing socket to external protocols.
 */
void Protocol::executeProtocols()
{
    connection_ptr conn = m_weak_connection.lock();
    if(!conn)
    {
        std::cout << "Error, no handle to connection!" << std::endl;
        return;
    }

#ifdef _WIN32
    WSAPROTOCOL_INFO pri;
    
// TODO REWORK    
//    WSADuplicateSocket(conn->socket().native(), GetCurrentProcessId(), &pri);
    m_socket_duplicate = WSASocket(pri.iAddressFamily, pri.iSocketType, pri.iProtocol, &pri, 0, 0);
#else
    m_socket_duplicate = dup(conn->socket().native());
#endif

    if (m_socket_duplicate < 1)
    {
        std::cout << "Error, unable to duplicate socket handle." << std::endl;
        return;
    }

    // Execute Thread for File Transfer
    std::thread t([=] { executeProcess(m_socket_duplicate); });
    t.detach();
}


#ifdef _WIN32
/**
 * @brief Call an Extenral process (Win32 Only)
 * @param socket_descriptor
 */
void Protocol::executeProcess(int socket_descriptor)
{
    std::cout << " Execute External Process " << std::endl;

    STARTUPINFO          si;
    SECURITY_ATTRIBUTES  sa;
    SECURITY_DESCRIPTOR  sd; //security information for pipes
    PROCESS_INFORMATION  pi;

    HANDLE newstdin, newstdout, read_stdout, write_stdin;  //pipe handles

    if(isWinNT())  //initialize security descriptor (Windows NT)
    {
        InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
        SetSecurityDescriptorDacl(&sd, true, NULL, false);
        sa.lpSecurityDescriptor = &sd;
    }
    else
        sa.lpSecurityDescriptor = NULL;

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = true;   //allow inheritable handles


    if(!CreatePipe(&newstdin, &write_stdin, &sa, 0))    //create stdin pipe
    {
        std::cout << "CreatePipe stdin failed" << std::endl;
        return;
    }

    if(!CreatePipe(&read_stdout, &newstdout, &sa, 0))   //create stdout pipe
    {
        std::cout << "CreatePipe stdout failed" << std::endl;
        CloseHandle(newstdin);
        CloseHandle(write_stdin);
        return;
    }


    GetStartupInfo(&si);      //set startupinfo for the spawned process

    //The dwFlags member tells CreateProcess how to make the process.
    //STARTF_USESTDHANDLES validates the hStd* members. STARTF_USESHOWWINDOW
    //validates the wShowWindow member.
    //
    //si.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
    si.dwFlags = STARTF_USESHOWWINDOW; // This shows or doesn't the console output.
    si.wShowWindow = SW_SHOW;
    si.hStdOutput = newstdout;
    si.hStdError = newstdout;
    si.hStdInput = newstdin;

    //char app_spawn[] = "C:\\Windows\\System32\\cmd.exe"; // Program to execute on command line.
    std::string path = "cmd /c \"";
    //std::string path = "";
    path.append(getProtocolPath());
    path.append("sexyz.exe ");
    //path.append("sexyz.exe ");
    path.append(std::to_string(socket_descriptor));
    path.append(" rz ");
    path.append(getDownloadPath());
    path.append("\" & pause");

    // Kickoff external process.
    std::cout << "External Process Path: " << path << std::endl;

    //spawn the child process
    if(!CreateProcess(NULL, (char *)path.c_str(),
                      NULL, NULL, true, CREATE_NEW_CONSOLE,
                      NULL, (char *)getProtocolPath().c_str(),
                      &si, &pi))
    {
        std::cout << "CreateProcess child failed" << std::endl;

        //ErrorMessage((char *)"CreateProcess child process");
        CloseHandle(newstdin);
        CloseHandle(newstdout);
        CloseHandle(read_stdout);
        CloseHandle(write_stdin);
        return;
    }

    unsigned long exit   = 0;  // process exit code
//    unsigned long bread  = 0;  // bytes read
//    unsigned long avail  = 0;  // bytes available

//    const int RCVBUFSIZE = 4096;
    //char echoBuffer[RCVBUFSIZE];
//    unsigned char buf[RCVBUFSIZE];
//    unsigned char tmpbuf[RCVBUFSIZE];

    // While process is active!!
    while(1)
    {
        GetExitCodeProcess(pi.hProcess, &exit);      //while the process is running
        if(exit != STILL_ACTIVE)
        {
            std::cout << "Exiting Protocol" << std::endl;

            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);

            session_ptr session = m_weak_session.lock();
            if (session)
            {
                // Reset the socket and read waiting data refresh.
// To Impliment
//              session->m_is_transfer = false;
                session->waitForSocketData();
            }
            break;
        }

        /*
         * Pull STDIO through pipes is not working.
         *
        // Handle Naped Pipes for STDIO Output of protocols.
        bool b = PeekNamedPipe(read_stdout, buf, RCVBUFSIZE, &bread, &avail, NULL);
        //printf( "-peek res=%d avail=%d bread=%d\n", b, avail, bread );

        if (bread > 0)
        {
            memset(&buf, 0, sizeof(buf));
            memset(&tmpbuf, 0, sizeof(tmpbuf));

            // Read From Piped Program to std_out, pass to std_out and Socket
            ReadFile(read_stdout, tmpbuf, RCVBUFSIZE, &bread, NULL);  //read the stdout pipe
            //printf( "-read res=%d avail=%d bread=%d\n", b, avail, bread );


            if (strlen((const char *)tmpbuf) > 0)
            {
                std::cout << tmpbuf << std::flush;

                session_ptr session = m_weak_session.lock();
                if (session)
                {
                    // Add Message Queue to display STDIO from protocol.
                    MessageQueue msgQueue;
                    msgQueue.m_text = (char *)tmpbuf;

                    session->m_data_queue.enqueue(msgQueue);
                }
            }

            memset(&buf, 0, sizeof(buf));
            memset(&tmpbuf, 0, sizeof(tmpbuf));
        }*/

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    CloseHandle(newstdin);
    CloseHandle(newstdout);
    CloseHandle(read_stdout);
    CloseHandle(write_stdin);

    std::cout << "File Transfer Completed!" << std::endl;
}
#else
/**
 * @brief Call an Extenral process (Posix Only)
 * @param socket_descriptor
 */
void Protocol::executeProcess(int socket_descriptor)
{

}
#endif
