#ifndef DIALING_MANAGER_HPP
#define DIALING_MANAGER_HPP

#include "common_methods.hpp"

#include <string>
#include <memory>
#include <regex>
#include <sstream>

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
    : BaseCommon
{
public:
    DialingManager(std::string path);
    ~DialingManager();

    bool previousXmlFileExists();
    std::string searchForRegexMatch(std::string str_data, std::regex re);
    void convertXmltoYaml();
    bool validateFile();
    dial_directory_ptr retrieveDialingDirectory();

private:

    dial_directory_dao_ptr m_dial_directory_dao;
    std::string            m_program_path;
};

typedef std::shared_ptr<DialingManager> dial_manager_ptr;

#endif // DIALING_MANAGER_HPP
