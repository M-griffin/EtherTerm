#ifndef SequenceParser_H
#define SequenceParser_H

#include "safe_queue.hpp"
#include "message_queue.hpp"

#include <boost/smart_ptr/shared_ptr.hpp>

#include <string>
#include <vector>

// Forward Decleration
class Session;
typedef boost::shared_ptr<Session> session_ptr;

/**
 * @class SequenceDecoder
 * @author Michael Griffin
 * @date 12/20/2015
 * @file sequence_decoder.hpp
 * @brief Processes incoming data into a message queue.
 */
class SequenceDecoder
{
public:

    SequenceDecoder(session_ptr session);
    ~SequenceDecoder();

    // {Main Execution Method}
    // Valide and Decode ESC Sequences
    void decodeEscSequenceData(std::string &input_string);

private:

    // Holds Indivdual Sequences
    session_ptr  m_session;
    MessageQueue m_message_queue;

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
    std::string            m_sequence_builder;

    //Holds the breakdown of the entire sequence
    std::vector<int>       m_sequence_params;

    // This string contains normal data passed through the sequence
    // Parser, At the end of a processing loop, this data is passed
    // for writing to the screen.
    std::string            m_valid_output_data;
    std::string::size_type m_escape_position;
};

typedef boost::shared_ptr<SequenceDecoder> sequence_decoder_ptr;

#endif
