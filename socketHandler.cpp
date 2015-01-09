
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "socketHandler.h"
#include "socketState.h"
#include "terminal.h"
#include <iostream>
#include <exception>

SocketHandler* SocketHandler::globalInstance = 0;

SocketHandler::SocketHandler() : active(false)
{ }

SocketHandler::~SocketHandler()
{ std::cout << "SocketHandler Released" << std::endl; }


int SocketHandler::send(unsigned char *buf, Uint32 len)
{
    return socket->sendSocket(buf,len);
}

int SocketHandler::recv(char *message)
{
    return socket->recvSocket(message);
}

int SocketHandler::update()
{
    //std::cout << "SocketHandler::update() active: " << active << std::endl;
    int ret;
    if(active)
    {
        ret = socket->pollSocket();
        if(ret == -1)
        {
            // Shutdown Socket.
            socket->onExit();
            active = false;
        }
        else if (ret == 0) // No Data!
        {
            // When no data is received, this is when we want to show the cursor!
            // Setup cursor in current x/y position Cursor.
            TheTerminal::Instance()->setupCursorChar();

            startBlinking = true;
            // Setup Timer for Blinking Cursor
            // Initial State = On, then Switch to off in next loop.
            if(cursorBlink % 2 == 0)
            {
                ttime2 = SDL_GetTicks();
                if(startBlinking && (ttime2 - ttime) > 400)
                {
                    TheTerminal::Instance()->renderCursorOffScreen();
                    TheTerminal::Instance()->drawTextureScreen();
                    --cursorBlink;
                    ttime = SDL_GetTicks();
                }
            }
            else
            {
                ttime2 = SDL_GetTicks();
                if(startBlinking && (ttime2 - ttime) > 400)
                {
                    TheTerminal::Instance()->renderCursorOnScreen();
                    TheTerminal::Instance()->drawTextureScreen();
                    ++cursorBlink;
                    ttime = SDL_GetTicks();
                }
            }
        }
        else
        {
            // We got data, turn off the cursor!
            ttime = SDL_GetTicks();
            startBlinking = false;
            cursorBlink = 0;
            TheTerminal::Instance()->renderCursorOffScreen();
            TheTerminal::Instance()->drawTextureScreen();
        }
    }
    else
    {
        // Inactive Connection
        ret = -1;        
    }
    return ret;
}

bool SocketHandler::initTelnet(std::string host, int port)
{
    std::cout << "SocketHandler::initTelnet" << std::endl;
    if(!active)
    {
        try
        {
            socketType = "TELNET";
            socket = new SDL_Socket(host, port);
            if(socket->onEnter())
            {
                active = true;
                std::cout << "SocketHandler::initTelnet active = true" << std::endl;
            }
            else
            {
                active = false;
                std::cout << "SocketHandler::initTelnet active = false" << std::endl;
                return false;
            }
        }
        catch(std::exception& e)
        {
            std::cerr << "exception new SDL_Socket: " << e.what() << '\n';
            return false;
        }
    }
    else
        return false;
    return true;
}

bool SocketHandler::initSSH(std::string host, int port, std::string username, std::string password)
{
    if(!active)
    {
        try
        {
            socketType = "SSH";
            socket = new SSH_Socket(host, port, username, password);
            if(socket->onEnter())
            {
                std::cout << "SocketHandler::initSSH active = true" << std::endl;
                active = true;
            }
            else
            {
                std::cout << "SocketHandler::initSSH active = false" << std::endl;
                active = false;
                return false;
            }
        }
        catch(std::exception& e)
        {
            std::cerr << "exception new SSH_Socket: " << e.what() << '\n';
            return false;
        }
    }
    else
        return false;
    return true;
}

void SocketHandler::reset()
{
    std::cout << "SocketHandler::initTelnet reset()" << std::endl;
    try
    {
        // Deactivate Socket, then Clean it.
        socket->onExit();
        delete socket;
        active = false;
        socketType.erase();
    }
    catch(std::exception& e)
    {
        std::cerr << "exception caught: " << e.what() << '\n';
    }
}
