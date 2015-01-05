
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "socketState.h"
#include "inputHandler.h"
#include "ansiParser.h"

#ifdef TARGET_OS_MAC
#include <SDL_net.h>
#elif _WIN32
#include <SDL_net.h>
#else
#include <SDL2/SDL_net.h>
#endif

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <time.h>

/*
 * Start of SDL_Socket Dervived Class (TELNET)
 */
/* send a string buffer over a TCP socket with error checking */
/* returns 0 on any errors, length sent on success */
int SDL_Socket::sendSocket(unsigned char *buf, Uint32 len)
{
    Uint32 result;

    /* send the buffer, with the NULL as well */
    result = SDLNet_TCP_Send(sock, buf, len);
    //printf("\r\n SOCKET SEND Len: %d, result: %u, str: %s", len, result, buf);

    if(result < strlen((char *)buf))
    {
        if(SDLNet_GetError() && strlen(SDLNet_GetError()))
        {
            printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
            return(0);
        }
    }
    /* return the length sent */
    return(result);
}


/* handle Telnet better */
int SDL_Socket::recvSocket(char *message)
{
    Uint32 result;
    std::string buf;

    /* get the string buffer over the socket */
    result = SDLNet_TCP_Recv(sock, message, 8192);
    //printf("\r\n SOCKET RECV Len: %d, result: %u \r\n", strlen((char *)message), result);

    // TCP Connection is broken. (because of error or closure)
    if(result <= 0)
    {
        onExit();
        return -1;
    }
    /* return the new buffer */
    return result;
}

int SDL_Socket::pollSocket()
{
    int numready = SDLNet_CheckSockets(set, 0);
    if(numready == -1)
    {
        std::cout << "SDLNet_CheckSockets: " << SDLNet_GetError() << std::endl;
        return numready;
    }

    if(numready && SDLNet_SocketReady(sock) == TRUE)
    {
        numready = 1;
        ttime = SDL_GetTicks();
        startBlinking = false;
        cursorBlink = 0;
    }
    else
    {
        numready = 0;
        startBlinking = true;
        //std::cout << "numready = 0;" << std::endl;
        // Setup Timer for Blinking Cursor
        // Initial State = On, then Switch to off in next loop.
        if(cursorBlink % 2 == 0)
        {
            ttime2 = SDL_GetTicks();
            //std::cout << "cursorBlink % 2 " << std::endl;
            //printf("\r\n 2 time diff: %i \r\n ",ttime2 - ttime);
            if(startBlinking && (ttime2 - ttime) > 400)
            {
                //std::cout << "renderCursorOffScreen = 0;" << std::endl;
                TheTerm::Instance()->renderCursorOffScreen(TheAnsiParser::Instance()->x_position-1, TheAnsiParser::Instance()->y_position-1);
                TheTerm::Instance()->drawTextureScreen();
                --cursorBlink;
                ttime = SDL_GetTicks();
            }
        }
        else
        {
            //std::cout << "ELSE !cursorBlink % 2 " << std::endl;
            ttime2 = SDL_GetTicks();
            //printf("\r\n 1 time diff: %i \r\n ",ttime2 - ttime);
            if(startBlinking && (ttime2 - ttime) > 400)
            {
                //std::cout << "renderCursorOnScreen" << std::endl;
                TheTerm::Instance()->renderCursorOnScreen(TheAnsiParser::Instance()->x_position-1, TheAnsiParser::Instance()->y_position-1);
                TheTerm::Instance()->drawTextureScreen();
                ++cursorBlink;
                ttime = SDL_GetTicks();
            }
        }
    }
    return numready;
}

bool SDL_Socket::onEnter()
{
    std::cout << "SDL_Socket::onEnter()" << std::endl;
    IPaddress ip;

    set=SDLNet_AllocSocketSet(1);
    if(!set)
    {
        printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        onExit();
        return false;
    }

    /* Resolve the argument into an IPaddress type */
    std::cout << "Connecting to " << host.c_str() << " on port " << port << std::endl;
    if(SDLNet_ResolveHost(&ip,host.c_str(),port)==-1)
    {
        printf("SDLNet_ResolveHost: %s\n",SDLNet_GetError());
        onExit();
        return false;
    }

    //open the socket
    sock=SDLNet_TCP_Open(&ip);
    if(!sock)
    {
        printf("SDLNet_TCP_Open: %s\n",SDLNet_GetError());
        onExit();
        return false;
    }

    if(SDLNet_TCP_AddSocket(set,sock)==-1)
    {
        printf("SDLNet_TCP_AddSocket: %s\n",SDLNet_GetError());
        onExit();
        return false;
    }
    return true;
}

bool SDL_Socket::onExit()
{
    std::cout << "SDL_Socket::onExit()" << std::endl;
    if(TheInputHandler::Instance()->isGlobalShutdown())
        SDLNet_TCP_Close(sock);

    return true;
}


/*
	MCCP: Mud Client Compression Protocol
    The following is for reference only on handeling MCCP

void *zlib_alloc( void *opaque, unsigned int items, unsigned int size )
{
	return calloc(items, size);
}

void zlib_free( void *opaque, void *address )
{
	free(address);
}

int start_compress( DESCRIPTOR_DATA *d )
{
	char start_mccp[] = { IAC, SB, TELOPT_MCCP, IAC, SE, 0 };
	z_stream *stream;

	if (d->mccp)
	{
		return TRUE;
	}

	stream = calloc(1, sizeof(z_stream));
	stream->next_in	    = NULL;
	stream->avail_in    = 0;
	stream->next_out    = mud->mccp_buf;
	stream->avail_out   = COMPRESS_BUF_SIZE;
	stream->data_type   = Z_ASCII;
	stream->zalloc      = zlib_alloc;
	stream->zfree       = zlib_free;
	stream->opaque      = Z_NULL;

	//	12, 5 = 32K of memory, more than enough
	if (deflateInit2(stream, Z_BEST_COMPRESSION, Z_DEFLATED, 12, 5, Z_DEFAULT_STRATEGY) != Z_OK)
	{
		log_printf("start_compress: failed deflateInit2 D%d@%s", d->descriptor, d->host);
		free(stream);
		return FALSE;
	}
	write_to_descriptor(d, start_mccp, 0);
	//	The above call must send all pending output to the descriptor, since from now on we'll be compressing.
	d->mccp = stream;
	return TRUE;
}

void end_compress( DESCRIPTOR_DATA *d )
{
	if (d->mccp == NULL)
	{
		return;
	}

	d->mccp->next_in	= NULL;
	d->mccp->avail_in	= 0;
	d->mccp->next_out	= mud->mccp_buf;
	d->mccp->avail_out	= COMPRESS_BUF_SIZE;

	if (deflate(d->mccp, Z_FINISH) != Z_STREAM_END)
	{
		log_printf("end_compress: failed to deflate D%d@%s", d->descriptor, d->host);
	}

	if (!HAS_BIT(d->comm_flags, COMM_FLAG_DISCONNECT))
	{
		process_compressed(d);
	}
	if (deflateEnd(d->mccp) != Z_OK)
	{
		log_printf("end_compress: failed to deflateEnd D%d@%s", d->descriptor, d->host);
	}
	free(d->mccp);
	d->mccp = NULL;
	return;
}

void write_compressed( DESCRIPTOR_DATA *d )
{
	d->mccp->next_in    = (unsigned char *) d->outbuf;
	d->mccp->avail_in   = d->outtop;
	d->mccp->next_out   = (unsigned char *) mud->mccp_buf;
	d->mccp->avail_out  = COMPRESS_BUF_SIZE;
	d->outtop           = 0;
	if (deflate(d->mccp, Z_SYNC_FLUSH) != Z_OK)
	{
		return;
	}
	process_compressed(d);
	return;
}

void process_compressed( DESCRIPTOR_DATA *d )
{
	int length;
	length = COMPRESS_BUF_SIZE - d->mccp->avail_out;
	if (write(d->descriptor, mud->mccp_buf, length) < 1)
	{
		log_printf("process_compressed D%d@%s", d->descriptor, d->host);
		SET_BIT(d->comm_flags, COMM_FLAG_DISCONNECT);
		return;
	}
	return;
}

int process_do_mccp( DESCRIPTOR_DATA *d, unsigned char *src, int srclen )
{
	start_compress(d);
	return 3;
}

int process_dont_mccp( DESCRIPTOR_DATA *d, unsigned char *src, int srclen )
{
	end_compress(d);
	return 3;
}

*/
