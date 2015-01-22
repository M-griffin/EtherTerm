/* Terminal Control Sequence Parser
 * (c) Michael Griffin <mrmisticismo@hotmail.com>
 * 12/27/2014
 * Runs through passed string data to parse text and control sequences
 * Handle the most basic terminal and a majority of Xterm Control Sequences.
 * Window Text and String sequences are not handeled.
 *
 * The job of this class is to seperate text and control sequences,
 * When control sequences are present they are parameterized into a vector
 * with the sequence terminator as the first parameter and following
 * paramerters incrimented to the next slot in the vector.
 *
 * ex.. ESC[0;21;30m would be broken up into:
 * [0] m
 * [1] 0
 * [2] 21
 * [3] 30
 *
 * Then passed to the Screen Parser for drawing to the screen.
 */

#include "sequenceParser.h"
#include "ansiParser.h"
#include "terminal.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

// Init the Global Instance
SequenceParser* SequenceParser::globalInstance = 0;

// Initalize Class Variables
SequenceParser::SequenceParser() :
    sequenceState(SEQ_NORMAL),
    sequence(0),
    parameter(0),
    foundSequence(false),
    foundParameters(false),
    invalidSequence(false),
    sequenceCompleted(false),
    sequenceLevel(0)
    { }

SequenceParser::~SequenceParser()
{
    std::vector<int>().swap(params); // Clear
    std::cout << "SequenceParser Released" << std::endl;
}

/*
 * Level 0 Parsing check for Start of CSI or Alternate ESC Sequecnes
 * Supported in Xterm that a server might send.
 * Switch Statement catches the Start of a Valid Sequence.
 */
void SequenceParser::processLevel0()
{
    switch (sequence)
    {
        case '[': // Control Sequence Introducer ( CSI is 0x9b).
            break;

        // Xterm Sequences Not implimented, pass through
        case ' ':
            //ESC SP F 7-bit controls (S7C1T).
            //ESC SP G 8-bit controls (S8C1T).
            //ESC SP L Set ANSI conformance level 1 (dpANS X3.134.1).
            //ESC SP M Set ANSI conformance level 2 (dpANS X3.134.1).
            //ESC SP N Set ANSI conformance level 3 (dpANS X3.134.1).
        case '#':
            //ESC # 3 DEC double-height line, top half (DECDHL).
            //ESC # 4 DEC double-height line, bottom half (DECDHL).
            //ESC # 5 DEC single-width line (DECSWL).
            //ESC # 6 DEC double-width line (DECDWL).
            //ESC # 8 DEC Screen Alignment Test (DECALN).
        case '%':
            //ESC % @ Select default character set. That is ISO 8859-1 (ISO 2022).
            //ESC % G Select UTF-8 character set (ISO 2022).
        case '(':
            //ESC ( C
            //C = U → CP437 Character Set. (Not shown on most! MF)
            //C = 0 → DEC Special Character and Line Drawing Set.
            //C = < → DEC Supplementary (VT200).
            //C = % 5 → DEC Supplementary Graphics (VT300).
            //C = > → DEC Technical (VT300).
            //C = A → United Kingdom (UK).
            //C = B → United States (USASCII).
            //C = 4 → Dutch.
            //C = C or 5 → Finnish.
            //C = R or f → French.
            //C = Q or 9 → French Canadian (VT200, VT300).
            //C = K → German.
            //C = Y → Italian.
            //C = ` , E or 6 → Norwegian/Danish.
            //C = % 6 → Portuguese (VT300).
            //C = Z → Spanish.
            //C = H or 7 → Swedish.
            //C = = → Swiss.
        case ')':
            //Designate G1 Character Set (ISO 2022, VT100).
            //The same character sets apply as for ESC ( C.
        case '?':
            // Set/Reset Modes
            // Invalid, should have CSI = ESC[ preceeding ?

        // Xterm C1 (8-Bit) Control Characters
        case 'D':  // Index ( IND is 0x84).
        case 'E':  // Next Line ( NEL is 0x85).
        case 'H':  // Tab Set ( HTS is 0x88).
        case 'M':  // Reverse Index ( RI is 0x8d).
        case 'N':  // Single Shift Select of G2 Character Set ( SS2 is 0x8e). This affects next character only.
        case 'O':  // Single Shift Select of G3 Character Set ( SS3 is 0x8f). This affects next character only.
        case 'P':  // Device Control String ( DCS is 0x90).
        case 'V':  // Start of Guarded Area ( SPA is 0x96).
        case 'W':  // End of Guarded Area ( EPA is 0x97).
        case 'X':  // Start of String ( SOS is 0x98).
        case 'Z':  // Return Terminal ID (DECID is 0x9a). Obsolete form of CSI c (DA).
        case ']':  // Operating System Command ( OSC is 0x9d).
        case '^':  // Privacy Message ( PM is 0x9e).
        case '_':  // Application Program Command ( APC is 0x9f).
        case '\\': // String Terminator ( ST is 0x9c).
        case '*':
            //ESC * C Designate G2 Character Set (ISO 2022, VT220).
            //The same character sets apply as for ESC ( C.
        case '+':
            //ESC + C Designate G3 Character Set (ISO 2022, VT220).
            //The same character sets apply as for ESC ( C.
        case '-':
            //ESC - C Designate G1 Character Set (VT300).
            //The same character sets apply as for ESC ( C.
        case '.':
            //ESC . C Designate G2 Character Set (VT300).
            //The same character sets apply as for ESC ( C.
        case '/':
            //ESC / C Designate G3 Character Set (VT300).
            //These work for 96-character sets only.
            //C = A → ISO Latin-1 Supplemental.
        case '6':  // ESC 6 Back Index (DECBI), VT420 and up.
        case '7':  // ESC 7 Save Cursor (DECSC).
        case '8':  // ESC 8 Restore Cursor (DECRC).
        case '9':  // ESC 9 Forward Index (DECFI), VT420 and up.
        case '=':  // ESC = Application Keypad (DECKPAM).
        case '>':  // ESC > Normal Keypad (DECKPNM).
        case 'F':  // ESC F Cursor to lower left corner of screen. This is enabled by the hpLowerleftBugCompat resource.
        case 'c':  // ESC c Full Reset (RIS).
        case 'l':  // ESC l Memory Lock (per HP terminals). Locks memory above the cursor.
        case 'm':  // ESC m Memory Unlock (per HP terminals).
        case 'n':  // ESC n Invoke the G2 Character Set as GL (LS2).
        case 'o':  // ESC o Invoke the G3 Character Set as GL (LS3).
        case '|':  // ESC | Invoke the G3 Character Set as GR (LS3R).
        case '}':  // ESC } Invoke the G2 Character Set as GR (LS2R).
        case '~':  // ESC ~ Invoke the G1 Character Set as GR (LS1R).
        case '\0':   // Catch any NULL characters after ESC
        case '\x1b': // catch any double ESC's from bad servers
            invalidSequence = true;
            break;

        default:
            // Nothing Matched, Shouldn't get here.
            invalidSequence = true;
#ifdef _DEBUG
            std::cout << "CSI parser exception: received ESC: "
                << sequence <<  std::endl;
#endif
            break;
    }
}

/*
 * Level 1 Parsing Comes After ESC[ = CSI.
 * Numbers and Seperators are found in the middle of sequences as Parameters
 * Switch Statement catch the end of a Valid Sequence.
 *
 * Any nonSupported sequences can have certain charactes after the CSI
 * and These are parsed so that they are skipped and marked Invalid.
 */
 void SequenceParser::processLevel1()
{
    // If we get here, only valid Sequences are ESC [ Then next Character.
    // First Check for Parameters in Sequence
    if (std::isdigit(sequence)) // Mark for Parameter
    {
        // Contine to next sequence
#ifdef _DEBUG
        std::cout << "Is a Number: " << sequence << std::endl;
#endif
        return;
    }
    else
    if (sequence == ';') // Mark for Multi-Parameter
    {
        // Continue to Next Sequence
#ifdef _DEBUG
        std::cout << "Is a Seperator: " << sequence << std::endl;
#endif
        return;
    }

    // Catch Valid ESC Sequence Terminators.
    switch (sequence)
    {
        case '@': // Insert P s (Blank) Character(s) (default = 1) (ICH).
        case 'A': // Cursor Up P s Times (default = 1) (CUU)
        case 'B': // Cursor Down P s Times (default = 1) (CUD)
        case 'C': // Cursor Forward P s Times (default = 1) (CUF)
        case 'D': // Cursor Backward P s Times (default = 1) (CUB)

        case 'E': // Cursor Next Line P s Times (default = 1) (CNL)
        case 'F': // Cursor Preceding Line P s Times (default = 1) (CPL)
        case 'G': // Cursor Character Absolute [column] (default = [row,1]) (CHA)
        case 'H': // Cursor Position [row;column] (default = [1,1]) (CUP)

        case 'I': // Cursor Forward Tabulation P s tab stops (default = 1) (CHT).
        case 'J':
            // Erase in Display (ED). - DECSED has ESC[?J
            //P s = 0 → Erase Below (default).
            //P s = 1 → Erase Above.
            //P s = 2 → Erase All.
            //P s = 3 → Erase Saved Lines (xterm).
        case 'K':
            // Erase in Line (EL). - DECSED has ESC[?K
            //P s = 0 → Erase to Right (default).
            //P s = 1 → Erase to Left.
            //P s = 2 → Erase All.
        case 'L': // Insert P s Line(s) (default = 1) (IL).
        case 'M': // Delete P s Line(s) (default = 1) (DL).
        case 'P': // Delete P s Character(s) (default = 1) (DCH).
        case 'S': // Scroll up P s lines (default = 1) (SU).
        case 'T': // Scroll down P s lines (default = 1) (SD).
        case 'X': // Erase P s Character(s) (default = 1) (ECH).
        case 'Z': // Cursor Backward Tabulation P s tab stops (default = 1) (CBT).

        case '`': // Character Position Absolute [column] (default = [row,1]) (HPA).
        case 'a': // Character Position Relative [columns] (default = [row,col+1]) (HPR).
        case 'b': // Repeat the preceding graphic character P s times (REP).
        case 'c':
            // Send Device Attributes (Primary DA).
            //P s = 0 or omitted → request attributes from terminal. The response depends on the decTerminalID resource setting.
            //→  CSI ? 1 ; 2 c (‘‘VT100 with Advanced Video Option’’)
            //→  CSI ? 1 ; 0 c (‘‘VT101 with No Options’’)
            //→  CSI ? 6 c (‘‘VT102’’)
            //→  CSI ? 6 2 ; P s c (‘‘VT220’’)
            //→  CSI ? 6 3 ; P s c (‘‘VT320’’)
            //→  CSI ? 6 4 ; P s c (‘‘VT420’’)
            // The VT100-style response parameters do not mean anything by themselves. VT220 (and higher) parameters do, telling the host what features the terminal supports:
            //P s = 1 → 132-columns.
            //P s = 2 → Printer.
            //P s = 3 → ReGIS graphics.
            //P s = 4 → Sixel graphics.
            //P s = 6 → Selective erase.
            //P s = 8 → User-defined keys.
            //P s = 9 → National Replacement Character sets.
            //P s = 1 5 → Technical characters.
            //P s = 1 8 → User windows.
            //P s = 2 1 → Horizontal scrolling.
            //P s = 2 2 → ANSI color, e.g., VT525.
            //P s = 2 9 → ANSI text locator (i.e., DEC Locator mode).
        case 'd': // Line Position Absolute [row] (default = [1,column]) (VPA).
        case 'e': // Line Position Relative [rows] (default = [row+1,column]) (VPR).
        case 'f': // equivalent to 'H'
        case 'g': // Tab Clear (TBC).
        case 'h':
            // Set Mode (SM).
            //P s = 2 → Keyboard Action Mode (AM).
            //P s = 4 → Insert Mode (IRM).
            //P s = 1 2 → Send/receive (SRM).
            //P s = 2 0 → Automatic Newline (LNM).

            // DEC Private Mode Set (DECSET). ESC [ ? h
            //P s = 1 → Application Cursor Keys (DECCKM).
            //P s = 2 → Designate USASCII for character sets G0-G3 (DECANM), and set VT100 mode.
            //P s = 3 → 132 Column Mode (DECCOLM).
            //P s = 4 → Smooth (Slow) Scroll (DECSCLM).
            //P s = 5 → Reverse Video (DECSCNM).
            //P s = 6 → Origin Mode (DECOM).
            //P s = 7 → Wraparound Mode (DECAWM).
            //P s = 8 → Auto-repeat Keys (DECARM).
            //P s = 9 → Send Mouse X & Y on button press. See the section Mouse Tracking. This is the X10 xterm mouse protocol.
            //P s = 1 0 → Show toolbar (rxvt).
            //P s = 1 2 → Start Blinking Cursor (att610).
            //P s = 1 8 → Print form feed (DECPFF).
            //P s = 1 9 → Set print extent to full screen (DECPEX).
            //P s = 2 5 → Show Cursor (DECTCEM).
            //P s = 3 0 → Show scrollbar (rxvt).
            //P s = 3 5 → Enable font-shifting functions (rxvt).
            //P s = 3 8 → Enter Tektronix Mode (DECTEK).
            //P s = 4 0 → Allow 80 → 132 Mode.
            //P s = 4 1 → more(1) fix (see curses resource).
            //P s = 4 2 → Enable National Replacement Character sets (DECNRCM).
            //P s = 4 4 → Turn On Margin Bell.
            //P s = 4 5 → Reverse-wraparound Mode.
            //P s = 4 6 → Start Logging. This is normally disabled by a compile-time option.
            //P s = 4 7 → Use Alternate Screen Buffer. (This may be disabled by the titeInhibit resource).
            //P s = 6 6 → Application keypad (DECNKM).
            //P s = 6 7 → Backarrow key sends backspace (DECBKM).
            //P s = 6 9 → Enable left and right margin mode (DECLRMM), VT420 and up.
            //P s = 9 5 → Do not clear screen when DECCOLM is set/reset (DECNCSM), VT510 and up.
            //P s = 1 0 0 0 → Send Mouse X & Y on button press and release. See the section Mouse Tracking. This is the X11 xterm mouse protocol.
            //P s = 1 0 0 1 → Use Hilite Mouse Tracking.
            //P s = 1 0 0 2 → Use Cell Motion Mouse Tracking.
            //P s = 1 0 0 3 → Use All Motion Mouse Tracking.
            //P s = 1 0 0 4 → Send FocusIn/FocusOut events.
            //P s = 1 0 0 5 → Enable UTF-8 Mouse Mode.
            //P s = 1 0 0 6 → Enable SGR Mouse Mode.
            //P s = 1 0 0 7 → Enable Alternate Scroll Mode.
            //P s = 1 0 1 0 → Scroll to bottom on tty output (rxvt).
            //P s = 1 0 1 1 → Scroll to bottom on key press (rxvt).
            //P s = 1 0 1 5 → Enable urxvt Mouse Mode.
            //P s = 1 0 3 4 → Interpret "meta" key, sets eighth bit. (enables the eightBitInput resource).
            //P s = 1 0 3 5 → Enable special modifiers for Alt and NumLock keys. (This enables the numLock resource).
            //P s = 1 0 3 6 → Send ESC when Meta modifies a key. (This enables the metaSendsEscape resource).
            //P s = 1 0 3 7 → Send DEL from the editing-keypad Delete key.
            //P s = 1 0 3 9 → Send ESC when Alt modifies a key. (This enables the altSendsEscape resource).
            //P s = 1 0 4 0 → Keep selection even if not highlighted. (This enables the keepSelection resource).
            //P s = 1 0 4 1 → Use the CLIPBOARD selection. (This enables the selectToClipboard resource).
            //P s = 1 0 4 2 → Enable Urgency window manager hint when Control-G is received. (This enables the bellIsUrgent resource).
            //P s = 1 0 4 3 → Enable raising of the window when Control-G is received. (enables the popOnBell resource).
            //P s = 1 0 4 7 → Use Alternate Screen Buffer. (This may be disabled by the titeInhibit resource).
            //P s = 1 0 4 8 → Save cursor as in DECSC. (This may be disabled by the titeInhibit resource).
            //P s = 1 0 4 9 → Save cursor as in DECSC and use Alternate Screen Buffer, clearing it first. (This may be disabled by the titeInhibit resource). This combines the effects of the 1 0 4 7 and 1 0 4 8 modes. Use this with terminfo-based applications rather than the 4 7 mode.
            //P s = 1 0 5 0 → Set terminfo/termcap function-key mode.
            //P s = 1 0 5 1 → Set Sun function-key mode.
            //P s = 1 0 5 2 → Set HP function-key mode.
            //P s = 1 0 5 3 → Set SCO function-key mode.
            //P s = 1 0 6 0 → Set legacy keyboard emulation (X11R6).
            //P s = 1 0 6 1 → Set VT220 keyboard emulation.
            //P s = 2 0 0 4 → Set bracketed paste mode.
        case 'i':
            // Media Copy (MC).  (DEC-specific) ESC [ ? i
            //P s = 0 → Print screen (default).
            //P s = 4 → Turn off printer controller mode.
            //P s = 5 → Turn on printer controller mode.

        case 'l':
            // Reset Mode (RM).
            //P s = 2 → Keyboard Action Mode (AM).
            //P s = 4 → Replace Mode (IRM).
            //P s = 1 2 → Send/receive (SRM).
            //P s = 2 0 → Normal Linefeed (LNM).

            // DEC Private Mode Reset (DECRST).
            //P s = 1 → Normal Cursor Keys (DECCKM).
            //P s = 2 → Designate VT52 mode (DECANM).
            //P s = 3 → 80 Column Mode (DECCOLM).
            //P s = 4 → Jump (Fast) Scroll (DECSCLM).
            //P s = 5 → Normal Video (DECSCNM).
            //P s = 6 → Normal Cursor Mode (DECOM).
            //P s = 7 → No Wraparound Mode (DECAWM).
            //P s = 8 → No Auto-repeat Keys (DECARM).
            //P s = 9 → Don’t send Mouse X & Y on button press.
            //P s = 1 0 → Hide toolbar (rxvt).
            //P s = 1 2 → Stop Blinking Cursor (att610).
            //P s = 1 8 → Don’t print form feed (DECPFF).
            //P s = 1 9 → Limit print to scrolling region (DECPEX).
            //P s = 2 5 → Hide Cursor (DECTCEM).
            //P s = 3 0 → Don’t show scrollbar (rxvt).
            //P s = 3 5 → Disable font-shifting functions (rxvt).
            //P s = 4 0 → Disallow 80 → 132 Mode.
            //P s = 4 1 → No more(1) fix (see curses resource).
            //P s = 4 2 → Disable National Replacement Character sets (DECNRCM).
            //P s = 4 4 → Turn Off Margin Bell.
            //P s = 4 5 → No Reverse-wraparound Mode.
            //P s = 4 6 → Stop Logging. (This is normally disabled by a compile-time option).
            //P s = 4 7 → Use Normal Screen Buffer.
            //P s = 6 6 → Numeric keypad (DECNKM).
            //P s = 6 7 → Backarrow key sends delete (DECBKM).
            //P s = 6 9 → Disable left and right margin mode (DECLRMM), VT420 and up.
            //P s = 9 5 → Clear screen when DECCOLM is set/reset (DECNCSM), VT510 and up.
            //P s = 1 0 0 0 → Don’t send Mouse X & Y on button press and release. See the section Mouse Tracking.
            //P s = 1 0 0 1 → Don’t use Hilite Mouse Tracking.
            //P s = 1 0 0 2 → Don’t use Cell Motion Mouse Tracking.
            //P s = 1 0 0 3 → Don’t use All Motion Mouse Tracking.
            //P s = 1 0 0 4 → Don’t send FocusIn/FocusOut events.
            //P s = 1 0 0 5 → Disable UTF-8 Mouse Mode.
            //P s = 1 0 0 6 → Disable SGR Mouse Mode.
            //P s = 1 0 0 7 → Disable Alternate Scroll Mode.
            //P s = 1 0 1 0 → Don’t scroll to bottom on tty output (rxvt).
            //P s = 1 0 1 1 → Don’t scroll to bottom on key press (rxvt).
            //P s = 1 0 1 5 → Disable urxvt Mouse Mode.
            //P s = 1 0 3 4 → Don’t interpret "meta" key. (This disables the eightBitInput resource).
            //P s = 1 0 3 5 → Disable special modifiers for Alt and NumLock keys. (This disables the numLock resource).
            //P s = 1 0 3 6 → Don’t send ESC when Meta modifies a key. (This disables the metaSendsEscape resource).
            //P s = 1 0 3 7 → Send VT220 Remove from the editing-keypad Delete key.
            //P s = 1 0 3 9 → Don’t send ESC when Alt modifies a key. (This disables the altSendsEscape resource).
            //P s = 1 0 4 0 → Do not keep selection when not highlighted. (This disables the keepSelection resource).
            //P s = 1 0 4 1 → Use the PRIMARY selection. (This disables the selectToClipboard resource).
            //P s = 1 0 4 2 → Disable Urgency window manager hint when Control-G is received. (This disables the bellIsUrgent resource).
            //P s = 1 0 4 3 → Disable raising of the window when Control-G is received. (This disables the popOnBell resource).
            //P s = 1 0 4 7 → Use Normal Screen Buffer, clearing screen first if in the Alternate Screen. (This may be disabled by the titeInhibit resource).
            //P s = 1 0 4 8 → Restore cursor as in DECRC. (This may be disabled by the titeInhibit resource).
            //P s = 1 0 4 9 → Use Normal Screen Buffer and restore cursor as in DECRC. (This may be disabled by the titeInhibit resource). This combines the effects of the 1 0 4 7 and 1 0 4 8 modes. Use this with terminfo-based applications rather than the 4 7 mode.
            //P s = 1 0 5 0 → Reset terminfo/termcap function-key mode.
            //P s = 1 0 5 1 → Reset Sun function-key mode.
            //P s = 1 0 5 2 → Reset HP function-key mode.
            //P s = 1 0 5 3 → Reset SCO function-key mode.
            //P s = 1 0 6 0 → Reset legacy keyboard emulation (X11R6).
            //P s = 1 0 6 1 → Reset keyboard emulation to Sun/PC style.
            //P s = 2 0 0 4 → Reset bracketed paste mode.
        case 'm':
            // Character Attributes (SGR).

            //P s = 0 → Normal (default).
            //P s = 1 → Bold.
            //P s = 2 → Faint, decreased intensity (ISO 6429).
            //P s = 3 → Italicized (ISO 6429).
            //P s = 4 → Underlined.
            //P s = 5 → Blink (appears as Bold).
            //P s = 7 → Inverse.
            //P s = 8 → Invisible, i.e., hidden (VT300).
            //P s = 9 → Crossed-out characters (ISO 6429).
            //P s = 2 1 → Doubly-underlined (ISO 6429).
            //P s = 2 2 → Normal (neither bold nor faint).
            //P s = 2 3 → Not italicized (ISO 6429).
            //P s = 2 4 → Not underlined.
            //P s = 2 5 → Steady (not blinking).
            //P s = 2 7 → Positive (not inverse).
            //P s = 2 8 → Visible, i.e., not hidden (VT300).
            //P s = 2 9 → Not crossed-out (ISO 6429).
            //P s = 3 0 → Set foreground color to Black.
            //P s = 3 1 → Set foreground color to Red.
            //P s = 3 2 → Set foreground color to Green.
            //P s = 3 3 → Set foreground color to Yellow.
            //P s = 3 4 → Set foreground color to Blue.
            //P s = 3 5 → Set foreground color to Magenta.
            //P s = 3 6 → Set foreground color to Cyan.
            //P s = 3 7 → Set foreground color to White.
            //P s = 3 9 → Set foreground color to default (original).
            //P s = 4 0 → Set background color to Black.
            //P s = 4 1 → Set background color to Red.
            //P s = 4 2 → Set background color to Green.
            //P s = 4 3 → Set background color to Yellow.
            //P s = 4 4 → Set background color to Blue.
            //P s = 4 5 → Set background color to Magenta.
            //P s = 4 6 → Set background color to Cyan.
            //P s = 4 7 → Set background color to White.
            //P s = 4 9 → Set background color to default (original).

            // If 16-color support is compiled, the following apply. Assume that xterm’s resources are set so that the ISO color codes are the first 8 of a set of 16.
            // Then the aixterm colors are the bright versions of the ISO colors:
            //P s = 9 0 → Set foreground color to Black.
            //P s = 9 1 → Set foreground color to Red.
            //P s = 9 2 → Set foreground color to Green.
            //P s = 9 3 → Set foreground color to Yellow.
            //P s = 9 4 → Set foreground color to Blue.
            //P s = 9 5 → Set foreground color to Magenta.
            //P s = 9 6 → Set foreground color to Cyan.
            //P s = 9 7 → Set foreground color to White.
            //P s = 1 0 0 → Set background color to Black.
            //P s = 1 0 1 → Set background color to Red.
            //P s = 1 0 2 → Set background color to Green.
            //P s = 1 0 3 → Set background color to Yellow.
            //P s = 1 0 4 → Set background color to Blue.
            //P s = 1 0 5 → Set background color to Magenta.
            //P s = 1 0 6 → Set background color to Cyan.
            //P s = 1 0 7 → Set background color to White.

            // If xterm is compiled with the 16-color support disabled, it supports the following, from rxvt:
            //P s = 1 0 0 → Set foreground and background color to default.
            // Xterm maintains a color palette whose entries are identified by an index beginning with zero. If 88- or 256-color support is compiled, the following apply:

            //All parameters are decimal integers.
            //RGB values range from zero (0) to 255.
            //ISO-8613-3 can be interpreted in more than one way; xterm allows the semicolons in this control to be replaced by colons (but after the first colon, colons must be used).

            // These ISO-8613-3 controls are supported:
            //P s = 3 8 ; 2 ; P r ; P g ; P b → Set foreground color to the closest match in xterm’s palette for the given RGB P r /P g /P b .
            //P s = 3 8 ; 5 ; P s → Set foreground color to the second P s .
            //P s = 4 8 ; 2 ; P r ; P g ; P b → Set background color to the closest match in xterm’s palette for the given RGB P r /P g /P b .
            //P s = 4 8 ; 5 ; P s → Set background color to the second P s .

        case 'n':
            // Device Status Report (DSR).
            //P s = 5 → Status Report. Result (‘‘OK’’) is
            //CSI 0 n
            //P s = 6 → Report Cursor Position (CPR) [row;column]. Result is
            //CSI r ; c R

            // Note: it is possible for this sequence to be sent by a function
            // key. For example, with the default keyboard configuration
            // the shifted F1 key may send (with shift-, control-, alt-modifiers)
            //CSI 1 ; 2 R , or
            //CSI 1 ; 5 R , or
            //CSI 1 ; 6 R , etc.

        case 'p':
        case 'q':
            // Load LEDs (DECLL).
            //P s = 0 → Clear all LEDS (default).
            //P s = 1 → Light Num Lock.
            //P s = 2 → Light Caps Lock.
            //P s = 3 → Light Scroll Lock.
            //P s = 2 1 → Extinguish Num Lock.
            //P s = 2 2 → Extinguish Caps Lock.
            //P s = 2 3 → Extinguish Scroll Lock.

        case 'r':  // Set Scrolling Region [top;bottom] (default = full size of window) (DECSTBM).
        case 's':  // Save cursor position (ANSI.SYS), available only when DECLRMM is disabled.
        case 't':  // Window manipulation (from dtterm, as well as extensions).

        case 'u': // Restore cursor position (ANSI.SYS).
        case 'x':
            // Request Terminal Parameters (DECREQTPARM).
            // if P s is a "0" (default) or "1", and xterm is emulating VT100,
            // the control sequence elicits a response of the same form whose
            // parameters describe the terminal:
            //P s → the given P s incremented by 2.
            //P n = 1 ← no parity.
            //P n = 1 ← eight bits.
            //P n = 1 ← 2 8 transmit 38.4k baud.
            //P n = 1 ← 2 8 receive 38.4k baud.
            //P n = 1 ← clock multiplier.
            //P n = 0 ← STP flags.
        case '!':  // Soft terminal reset (DECSTR).
            // Valid Sequences Ended.
            sequenceCompleted = true;
            break;

            // Unsupported Text and Keyboard Modifiers.
            // These are Preceeding Modifiers ie after ESC [ >
        case '>':
        case '$':
        case '"':
        case '*':
            invalidSequence = true;
            break;

        case '?': // -- Switch to next sequenceLevel
                  // Ie.. handle ESC[?7h, ESC[?7l, and other ? DEC Sequences.
                  // These Sequences DEC Level Sequecnes and need extra Parsing.
        case ' ': // Need to Precheck SyncTerm Font switching ' D' Space D.
            ++sequenceLevel;
            break;
        default:
            // Nothing Matched, Shouldn't get here.
            invalidSequence = true;
#ifdef _DEBUG
            std::cout << "CSI parser exception: received ESC: "
                << sequence <<  std::endl;
#endif
            break;
    }
}

/*
 * Level 2 Parsing Catches (2) Different Sequence Styles and Comes After ESC[ = CSI.
 * Specifically for ? preceeding sequenceing, and ' ' Space D ending Sequence
 * For syncterm font switching.
 *
 * Numbers and Seperators are found in the middle of sequences as Parameters
 * Switch Statement catch the end of a Valid Sequence.
 *
 * Any nonSupported sequences can have certain charactes after the CSI
 * and These are parsed so that they are skipped and marked Invalid.
 */
void SequenceParser::processLevel2()
{
    // If the last addition to seqeunce is a space and we are now here.
    // The only valid terminator = 'D' for Sycnterm Font Switching.
#ifdef _DEBUG
    std::cout << "bad sequence check: '" << sequenceBuilder[sequenceBuilder.size()-2] << "' " << sequence << std::endl;
#endif
    if (sequenceBuilder[sequenceBuilder.size()-2] == ' ' && sequence != 'D')
    {
#ifdef _DEBUG
    std::cout << "Invalid space found in sequence!" << std::endl;
#endif
        invalidSequence = true;
        return;
    }

    // If we get here, only valid Sequences are ESC [ Then next Character.
    // First Check for Parameters in Sequence
    if (std::isdigit(sequence)) // Mark for Parameter
    {
        // Contine to next sequence
#ifdef _DEBUG
        std::cout << "Is a Number: " << sequence << std::endl;
#endif
        return;
    }
    else
    if (sequence == ';') // Mark for Multi-Parameter
    {
        // Continue to Next Sequence
#ifdef _DEBUG
        std::cout << "Is a Seperator: " << sequence << std::endl;
#endif
        return;
    }

    // Catch Valid ESC Sequence Terminators.
    switch (sequence)
    {
        case 'D':
            // SyncTerm Font Switching Sequences
            // Syncterm Sequences have a Space before the D,
            // If a space is found, the sequence is passed from the previous level.
            // ESC [0;0 D

        // First we are catching DEC Style ESC[? Parameters here.
        case 'J':
            // Erase in Display (ED). - DECSED has ESC[?J
            //P s = 0 → Erase Below (default).
            //P s = 1 → Erase Above.
            //P s = 2 → Erase All.
        case 'K':
            // Erase in Line (EL). - DECSED has ESC[?K
            //P s = 0 → Erase to Right (default).
            //P s = 1 → Erase to Left.
            //P s = 2 → Erase All.
        case 'h':
            // DEC Private Mode Set (DECSET). ESC [ ? h
            //P s = 1 → Application Cursor Keys (DECCKM).
            //P s = 2 → Designate USASCII for character sets G0-G3 (DECANM), and set VT100 mode.
            //P s = 3 → 132 Column Mode (DECCOLM).
            //P s = 4 → Smooth (Slow) Scroll (DECSCLM).
            //P s = 5 → Reverse Video (DECSCNM).
            //P s = 6 → Origin Mode (DECOM).
            //P s = 7 → Wraparound Mode (DECAWM).
            //P s = 8 → Auto-repeat Keys (DECARM).
            //P s = 9 → Send Mouse X & Y on button press. See the section Mouse Tracking. This is the X10 xterm mouse protocol.
            //P s = 1 0 → Show toolbar (rxvt).
            //P s = 1 2 → Start Blinking Cursor (att610).
            //P s = 1 8 → Print form feed (DECPFF).
            //P s = 1 9 → Set print extent to full screen (DECPEX).
            //P s = 2 5 → Show Cursor (DECTCEM).
            //P s = 3 0 → Show scrollbar (rxvt).
            //P s = 3 5 → Enable font-shifting functions (rxvt).
            //P s = 3 8 → Enter Tektronix Mode (DECTEK).
            //P s = 4 0 → Allow 80 → 132 Mode.
            //P s = 4 1 → more(1) fix (see curses resource).
            //P s = 4 2 → Enable National Replacement Character sets (DECNRCM).
            //P s = 4 4 → Turn On Margin Bell.
            //P s = 4 5 → Reverse-wraparound Mode.
            //P s = 4 6 → Start Logging. This is normally disabled by a compile-time option.
            //P s = 4 7 → Use Alternate Screen Buffer. (This may be disabled by the titeInhibit resource).
            //P s = 6 6 → Application keypad (DECNKM).
            //P s = 6 7 → Backarrow key sends backspace (DECBKM).
            //P s = 6 9 → Enable left and right margin mode (DECLRMM), VT420 and up.
            //P s = 9 5 → Do not clear screen when DECCOLM is set/reset (DECNCSM), VT510 and up.
            //P s = 1 0 0 0 → Send Mouse X & Y on button press and release. See the section Mouse Tracking. This is the X11 xterm mouse protocol.
            //P s = 1 0 0 1 → Use Hilite Mouse Tracking.
            //P s = 1 0 0 2 → Use Cell Motion Mouse Tracking.
            //P s = 1 0 0 3 → Use All Motion Mouse Tracking.
            //P s = 1 0 0 4 → Send FocusIn/FocusOut events.
            //P s = 1 0 0 5 → Enable UTF-8 Mouse Mode.
            //P s = 1 0 0 6 → Enable SGR Mouse Mode.
            //P s = 1 0 0 7 → Enable Alternate Scroll Mode.
            //P s = 1 0 1 0 → Scroll to bottom on tty output (rxvt).
            //P s = 1 0 1 1 → Scroll to bottom on key press (rxvt).
            //P s = 1 0 1 5 → Enable urxvt Mouse Mode.
            //P s = 1 0 3 4 → Interpret "meta" key, sets eighth bit. (enables the eightBitInput resource).
            //P s = 1 0 3 5 → Enable special modifiers for Alt and NumLock keys. (This enables the numLock resource).
            //P s = 1 0 3 6 → Send ESC when Meta modifies a key. (This enables the metaSendsEscape resource).
            //P s = 1 0 3 7 → Send DEL from the editing-keypad Delete key.
            //P s = 1 0 3 9 → Send ESC when Alt modifies a key. (This enables the altSendsEscape resource).
            //P s = 1 0 4 0 → Keep selection even if not highlighted. (This enables the keepSelection resource).
            //P s = 1 0 4 1 → Use the CLIPBOARD selection. (This enables the selectToClipboard resource).
            //P s = 1 0 4 2 → Enable Urgency window manager hint when Control-G is received. (This enables the bellIsUrgent resource).
            //P s = 1 0 4 3 → Enable raising of the window when Control-G is received. (enables the popOnBell resource).
            //P s = 1 0 4 7 → Use Alternate Screen Buffer. (This may be disabled by the titeInhibit resource).
            //P s = 1 0 4 8 → Save cursor as in DECSC. (This may be disabled by the titeInhibit resource).
            //P s = 1 0 4 9 → Save cursor as in DECSC and use Alternate Screen Buffer, clearing it first. (This may be disabled by the titeInhibit resource). This combines the effects of the 1 0 4 7 and 1 0 4 8 modes. Use this with terminfo-based applications rather than the 4 7 mode.
            //P s = 1 0 5 0 → Set terminfo/termcap function-key mode.
            //P s = 1 0 5 1 → Set Sun function-key mode.
            //P s = 1 0 5 2 → Set HP function-key mode.
            //P s = 1 0 5 3 → Set SCO function-key mode.
            //P s = 1 0 6 0 → Set legacy keyboard emulation (X11R6).
            //P s = 1 0 6 1 → Set VT220 keyboard emulation.
            //P s = 2 0 0 4 → Set bracketed paste mode.
        case 'i':
            // Media Copy (MC, DEC-specific).
            //P s = 1 → Print line containing cursor.
            //P s = 4 → Turn off autoprint mode.
            //P s = 5 → Turn on autoprint mode.
            //P s = 1 0 → Print composed display, ignores DECPEX.
            //P s = 1 1 → Print all pages.

        case 'l':
            // DEC Private Mode Reset (DECRST).
            //P s = 1 → Normal Cursor Keys (DECCKM).
            //P s = 2 → Designate VT52 mode (DECANM).
            //P s = 3 → 80 Column Mode (DECCOLM).
            //P s = 4 → Jump (Fast) Scroll (DECSCLM).
            //P s = 5 → Normal Video (DECSCNM).
            //P s = 6 → Normal Cursor Mode (DECOM).
            //P s = 7 → No Wraparound Mode (DECAWM).
            //P s = 8 → No Auto-repeat Keys (DECARM).
            //P s = 9 → Don’t send Mouse X & Y on button press.
            //P s = 1 0 → Hide toolbar (rxvt).
            //P s = 1 2 → Stop Blinking Cursor (att610).
            //P s = 1 8 → Don’t print form feed (DECPFF).
            //P s = 1 9 → Limit print to scrolling region (DECPEX).
            //P s = 2 5 → Hide Cursor (DECTCEM).
            //P s = 3 0 → Don’t show scrollbar (rxvt).
            //P s = 3 5 → Disable font-shifting functions (rxvt).
            //P s = 4 0 → Disallow 80 → 132 Mode.
            //P s = 4 1 → No more(1) fix (see curses resource).
            //P s = 4 2 → Disable National Replacement Character sets (DECNRCM).
            //P s = 4 4 → Turn Off Margin Bell.
            //P s = 4 5 → No Reverse-wraparound Mode.
            //P s = 4 6 → Stop Logging. (This is normally disabled by a compile-time option).
            //P s = 4 7 → Use Normal Screen Buffer.
            //P s = 6 6 → Numeric keypad (DECNKM).
            //P s = 6 7 → Backarrow key sends delete (DECBKM).
            //P s = 6 9 → Disable left and right margin mode (DECLRMM), VT420 and up.
            //P s = 9 5 → Clear screen when DECCOLM is set/reset (DECNCSM), VT510 and up.
            //P s = 1 0 0 0 → Don’t send Mouse X & Y on button press and release. See the section Mouse Tracking.
            //P s = 1 0 0 1 → Don’t use Hilite Mouse Tracking.
            //P s = 1 0 0 2 → Don’t use Cell Motion Mouse Tracking.
            //P s = 1 0 0 3 → Don’t use All Motion Mouse Tracking.
            //P s = 1 0 0 4 → Don’t send FocusIn/FocusOut events.
            //P s = 1 0 0 5 → Disable UTF-8 Mouse Mode.
            //P s = 1 0 0 6 → Disable SGR Mouse Mode.
            //P s = 1 0 0 7 → Disable Alternate Scroll Mode.
            //P s = 1 0 1 0 → Don’t scroll to bottom on tty output (rxvt).
            //P s = 1 0 1 1 → Don’t scroll to bottom on key press (rxvt).
            //P s = 1 0 1 5 → Disable urxvt Mouse Mode.
            //P s = 1 0 3 4 → Don’t interpret "meta" key. (This disables the eightBitInput resource).
            //P s = 1 0 3 5 → Disable special modifiers for Alt and NumLock keys. (This disables the numLock resource).
            //P s = 1 0 3 6 → Don’t send ESC when Meta modifies a key. (This disables the metaSendsEscape resource).
            //P s = 1 0 3 7 → Send VT220 Remove from the editing-keypad Delete key.
            //P s = 1 0 3 9 → Don’t send ESC when Alt modifies a key. (This disables the altSendsEscape resource).
            //P s = 1 0 4 0 → Do not keep selection when not highlighted. (This disables the keepSelection resource).
            //P s = 1 0 4 1 → Use the PRIMARY selection. (This disables the selectToClipboard resource).
            //P s = 1 0 4 2 → Disable Urgency window manager hint when Control-G is received. (This disables the bellIsUrgent resource).
            //P s = 1 0 4 3 → Disable raising of the window when Control-G is received. (This disables the popOnBell resource).
            //P s = 1 0 4 7 → Use Normal Screen Buffer, clearing screen first if in the Alternate Screen. (This may be disabled by the titeInhibit resource).
            //P s = 1 0 4 8 → Restore cursor as in DECRC. (This may be disabled by the titeInhibit resource).
            //P s = 1 0 4 9 → Use Normal Screen Buffer and restore cursor as in DECRC. (This may be disabled by the titeInhibit resource). This combines the effects of the 1 0 4 7 and 1 0 4 8 modes. Use this with terminfo-based applications rather than the 4 7 mode.
            //P s = 1 0 5 0 → Reset terminfo/termcap function-key mode.
            //P s = 1 0 5 1 → Reset Sun function-key mode.
            //P s = 1 0 5 2 → Reset HP function-key mode.
            //P s = 1 0 5 3 → Reset SCO function-key mode.
            //P s = 1 0 6 0 → Reset legacy keyboard emulation (X11R6).
            //P s = 1 0 6 1 → Reset keyboard emulation to Sun/PC style.
            //P s = 2 0 0 4 → Reset bracketed paste mode.

        case 'n':
            // Device Status Report (DSR, DEC-specific).
            //P s = 6 → Report Cursor Position (DECXCPR) [row;column] as CSI ? r ; c R (assumes the default page, i.e., "1").
            //P s = 1 5 → Report Printer status as CSI ? 1 0 n (ready). or CSI ? 1 1 n (not ready).
            //P s = 2 5 → Report UDK status as CSI ? 2 0 n (unlocked) or CSI ? 2 1 n (locked).
            //P s = 2 6 → Report Keyboard status as
            //CSI ? 2 7 ; 1 ; 0 ; 0 n (North American).
            //The last two parameters apply to VT400 & up, and denote keyboard ready and LK01 respectively.
            //P s = 5 3 → Report Locator status as CSI ? 5 3 n Locator available, if compiled-in, or CSI ? 5 0 n No Locator, if not.
            //P s = 5 5 → Report Locator status as CSI ? 5 3 n Locator available, if compiled-in, or CSI ? 5 0 n No Locator, if not.
            //P s = 5 6 → Report Locator type as CSI ? 5 7 ; 1 n Mouse, if compiled-in, or CSI ? 5 7 ; 0 n Cannot identify, if not.
            //P s = 6 2 → Report macro space (DECMSR) as CSI P n \* {
            //P s = 6 3 → Report memory checksum (DECCKSR) as DCS P t ! x x x x ST
            //P t is the request id (from an optional parameter to the request).
            //The x’s are hexadecimal digits 0-9 and A-F.
            //P s = 7 5 → Report data integrity as CSI ? 7 0 n (ready, no errors)
            //P s = 8 5 → Report multi-session configuration as CSI ? 8 3 n (not configured for multiple-session operation).

        case 'p':
        case 'q':
            // Load LEDs (DECLL).
            //P s = 0 → Clear all LEDS (default).
            //P s = 1 → Light Num Lock.
            //P s = 2 → Light Caps Lock.
            //P s = 3 → Light Scroll Lock.
            //P s = 2 1 → Extinguish Num Lock.
            //P s = 2 2 → Extinguish Caps Lock.
            //P s = 2 3 → Extinguish Scroll Lock.

        case 'r':  // Restore DEC Private Mode Values. The value of P s previously saved is restored. P s values are the same as for DECSET.
        case 's':  // Save DEC Private Mode Values. P s values are the same as for DECSET.
        case '!':  // Soft terminal reset (DECSTR).
            // Valid Sequences Ended.
            sequenceCompleted = true;
            break;

            // Unsupported Text and Keyboard Modifiers.
            // These are Preceeding Modifiers ie after ESC [ >
        case '>':
        case '$':
        case '"':
        case '*':
        case ' ':
            invalidSequence = true;
            break;

        default:
            // Nothing Matched, Shouldn't get here.
            invalidSequence = true;
#ifdef _DEBUG
            std::cout << "CSI parser exception: received ESC: "
                << sequence << std::endl;
#endif
            break;
    }
}

/*
 * Validate Complete ESC Control Sequences
 * Break up the parameters into a Vector
 * For easier pre-parsing of Screen Display.
 */
void SequenceParser::validateSequence()
{
#ifdef _DEBUG
    std::cout << "Validate Sequence: " << sequenceBuilder << std::endl;
#endif
    int sequenceTerminator = 0;

    // Check and clear vector for fresh sequence
    // We only Validate on complete sequences, so we can clear here
    // to make sure each run.
    if (params.size() > 0)
        std::vector<int>().swap(params);

    // Handle SyncTerm Font Switching sequences here.
    // Makes it quicker to no go through extra parsing.
    // Default CP437 VGA8x16

    if (sequenceBuilder == "\x1b[0;0 D")
    {
        std::cout << "switch to CP437 Font" << std::endl;
        TheTerminal::Instance()->setCurrentFont("vga8x16.bmp");
        if (TheTerminal::Instance()->didFontChange())
            TheTerminal::Instance()->loadBitmapImage(
                TheTerminal::Instance()->getCurrentFont());

        //std::cout << "sequenceBuilder.erase();" << std::endl;
    //    sequenceBuilder.erase();
    //    sequenceState = SEQ_NORMAL; // Reset to passthrough
    //    return;
    }
    //37 - P0T NOoDLE (Amiga)
    else
    if (sequenceBuilder == "\x1b[0;37 D")
    {
        std::cout << "switch to Pot-Noodle Font" << std::endl;
        TheTerminal::Instance()->setCurrentFont("potNoodle-8x16.bmp");
        if (TheTerminal::Instance()->didFontChange())
            TheTerminal::Instance()->loadBitmapImage(
                TheTerminal::Instance()->getCurrentFont());
    //    sequenceBuilder.erase();
    //    sequenceState = SEQ_NORMAL; // Reset to passthrough
    //    return;
    }
    //38 - mO'sOul (Amiga)
    else
    if (sequenceBuilder == "\x1b[0;38 D")
    {
        std::cout << "switch to mO'sOul Font" << std::endl;
        TheTerminal::Instance()->setCurrentFont("mo'soul-8x16.bmp");
        if (TheTerminal::Instance()->didFontChange())
            TheTerminal::Instance()->loadBitmapImage(
                TheTerminal::Instance()->getCurrentFont());
    //    sequenceBuilder.erase();
    //    sequenceState = SEQ_NORMAL; // Reset to passthrough
    //    return;
    }
    //39 - MicroKnight (Amiga)
    else
    if (sequenceBuilder == "\x1b[0;39 D")
    {
        std::cout << "switch to Micro-Knight+ Font" << std::endl;
        TheTerminal::Instance()->setCurrentFont("microKnightPlus-8x16.bmp");
        if (TheTerminal::Instance()->didFontChange())
            TheTerminal::Instance()->loadBitmapImage(
                TheTerminal::Instance()->getCurrentFont());
    //    sequenceBuilder.erase();
    //    sequenceState = SEQ_NORMAL; // Reset to passthrough
    //    return;
    }
    //40 - Topaz (Amiga)
    else
    if (sequenceBuilder == "\x1b[0;40 D")
    {
        std::cout << "switch to Topaz+ Font" << std::endl;
        TheTerminal::Instance()->setCurrentFont("topazPlus-8x16.bmp");
        if (TheTerminal::Instance()->didFontChange())
            TheTerminal::Instance()->loadBitmapImage(
                TheTerminal::Instance()->getCurrentFont());
    //    sequenceBuilder.erase();
    //    sequenceState = SEQ_NORMAL; // Reset to passthrough
    //    return;
    }

#ifdef _DEBUG
    std::cout << "Params Size(): " << params.size() << std::endl;
#endif
    // If we get there, we have full CSI with possiable ; ; seperators.
    try
    {
        sequenceBuilder.erase(0,2);                      // Remove ESC [
        sequenceTerminator =                            // Get Terminator
            sequenceBuilder[sequenceBuilder.size()-1];

        // First Parameter is always the SequenceTerminator.
        params.push_back(sequenceTerminator);

        // Remove Sequence Terminator from string to text for parameters.
        sequenceBuilder.erase(sequenceBuilder.size()-1,1);
    }
    catch (std::exception e)
    {
        std::cout << "Exception removing ESC and Terminator Sequences: "
            << e.what() << std::endl;
    }

    // Split String by ';' character to seperate parameters if it exists.
    if (sequenceBuilder.size() == 0)
    {
        // We have no parameters, just terminator for single sequence.
#ifdef _DEBUG
        std::cout << "No params Vector: " << std::endl;
        int ii = 1;
        std::vector<int>::iterator it;
        for(it = params.begin(); it != params.end(); ++it)
        {
            std::cout << ii++ << " " << *it << " " << static_cast<char>(*it) << std::endl;
        }
#endif
        return;
    }

    // If we have a parameter seqerator, then tokenize the parameters.
    std::string::size_type result = sequenceBuilder.find(";",0);
    if (result != std::string::npos)
    {
        std::istringstream inStream(sequenceBuilder);
        std::istringstream tokenStream;
        std::string token;

        // 0;33;47 Splits 0 33 47
        // ignores spaces.
#ifdef _DEBUG
        int ii = 1;
        std::cout << "result = TRUE, found ; seperator." << std::endl;
#endif
        while(std::getline(inStream, token, ';'))
        {
            tokenStream.clear();
            if (token.size() == 0)
            {
#ifdef _DEBUG
                std::cout << "Empty " << std::flush;
#endif
                token = "-1"; // Empty fields are set to -1 for implied parms.
            }
#ifdef _DEBUG
            std::cout << ii++ << " " << "Token: " << token << std::endl;
#endif
            tokenStream.str(token);                   // Token to Stream
            if ((tokenStream >> parameter).fail())    // String to Int
            {
                // Skip this character.
                std::cout << "Exception String to Int Parameter Failure."
                    << std::endl;
                continue;
            }
            params.push_back(parameter); // Add to Parameteres Vector.
        }
    }
    else
    {
        // First check for ? DEC Sequence Starter
        if (sequenceBuilder[0] == '?')
        {
#ifdef _DEBUG
            std::cout << "removing ? from sequence" << std::endl;
#endif
            try
            {
                params.push_back('?');      // Add to Parameteres Vector.
                sequenceBuilder.erase(0,1); // Remove ?
            }
            catch (std::exception e)
            {
                std::cout << "Exception removing ? sequence starter: "
                    << e.what() << std::endl;
            }
        }
        // No seperator, translate the 1-3 digit code that should be present.
        std::istringstream tokenStream;
        tokenStream.str(sequenceBuilder);         // String to Stream
        if ((tokenStream >> parameter).fail())    // String to Int
        {
            // Skip this character.
            std::cout << "Exception String to Int Parameter Failure."
                << std::endl;
        }
        else
            params.push_back(parameter); // Add to Parameteres Vector.
    }

#ifdef _DEBUG
    // Testing output of vector.
    std::cout << "Vector: " << std::endl;
    int ii = 1;
    std::vector<int>::iterator it;
    for(it = params.begin(); it != params.end(); ++it)
    {
        std::cout << ii++ << " " << *it << " " << static_cast<char>(*it) << std::endl;
    }
#endif
}

/*
 * Takes String input and parses for ESC Control Sequences
 * State machine stays actives waiting for complete control sequences
 * before pushing any actions forwards.  Normal Text data is passed through.
 */
void SequenceParser::processSequence(std::string inputString)
{
    //Parse entire string and remove any double ESC Sequences.
    std::string::size_type result = 0;
    while (result != std::string::npos)
    {
        result = inputString.find("\x1b\x1b");
        if (result != std::string::npos)
        {
#ifdef _DEBUG
            std::cout << "Found Double ESC!" << std::endl;
#endif
            try
            {
                //inputString.replace(result,1,"^");
                // Pass through, remove double ESC!
                inputString.replace(result,1,"");
            }
            catch (std::exception e)
            {
                std::cout << "Exception replacing double ESC characters: "
                    << e.what() << std::endl;
                sequenceState = SEQ_ERROR; // Reset The State
            }
        }
    }

    // Now loop enire string and find valid escape sequences.
    for (std::string::size_type i = 0; i < inputString.size(); i++)
    {
#ifdef _DEBUG
        std::cout << "sequenceState: " << sequenceState << std::endl;
#endif
        // Get next Input Sequence
        sequence = inputString[i];

        // Remove Bell from displaying. Anoying in Shell!
        // When not displaying, we'll push this to console so it beeps!
        if (sequence == '\x07')
            continue;

        // Check for Start of Sequence, if we hit a sequence we then need
        // To send all Valid Output that Proceeds this sequence so everything
        // is FIFO with regards to how incoming data is handeled.
        if (sequence == '\x1b')
        {
            sequenceState = SEQ_START;
        }
        // Pre-Handle the Parser State
        switch(sequenceState)
        {
            // Normal Text Data, Append to Buffer
            case SEQ_NORMAL:
                validOutputData += sequence;
                break;

            // Start of Sequence, Push ValidOutputData Buffer Through
            // Then switch to Processing State, So we have FIFO Data
            // Parsing.
            case SEQ_START:
                TheAnsiParser::Instance()->textInput(validOutputData);
                validOutputData.erase();
                sequenceState = SEQ_PROCESSING;
                sequenceBuilder += sequence;
                break;

            case SEQ_PROCESSING:
                // Were in the middle of processing a control sequence
                // Keep is going till we get DONE!
                sequenceBuilder += sequence;
                break;

            case SEQ_ERROR:
                // Validing the Sequence Bombed, Clear the vector and continue.
                std::vector<int>().swap(params); // Clear for next run.
                sequenceBuilder.erase();         // Clear Any Prebuilt.
                sequenceState = SEQ_NORMAL;
                continue;
        }

        if (sequence == '\x1b' && !sequenceCompleted)
        {
            // Error Checking, If were in the middle of an incomplete sequence
            // Then we shouldn't get an ESC before we complete the sequence!
            if (foundSequence)
            {
#ifdef _DEBUG
                std::cout << "Invalid Sequence, ESC in current sequence"
                    << std::endl;
#endif
                // We now determine the preceeding sequence is invalid
                // And remove the ESC, then proceed with the current sequence
                /*
                try
                {
                    // Change to SequenceBuilder, don't touch input string
                    // If it's broken into multiple input, then this is no
                    // longer viable.

                    // Turn this off so sequences pass through to shell.
                    //sequenceBuilder.replace(0,1,"^");  // ESC to ^
                }
                catch (std::exception e)
                {
                    std::cout << "Exception replace escapePosition: "
                        << e.what() << std::endl;
                    sequenceState = SEQ_ERROR; // Reset The State
                }*/
            }
#ifdef _DEBUG
            std::cout << "ESC starting new sequence parsing" << std::endl;
#endif
            escapePosition = i;
            foundSequence = true;
            continue;
        }

        // Each Level is the next character after the ESC
        // Check the first few levels to make sure we have a valid CSI
        if (foundSequence && !invalidSequence)
        {
            switch(sequenceLevel)
            {
                case 0: // Process first char after ESC '['
#ifdef _DEBUG
                    std::cout << "sequenceLevel: " << sequenceLevel << " : "
                        << sequence << std::endl;
#endif
                    processLevel0();
                    // Move to next Level if valid.
                    if (!invalidSequence)
                        ++sequenceLevel;
                    break;

                case 1: // Process second char after ESC [ 'c'
#ifdef _DEBUG
                    std::cout << "sequenceLevel: " << sequenceLevel << " : "
                        << sequence << std::endl;
#endif
                    processLevel1();
                    break;

                case 2: // Process 2nd char after ESC [ '?' DEC Sequences
#ifdef _DEBUG
                    std::cout << "sequenceLevel: " << sequenceLevel << " : "
                        << sequence << std::endl;
#endif
                    processLevel2();
                    break;
            }
        }

        // Handle Completed Control Sequences
        if (sequenceCompleted)
        {
#ifdef _DEBUG
            std::cout << "SequenceCompleted!" << std::endl;
#endif
            // Copy just this sequenec into string for validation
            try
            {
                //escapeSequence =
                //    inputString.substr(escapePosition,(i+1)-escapePosition);

                // Break up the sequence into seperate parameters for
                // The Screen Parser.
                validateSequence();
                sequenceState = SEQ_DONE;
            }
            catch (std::exception e)
            {
                std::cout << "Exception substring on escapeSequence: "
                    << e.what() << std::endl;
                sequenceState = SEQ_ERROR;
            }

            // If the sequence is completed, The Parse the parameters and
            // Setup for ANSI Parser and Drawing to Screen.
            sequenceCompleted = false;
            foundSequence     = false;
            invalidSequence   = false;
            sequenceLevel     = 0;
        }
        // Invalid Sequences, Replace ESC with ^ characters for display
        // Then move on.
        if (invalidSequence)
        {
#ifdef _DEBUG
            std::cout << " *** SequenceInvalid!" << std::endl;
#endif
            // Handle Invalid Sequences by removeing the ESC character, then
            // Continue on to the next sequence.
            /*
            try
            {
                // Turn off for pass through on shell.
                //sequenceBuilder.replace(0,1,"^");
            }
            catch (std::exception e)
            {
                std::cout << "Exception replace escapePosition: "
                    << e.what() << std::endl;
                sequenceState = SEQ_ERROR; // Reset The State
            }*/

            foundSequence   = false;
            invalidSequence = false;
            sequenceLevel   = 0;

            // First grab the entire sequence parsed so far, from ESC position
            // Place it into validOutputData to display on the screen,
            // We don't want To reset the loop becasue it will incriment,
            // so we grab all characters parsed so far.
            try
            {
                validOutputData += sequenceBuilder;
                sequenceBuilder.erase();
                //inputString.substr(escapePosition, (i+1) - escapePosition);
            }
            catch (std::exception e)
            {
                std::cout << "Exception substring on ESC Position: "
                    << e.what() << std::endl;
            }
            // Reset the State
            sequenceState = SEQ_ERROR;
        }

        // Post-Handle the Parser State
        switch(sequenceState)
        {
            case SEQ_NORMAL:
                // If we are reset to this state, reset the level
                break;

            case SEQ_PROCESSING:
                // Were in the middle of processing a control sequence
                // Keep is going till we get DONE!
                break;

            case SEQ_DONE:
                // We hit an entire sequence, pass the vector
                TheAnsiParser::Instance()->sequenceInput(params);
                std::vector<int>().swap(params); // Clear for next run.
                sequenceState = SEQ_NORMAL; // Reset The State
                sequenceBuilder.erase();
                break;

            case SEQ_ERROR:
                // Validing the Sequence Bombed, Clear the vector and continue.
                std::vector<int>().swap(params); // Clear for next run.
                sequenceState = SEQ_NORMAL; // Reset The State
                sequenceBuilder.erase();
                break;
        }
    }
#ifdef _DEBUG
    // Test Display Valid Output Data
    std::cout << std::endl
        << "==========================================================="
        << std::endl;
    std::cout << std::endl << "valid data: " << validOutputData
        << std::endl << std::endl;
#endif
    // Catch any echo'ed input that passed through with no sequences
    if (sequenceState == SEQ_NORMAL && validOutputData.size() > 0)
    {
        TheAnsiParser::Instance()->textInput(validOutputData);
        validOutputData.erase();
    }    

    // Display final screen.
    TheTerminal::Instance()->renderScreen();
    TheTerminal::Instance()->drawTextureScreen();

    // When no data received, this is when we want to show the cursor!
    // Setup cursor in current x/y position Cursor.
    if (TheAnsiParser::Instance()->isCursorActive())
    {
        TheTerminal::Instance()->setupCursorChar();
        TheTerminal::Instance()->renderCursorOnScreen();
        TheTerminal::Instance()->drawTextureScreen();
    }
}

// Test Cases for Class.
/*
int main(int argc, char **argv)
{
    // ESC Sequence
    std::string inputString;

    // Test a split ESC sequence in seperate strings
    // Valid Sequence
    inputString = "\x1b[1;34;2";
    TheSequenceParser::Instance()->ProcessSequence(inputString);

    // Part ][
    inputString = "55;255;255m";
    TheSequenceParser::Instance()->ProcessSequence(inputString);

    // Invalid & Valid Sequence Sequence
    inputString = "\x1b\x1b[A";
    TheSequenceParser::Instance()->ProcessSequence(inputString);

    // Valid Text Data
    inputString = " ***Testing*** ";
    TheSequenceParser::Instance()->ProcessSequence(inputString);

    // Invalid Sequence
    inputString = "\x1b[0;0 M";
    TheSequenceParser::Instance()->ProcessSequence(inputString);

    // Valid Sequence
    inputString = "\x1b[0;0 D";
    TheSequenceParser::Instance()->ProcessSequence(inputString);

     // Valid Sequence
    inputString = "\x1b[A";
    TheSequenceParser::Instance()->ProcessSequence(inputString);

     // Valid Sequence
    inputString = "\x1b[422A";
    TheSequenceParser::Instance()->ProcessSequence(inputString);

    // Valid Sequence
    inputString = "\x1b[0;48;255;255;255m";
    TheSequenceParser::Instance()->ProcessSequence(inputString);


    // Invalid Spaces in the middle
    // Space only valid if Ending sequence = 'D'
    inputString = "\x1b[0; 48;255 ;255;255m";
    TheSequenceParser::Instance()->ProcessSequence(inputString);

     // Valid DESC Sequence with Pre-Control Characters.
    inputString = "\x1b[?7h";
    TheSequenceParser::Instance()->ProcessSequence(inputString);

    // Clear and Release Instance.
    TheSequenceParser::ReleaseInstance();
	return 0;
}
*/
