
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "socketState.h"
#include "socketHandler.h"
#include "ansiParser.h"

#ifdef TARGET_OS_MAC
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
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
#include <iostream>

/*
 * Start of SSH_Socket Dervived Class (SSH)
 */

/* send a string buffer over a TCP socket with error checking */
/* returns 0 on any errors, length sent on success */
int SSH_Socket::sendSocket(unsigned char *buf, Uint32 len)
{
    Uint32 result = 0;

    result = ssh_channel_write(sshChannel, buf, len);
    if(result < strlen((char *)buf))
    {
        return SSH_ERROR; // or Zero.
    }
    return(result);
}

/* handle Telnet better */
int SSH_Socket::recvSocket(char *message)
{
    int result = 0;
    std::string buf;

    result = ssh_channel_read_nonblocking(sshChannel, message, 8192, 0);
    if(result < 0)
    {
        std::cout << "Error: ssh_channel_read_nonblocking" << std::endl;
        return SSH_ERROR;
    }
    return result;
}

int SSH_Socket::pollSocket()
{
    int numready;
    if(sshChannel && ssh_channel_is_open(sshChannel) && ssh_channel_poll(sshChannel,0) > 0)
    {
        numready = 1;
    }
    else
    {
        numready = 0;
    }
    if(ssh_channel_poll(sshChannel,0) < 0)
        TheSocketHandler::Instance()->setActive(false);
    return numready;
}

bool SSH_Socket::onEnter()
{
    // For testing and getting debugging output
    int verb = SSH_LOG_PROTOCOL;
    int rc;

    // Setup new SSH Shell
    session = ssh_new();
    if(session == NULL)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
            "Closed Session", "User has closed the program.", NULL);

        TheSocketHandler::Instance()->setActive(false);
        return false;
    }

    std::cout << "Connecting to: " << host << ":" << port << std::endl;
    std::cout << "User: " << userId << std::endl;

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
    ssh_options_set(session, SSH_OPTIONS_USER, userId.c_str());

    std::cout << "Starting up SSH Connection, this can take few moments" << std::endl;

    // SSH Connect
    rc = ssh_connect(session);
    if(rc != SSH_OK)
    {
        std::cout << "Error: ssh_connect: " << host
            << " " << ssh_get_error(session) << std::endl;

        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
            "Closed Session", "Unable to Connect to Server!", NULL);

        TheSocketHandler::Instance()->setActive(false);
        return false;
    }

    // Verify Server is a known host.
    rc = verify_knownhost();
    if(rc < 0)
    {
        std::cout << "Error: verify_knownhost: " << host
            << " " << ssh_get_error(session) << " " << rc << std::endl;

        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
            "Closed Session", "User has closed the program.", NULL);

        TheSocketHandler::Instance()->setActive(false);
        return false;
    }

    //if ((ssh_userauth_password(session, NULL, password) == SSH_AUTH_SUCCESS)
    rc = authenticate_console();
    if(rc != SSH_AUTH_SUCCESS)
    {
         std::cout << "Error: authenticate: " << host
            << " " << ssh_get_error(session) << " " << rc << std::endl;

        TheSocketHandler::Instance()->setActive(false);
        return false;
    }

    // Setup Channel for Socket Communications
    sshChannel = ssh_channel_new(session);
    if(sshChannel == NULL)
    {
        std::cout << "Error: ssh_channel_new: " << host
            << " " << ssh_get_error(session) << " " << rc << std::endl;

        TheSocketHandler::Instance()->setActive(false);
        return false;
    }

    // Open A shell Session
    rc = ssh_channel_open_session(sshChannel);
    if(rc != SSH_OK)
    {
        std::cout << "Error: ssh_channel_open_session: " << host
            << " " << ssh_get_error(session) << " " << rc << std::endl;

        TheSocketHandler::Instance()->setActive(false);
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
    if(ssh_channel_request_pty_size(sshChannel, "ansi", 80, 25))
    {
        std::cout << "Error: ssh_channel_request_pty_size: " << host
            << " " << ssh_get_error(session) << " " << rc << std::endl;

        // Not an error to exit the program on.
        //return 0;
    }

    // Now reqeust a shell with the pty to get read/write
    if(ssh_channel_request_shell(sshChannel))
    {
        std::cout << "Error: ssh_channel_request_shell: " << host
            << " " << ssh_get_error(session) << " " << rc << std::endl;

        TheSocketHandler::Instance()->setActive(false);
        return false;
    }
    return true;
}


bool SSH_Socket::onExit()
{
    std::cout << "SSH releasing channel" << std::endl;
    if(sshChannel)
    {
        if (TheSocketHandler::Instance()->isActive())
            ssh_channel_send_eof(sshChannel);
        ssh_channel_close(sshChannel);
        ssh_channel_free(sshChannel);
        sshChannel = NULL;
    }
    std::cout << "SSH releasing session" << std::endl;
    if(session)
    {
        ssh_disconnect(session);
        ssh_free(session);
        session = NULL;
    }
    return true;
}

// Verify if Server is a known host.
int SSH_Socket::verify_knownhost()
{
    int state;
    size_t hlen;
    unsigned char *hash = NULL;
    char *hexa;

    state = ssh_is_server_known(session);
    ssh_key srv_pubkey;
    int rc;

    rc = ssh_get_publickey(session, &srv_pubkey);
    if(rc < 0)
    {
        return -1;
    }

    rc = ssh_get_publickey_hash(srv_pubkey, SSH_PUBLICKEY_HASH_SHA1, &hash, &hlen);
    ssh_key_free(srv_pubkey);
    if(rc < 0)
    {
        return -1;
    }

    switch(state)
    {
        case SSH_SERVER_KNOWN_OK:
            break; /* ok */

        case SSH_SERVER_KNOWN_CHANGED:
            std::cout << "Host key for server changed: it is now" << std::endl;
            ssh_print_hexa("Public key hash", hash, hlen);
            std::cout << "For security reasons, connection will be stopped" << std::endl;
            free(hash);
            return -1;

        case SSH_SERVER_FOUND_OTHER:
            std::cout << "The host key for this server was not found but an other"
                      << "type of key exists." << std::endl;
            std::cout << "An attacker might change the default server key to"
                      << "confuse your client into thinking the key does not exist" << std::endl;
            free(hash);
            return -1;

        case SSH_SERVER_FILE_NOT_FOUND:
            std::cout << "Could not find known host file." << std::endl;
            std::cout << "If you accept the host key here, the file will be"
                      << "automatically created." << std::endl;

            /* fallback to SSH_SERVER_NOT_KNOWN behavior */
        case SSH_SERVER_NOT_KNOWN:
            hexa = ssh_get_hexa(hash, hlen);
            //Do you trust the host key?\n");
            std::cout << "The server is unknown. Adding Key." << std::endl;
            std::cout << "Public key hash: " << hexa << std::endl;
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
            if(ssh_write_knownhost(session) < 0)
            {
                std::cout << "Error: " << strerror(errno) << std::endl;
                free(hash);
                return -1;
            }
            break;

        case SSH_SERVER_ERROR:
            std::cout << "Error: " << ssh_get_error(session) << std::endl;
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
    while(rc == SSH_AUTH_INFO)
    {
        const char *name, *instruction;
        int nprompts, iprompt;

        name = ssh_userauth_kbdint_getname(session);
        instruction = ssh_userauth_kbdint_getinstruction(session);
        nprompts = ssh_userauth_kbdint_getnprompts(session);

        if(strlen(name) > 0)
            printf("%s\n", name);

        if(strlen(instruction) > 0)
            printf("%s\n", instruction);

        for(iprompt = 0; iprompt < nprompts; iprompt++)
        {
            const char *prompt;
            char echo;

            prompt = ssh_userauth_kbdint_getprompt(session, iprompt, &echo);
            if(echo)
            {
                char buffer[128], *ptr;
                printf("%s", prompt);

                if(fgets(buffer, sizeof(buffer), stdin) == NULL)
                    return SSH_AUTH_ERROR;

                buffer[sizeof(buffer) - 1] = '\0';
                if((ptr = strchr(buffer, '\n')) != NULL)
                    *ptr = '\0';

                if(ssh_userauth_kbdint_setanswer(session, iprompt, buffer) < 0)
                    return SSH_AUTH_ERROR;

                memset(buffer, 0, strlen(buffer));
            }
            else
            {
                char *ptr;
                ptr = (char *)password.c_str(); //getpass(prompt);
                if(ssh_userauth_kbdint_setanswer(session, iprompt, ptr) < 0)
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
    char *banner;

    // Try to authenticate
    rc = ssh_userauth_none(session, NULL);
    switch(rc)
    {
        case SSH_AUTH_ERROR:   //some error happened during authentication
            std::cout << "ssh_userauth_none SSH_AUTH_ERROR!" << std::endl;
            error();
            return rc;
        case SSH_AUTH_DENIED:  //no key matched
            std::cout << "ssh_userauth_none SSH_AUTH_DENIED!" << std::endl;
            break;
        case SSH_AUTH_SUCCESS: //you are now authenticated
            std::cout << "ssh_userauth_none SSH_AUTH_SUCCESS!" << std::endl;
            break;
        case SSH_AUTH_PARTIAL:
            //some key matched but you still have
            //to provide an other mean of authentication (like a password).
            std::cout << "ssh_userauth_none SSH_AUTH_PARTIAL!" << std::endl;
            break;
    }

    // Get a list of excepted Auth Sessions server wants.
    method = ssh_auth_list(session);
    while(rc != SSH_AUTH_SUCCESS)
    {
/*
    Retrieve the public key with ssh_import_pubkey_file().
    Offer the public key to the SSH server using ssh_userauth_try_publickey().
     * If the return value is SSH_AUTH_SUCCESS, the SSH server accepts
     * to authenticate using the public key and you can go to the next step.
    Retrieve the private key, using the ssh_pki_import_privkey_file() function.
     * If a passphrase is needed, either the passphrase specified
     * as argument or a callback will be used.
    Authenticate using ssh_userauth_publickey() with your private key.
    Do not forget cleaning up memory using ssh_key_free().
*/
        // The function ssh_userauth_autopubkey() does this using the
        // available keys in "~/.ssh/" or sshagent. The return values are the following:
        // ** Public Key Needs more testing.
        if(method & SSH_AUTH_METHOD_PUBLICKEY)
        {            
            rc = ssh_userauth_autopubkey(session, NULL);
            switch(rc)
            {
                case SSH_AUTH_ERROR:   //some serious error happened during authentication
                    std::cout << "SSH_AUTH_METHOD_PUBLICKEY - SSH_AUTH_ERROR!" << std::endl;
                    error();
                    return rc;
                case SSH_AUTH_DENIED:  //no key matched
                    std::cout << "SSH_AUTH_METHOD_PUBLICKEY - SSH_AUTH_DENIED!" << std::endl;
                    break;
                case SSH_AUTH_SUCCESS: //you are now authenticated
                    std::cout << "SSH_AUTH_METHOD_PUBLICKEY - SSH_AUTH_SUCCESS!" << std::endl;
                    break;
                case SSH_AUTH_PARTIAL:
                    // some key matched but you still have
                    // to provide an other mean of authentication (like a password).
                    std::cout << "SSH_AUTH_METHOD_PUBLICKEY - SSH_AUTH_PARTIAL!" << std::endl;
                    break;
                default:
                    break;
            }
/*
            // Validate with specific public key file
            rc = ssh_userauth_try_publickey(session, NULL, "pubkey.pub");
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
                case SSH_AUTH_PARTIAL: //some key matched but you still have to
                                       //provide an other mean of authentication (like a password).
                    printf("\r\n try_publickey - SSH_AUTH_PARTIAL! \r\n");
                    break;
                default:
                    break;
            }
             */
        }

        // Try to authenticate with keyboard interactive";
        if(method & SSH_AUTH_METHOD_INTERACTIVE)
        {
            //rc = authenticate_kbdint(session, NULL);
            switch(rc)
            {
                case SSH_AUTH_ERROR:   //some serious error happened during authentication
                    std::cout << "SSH_AUTH_METHOD_INTERACTIVE - SSH_AUTH_ERROR!" << std::endl;
                    error();
                    return rc;
                case SSH_AUTH_DENIED:  //no key matched
                    std::cout << "SSH_AUTH_METHOD_INTERACTIVE - SSH_AUTH_DENIED!" << std::endl;
                    break;
                case SSH_AUTH_SUCCESS: //you are now authenticated
                    std::cout << "SSH_AUTH_METHOD_INTERACTIVE - SSH_AUTH_SUCCESS!" << std::endl;
                    break;
                case SSH_AUTH_PARTIAL: //some key matched but you still have to
                                       //provide an other mean of authentication (like a password).
                    std::cout << "SSH_AUTH_METHOD_INTERACTIVE - SSH_AUTH_PARTIAL!" << std::endl;
                    break;
                default:
                    break;
            }
        }
        /*
        if (ssh_getpass("Password: ", password, sizeof(password), 0, 0) < 0)
        {
            return SSH_AUTH_ERROR;
        }*/

        // Try to authenticate with password
        if(method & SSH_AUTH_METHOD_PASSWORD)
        {
            rc = ssh_userauth_password(session, NULL, password.c_str());
            switch(rc)
            {
                case SSH_AUTH_ERROR:   //some serious error happened during authentication
                    std::cout << "SSH_AUTH_METHOD_PASSWORD - SSH_AUTH_ERROR!" << std::endl;
                    error();
                    return rc;
                case SSH_AUTH_DENIED:  //no key matched
                    std::cout << "SSH_AUTH_METHOD_PASSWORD - SSH_AUTH_DENIED!" << std::endl;
                    break;
                case SSH_AUTH_SUCCESS: //you are now authenticated
                    std::cout << "SSH_AUTH_METHOD_PASSWORD - SSH_AUTH_SUCCESS!" << std::endl;
                    break;
                case SSH_AUTH_PARTIAL: //some key matched but you still have to
                                       // provide an other mean of authentication (like a password).
                    std::cout << "SSH_AUTH_METHOD_PASSWORD - SSH_AUTH_PARTIAL!" << std::endl;
                    break;
                default:
                    break;
            }
        }
    }
    banner = ssh_get_issue_banner(session);
    if(banner)
    {
        std::cout << banner << std::endl;
        ssh_string_free_char(banner);
    }
    return rc;
}
