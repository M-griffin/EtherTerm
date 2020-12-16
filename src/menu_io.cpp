#include "menu_io.hpp"
#include "common_io.hpp"
#include "sequence_decoder.hpp"
#include "static_methods.hpp"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>

MenuIO::MenuIO(sequence_decoder_ptr &decoder, const std::string &program_path, common_io_ptr &common_io)
    : m_sequence_decoder(decoder)
    , m_program_path(program_path)
    , m_common_io(common_io)

{
    std::cout << "MenuIO Created!" << std::endl;
}

MenuIO::~MenuIO()
{
    std::cout << "~MenuIO" << std::endl;
}


/**
 * @brief Parses MCI and PIPE Codes to ANSI Sequences
 * @param sequence
 */
void MenuIO::sequenceToAnsi(const std::string &sequence)
{
    std::string::size_type id1 = 0;  // Pipe Position
    char pipe_sequence[3];           // Holds 1, 2nd digit of Pipe
    char pipe_position1[3];          // Hold XY Pos for ANSI Position Codes
    char pipe_position2[3];          // Hold XY Pos for ANSI Position Codes
    char esc_sequence[1024];         // Holds Converted Pipe 2 ANSI

    std::string data_string = sequence;
#define SP 0x20

    // Search for First Pipe
    while(id1 != std::string::npos)
    {
        id1 = data_string.find("|", id1);

        if(id1 != std::string::npos && id1+2 < data_string.size())
        {
            memset(&pipe_sequence,0,sizeof(pipe_sequence));
            memset(&esc_sequence,0,sizeof(esc_sequence));
            pipe_sequence[0] = data_string[id1+1];  // Get First # after Pipe
            pipe_sequence[1] = data_string[id1+2];  // Get Second Number After Pipe

            if(pipe_sequence[0] == '\0' || pipe_sequence[0] == '\r' || pipe_sequence[0] == EOF) break;

            if(pipe_sequence[1] == '\0' || pipe_sequence[1] == '\r' || pipe_sequence[0] == EOF) break;

            //std::cout << "\r\n*** pipe_sequence: " << pipe_sequence << std::endl;
            if(isdigit(pipe_sequence[0]) && isdigit(pipe_sequence[1]))
            {
                std::string temp_sequence = "";
                std::string::size_type type;
                temp_sequence += pipe_sequence[0];
                temp_sequence += pipe_sequence[1];
                int pipe_color_value = 0;

                try
                {
                    pipe_color_value = std::stoi(temp_sequence, &type);

                    if(pipe_color_value >= 0 && pipe_color_value < 16)
                    {
                        m_common_io->foregroundColorSequence(esc_sequence, pipe_color_value);
                    }
                    else if(pipe_color_value >= 16 && pipe_color_value < 24)
                    {
                        m_common_io->backgroundColorSequence(esc_sequence, pipe_color_value);
                    }
                    else
                    {
                        ++id1;
                    }

                    // Replace pipe code with ANSI Sequence
                    if(strcmp(esc_sequence,"") != 0)
                    {
                        data_string.replace(id1, 3, esc_sequence);
                    }

                }
                catch(const std::invalid_argument& ia)
                {
                    std::cout << "Invalid argument: " << ia.what() << '\n';
                    ++id1;
                }
            }
            // Else not a Pipe Color / Parse for Screen Modification
            else if(pipe_sequence[0] == 'C')
            {
                // Carriage Return / New Line
                if(strcmp(pipe_sequence,"CR") == 0)
                {
                    m_common_io->backgroundColorSequence(esc_sequence, 16);  // Clear Background Attribute first
                    strcat(esc_sequence,"\r\n");
                    data_string.replace(id1, 3, esc_sequence);
                    id1 = 0;
                }
                // Clear Screen
                else if(strcmp(pipe_sequence,"CS") == 0)
                {
                    m_common_io->backgroundColorSequence(esc_sequence, 16);
                    // Set Scroll Region, Clear Background, Then Home Cursor.
                    strcat(esc_sequence,"\x1b[2J\x1b[1;1H");
                    data_string.replace(id1, 3, esc_sequence);
                    id1 = 0;
                }
            }
            else
            {
                if(strcmp(pipe_sequence,"XY") == 0 && id1+6 < data_string.size())
                {
                    memset(&pipe_position1, 0, sizeof(pipe_position1));
                    memset(&pipe_position2, 0, sizeof(pipe_position2));
                    // X Pos
                    pipe_position1[0] = data_string[id1+3];
                    pipe_position1[1] = data_string[id1+4];
                    // Y Pos
                    pipe_position2[0] = data_string[id1+5];
                    pipe_position2[1] = data_string[id1+6];

                    // Clear Background Attribute first
                    m_common_io->backgroundColorSequence(esc_sequence, 16);

                    data_string.replace(id1, 7, stdStringFormat(
                                            "%s\x1b[%i;%iH",
                                            esc_sequence,
                                            atoi(pipe_position2),
                                            atoi(pipe_position1))
                                       );
                }
                else
                {
                    // End of the Line, nothing parsed out so
                    // Skip ahead past current code.
                    ++id1;
                }
            }
        }
        else
        {
            break;
        }

        id1 = data_string.find("|",id1);
    }

    m_sequence_decoder->decodeEscSequenceData(data_string);
}


/**
 * @brief Reads in ANSI text file and pushes to Display
 * @param file_name
 */
void MenuIO::displayMenuAnsi(const std::string &file_name)
{
    std::string path = StaticMethods::getDirectoryPath(m_program_path);
    path.append(file_name);

    std::string buff;
    FILE *fp;
    int sequence = 0;

    if((fp = fopen(path.c_str(), "r+")) ==  nullptr)
    {
        std::cout << "MenuIO displayAnsiFile(): not found: "
                  << std::endl << path << std::endl;
        return;
    }

    do
    {
        sequence = getc(fp);

        if(sequence != EOF)
            buff += sequence;
    }
    while(sequence != EOF);

    fclose(fp);

    // Send to the Sequence Manager.
    m_sequence_decoder->decodeEscSequenceData(buff);
}
