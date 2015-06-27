
// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "safeQueue.hpp"
#include "messageQueue.hpp"
#include "sequenceManager.hpp"
#include "queueManager.hpp"
#include "sequenceParser.hpp"
#include "renderer.hpp"

#include <iostream>

// Init the Global Instance
SequenceManager* SequenceManager::m_globalInstance = nullptr;

SequenceManager::SequenceManager()
{ }

SequenceManager::~SequenceManager()
{
    std::cout << "SequenceManager Released." << std::endl;
}

/*
 * Process Any Waiting Decided Data
 * Push to the Parser for Display
 */
void SequenceManager::update()
{
    //std::cout << "SequenceManager upadate() " << std::endl;


    if(TheQueueManager::Instance()->m_queue.isEmpty())
        return;

    /*
     * Loop the Queue and Send all decoded data
     * Through to be parsed and displayed.
     */
    MessageQueue msgQueue;
    while(!TheQueueManager::Instance()->m_queue.isEmpty())
    {
        msgQueue = std::move(TheQueueManager::Instance()->m_queue.dequeue());
        if(msgQueue.m_text.empty())
        {
            // Make Sure Vector is not Empty!
            if(msgQueue.m_queueParams.size() > 0)
            {
                TheSequenceParser::Instance()->sequenceInput(
                    msgQueue.m_queueParams);
            }
        }
        else
        {
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

    TheRenderer::Instance()->renderScreen();
    TheRenderer::Instance()->drawTextureScreen();

    // When no data received, this is when we want to show the cursor!
    // Setup cursor in current x/y position Cursor. after getting
    // New Data Received.
    if (TheSequenceParser::Instance()->isCursorActive())
    {
       TheRenderer::Instance()->setupCursorChar();
       TheRenderer::Instance()->renderCursorOnScreen();
       TheRenderer::Instance()->drawTextureScreen();
    }

    // Clear And Move on.
    msgQueue.clear();
}

/*
 * Decode and Store Data in the Queue
 */
void SequenceManager::decode(std::string input)
{
    //SequenceDecoder seq;
    //seq.decodeEscSequenceData(input);
    decodeEscSequenceData(input);

}
