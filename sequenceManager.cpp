
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "safeQueue.hpp"
#include "messageQueue.hpp"
#include "sequenceManager.hpp"
#include "sequenceParser.hpp"
#include "terminal.hpp"

// Init the Global Instance
SequenceManager* SequenceManager::m_globalInstance = nullptr;

SequenceManager::SequenceManager()
{ }

SequenceManager::~SequenceManager()
{
    std::cout << "SequenceManager Released" << std::endl;
}

/*
 * Process Any Waiting Decided Data
 * Push to the Parser for Display
 */
void SequenceManager::update()
{
    //std::cout << "SequenceManager upadate() " << std::endl;
    SafeQueue<MessageQueue> sq;
    MessageQueue msgQueue;

    /*
     * Copy the queue out so we can run through the entire set
     * Without locking from threads. Process in batches!
     */
    if (m_sequenceQueue.isEmpty())
        return;

    sq = std::move(m_sequenceQueue);

    /*
     * Loop the Queue and Send all decoded data
     * Through to be parsed and displayed.
     */
    while(!sq.isEmpty())
    {
        msgQueue = sq.dequeue();
        if(msgQueue.m_text.empty())
        {
            // Make Sure Vector is not Empty!
            if (msgQueue.m_queueParams.size() > 0)
            {
                TheSequenceParser::Instance()->sequenceInput(
                    msgQueue.m_queueParams);
            }
        }
        else {
            TheSequenceParser::Instance()->textInput(
                msgQueue.m_text);
        }
        msgQueue.clear();
    }

    /*
     * Render The final screen data
     * 1. Copy Surface to Texture
     * 2. Render the Texture to Monitor.
     */
    TheTerminal::Instance()->renderScreen();
    TheTerminal::Instance()->drawTextureScreen();

    // When no data received, this is when we want to show the cursor!
    // Setup cursor in current x/y position Cursor. after getting
    // New Data Received.
    if (TheSequenceParser::Instance()->isCursorActive())
    {
        TheTerminal::Instance()->setupCursorChar();
        TheTerminal::Instance()->renderCursorOnScreen();
        TheTerminal::Instance()->drawTextureScreen();
    }
    // Clear And Move on.
    msgQueue.clear();
    sq.clear();
}

/*
 * Decode and Store Data in the Queue
 */
void SequenceManager::decode(std::string input)
{
    decodeEscSequenceData(input);
}
