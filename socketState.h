
#ifndef __MY_SOCKET__H
#define __MY_SOCKET__H

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "terminal.h"

#ifdef TARGET_OS_MAC
#include <SDL.h>
#include <SDL_net.h>
#elif _WIN32
#include <SDL.h>
#include <SDL_net.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#endif

#include <libssh/libssh.h>

#include <iostream>
#include <string>

/*
 * Virtual Class Container for Common access to different
 * Socket And connection types.
 */
class SocketState
{
public:

    virtual ~SocketState()
    {
        std::cout << "Shutting Down SocketState" << std::endl;
    }
    virtual int sendSocket(unsigned char *message, Uint32 len) = 0;
    virtual int recvSocket(char *message)= 0;
    virtual int pollSocket() = 0;
    virtual bool onEnter() = 0;
    virtual bool onExit() = 0;
};

/*
 * Class for TCP Telnet Sockets
 */
class SDL_Socket : public SocketState
{
public:

    SDL_Socket(std::string _host, int _port) :
        sock(0),
        set(0)
    {
        host = _host;
        port = _port;
    }

    virtual ~SDL_Socket()
    {
        std::cout << "Shutting Down SDL_SocketState" << std::endl;
    }
    virtual int sendSocket(unsigned char *message, Uint32 len);
    virtual int recvSocket(char *message);
    virtual int pollSocket();
    virtual bool onEnter();
    virtual bool onExit();

private:

    std::string host;
    int port;
    TCPsocket sock;
    SDLNet_SocketSet set;

};

/*
 * Class for TCP FTP Sockets
 */
class FTP_Socket : public SocketState
{
public:

    FTP_Socket(std::string _host, int _port, std::string _user, std::string _pass) :
        controlSocket(0),
        listenSocket(0),
        dataSocket(0),
        set(0)
    {
        host = _host;
        port = _port;
        userId = _user;
        password = _pass;
    }

    virtual ~FTP_Socket()
    {
        std::cout << "Shutting Down FTP SDL_SocketState" << std::endl;
    }
    virtual int sendSocket(unsigned char *message, Uint32 len);
    virtual int recvSocket(char *message);
    virtual int pollSocket();
    virtual bool onEnter();
    virtual bool onExit();

private:

    std::string host;
    int port;
    std::string userId;
    std::string password;

    TCPsocket controlSocket;
    TCPsocket listenSocket;
    TCPsocket dataSocket;
    SDLNet_SocketSet set;

};

/*
 * Class for TCP SSH Connections
 */
class SSH_Socket : public SocketState
{
public:

    SSH_Socket(std::string _host, int _port, std::string _user, std::string _pass) : sshChannel(0), session(0)
    {
        host = _host;
        port = _port;
        userId = _user;
        password = _pass;
    }

    virtual  ~SSH_Socket()
    {
        std::cout << "Shutting Down SSH_SocketState" << std::endl;
    }
    virtual int sendSocket(unsigned char *message, Uint32 len);
    virtual int recvSocket(char *message);
    virtual int pollSocket();
    virtual bool onEnter();
    virtual bool onExit();

    // Specific Functions for SSH
    int show_remote_processes();
    int verify_knownhost();
    int authenticate_console();
    int authenticate_kbdint();
    void error()
    {
        std::cout << "Authentication failed: " << ssh_get_error(session) << std::endl;
    }


private:

    int cursorBlink;
    int startBlinking;
    time_t ttime, ttime2;

    ssh_channel sshChannel;
    ssh_session session;

    std::string host;
    int port;
    std::string userId;
    std::string password;

};


/*
 * Handle IMCP for Ping / Traceroutes.
 *
 */

#ifndef _WIN32

// Windows Types

typedef unsigned char BYTE;
typedef BOOL *PBOOL,*LPBOOL;
typedef unsigned short WORD;
typedef float FLOAT;
typedef FLOAT *PFLOAT;
typedef BYTE *PBYTE,*LPBYTE;
typedef int *PINT,*LPINT;
typedef WORD *PWORD,*LPWORD;
typedef long *LPLONG;
typedef DWORD *PDWORD,*LPDWORD;
typedef CONST void *PCVOID,*LPCVOID;
typedef int INT;
typedef unsigned int UINT,*PUINT,*LPUINT;

typedef char CHAR;
typedef short SHORT;
typedef long LONG;
typedef char CCHAR, *PCCHAR;
typedef unsigned char UCHAR,*PUCHAR;
typedef unsigned short USHORT,*PUSHORT;
typedef unsigned long ULONG,*PULONG;

#endif

// ICMP packet types
#define ICMP_ECHO_REPLY 0
#define ICMP_DEST_UNREACH 3
#define ICMP_TTL_EXPIRE 11
#define ICMP_ECHO_REQUEST 8

// Minimum ICMP packet size, in bytes
#define ICMP_MIN 8

#ifdef _MSC_VER
// The following two structures need to be packed tightly, but unlike
// Borland C++, Microsoft C++ does not do this by default.
#pragma pack(1)
#endif

// The IP header
struct __attribute__((packed)) IPHeader
{
    BYTE h_len:4;           // Length of the header in dwords
    BYTE version:4;         // Version of IP
    BYTE tos;               // Type of service
    USHORT total_len;       // Length of the packet in dwords
    USHORT ident;           // unique identifier
    USHORT flags;           // Flags
    BYTE ttl;               // Time to live
    BYTE proto;             // Protocol number (TCP, UDP etc)
    USHORT checksum;        // IP checksum
    ULONG source_ip;
    ULONG dest_ip;
};

// ICMP header
struct __attribute__((packed)) ICMPHeader
{
    BYTE type;          // ICMP packet type
    BYTE code;          // Type sub code
    USHORT checksum;
    USHORT id;
    USHORT seq;
    ULONG timestamp;    // not part of ICMP, but we need it
};

#define DEFAULT_PACKET_SIZE 32
#define DEFAULT_TTL 30
#define MAX_PING_DATA_SIZE 1024
#define MAX_PING_PACKET_SIZE (MAX_PING_DATA_SIZE + sizeof(IPHeader))

#ifdef _MSC_VER
#pragma pack()
#endif

/*
 * Class for IMCP Ping for Server connections
 */
class ICMP_Socket : public SocketState
{
public:

    ICMP_Socket(std::string _host, int _port) : sock(0), set(0)
    {
        host = _host;
        port = _port;
    }

    virtual ~ICMP_Socket() { }
    virtual int sendSocket(unsigned char *message, Uint32 len);
    virtual int recvSocket(char *message);
    virtual int pollSocket();
    virtual bool onEnter();
    virtual bool onExit();

private:

    std::string host;
    int port;

    UDPsocket sock;
    //UDPpacket *packet;
    SDLNet_SocketSet set;

    IPHeader   *ipHeader;
    ICMPHeader *icmpHeader;

};
#endif
