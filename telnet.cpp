
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$


#include "telnetState.h"
#include "socket.h"


unsigned char Telnet::telnet_opt_ack(unsigned char cmd)
{
    switch(cmd) {
        case DO:    return WILL;
        case DONT:    return WONT;
        case WILL:    return DO;
        case WONT:    return DONT;
    }
    return 0;
}

unsigned char Telnet::telnet_opt_nak(unsigned char cmd)
{
    switch(cmd) {
        case DO:    return WONT;
        case DONT:    return WILL;
        case WILL:    return DONT;
        case WONT:    return DO;
    }
    return 0;
}


/**
 * Sends IAC Sequence reply for NAWS or Terminal Window Size 80x50
 */
int Telnet::telnet_build_NAWS_reply()
{
    unsigned char temp[50]={0};
    sprintf ((char *) temp, "%c%c%c%c%c%c%c%c%c",
        IAC, SB, TELOPT_NAWS, 0, 80, 0, NUM_LINES, IAC, SE); // 9

//    socket_send(temp, 9);
    return 0;
}

/**
 * Sends IAC Sequence reply for Terminal Type
 */
int Telnet::telnet_build_TTYPE_reply()
{
    unsigned char temp[50]={0};
    sprintf ((char *) temp, "%c%c%c%c%s%c%c",
        IAC, SB, TELOPT_TTYPE, TELQUAL_IS, "ansi", IAC, SE); // 10

//    socket_send(temp, 10);
    return 0;
}


/**
 * Sends IAC Sequence back to Users Client for Terminal Negoation.
 */
int Telnet::send_iac(Uint32 command, Uint32 option)
{
    unsigned char temp[50]={0};
    sprintf ((char *) temp, "%c%c%c", IAC, command, option);

//    socket_send(temp, 3);
    return 0;
}


/*
 * Parses Telnet Options negoations between client / severs
 * On which Features are supports and terminal inforamtion.
 */
unsigned char Telnet::telnet_process_char(unsigned char c)
{

    //ofstream myfile;
    //myfile.open ("example.txt", ios::app );
    //if (c==255) myfile << "\r\n" << (int)c << ",";
    //else myfile << (int)c << ",";
    //myfile.close();
    //printf("\r\n [***] %i \r\n, ",c);

    // TELOPT Pasrer
    switch (stage)
    {
        // Find IAC
        case 0:
            // Check first Char in Fresh Sequence
            if (c != IAC)
            {
                // Add character to output buffer.
                return c;
            }
            else
            {
                // Sequence Found with IAC, set to next Stage.
                printf("\r\n ========================================== \r\n");
                printf("\r\n Received #255 = IAC \r\n");
                stage++;
            }
            break;

        // Find Command
        case 1:
            if (c != IAC)
            {

                switch (c)
                {
                    // Catch Passthrough commands.
                    case GA:    //     249        /* you may reverse the line */
                    case EL:    //     248        /* erase the current line */
                    case EC:    //     247        /* erase the current character */
                    case AYT:   //     246        /* are you there */
                    case AO:    //     245        /* abort output--but let prog finish */
                    case IP:    //     244        /* interrupt process--permanently */
                    case BREAK: //   243        /* break */
                    case DM:    //   242        /* data mark--for connect. cleaning */
                    case NOP:   //      241        /* nop */
                    case SE:    //      240        /* end sub negotiation */
                    case EOR:   //   239        /* end of record (transparent mode) */
                    case ABORT: //   238        /* Abort process */
                    case SUSP:  //   237        /* Suspend process */
                    case xEOF:  //   236        /* End of file: EOF is already used... */

                        // Pass Through commands that don't need Response.
                        printf("\r\n [IAC][%d] 'PASSTHROUGH' \r\n", c);
                        stage = 0;
                        break;

                    default:

                        // Move to Comamnd Parsing.
                        printf("\r\n [IAC][%d] \r\n", c);
                        cmd = c;
                        stage++;
                        break;
                }

            }
            else
            {
                // Return state to normal Binary Mode was Sent.
                printf("\r\n Got double #255 !!\r\n");
                stage = 0;
            }
            break;

        // Find Option
        case 2:

            stage = 0;

            // CAtch if were getting Invalid Commands.
            if (TELCMD_OK(cmd))
                printf("\r\n [*****] [IAC][%i][%i] \r\n", cmd,c);
            else
            {
                // Hopefully won't get here!
                printf("\r\n [*** BAD ***] [IAC][%i][%i] \r\n", cmd,c);
                stage = 0;
                break;
            }

            switch (cmd)
            {
                // No responses needed, just stuff these.
                case DONT:

                    printf("\r\n [DONT telnet request received] \r\n");

                    switch (c)
                    {
                        case IAC :
                            stage = 1;
                            break;

                        default:
                            printf("\r\n [DONT - responsed WONT %i] \r\n",c);
                            send_iac(sock,telnet_opt_ack(cmd),c);
                            stage = 0;
                            break;
                    }
                    break;

                case DO: // Replies WILL / WON'T

                    printf("\r\n [DO telnet request received] \r\n");

                    switch (c)
                    {

                        case TELOPT_ECHO:
                            printf("\r\n [DO - TELOPT_ECHO responsed WONT] \r\n");
                            send_iac(sock,telnet_opt_nak(cmd),c);
                            break;

                        case TELOPT_BINARY:
                            printf("\r\n [DO - TELOPT_BINARY responsed WILL] \r\n");
                            send_iac(sock,telnet_opt_ack(cmd),c);
                            binary_mode = 1;
                            break;

                        case TELOPT_SGA:
                            printf("\r\n [DO - TELOPT_SGA responsed WILL] \r\n");
                            send_iac(sock,telnet_opt_ack(cmd),c);
                            did_SGA = 1;
                            break;

                        case TELOPT_TTYPE:
                            printf("\r\n [DO - TELOPT_TTYPE responsed WILL] \r\n");
                            send_iac(sock,telnet_opt_ack(cmd),c);
                            did_TERM = 1;
                            break;

                        case TELOPT_NAWS:
                            printf("\r\n [DO - TELOPT_NAWS responsed WILL] \r\n");
                            send_iac(sock,telnet_opt_ack(cmd),c);
                            telnet_build_NAWS_reply(sock);
                            did_NAWS = 1;
                            break;

                        /*
                        case IAC :
                            printf("\r\n [DO - INCORRECT IAC Received, resetting.] \r\n");
                            stage = 1;
                            return 255;
                        */

                        default:
                            printf("\r\n [DO - responsed WONT %i] \r\n",c);
                            send_iac(sock,telnet_opt_nak(cmd),c);
                            break;
                    }
                    stage = 0;
                    break;

                case WILL: // Replies DO And DONT

                    printf("\r\n [WILL telnet request received] \r\n");

                    // Don't response to WILL Requests.
                    switch (c)
                    {

                        case TELOPT_ECHO:
                            if (!did_ECHO)
                            {
                                printf("\r\n [WILL - TELOPT_ECHO responsed DO] \r\n");
                                send_iac(sock,telnet_opt_ack(cmd),c);
                                did_ECHO = 1;
                            }
                            break;

                        case TELOPT_BINARY :
                            if (!did_BIN)
                            {
                                printf("\r\n [WILL - TELOPT_BINARY responsed DO] \r\n");
                                send_iac(sock,telnet_opt_ack(cmd),c);
                                did_BIN = 1;
                            }
                            break;

                        case TELOPT_SGA :
                            if (!did_SGA)
                            {
                                printf("\r\n [WILL - TELOPT_SGA responsed DO] \r\n");
                                send_iac(sock,telnet_opt_ack(cmd),c);
                                did_SGA = 1;
                            }
                            break;
                        /*
                        case IAC :
                            stage = 1;
                            return 255;
                        */
                        default :
                            send_iac(sock,telnet_opt_nak(cmd),c);
                            printf("\r\n [WILL - responsed DONT %i] \r\n",c);
                            break;
                    }

                    stage = 0;
                    break;

                case WONT:

                    // Don't responset to WONT
                    printf("\r\n [WONT telnet request received] \r\n");
                    send_iac(sock,telnet_opt_ack(cmd),c);
                    printf("\r\n [WONT - responsed DONT %i] \r\n",c);

                    stage = 0;
                    break;

                // Start of Sub Negoations and Stages 3 - 4
                case SB: // 250

                    printf("\r\n [TELNET_STATE_SB ENTERED] \r\n");
                    if (c == TELOPT_TTYPE)
                    {
                        opt = c;
                        stage = 3;
                    }
                    else
                    {
                        printf("\r\n [TELNET_STATE_SB LEAVING] \r\n");
                        // Invalid, reset back.
                        stage = 0;
                    }
                    break;

                default:

                    // Options or Conmmands Not Pasrsed, RESET.
                    printf("\r\n [*****] DEFAULT CMD - %i / %i \r\n", cmd,c);
                    stage = 0;
                    break;
            }
            break;

        case 3:
            printf("\r\n [Stage 3] - %i, %i \r\n",opt, c);

            //Options will be 1 After SB
            //IAC SB TTYPE SEND IAC SE
            switch (opt)
            {
                case TELOPT_TTYPE:
                    if (c == TELQUAL_SEND) // SEND
                    {
                        printf("\r\n [Stage 3 - TELQUAL_SEND] goto Stage 4");
                        stage = 4;
                    }
                    else
                        stage = 0;
                    break;

                default:
                    //printf("\r\n [Stage 3 - unregistered suff it] - %i, %i \r\n",opt, c);
                    if (c == SE)
                    {
                        printf("\r\n [TELNET_STATE_SB SE] \r\n");
                        stage = 0;
                    }
                    else
                    {
                        // Reset
                        stage = 0;
                    }
                    break;
            }
            break;

        // Only Gets here on TTYPE Subnegoation.
        case 4:

            printf("\r\n [Stage 4] - %i \r\n",c);

            switch (c)
            {

                case IAC:
                    printf("\r\n [TELNET_STATE_SB IAC] \r\n");
                    break;

                case SE:
                    printf("\r\n [TELNET_STATE_SB SE] \r\n");

                    // Send TTYPE After End of Compelte Sequence is Registered.
                    telnet_build_TTYPE_reply(sock);

                    stage = 0;
                    break;

            }
            break;
    }
    return '\0';
}


/*
 * Handles Reading the Socket for Data, then Pasring the Data.
 */
void Telnet::handleSession()
{
    // Output file, testing to caputure ESC Sequences from Server.
    //ofstream myfile1;
    //myfile.open ("esc_codes.txt", ios::app );

    //ofstream myfile1;
    //myfile1.open ("esc_codes1.txt", ios::app );

    std::string ansiret;
    std::string output;

    int len = 0;
    //char message[16385]={'\0'};
    //char message[16385]={'\0'};
    char message[4097]={'\0'};
    std::string newstring;

    // Escape parsing
    static std::string esc_sequence;
    static char more_params = 0;
    unsigned char c, ch;

    int translateUTF8 = FALSE; // WIP!


    // Get Socket Data From Server
//    len = socket_recv(message);
    if ( len < 0)
    {

        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                         "Closed Connection",
                         "Server has shutdown the connection.",
                         NULL);

        if (!useSSH)
        {
            //SDLNet_TCP_Close(sock); // maybe remove.
            SDLNet_Quit();
        }

//        renderer->Close();
        //delete renderer;
        exit(1);
    }

    //myfile1 << message;
    //myfile1.close();

    // If char is utf8 then translate to cp437
    //if (translateUTF8)
    //{
    //    newstring = UTF8tocp437(message);
//    }


    // Loop through data for IAC Telnet Commands
    for (int i = 0; i < len; i++ )
    {

        if (translateUTF8)
            c = newstring[i];
        else
            c = message[i];

        try
        {
            ch = telnet_process_char(sock, c);
        }
        catch (exception& e)
        {
            cout << "Exception telnet_process_char: " << e.what() << '\n';
        }

        if (ch == '\0')
            continue;

        //Handle escape sequence Parsing, Make Sure we get Complete ESC Sequences
        // Before throwing data at the ansi pasrser.
        if (ch == '\x1b' || more_params == TRUE)
        {
            // Debug ESC Sequences.
            //if (ch==27) myfile << "\r\n" << (int)ch << "," << ch;
            //else myfile << (int)c << "," << ch;
            //if (ch==27) myfile << "\r\n" << ch;
            //else myfile << ch;

            more_params = TRUE;

            // Loop through Ending Sequence of
            // of Escape Code once we have this, then add the ESC sequence
            // to the DataBuffer
            switch(ch)
            {
            case CURSOR_POSITION:
            case CURSOR_POSITION_ALT:
            case CURSOR_UP:
            case CURSOR_DOWN:
            case CURSOR_FORWARD:
            case CURSOR_BACKWARD:
            case CURSOR_X_POSITION:
            case CURSOR_NEXT_LINE:
            case CURSOR_PREV_LIVE:
            case ERASE_DISPLAY:
            case ERASE_TO_EOL:
            case SAVE_CURSOR_POS:
            case RESTORE_CURSOR_POS:
            case SET_GRAPHICS_MODE:
            case SET_MODE:
            case RESET_MODE:
            case SET_KEYBOARD_STRINGS:
            case ANSI_DETECTION:
            case SCROLL_REGION:
                esc_sequence += ch;
                output += esc_sequence;

                //printf("\r\n ESC SEQUENCE - %s \r\n", esc_sequence.c_str());

                esc_sequence.erase();
                more_params = FALSE;
                break;
            default:
                // continue with the next character
                esc_sequence += ch;

                // Bad Sequence, push it though
                // So were not stuck in endless loop.
                if (esc_sequence.size() > 15)
                {
                    printf("\r\n BAD ESC SEQUENCE - %s \r\n", esc_sequence.c_str());
                    output += esc_sequence;
                    esc_sequence.erase();
                    more_params = FALSE;
                }
                break;
            }
        }
        else
        {
            //myfile << ch;
            //if (ch == 10 || ch == 13)
                //printf("\r\n ch - %i \r\n",ch);
            output += ch;
        }
    }


    // If available, push Data through ANSI Pasrser then Screen.
    if (output.size() > 0)
    {
        // Turn off Cursor Before rendering new screen.
        renderer->RenderCursorOffScreen(x_position-1, y_position-1);

        // Parse Ansi
        try
        {
            ansiparse(sock, renderer, (unsigned char *)output.c_str(), FALSE);
        }
        catch (exception& e)
        {
            cout << "Exception ansiparse: " << e.what() << '\n';
        }


        // Setup cursor in current x/y position Cursor.
        renderer->SetupCursorChar(x_position-1, y_position-1);

        // Look at cursor timing. If were getting input that we've typed
        // Back from server, then we want to display the cursor,
        // Otherwise for Ansi screens and animation we don't want to show it
        if (output.size() <= 2)
            renderer->RenderCursorOnScreen(x_position-1, y_position-1);

        // Write out final screen.
        renderer->DrawTextureScreen();
        output.erase();
    }

    //myfile.close();
    //myfile1.close();
}
