#ifndef __LOADER_PARAMS__
#define __LOADER_PARAMS__

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include <string>

class LoaderParams
{
public:

    LoaderParams(int swidth, int sheight, int wwidth, int wheight, std::string textureID, int callbackID = 0) :

    surfaceWidth(swidth),
    surfaceHeight(sheight),
    windowWidth(swidth),
    windowHeight(sheight),
    callbackID(callbackID),
    textureID(textureID)

    {}

    int getSurfaceWidth() const { return surfaceWidth; }
    int getSurfaceHeight() const { return surfaceHeight; }
    int getWindowWidth() const { return windowWidth; }
    int getWindowHeight() const { return windowHeight; }
    std::string getTextureID() const { return textureID; }
    int getCallbackID() const { return callbackID; }

private:

    int surfaceWidth;
    int surfaceHeight;

    int windowWidth;
    int windowHeight;

    int callbackID;

    std::string textureID;

};

#endif
