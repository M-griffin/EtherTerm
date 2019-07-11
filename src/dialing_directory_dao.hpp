#ifndef DIALING_DIRECTORY_DAO_HPP
#define DIALING_DIRECTORY_DAO_HPP

#include <memory>
#include <string>

class DialingDirectory;
typedef std::shared_ptr<DialingDirectory> dial_directory_ptr;

/**
 * @class DialingDirectoryDao
 * @author Micahel Griffin
 * @date 09/07/2019
 * @file dialing_directory_dao.hpp
 * @brief Data Access Objects for The Dialing Directory
 */
class DialingDirectoryDao
{
public:
    DialingDirectoryDao(std::string path);
    ~DialingDirectoryDao();

    /**
     * @brief Check if the file exists and we need to create a new one.
     * @return
     */
    bool fileExists();

    /**
     * @brief Creates and Saves a newly Generated File.
     * @param menu
     * @return
     */
    bool saveDialingDirectory(dial_directory_ptr dialing_directory);

    /**
     * @brief Loads a Menu file into the m_menu stub for access.
     * @return
     */
    dial_directory_ptr loadDialingDirectory();

    /**
     * @brief Moves the Loaded Menu to the shared pointer.
     * @param rhs
     * @return
     */
    void encode(const DialingDirectory &rhs);

    /**
     * @brief Retrieve the existing Filename
     * @return
     */
    std::string getFileName() const
    {
        return m_filename;
    }


    dial_directory_ptr m_dial_directory;
    std::string        m_program_path;
    std::string        m_filename;
};

typedef std::shared_ptr<DialingDirectoryDao> dial_directory_dao_ptr;

#endif // DIALING_DIRECTORY_DAO_HPP
