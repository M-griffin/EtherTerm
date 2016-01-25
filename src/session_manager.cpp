#include "session_manager.hpp"
#include "session.hpp"

#include <algorithm>
#include <boost/bind.hpp>


SessionManager::SessionManager()
    : m_is_system_disconnected(false)
    , m_cursor_blink(2)
    , m_start_blinking(false)
    , m_ttime(0)
    , m_ttime2(0)
{
    std::cout << "SessionManager Created" << std::endl;
}

SessionManager::~SessionManager()
{
    std::cout << "~SessionManager" << std::endl;
    // Clear Any new Waiting connections on shutdown.
    m_new_connections.clear();
}

/**
 * @brief Notifies that a user has joined the room
 * @param participant
 */
void SessionManager::join(session_ptr session)
{
    std::cout << "Joined SessionManager" << std::endl;
    m_sessions.insert(session);
}

/**
 * @brief Notifies that a user has left the room
 * @param participant
 */
void SessionManager::leave(session_ptr session)
{
    std::cout << "Left SessionManager" << std::endl;

    // If session is connected, we need to disconnect it first
    if(session->m_connection)
    {
        if(session->m_is_connected)
        {
            // Shutdown the Connection before closing
            session->m_connection->shutdown();
            // Force no longer being connected now.
            session->m_is_connected = false;
        }
        session->m_connection->close();
    }

    // Remove the Session
    m_sessions.erase(session);

    // Mark that a system has disconnect to reset window focus
    m_is_system_disconnected = true;
}

/**
 * @brief Window Session is Killed we want to switch focus to the first available window.
 */
void SessionManager::grabNewWindowFocus()
{
    // If a system hasn't diconnected then return;
    if (!m_is_system_disconnected)
    {
        return;
    }

    // Move to the first window in the list and raise that
    // Window for focus once the current windows closes.
    auto it = m_sessions.begin();
    if(it != m_sessions.end())
    {
        std::cout << "Changing Window Focus to WindowId: "
                  << (*it)->m_window_manager->getWindowId()
                  << std::endl;

        (*it)->m_window_manager->grabWindow();
    }

    // Reset for next system detected.
    m_is_system_disconnected = false;
}

/**
 * @brief Sends message to all users in the current room.
 * @param participant
 *
void SessionManager::deliver(std::string msg)
{
    if(msg.size() == 0)
        return;

    // This isn't used at this time!
    std::cout << "Deliver Global SessionManager notices: " << msg << std::endl;
    std::for_each(m_sessions.begin(), m_sessions.end(),
                  boost::bind(&Session::deliver, _1, boost::ref(msg)));
}
*/

/**
 * @brief Retrieve Number of users connected
 * Also helpful for determinging next node number.
 * @return
 */
int SessionManager::numberOfSessions()
{
    return m_sessions.size();
}

/**
 * @brief Toggles off the Blinking of the cursor while data is displayed.
 */
void SessionManager::stopBlinkingCursor()
{
    m_ttime = SDL_GetTicks();
    m_start_blinking = false;
    m_cursor_blink = 0;
}

/**
 * @brief Update, Hits Session Updates for processing Data Queue.
 * @return
 */
void SessionManager::update()
{
    // Becasue disconnects can happen in a thread, we need to double check
    // If a session was poped off in the middle of the Iteration
    std::string::size_type num_sessions = m_sessions.size();
    for(auto it = m_sessions.begin(); it != m_sessions.end(); ++it)
    {
        // Make sure handle is valid once we call it.
        if (num_sessions != m_sessions.size())
        {
            return;
        }

        // Run Session Update for Event Processing.
        (*it)->update();

        // If socket data was received and processed
        // Then Redraw Blinking Cursor
        if ((*it)->m_received_data)
        {
            // Setup and render the cursor after rending new screen.
            if((*it)->m_sequence_parser->isCursorActive())
            {
                if ((*it)->m_is_connected)
                {
                    // Cursor Character fucks it up!
                    (*it)->m_renderer->setupCursorCharacter();
                    (*it)->m_renderer->renderCursorOnScreen();
                    (*it)->m_renderer->drawTextureScreen();
                }
            }
            (*it)->m_received_data = false;
        }
        else
        {
            // Setup and render the cursor after rending new screen.
            if((*it)->m_sequence_parser->isCursorActive())
            {
                // Loop here to blink the cursor.
                if ((*it)->m_is_connected)
                {
                    if((*it)->m_sequence_parser->isCursorActive())
                    {
                        m_start_blinking = true;
                        // Setup Timer for Blinking Cursor
                        // Initial State = On, then Switch to off in next loop.
                        if(m_cursor_blink % 2 == 0)
                        {
                            m_ttime2 = SDL_GetTicks();
                            if(m_start_blinking && (m_ttime2 - m_ttime) > 400)
                            {
                                (*it)->m_renderer->renderCursorOffScreen();
                                (*it)->m_renderer->drawTextureScreen();
                                --m_cursor_blink;
                                m_ttime = SDL_GetTicks();
                            }
                        }
                        else
                        {
                            m_ttime2 = SDL_GetTicks();
                            if(m_start_blinking && (m_ttime2 - m_ttime) > 400)
                            {
                                (*it)->m_renderer->renderCursorOnScreen();
                                (*it)->m_renderer->drawTextureScreen();
                                ++m_cursor_blink;
                                m_ttime = SDL_GetTicks();
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 * @brief Broacaster Anchors the Sessions, shutdown all session.
 * @return
 */
void SessionManager::shutdown()
{
    // Swap to pop all enteries off the stack.
    std::set<session_ptr>().swap(m_sessions);
}
