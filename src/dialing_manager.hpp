#ifndef DIALING_MANAGER_HPP
#define DIALING_MANAGER_HPP

#include <string>
#include <memory>

class DialingDirectory;
typedef std::shared_ptr<DialingDirectory> dial_directory_ptr;

class DialingDirectoryDao;
typedef std::shared_ptr<DialingDirectoryDao> dial_directory_dao_ptr;

/**
 * @class DialingManager
 * @author Michael Griffin
 * @date 10/07/2019
 * @file dialing_manager.hpp
 * @brief Manages The Dialing Direction Creation, And Updates
 */
class DialingManager
{
public:
    DialingManager(std::string path);
    ~DialingManager();

    bool validateFile();

private:
    std::string m_path;
    dial_directory_dao_ptr m_dial_directory_dao;
};

typedef std::shared_ptr<DialingManager> dial_manager_ptr;

#endif // DIALING_MANAGER_HPP
