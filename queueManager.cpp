#include "queueManager.hpp"

// Init the Global Instance
QueueManager* QueueManager::m_globalInstance = nullptr;

QueueManager::QueueManager()
{ }

QueueManager::~QueueManager()
{
    std::cout << "~QueueManager" << std::endl;
}