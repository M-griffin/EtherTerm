#ifndef SequenceParser_H
#define SequenceParser_H

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "safeQueue.hpp"
#include "messageQueue.hpp"

#include <string>
#include <vector>

class SequenceDecoder
{
public:

    SequenceDecoder();
    ~SequenceDecoder();

    // Vector<int> holds the broken down squeue,
    // Vector<int> Empty, holds std::string or Text Data passed.
    SafeQueue<MessageQueue> m_sequenceQueue;

    // {Main Execution Method}
    // Valide and Decode ESC Sequences
    void decodeEscSequenceData(std::string inputString);


private:

    // Holds Indivdual Sequences
    MessageQueue m_messageQueue;

    // Sequence Parser State
    enum
    {
        SEQ_NORMAL     = 0, // Normal Text Data
        SEQ_START      = 1, // Start of ESC Sequence
        SEQ_PROCESSING = 2, // Processing for Complete Sequence
        SEQ_DONE       = 3, // ESC Sequence Completed OK.
        SEQ_ERROR      = 4  // Bad Sequence, Kill it!
    };

    int m_sequenceState;
    unsigned
    char m_sequence;
    int  m_parameter;
    bool m_foundSequence;
    bool m_foundParameters;
    bool m_isInvalidSequence;
    bool m_isSequenceCompleted;
    int  m_sequenceLevel;

    void processSequenceLevel0();
    void processSequenceLevel1();
    void processSequenceLevel2();

    void handleFontChangeSequences();
    void validateSequence();

    // Holds the intern data while we build the sequence,
    // This is needed if inputString doesn't have a complete sequence
    // Then we need to append so that the string will have the
    // Original first half of the already parsed sequence.
    std::string m_sequenceBuilder;

    //Holds the breakdown of the entire sequence
    std::vector<int> m_sequenceParams;

    // This string contains normal data passed through the sequence
    // Parser, At the end of a processing loop, this data is passed
    // for writing to the screen.
    std::string m_validOutputData;
    std::string::size_type m_escapePosition;


};

#endif
