#ifndef MENUIO_HPP
#define MENUIO_HPP

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include <string>

class MenuIO
{
public:
    MenuIO();
    ~MenuIO();

    static void rightSpacing(std::string &str, int space);
    static void leftSpacing(std::string &str, int space);
    static void maskInput(std::string &str);
    static void inputField(std::string &text, int &len);

    static int getKey();
    static int checkKey();
    static void getLine(char *line,      // Returns Input into Line
                 int   length,           // Max Input Length of String
                 char *leadoff = 0,      // Data to Display in Default String {Optional}
                 int   hid     = false); // If input is Echoed as hidden      {Optional}

    static void ansiForegroundColor(char *data, int fg);
    static void ansiBackgroundColor(char *data, int bg);
    static void sequenceToAnsi(const std::string &sequence);
    static void displayAnsiFile(const std::string &fileName);
};

#endif // MENUIO_HPP
