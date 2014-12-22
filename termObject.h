#ifndef __TERM_OBJECT__
#define __TERM_OBJECT__

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include "loaderParams.h"
//#include "Vector2D.h"
#include <string>
#include <memory>

class TermObject
{
public:

    // base class needs virtual destructor
    virtual ~TermObject() {}

    // load from file - int x, int y, int width, int height, std::string textureID, int numFrames, int callbackID = 0, int animSpeed = 0
    virtual void load(std::unique_ptr<LoaderParams> const &Params)=0;

    // draw the object
    virtual void draw()=0;

    // do update stuff
    virtual void update()=0;

    // remove anything that needs to be deleted
    virtual void clean()=0;

    // get the type of the object
    virtual std::string type() = 0;

    int getSurfaceWidth() { return surfaceWidth; }
    int getSurfaceHeight() { return surfaceHeight; }

    int getWindowWidth() { return windowWidth; }
    int getWindowHeight() { return windowHeight; }

    // is the object currently being updated?
    bool updating() { return isUpdating; }

    // set whether to update the object or not
    void setUpdating(bool updating) { isUpdating = updating; }

protected:

    // constructor with default initialisation list
    TermObject() :  surfaceWidth(0),
                    surfaceHeight(0),
                    windowWidth(0),
                    windowHeight(0),
                    isUpdating(false)
    {  }

    // size variables
    int surfaceWidth;
    int surfaceHeight;

    int windowWidth;
    int windowHeight;


    std::string textureID;

    // common boolean variables
    bool isUpdating;

};

#endif
