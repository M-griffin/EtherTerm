#include "irc_manager.hpp"
#include "session.hpp"
#include "session_io.hpp"

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>

IrcManager::IrcManager(session_ptr session)
    : m_weak_session(session)
    , m_session_io(new SessionIO(session))
    , m_channels()
    , m_incoming_buffer("")
{
    std::cout << "IrcManager Created." << std::endl;
}

IrcManager::~IrcManager()
{
    std::vector<channel_ptr>().swap(m_channels);
    std::cout << "~IrcManager" << std::endl;
}


/**
 * @brief Get Current HH:MM Time Stamp
 * @return
 */
std::string IrcManager::getTimeStamp()
{
    std::time_t t = std::time(NULL);
    std::tm bt = *std::localtime(&t);

    std::stringstream oss;
    oss << std::put_time(&bt, "|04|16%H|08:|04%M"); // HH:MM ? 24 Hr format??
    return oss.str();
}


/**
 * @brief Parse Data By Newlines coming from Server
 * @param data
 */
void IrcManager::handleLineRead(std::string &data)
{
    std::string data_to_process = "";

    // Clean and deal with \r only, clear out all \n
    data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());

    // 1. is \r final char in string
    std::string::size_type index = 0;
    index = data.find_last_of('\r', 0);

    if (index != std::string::npos && index != data.size() && index < data.size())
    {
        std::cout << "* CR not last char in string!" << std::endl;

        std::cout << "Index: " << index << std::endl;
        std::cout << "Size : " << data.size() << std::endl;

        std::string temp_data = data.substr(index);
        data.erase(index);

        m_incoming_buffer += data;
        data_to_process = m_incoming_buffer;

        // reset buffer.
        m_incoming_buffer.erase();
        m_incoming_buffer = temp_data;
    }
    else
    {
        m_incoming_buffer += data;
        data_to_process = m_incoming_buffer;

        // reset buffer
        m_incoming_buffer.erase();
        m_incoming_buffer = "";
    }


    if (data_to_process.size() > 0)
    {
        session_ptr session = m_weak_session.lock();
        if (session)
        {
            std::stringstream is( data_to_process );
            std::string line;
            while( std::getline( is, line, '\r' ) )
            {
                //std::cout << "line: " << line << std::endl;

                std::string::size_type ping_index = 0;
                ping_index = line.find("PING", 0);
                // Check for Ping/Pong Response.
                if (ping_index != std::string::npos && ping_index == 0)
                {
                    std::string key = line.substr(6);
                    std::cout << "key: " << key << std::endl;
                    std::string ping_reponse = "PONG :" + key + "\r\n";
                    std::cout << " Response PONG" << std::endl;
                    session->deliver(ping_reponse);
                }
                else
                {
                    // TODO Initial Testing, to be moved to irc manager/decoder

                    // split text on : and setup initial line prefix of -.  Lets add date/time stamp.
                    std::string::size_type colon_pos = 0;
                    colon_pos = line.find(":", 1);
                    if (colon_pos != std::string::npos)
                    {
                        // Setup Structure for parsing.
                        ctrl_message_ptr ctrl_message(new ControlMessage());
                        ctrl_message->m_control = line.substr(1, colon_pos);
                        ctrl_message->m_control = line.substr(colon_pos + 1);

                        std::cout << "control: " << ctrl_message->m_control << std::endl;
                        std::cout << "message: " << ctrl_message->m_control << std::endl;

                        parseControlMessage(ctrl_message);
                    }

                    line.insert(0, " |08- |15");
                    line.insert(0, getTimeStamp());

                    // Each Line should start at the boottom of screen and scroll up.
                    std::string prefix = "\x1b[s|16\x1b[" + std::to_string(session->m_term_height - 3) + ";" +
                                         std::to_string(session->m_term_width) + "H ";

                    // Push Text down, write new line, then restore back for input.
                    prefix += line + "\x1b[u";

                    std::string output = m_session_io->pipe2ansi(prefix);
                    session->m_sequence_decoder->decodeEscSequenceData(output);
                }
            }
        }
    }

}


/**
 * @brief General Output for Text Box
 * @param message
 */
void IrcManager::prepareOut(std::string &message)
{
    session_ptr session = m_weak_session.lock();
    if (session)
    {
        // Each Line should start at the boottom of screen and scroll up.
        std::string prefix = "\x1b[s\x1b[" + std::to_string(session->m_term_height - 3) + ";" +
                             std::to_string(session->m_term_width) + "H ";

        // Push Text down, write new line, then restore back for input.
        prefix += message + "\x1b[u";
        session->m_sequence_decoder->decodeEscSequenceData(prefix);
    }
}


/**
 * @brief Handles Notices
 * @param message
 */
void IrcManager::onNotice(std::string &)
{




}


/**
 * @brief Parses Control Structure for IRC Server Commands and Messages
 * @param ctrl_message
 */
void IrcManager::parseControlMessage(ctrl_message_ptr &ctrl_message)
{
    //std::string::size_type index = 0;
    if (ctrl_message->m_control.find("NOTICE *", 0) != std::string::npos)
        onNotice(ctrl_message->m_message);

}


/**
 * @brief Irc Manager Setup
 */
void IrcManager::startUp()
{
    session_ptr session = m_weak_session.lock();
    if (session)
    {
        // Setup Scroll Region
        std::string sequence = setScrollRegion();

        // Setup Status Bar
        sequence += drawStatusBar();

        // move to prompt
        sequence += moveToUserPrompt();

        // draw the prompt with field input.
        sequence += drawUserPrompt();

        // Convert ANSI Sequences and Draw to Screen.
        std::string output = m_session_io->pipe2ansi(sequence);
        session->m_sequence_decoder->decodeEscSequenceData(output);
    }
}

/**
 * @brief Handles Parsing User Input Sequences
 * @param input_sequence
 */
std::string IrcManager::setScrollRegion()
{
    // Setup Scroll Region
    std::string sequence = "\x1b[0;40 D";
    session_ptr session = m_weak_session.lock();
    if (session)
    {
        std::string scroll_region = "\x1b[1;" + std::to_string(session->m_term_height - 3) + "r";
        sequence += scroll_region;
    }
    return sequence;
}

/**
 * @brief Handles Parsing User Input Sequences
 * @param input_sequence
 */
std::string IrcManager::drawStatusBar()
{
    std::string status_bar = "";
    session_ptr session = m_weak_session.lock();
    if (session)
    {
        status_bar = "\x1b[" + std::to_string(session->m_term_height - 2) + ";1H";
        status_bar.append("|20|15--] Status Bar [--");
        status_bar.append(80 - 18, 0x20);
    }
    return status_bar;
}

/**
 * @brief Handles Parsing User Input Sequences
 * @param input_sequence
 */
std::string IrcManager::moveToUserPrompt()
{
    std::string prompt = "";
    session_ptr session = m_weak_session.lock();
    if (session)
    {
        prompt = "\x1b[" + std::to_string(session->m_term_height - 1) + ";1H";
    }
    return prompt;
}

/**
 * @brief Handles Parsing User Input Sequences
 * @param input_sequence
 */
std::string IrcManager::drawUserPrompt()
{
    int length = 80-3;
    std::string field_prompt = "|12|16#|08: ";
    m_session_io->createInputField(field_prompt, length);
    return field_prompt;
}

/**
 * @brief Handles Parsing User Input Sequences
 * @param input_sequence
 */
void IrcManager::clearUserPrompt()
{
    std::string clear = "\x1b[1K\r";
    clear += drawUserPrompt();
    std::string sequence = m_session_io->pipe2ansi(clear);

    session_ptr session = m_weak_session.lock();
    if (session)
        session->m_sequence_decoder->decodeEscSequenceData(sequence);
}

/**
 * @brief Handles Parsing User Input Sequences
 * @param input_sequence
 */
void IrcManager::handleUserInput(const std::string &input)
{
    // Echo Input, Server doesn't echo input typed by user.
    std::cout << "\n *** irc input: " << input << std::endl;
    //m_sequence_decoder->decodeEscSequenceData(input_sequence);

    std::string key = "";
    std::string result = m_session_io->getInputField(input, key, 80-3);

    // ESC was hit
    if(result == "aborted")
    {
        std::cout << "aborted!" << std::endl;
    }
    else if(result[0] == '\n')
    {
        // Key == 0 on [ENTER] pressed alone. then invalid!
        if(key.size() > 0)
        {
            // Return and don't do anything.
            std::cout << "baseProcessDeliverNewLine (key): " << key << std::endl;
            // String is completed after ENTER, sent to Server!
            session_ptr session = m_weak_session.lock();
            if (session)
            {
                // Clean Input Field
                clearUserPrompt();

                // Send User Input to Server (ToDO Command parser)
                std::string output = key + "\r\n";
                session->deliver(output);
            }
        }
    }
    else
    {
        // Send back the single input received to show client key presses.
        // Only if return data shows a processed key returned.
        if (result != "empty")
        {
            // Send to Terminal to draw user input by keypress.
            session_ptr session = m_weak_session.lock();
            if (session)
            {
                std::string output = "|07" + result;
                std::string sequence = m_session_io->pipe2ansi(output);
                session->m_sequence_decoder->decodeEscSequenceData(sequence);
            }
        }
    }

}
