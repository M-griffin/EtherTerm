#ifndef __State_Parser__
#define __State_Parser__


// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include <iostream>
#include <vector>
#include "tinyxml.h"

class TermObject;

class StateParser
{
public:
    
    StateParser() {}
    ~StateParser() {}
    
    bool parseState(const char* stateFile, std::string stateID, std::vector<TermObject*> *pObjects, std::vector<std::string> *pTextureIDs);
    
private:
    
    void parseObjects(TiXmlElement* pStateRoot, std::vector<TermObject*> *pObjects);      // Dialing Directory are the objects.
    void parseTextures(TiXmlElement* pStateRoot, std::vector<std::string> *pTextureIDs);  // Change to Surfaces for Fonts.
};

#endif
