#ifndef MENUIO_HPP
#define MENUIO_HPP

#include <common_methods.hpp>
#include <string>
#include <memory>

class SequenceDecoder;
typedef std::shared_ptr<SequenceDecoder> sequence_decoder_ptr;

class CommonIO;
typedef std::shared_ptr<CommonIO> common_io_ptr;


/**
 * @class MenuIO
 * @author Michael Griffin
 * @date 12/20/2015
 * @file menu_io.hpp
 * @brief Menu IO handling
 * NOTES: Need to Rewrite GetLine() for pass through
 */
class MenuIO
    : BaseCommon
{
public:
    MenuIO(sequence_decoder_ptr &decoder, const std::string &program_path, common_io_ptr &common_io);
    ~MenuIO();

    sequence_decoder_ptr m_sequence_decoder;
    std::string          m_program_path;
    common_io_ptr        m_common_io;

    /**
     * @brief Parses MCI and PIPE Codes to ANSI Sequences
     * @param sequence
     */
    void sequenceToAnsi(const std::string &sequence);

    /**
     * @brief Reads in ANSI text file and pushes to Display
     * @param file_name
     */
    void displayMenuAnsi(const std::string &file_name);

};

#endif // MENUIO_HPP
