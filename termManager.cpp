#include "termManager.hpp"

/*
 * The Main Focus of This Class to To handle
 * Object Management:
 *
 * 1. Translations of Terminal/Rendering
 * 2. Reading SequenceQueue, and Passing along
 *    to The AnsiParse/ScreenManager for Drawing
 *    to the Screen.
 * 3. Handling Input.
 *
 * Since in SDL Input and Rendering must all be done in the main
 * Thread. We need a central location to hub this communication.
 *
 * This class will take the Singleton Place of the Terminal/Renderer.
 * We will also expand this to handle multiple windows!
 */

TermManager::TermManager()
{
}

TermManager::~TermManager()
{
}

