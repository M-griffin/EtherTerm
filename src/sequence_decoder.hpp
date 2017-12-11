#ifndef SequenceParser_H
#define SequenceParser_H

#include "safe_queue.hpp"
#include "message_queue.hpp"

#include <memory>
#include <string>
#include <vector>

// Forward Deceleration
class Session;
typedef std::shared_ptr<Session> session_ptr;
typedef std::weak_ptr<Session> session_weak_ptr;

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

    explicit SequenceDecoder(session_ptr session);
    ~SequenceDecoder();

    /**
     * @brief Queue the Pasred Data back to the Session for Display
     * @return
     */
    bool sessionQueue();

    // {Main Execution Method}
    // Validate and Decode ESC Sequences
    void decodeEscSequenceData(std::string &input_string);

private:

    // Handle to Session for Sending Responses to Client.
    session_weak_ptr  m_weak_session;

    // Holds Individual Sequences
    MessageQueue      m_message_queue;

    // Sequence Parser State
    enum
    {
        SEQ_NORMAL     = 0, // Normal Text Data
        SEQ_START      = 1, // Start of ESC Sequence
        SEQ_PROCESSING = 2, // Processing for Complete Sequence
        SEQ_DONE       = 3, // ESC Sequence Completed OK.
        SEQ_ERROR      = 4  // Bad Sequence, Kill it!
    };

    int unsigned  m_sequence_state;
    unsigned char m_sequence;
    int           m_parameter;
    bool          m_is_sequence;
    bool          m_is_parameter;
    bool          m_is_invalid_sequence;
    bool          m_is_sequence_completed;
    int           m_sequence_level;

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

    /**
     * @brief Level 0 Parsing check for Start of CSI or Alternate ESC Sequences
     */
    void processSequenceLevel0();

    /**
     * @brief Level 1 Parsing Comes After ESC[ = CSI.
     * Numbers and Separators are found in the middle of sequences as Parameters
     * Switch Statement catch the end of a Valid Sequence.
     *
     * Any non-supported sequences can have certain characters after the CSI
     * and These are parsed so that they are skipped and marked Invalid.
     */
    void processSequenceLevel1();

    /**
     * @brief Level 2 Parsing Catches (2) Different Sequence Styles and Comes After ESC[ = CSI.
     * Specifically for ? preceding sequencing, and ' ' Space D ending Sequence
     * For syncterm font switching.
     *
     * Numbers and Separators are found in the middle of sequences as Parameters
     * Switch Statement catch the end of a Valid Sequence.
     *
     * Any non-supported sequences can have certain characters after the CSI
     * and These are parsed so that they are skipped and marked Invalid.
     */
    void processSequenceLevel2();

    /**
     * @brief Handle SyncTerm Font Change Sequences
     */
    void handleFontChangeSequences();

    /**
     * @brief Decode and Validate Escapce Sequences.
     */
    void validateSequence();

public:
    /**
     * @brief Reset the Sequence Parser to refresh the screen
     */
    void resetParser();

};

typedef std::shared_ptr<SequenceDecoder> sequence_decoder_ptr;

#endif
