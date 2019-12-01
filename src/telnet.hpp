#ifndef TELNET_HPP
#define TELNET_HPP

/*
 * Reference BSD Default Telnet Definitions.
 * With some Extra MUD Options added for support later on
 */

// maximum size of telnet sub-negotiation string, allowing for a fairly
// large value for NEW_ENVIRON.
enum
{
    SB_MAXLEN = 65534
};

/**
 * Telnet Commands
 */
enum
{
    IAC     = 255,        // interpret as command:
    DONT    = 254,        // you are not to use option
    DO      = 253,        // please, you use option
    WONT    = 252,        // I won't use option
    WILL    = 251,        // I will use option
    SB      = 250,        // interpret as sub-negotiation
    GA      = 249,        // you may reverse the line
    EL      = 248,        // erase the current line
    EC      = 247,        // erase the current character
    AYT     = 246,        // are you there
    AO      = 245,        // abort output--but let prog finish
    IP      = 244,        // interrupt process--permanently
    BREAK   = 243,        // break
    SYNCH   = 242,        // for telfunc calls
    DM      = 242,        // data mark--for connect. cleaning
    NOP     = 241,        // nop
    SE      = 240,        // end sub negotiation
    EOR     = 239,        // end of record (transparent mode)
    ABORT   = 238,        // Abort process
    SUSP    = 237,        // Suspend process
    xEOF    = 236         // End of file: EOF is already used...
};


#ifdef TELCMDS
const char *telcmds[] =
{
    "EOF", "SUSP", "ABORT", "EOR",
    "SE", "NOP", "DMARK", "BRK", "IP", "AO", "AYT", "EC",
    "EL", "GA", "SB", "WILL", "WONT", "DO", "DONT", "IAC", 0
};
#else
extern const char *telcmds[];
#endif

#define    TELCMD_FIRST  xEOF
#define    TELCMD_LAST   IAC
#define    TELCMD_OK(x)  ((unsigned int)(x) <= TELCMD_LAST && \
                          (unsigned int)(x) >= TELCMD_FIRST)

#define    TELCMD(x)      telcmds[(x)-TELCMD_FIRST]

/**
 * Telnet Options
 */
enum
{
    TELOPT_BINARY         =  0,    // 8-bit data path
    TELOPT_ECHO           =  1,    // echo
    TELOPT_RCP            =  2,    // prepare to reconnect
    TELOPT_SGA            =  3,    // suppress go ahead
    TELOPT_NAMS           =  4,    // approximate message size
    TELOPT_STATUS         =  5,    // give status
    TELOPT_TM             =  6,    // timing mark
    TELOPT_RCTE           =  7,    // remote controlled transmission and echo
    TELOPT_NAOL           =  8,    // negotiate about output line width
    TELOPT_NAOP           =  9,    // negotiate about output page size
    TELOPT_NAOCRD         = 10,    // negotiate about CR disposition
    TELOPT_NAOHTS         = 11,    // negotiate about horizontal tab-stops
    TELOPT_NAOHTD         = 12,    // negotiate about horizontal tab disposition
    TELOPT_NAOFFD         = 13,    // negotiate about form-feed disposition
    TELOPT_NAOVTS         = 14,    // negotiate about vertical tab stops
    TELOPT_NAOVTD         = 15,    // negotiate about vertical tab disposition
    TELOPT_NAOLFD         = 16,    // negotiate about output LF disposition
    TELOPT_XASCII         = 17,    // extended ASCII character set
    TELOPT_LOGOUT         = 18,    // force log out
    TELOPT_BM             = 19,    // byte macro
    TELOPT_DET            = 20,    // data entry terminal
    TELOPT_SUPDUP         = 21,    // supdup protocol
    TELOPT_SUPDUPOUTPUT   = 22,    // supdup output
    TELOPT_SNDLOC         = 23,    // send location
    TELOPT_TTYPE          = 24,    // terminal type
    TELOPT_EOR            = 25,    // end or record
    TELOPT_TUID           = 26,    // TACACS user identification
    TELOPT_OUTMRK         = 27,    // output marking
    TELOPT_TTYLOC         = 28,    // terminal location number
    TELOPT_3270REGIME     = 29,    // 3270 regime
    TELOPT_X3PAD          = 30,    // X.3 PAD
    TELOPT_NAWS           = 31,    // window size
    TELOPT_TSPEED         = 32,    // terminal speed
    TELOPT_LFLOW          = 33,    // remote flow control
    TELOPT_LINEMODE       = 34,    // Line-mode option
    TELOPT_XDISPLOC       = 35,    // X Display Location
    TELOPT_OLD_ENVIRON    = 36,    // Old - Environment variables
    TELOPT_AUTHENTICATION = 37,    // Authenticate
    TELOPT_ENCRYPT        = 38,    // Encryption option
    TELOPT_NEW_ENVIRON    = 39,    // New - Environment variables
    TELOPT_TN3270E        = 40,    // RFC2355 - TN3270 Enhancements
    TELOPT_CHARSET        = 42,    // RFC2066 - Charset
    TELOPT_COMPORT        = 44,    // RFC2217 - Com Port Control
    TELOPT_KERMIT         = 47,    // RFC2840 - Kermit
    // MUD Specific
    TELOPT_MSDP           = 69,   // used to send mud server data
    TELOPT_MSSP           = 70,   // used to send mud server information
    TELOPT_MCCP_1         = 85,   // MUD zlib stream compression Version 1
    TELOPT_MCCP_2         = 86,   // MUD zlib stream compression Version 2 RFC1950.
    TELOPT_MSP            = 90,   // used to toggle Mud Sound Protocol
    TELOPT_MXP            = 91,   // used to toggle Mud Extension Protocol
    TELOPT_EXOPL          = 255   // extended-options-list
};

#define TELOPT_FIRST    TELOPT_BINARY
#define TELOPT_LAST     TELOPT_KERMIT

#define TELOPT_OK(x)    ((unsigned int)(x) <= TELOPT_LAST)
#define TELOPT(x)       telopts[(x)-TELOPT_FIRST]

#define NTELOPTS        (1+TELOPT_LAST)

#ifdef TELOPTS
const char *telopts[NTELOPTS+1] =
{
    "BINARY", "ECHO", "RCP", "SUPPRESS GO AHEAD", "NAME",
    "STATUS", "TIMING MARK", "RCTE", "NAOL", "NAOP",
    "NAOCRD", "NAOHTS", "NAOHTD", "NAOFFD", "NAOVTS",
    "NAOVTD", "NAOLFD", "EXTEND ASCII", "LOGOUT", "BYTE MACRO",
    "DATA ENTRY TERMINAL", "SUPDUP", "SUPDUP OUTPUT",
    "SEND LOCATION", "TERMINAL TYPE", "END OF RECORD",
    "TACACS UID", "OUTPUT MARKING", "TTYLOC",
    "3270 REGIME", "X.3 PAD", "NAWS", "TSPEED", "LFLOW",
    "LINEMODE", "XDISPLOC", "OLD-ENVIRON", "AUTHENTICATION",
    "ENCRYPT", "NEW-ENVIRON", "TN3270E", "CHARSET", "COM-PORT",
    "KERMIT",
    0
};
#endif

// sub-option qualifiers
enum
{
    TELQUAL_IS         = 0,    // option is...
    TELQUAL_SEND       = 1,    // send option
    TELQUAL_INFO       = 2,    // ENVIRON: informational version of IS
    TELQUAL_REPLY      = 2,    // AUTHENTICATION: client version of IS
    TELQUAL_NAME       = 3     // AUTHENTICATION: client version of IS
};

enum
{
    LFLOW_OFF          = 0,    // Disable remote flow control
    LFLOW_ON           = 1,    // Enable remote flow control
    LFLOW_RESTART_ANY  = 2,    // Restart output on any char
    LFLOW_RESTART_XON  = 3     // Restart output only on XON
};

// LINE-MODE sub-options
enum
{
    LM_MODE          = 1,
    LM_FORWARDMASK   = 2,
    LM_SLC           = 3,
    MODE_EDIT        = 0x01,
    MODE_TRAPSIG     = 0x02,
    MODE_ACK         = 0x04,
    MODE_SOFT_TAB    = 0x08,
    MODE_LIT_ECHO    = 0x10,
    MODE_MASK        = 0x1f
};

// Not part of protocol, but needed to simplify things...
enum
{
    MODE_FLOW        = 0x0100,
    MODE_ECHO        = 0x0200,
    MODE_INBIN       = 0x0400,
    MODE_OUTBIN      = 0x0800,
    MODE_FORCE       = 0x1000
};

enum
{
    SLC_SYNCH   = 1,
    SLC_BRK     = 2,
    SLC_IP      = 3,
    SLC_AO      = 4,
    SLC_AYT     = 5,
    SLC_EOR     = 6,
    SLC_ABORT   = 7,
    SLC_EOF     = 8,
    SLC_SUSP    = 9,
    SLC_EC      = 10,
    SLC_EL      = 11,
    SLC_EW      = 12,
    SLC_RP      = 13,
    SLC_LNEXT   = 14,
    SLC_XON     = 15,
    SLC_XOFF    = 16,
    SLC_FORW1   = 17,
    SLC_FORW2   = 18,
    SLC_MCL     = 19,
    SLC_MCR     = 20,
    SLC_MCWL    = 21,
    SLC_MCWR    = 22,
    SLC_MCBOL   = 23,
    SLC_MCEOL   = 24,
    SLC_INSRT   = 25,
    SLC_OVER    = 26,
    SLC_ECR     = 27,
    SLC_EWR     = 28,
    SLC_EBOL    = 29,
    SLC_EEOL    = 30,
    NSLC        = 30
};

// For backwards compatibility, we define SLC_NAMES to be the
// list of names if SLC_NAMES is not defined.

#define    SLC_NAMELIST    "0", "SYNCH", "BRK", "IP", "AO", "AYT", "EOR",    \
    "ABORT", "EOF", "SUSP", "EC", "EL", "EW", "RP",    \
    "LNEXT", "XON", "XOFF", "FORW1", "FORW2",    \
    "MCL", "MCR", "MCWL", "MCWR", "MCBOL",        \
    "MCEOL", "INSRT", "OVER", "ECR", "EWR",        \
    "EBOL", "EEOL",                    \
    0

#ifdef    SLC_NAMES
char *slc_names[] =
{
    SLC_NAMELIST
};
#else
extern char *slc_names[];
#define    SLC_NAMES SLC_NAMELIST
#endif

#define    SLC_NAME_OK(x)    ((unsigned int)(x) <= NSLC)
#define    SLC_NAME(x)    slc_names[x]

enum
{
    SLC_NOSUPPORT    = 0,
    SLC_CANTCHANGE   = 1,
    SLC_VARIABLE     = 2,
    SLC_DEFAULT      = 3,
    SLC_LEVELBITS    = 0x03,

    SLC_FUNC         = 0,
    SLC_FLAGS        = 1,
    SLC_VALUE        = 2,

    SLC_ACK          = 0x80,
    SLC_FLUSHIN      = 0x40,
    SLC_FLUSHOUT     = 0x20
};

enum
{
    OLD_ENV_VAR      = 1,
    OLD_ENV_VALUE    = 0,
    NEW_ENV_VAR      = 0,
    NEW_ENV_VALUE    = 1,
    ENV_ESC          = 2,
    ENV_USERVAR      = 3
};

//AUTHENTICATION suboptions
enum
{
//Who is authenticating who ...
    AUTH_WHO_CLIENT         = 0,    // Client authenticating server
    AUTH_WHO_SERVER         = 1,    // Server authenticating client
    AUTH_WHO_MASK           = 1,

// amount of authentication done
    AUTH_HOW_ONE_WAY        = 0,
    AUTH_HOW_MUTUAL         = 2,
    AUTH_HOW_MASK           = 2,

// should we be encrypting? (not yet formally standardized)
    AUTH_ENCRYPT_OFF        = 0,
    AUTH_ENCRYPT_ON         = 4,
    AUTH_ENCRYPT_MASK       = 4,

    AUTHTYPE_NULL           = 0,
    AUTHTYPE_KERBEROS_V4    = 1,
    AUTHTYPE_KERBEROS_V5    = 2,
    AUTHTYPE_SPX            = 3,
    AUTHTYPE_MINK           = 4,
    AUTHTYPE_SRA            = 6,
    AUTHTYPE_CNT            = 7,

    AUTHTYPE_TEST           = 99,
};

#ifdef    AUTH_NAMES
const char *authtype_names[] =
{
    "NULL", "KERBEROS_V4", "KERBEROS_V5", "SPX", "MINK", nullptr, "SRA",
    0
};
#else
extern const char *authtype_names[];
#endif

#define    AUTHTYPE_NAME_OK(x)    ((unsigned int)(x) < AUTHTYPE_CNT)
#define    AUTHTYPE_NAME(x)    authtype_names[x]

// ENCRYPTION sub-options
enum
{
    ENCRYPT_IS          = 0,    // I pick encryption type ...
    ENCRYPT_SUPPORT     = 1,    // I support encryption types ...
    ENCRYPT_REPLY       = 2,    // Initial setup response
    ENCRYPT_START       = 3,    // Am starting to send encrypted
    ENCRYPT_END         = 4,    // Am ending encrypted
    ENCRYPT_REQSTART    = 5,    // Request you start encrypting
    ENCRYPT_REQEND      = 6,    // Request you send encrypting
    ENCRYPT_ENC_KEYID   = 7,
    ENCRYPT_DEC_KEYID   = 8,
    ENCRYPT_CNT         = 9,

    ENCTYPE_ANY         = 0,
    ENCTYPE_DES_CFB64   = 1,
    ENCTYPE_DES_OFB64   = 2,
    ENCTYPE_CNT         = 3
};
#ifdef    ENCRYPT_NAMES
const char *encrypt_names[] =
{
    "IS", "SUPPORT", "REPLY", "START", "END",
    "REQUEST-START", "REQUEST-END", "ENC-KEYID", "DEC-KEYID",
    0
};
const char *enctype_names[] =
{
    "ANY", "DES_CFB64",  "DES_OFB64",
    0
};
#else
extern const char *encrypt_names[];
extern const char *enctype_names[];
#endif

#define    ENCRYPT_NAME_OK(x)    ((unsigned int)(x) < ENCRYPT_CNT)
#define    ENCRYPT_NAME(x)        encrypt_names[x]

#define    ENCTYPE_NAME_OK(x)    ((unsigned int)(x) < ENCTYPE_CNT)
#define    ENCTYPE_NAME(x)        enctype_names[x]

#endif // TELNET_HPP
