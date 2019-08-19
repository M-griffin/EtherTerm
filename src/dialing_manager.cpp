#include "dialing_manager.hpp"
#include "dialing_directory_dao.hpp"
#include "dialing_directory.hpp"

DialingManager::DialingManager(std::string path)
    : m_dial_directory_dao(new DialingDirectoryDao(path))
{
}

DialingManager::~DialingManager()
{
}

bool DialingManager::validateFile()
{
    if(!m_dial_directory_dao->fileExists())
    {
        // Create a NEw file here with at least one system.
        dial_directory_ptr dial_directory(new DialingDirectory());
        SystemEntry entry;
        entry.index = 0;
        entry.name = "Haunting The Chapel";
        entry.ip = "htc.zapto.org";
        entry.port = 23;
        entry.protocol = "Telnet";
        entry.login = "";
        entry.password = "";
        entry.font = "vga8x16.bmp";
        entry.key_map = "VT100";
        entry.term_type = "ANSI";
        entry.term_size = "80x25";

        dial_directory->system_entries.push_back(entry);

        if(!m_dial_directory_dao->saveDialingDirectory(dial_directory))
        {
            return false;
        }
    }
    else
    {
        return true;
    }

    return true;
}

dial_directory_ptr DialingManager::retrieveDialingDirectory()
{
    return (m_dial_directory_dao->loadDialingDirectory());
}
