// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "stateParser.h"
#include "terminal.h"
#include "termObjectFactory.h"

using namespace std;

bool StateParser::parseState(const char *stateFile, string stateID, vector<TermObject *> *pObjects, std::vector<std::string> *pTextureIDs)
{
    // create the XML document
    TiXmlDocument xmlDoc;

    // load the state file
    if(!xmlDoc.LoadFile(stateFile))
    {
        cerr << xmlDoc.ErrorDesc() << "\n";
        return false;
    }

    // get the root element
    TiXmlElement* pRoot = xmlDoc.RootElement();

    // pre declare the states root node
    TiXmlElement* pStateRoot = 0;
    // get this states root node and assing it to pStateRoot
    for(TiXmlElement* e = pRoot->FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
    {
        if(e->Value() == stateID)
        {
            pStateRoot = e;
            break;
        }
    }

    // pre declare the texture root
    TiXmlElement* pTextureRoot = 0;

    // get the root of the texture elements
    for(TiXmlElement* e = pStateRoot->FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
    {
        if(e->Value() == string("TEXTURES"))
        {
            pTextureRoot = e;
            break;
        }
    }

    // now parse the textures
    parseTextures(pTextureRoot, pTextureIDs);

    // pre declare the object root node
    TiXmlElement* pObjectRoot = 0;

    // get the root node and assign it to pObjectRoot
    for(TiXmlElement* e = pStateRoot->FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
    {
        if(e->Value() == string("OBJECTS"))
        {
            pObjectRoot = e;
            break;
        }
    }

    // now parse the objects
    parseObjects(pObjectRoot, pObjects);
    return true;
}

void StateParser::parseTextures(TiXmlElement* pStateRoot, std::vector<std::string> *pTextureIDs)
{
    // for each texture we get the filename and the desired ID and add them to the texture manager
    for(TiXmlElement* e = pStateRoot->FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
    {
        string filenameAttribute = e->Attribute("filename");
        string idAttribute = e->Attribute("ID");
        pTextureIDs->push_back(idAttribute); // push the id into the list
        //     TheTextureManager::Instance()->load(filenameAttribute, idAttribute, TheGame::Instance()->getRenderer());
    }
}

void StateParser::parseObjects(TiXmlElement *pStateRoot, std::vector<TermObject *> *pObjects)
{
    for(TiXmlElement* e = pStateRoot->FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
    {
        int x, y, width, height, numFrames, callbackID, animSpeed;
        string textureID;

        e->Attribute("x", &x);
        e->Attribute("y", &y);
        e->Attribute("width",&width);
        e->Attribute("height", &height);
        e->Attribute("numFrames", &numFrames);
        e->Attribute("callbackID", &callbackID);
        e->Attribute("animSpeed", &animSpeed);
        textureID = e->Attribute("textureID");
        //int x, int y, int width, int height, std::string textureID, int numFrames, void()
        TermObject* pTermObject = TheTermObjectFactory::Instance()->create(e->Attribute("type"));
//        pTermObject->load(std::unique_ptr<LoaderParams>(new LoaderParams(x, y, width, height, textureID, numFrames, callbackID, animSpeed)));
        pObjects->push_back(pTermObject);
    }
}
