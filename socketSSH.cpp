
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "socketState.h"
#include "socketHandler.h"
#include "ansiParser.h"

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

#include <cstdio>
#include <cstring>
#include <cerrno>


/*
 * Start of SSH_Socket Dervived Class (SSH)
 *
 *
 */


/* send a string buffer over a TCP socket with error checking */
/* returns 0 on any errors, length sent on success */
int SSH_Socket::sendSocket(unsigned char *buf, Uint32 len)
{

    Uint32 result = 0;
    /* send the buffer, with the NULL as well */
    result = ssh_channel_write(sshChannel, buf, len);

    if(result < strlen((char *)buf))
    {
        ssh_channel_close(sshChannel);
        ssh_channel_free(sshChannel);
        return SSH_ERROR; // or Zero.
    }

    /* return the length sent */
    return(result);
}


/* handle Telnet better */
int SSH_Socket::recvSocket(char *message)
{
    Uint32 result = 0;
    std::string buf;

    //printf(" \r\n GETMSG \r\n ");

    /* get the string buffer over the socket */
    //result = ssh_channel_read(sshChannel, message, sizeof(message), 0);
    result = ssh_channel_read_nonblocking(sshChannel, message, 8192, 0);
    if (result < 0)
    {
        printf(" \r\n *** GETMSG ERROR \r\n ");
        ssh_channel_close(sshChannel);
        ssh_channel_free(sshChannel);
        return SSH_ERROR;
    }

    //printf (" \r\n message: %i \r\n", strlen(message));

    /* return the new buffer */
    return result;
}



int SSH_Socket::pollSocket()
{
    int numready;
    if (sshChannel && ssh_channel_is_open(sshChannel) && ssh_channel_poll(sshChannel,0) > 0)
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
        if (cursorBlink % 2 == 0)
        {
            ttime2 = SDL_GetTicks();

            //std::cout << "cursorBlink % 2 " << std::endl;

            //printf("\r\n 2 time diff: %i \r\n ",ttime2 - ttime);
            if (startBlinking && (ttime2 - ttime) > 400)
            {

                //std::cout << "RenderCursorOffScreen = 0;" << std::endl;

                TheTerm::Instance()->RenderCursorOffScreen(TheAnsiParser::Instance()->x_position-1, TheAnsiParser::Instance()->y_position-1);
                TheTerm::Instance()->DrawTextureScreen();
                --cursorBlink;
                ttime = SDL_GetTicks();
            }
        }
        else
        {
            //std::cout << "ELSE !cursorBlink % 2 " << std::endl;
            ttime2 = SDL_GetTicks();

            //printf("\r\n 1 time diff: %i \r\n ",ttime2 - ttime);
            if (startBlinking && (ttime2 - ttime) > 400)
            {

                //std::cout << "RenderCursorOnScreen" << std::endl;

                TheTerm::Instance()->RenderCursorOnScreen(TheAnsiParser::Instance()->x_position-1, TheAnsiParser::Instance()->y_position-1);
                TheTerm::Instance()->DrawTextureScreen();
                ++cursorBlink;
                ttime = SDL_GetTicks();
            }
        }
    }

    if (ssh_channel_poll(sshChannel,0) < 0)
        TheSocketHandler::Instance()->setActive(false);


    return numready;
}

bool SSH_Socket::onEnter()
{
    int verb = SSH_LOG_PROTOCOL;
    int rc;

    // Setup new SSH Shell
    session = ssh_new();
    if (session == NULL)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
             "Closed Session",
             "User has closed the program.",
             NULL);
        onExit();
        return false;
    }


    //SSH Set Connection Options
    ssh_options_set(session, SSH_OPTIONS_HOST, host.c_str());

    // Testing only, otherwise slows down display!!
    ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &verb);

    // Only Slllow SSH2 Connections.
    int disAllow = 0;
    int allow    = 1;
    int timeout  = 30;

    ssh_options_set(session, SSH_OPTIONS_SSH1, &disAllow);
    ssh_options_set(session, SSH_OPTIONS_SSH2, &allow);
    ssh_options_set(session, SSH_OPTIONS_TIMEOUT, &timeout);
    ssh_options_set(session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(session, SSH_OPTIONS_USER, sshUser.c_str());

    // SSH Connect
    rc = ssh_connect(session);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "\r\n SSH Error connecting to %s: %s \r\n",
        host.c_str(), ssh_get_error(session));
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
             "Closed Session",
             "User has closed the program.",
             NULL);
        onExit();
        return false;
    }

    // Verify Server is a known host.
    rc = verify_knownhost();
    if (rc < 0)
    {
        fprintf(stderr, "\r\n SSH Error, Can't Validate Host Server to %s: %s - %i \r\n",
        host.c_str(), ssh_get_error(session), rc);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
             "Closed Session",
             "User has closed the program.",
             NULL);
        onExit();
        return false;
    }

    //if ((ssh_userauth_password(session, NULL, password) == SSH_AUTH_SUCCESS)
    rc = authenticate_console();
    if (rc != SSH_AUTH_SUCCESS)
    {
        printf("\r\n SSH Error, Can't Authenticate User %s: %s - %i \r\n",
        host.c_str(), ssh_get_error(session), rc);
        onExit();
        return false;
    }


    // Setup Channel for Socket Communications
    sshChannel = ssh_channel_new(session);
    if (sshChannel == NULL)
    {
        printf("\r\n SSH Error, Setup Channel for Socket Communications %s: %s - %i \r\n",
        host.c_str(), ssh_get_error(session), rc);
        onExit();
        return false;
    }

    // Open A shell Session
    rc = ssh_channel_open_session(sshChannel);
    if (rc != SSH_OK)
    {
        printf("\r\n SSH Error, Open Channel for Socket Communications %s: %s - %i \r\n",
        host.c_str(), ssh_get_error(session), rc);
        ssh_channel_free(sshChannel);
        onExit();
        return false;
    }

    /* Don't need this when we use the PTY size, it does both!
    // Setup sockets or "channels" within the session
    if(ssh_channel_request_pty(sshChannel))
    {
        printf("\r\n SSH Error, Request for PTY Failed. %s: %s - %i \r\n",
        host, ssh_get_error(session), rc);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
             "Closed Session",
             "User has closed the program.",
             NULL);
        onExit();
    }*/

    // Set the term and pty size of the terminal.
    // Only After the Shell has been initalized.
    if(ssh_channel_request_pty_size(sshChannel,"ansi",80,25))
    {
        printf("\r\n SSH Error, Request for PTY term and size failed. %s: %s - %i \r\n",
        host.c_str(), ssh_get_error(session), rc);

        // Not an error to exit the program on.
        //return 0;
    }

    // Now reqeust a shell with the pty to get read/write
    if(ssh_channel_request_shell(sshChannel))
    {
        printf("\r\n SSH Error, Request for shell on channel failed. %s: %s - %i \r\n",
        host.c_str(), ssh_get_error(session), rc);
        onExit();
        return false;
    }

    // Set the term and pty size of the terminal.
    //if(ssh_channel_request_env(sshChannel,"LANG","en.us_CP437"))
    //{
    //    printf(" *** Requesting request_env : %s\n",ssh_get_error(session));
        //return 0;
    //}

    // Send echo commands for CP437
    //sendSocket((unsigned char *)"\033[%@\033(U",7);


    return true;
}


bool SSH_Socket::onExit()
{
    if (sshChannel)
    {
        ssh_channel_send_eof(sshChannel);
        ssh_channel_close(sshChannel);
        ssh_channel_free(sshChannel);
    }
    if (session)
    {
        ssh_disconnect(session);
        ssh_free(session);
    }

    return true;
}



// Example communications
int SSH_Socket::show_remote_processes()
{
    ssh_channel channel;
    int rc;
    char buffer[256];
    int nbytes;
    channel = ssh_channel_new(session);

    if (channel == NULL)
        return SSH_ERROR;

    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        ssh_channel_free(channel);
        return rc;
    }
    rc = ssh_channel_request_exec(channel, "ps aux");
    if (rc != SSH_OK)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return rc;
    }
    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    while (nbytes > 0)
    {
        if (write(1, buffer, nbytes) != nbytes)
        {
            ssh_channel_close(channel);
            ssh_channel_free(channel);
            return SSH_ERROR;
        }
        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    }
    if (nbytes < 0)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return SSH_ERROR;
    }
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return SSH_OK;
}


// Verify is Server is a known host.
int SSH_Socket::verify_knownhost()
{
    size_t state, hlen;
    unsigned char *hash = NULL;
    char *hexa;
    //char buf[10]={0};

    state = ssh_is_server_known(session);

    ssh_key srv_pubkey;
    int rc;

    rc = ssh_get_publickey(session, &srv_pubkey);
    if (rc < 0)
    {
        return -1;
    }

    //hlen = ssh_get_pubkey_hash(session, &hash);

    rc = ssh_get_publickey_hash(srv_pubkey, SSH_PUBLICKEY_HASH_SHA1, &hash, &hlen);
    ssh_key_free(srv_pubkey);
    if (rc < 0)
    {
        return -1;
    }


    switch (state)
    {
    case SSH_SERVER_KNOWN_OK:
        break; /* ok */

    case SSH_SERVER_KNOWN_CHANGED:
        fprintf(stderr, "Host key for server changed: it is now:\n");
        ssh_print_hexa("Public key hash", hash, hlen);
        fprintf(stderr, "For security reasons, connection will be stopped\n");
        free(hash);
        return -1;

    case SSH_SERVER_FOUND_OTHER:
        fprintf(stderr, "The host key for this server was not found but an other"
                "type of key exists.\n");
        fprintf(stderr, "An attacker might change the default server key to"
                "confuse your client into thinking the key does not exist\n");
        free(hash);
        return -1;

    case SSH_SERVER_FILE_NOT_FOUND:
        fprintf(stderr, "Could not find known host file.\n");
        fprintf(stderr, "If you accept the host key here, the file will be"
                "automatically created.\n");

        /* fallback to SSH_SERVER_NOT_KNOWN behavior */
    case SSH_SERVER_NOT_KNOWN:
        hexa = ssh_get_hexa(hash, hlen);
        fprintf(stderr,"The server is unknown. Adding Key."); //Do you trust the host key?\n");
        fprintf(stderr, "Public key hash: %s\n", hexa);
        free(hexa);

        /*
        if (fgets(buf, sizeof(buf), stdin) == NULL)
        {
            free(hash);
            return -1;
        }
        if (strncasecmp(buf, "yes", 3) != 0)
        {
            free(hash);
            return -1;
        }
        */
        if (ssh_write_knownhost(session) < 0)
        {
            fprintf(stderr, "Error %s\n", strerror(errno));
            free(hash);
            return -1;
        }
        break;

    case SSH_SERVER_ERROR:
        fprintf(stderr, "Error %s", ssh_get_error(session));
        free(hash);
        return -1;
    }
    free(hash);
    return 0;
}

int SSH_Socket::authenticate_kbdint()
{
    int rc;
    rc = ssh_userauth_kbdint(session, NULL, NULL);
    while (rc == SSH_AUTH_INFO)
    {
        const char *name, *instruction;
        int nprompts, iprompt;

        name = ssh_userauth_kbdint_getname(session);
        instruction = ssh_userauth_kbdint_getinstruction(session);
        nprompts = ssh_userauth_kbdint_getnprompts(session);

        if (strlen(name) > 0)
            printf("%s\n", name);

        if (strlen(instruction) > 0)
            printf("%s\n", instruction);

        for (iprompt = 0; iprompt < nprompts; iprompt++)
        {
            const char *prompt;
            char echo;

            prompt = ssh_userauth_kbdint_getprompt(session, iprompt, &echo);

            if (echo)
            {
                char buffer[128], *ptr;
                printf("%s", prompt);

                if (fgets(buffer, sizeof(buffer), stdin) == NULL)
                    return SSH_AUTH_ERROR;

                buffer[sizeof(buffer) - 1] = '\0';
                if ((ptr = strchr(buffer, '\n')) != NULL)
                    *ptr = '\0';

                if (ssh_userauth_kbdint_setanswer(session, iprompt, buffer) < 0)
                    return SSH_AUTH_ERROR;

                memset(buffer, 0, strlen(buffer));
            }
            else
            {
                char *ptr;
                ptr = (char *)password.c_str(); //getpass(prompt);
                if (ssh_userauth_kbdint_setanswer(session, iprompt, ptr) < 0)
                    return SSH_AUTH_ERROR;
            }
        }
        rc = ssh_userauth_kbdint(session, NULL, NULL);
    }
    return rc;
}


int SSH_Socket::authenticate_console()
{
    int rc;
    int method;
    //char password[128] = {0};
    char *banner;

    // Try to authenticate
    rc = ssh_userauth_none(session, NULL);
    switch (rc)
    {
    case SSH_AUTH_ERROR:   //some serious error happened during authentication
        printf("\r\n ssh_userauth_none SSH_AUTH_ERROR! \r\n");
        error();
        return rc;
    case SSH_AUTH_DENIED:  //no key matched
        printf("\r\n ssh_userauth_none SSH_AUTH_DENIED! \r\n");
        break;
    case SSH_AUTH_SUCCESS: //you are now authenticated
        printf("\r\n ssh_userauth_none SSH_AUTH_SUCCESS! \r\n");
        break;
    case SSH_AUTH_PARTIAL: //some key matched but you still have to provide an other mean of authentication (like a password).
        printf("\r\n ssh_userauth_none SSH_AUTH_PARTIAL! \r\n");
        break;
    }

    method = ssh_auth_list(session);
    while (rc != SSH_AUTH_SUCCESS)
    {
        // Try to authenticate with public key first
        if (method & SSH_AUTH_METHOD_PUBLICKEY)
        {
            rc = ssh_userauth_autopubkey(session, NULL);
            //rc = ssh_userauth_publickey_auto(session, NULL, NULL);
            switch (rc)
            {
            case SSH_AUTH_ERROR:   //some serious error happened during authentication
                printf("\r\n SSH_AUTH_METHOD_PUBLICKEY - SSH_AUTH_ERROR! \r\n");
                error();
                return rc;
            case SSH_AUTH_DENIED:  //no key matched
                printf("\r\n SSH_AUTH_METHOD_PUBLICKEY - SSH_AUTH_DENIED! \r\n");
                break;
            case SSH_AUTH_SUCCESS: //you are now authenticated
                printf("\r\n SSH_AUTH_METHOD_PUBLICKEY - SSH_AUTH_SUCCESS! \r\n");
                break;
            case SSH_AUTH_PARTIAL: //some key matched but you still have to provide an other mean of authentication (like a password).
                printf("\r\n SSH_AUTH_METHOD_PUBLICKEY - SSH_AUTH_PARTIAL! \r\n");
                break;
            }

            /*
            rc = ssh_userauth_try_publickey(session, NULL, "pubkey.pem");
            switch (rc)
            {
            case SSH_AUTH_ERROR:   //some serious error happened during authentication
                printf("\r\n try_publickey - SSH_AUTH_ERROR! \r\n");
                error(session);
                return rc;
            case SSH_AUTH_DENIED:  //no key matched
                printf("\r\n try_publickey - SSH_AUTH_DENIED! \r\n");
                break;
            case SSH_AUTH_SUCCESS: //you are now authenticated
                printf("\r\n try_publickey - SSH_AUTH_SUCCESS! \r\n");
                break;
            case SSH_AUTH_PARTIAL: //some key matched but you still have to provide an other mean of authentication (like a password).
                printf("\r\n try_publickey - SSH_AUTH_PARTIAL! \r\n");
                break;
            }*/
        }

        // Try to authenticate with keyboard interactive";
        if (method & SSH_AUTH_METHOD_INTERACTIVE)
        {
            //rc = authenticate_kbdint(session, NULL);
            switch (rc)
            {
            case SSH_AUTH_ERROR:   //some serious error happened during authentication
                printf("\r\n SSH_AUTH_METHOD_INTERACTIVE - SSH_AUTH_ERROR! \r\n");
                error();
                return rc;
            case SSH_AUTH_DENIED:  //no key matched
                printf("\r\n SSH_AUTH_METHOD_INTERACTIVE - SSH_AUTH_DENIED! \r\n");
                break;
            case SSH_AUTH_SUCCESS: //you are now authenticated
                printf("\r\n SSH_AUTH_METHOD_INTERACTIVE - SSH_AUTH_SUCCESS! \r\n");
                break;
            case SSH_AUTH_PARTIAL: //some key matched but you still have to provide an other mean of authentication (like a password).
                printf("\r\n SSH_AUTH_METHOD_INTERACTIVE - SSH_AUTH_PARTIAL! \r\n");
                break;
            }
        }

        /*
        if (ssh_getpass("Password: ", password, sizeof(password), 0, 0) < 0)
        {
            return SSH_AUTH_ERROR;
        }*/

        // Try to authenticate with password
        if (method & SSH_AUTH_METHOD_PASSWORD)
        {
            rc = ssh_userauth_password(session, NULL, password.c_str());
            switch (rc)
            {
            case SSH_AUTH_ERROR:   //some serious error happened during authentication
                printf("\r\n SSH_AUTH_METHOD_PASSWORD - SSH_AUTH_ERROR! \r\n");
                error();
                return rc;
            case SSH_AUTH_DENIED:  //no key matched
                printf("\r\n SSH_AUTH_METHOD_PASSWORD - SSH_AUTH_DENIED! \r\n");
                break;
            case SSH_AUTH_SUCCESS: //you are now authenticated
                printf("\r\n SSH_AUTH_METHOD_PASSWORD - SSH_AUTH_SUCCESS! \r\n");
                break;
            case SSH_AUTH_PARTIAL: //some key matched but you still have to provide an other mean of authentication (like a password).
                printf("\r\n SSH_AUTH_METHOD_PASSWORD - SSH_AUTH_PARTIAL! \r\n");
                break;
            }
        }
    }

    banner = ssh_get_issue_banner(session);
    if (banner)
    {
        printf("%s\n",banner);
        ssh_string_free_char(banner);
    }

    return rc;
}
