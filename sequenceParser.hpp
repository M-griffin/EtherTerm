#ifndef SequenceParser_H
#define SequenceParser_H

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include <string>
#include <vector>

class SequenceParser
{
public:

    //  Instantiate the Singleton
    static SequenceParser* Instance()
    {
        if(globalInstance == 0)
        {
            globalInstance = new SequenceParser();
            return globalInstance;
        }
        return globalInstance;
    }

    // Release And Clear the Singleton
    static void ReleaseInstance()
    {
        if(globalInstance != 0)
        {
            delete globalInstance;
            globalInstance = 0;
        }
        return;
    }
    // Parser String data for ESC Sequence.
    void processSequence(std::string inputString);

private:

    // Sequence Parser State
    enum
    {
        SEQ_NORMAL     = 0, // Normal Text Data
        SEQ_START      = 1, // Start of ESC Sequence
        SEQ_PROCESSING = 2, // Processing for Complete Sequence
        SEQ_DONE       = 3, // ESC Sequence Completed OK.
        SEQ_ERROR      = 4  // Bad Sequence, Kill it!
    };

    int sequenceState;
    unsigned
    char sequence;
    int  parameter;
    bool foundSequence;
    bool foundParameters;
    bool invalidSequence;
    bool sequenceCompleted;
    int  sequenceLevel;

    void processLevel0();
    void processLevel1();
    void processLevel2();
    void validateSequence();

    // Holds the intern data while we build the sequence,
    // This is needed if inputString doesn't have a complete sequence
    // Then we need to append so that the string will have the
    // Original first half of the already parsed sequence.
    std::string sequenceBuilder;

    //Holds the breakdown of the entire sequence
    std::vector<int> params;

    // This string contains normal data passed through the sequence
    // Parser, At the end of a processing loop, this data is passed
    // for writing to the screen.
    std::string validOutputData;
    std::string::size_type
         escapePosition;

    static SequenceParser* globalInstance;

    SequenceParser();
    ~SequenceParser();

    SequenceParser(const SequenceParser&);
    SequenceParser& operator=(const SequenceParser&);

};

//Setup the Class Type
typedef SequenceParser TheSequenceParser;

#endif
