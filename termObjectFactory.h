#ifndef __TERMOBJECT_Factory__
#define __TERMOBJECT_Factory__

// EtherTerm SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#include <string>
#include <map>
#include <iostream>
#include "termObject.h"

class BaseCreator
{
public:
    
    virtual TermObject* createTermObject() const = 0;
    virtual ~BaseCreator() {}
};

class TermObjectFactory
{
public:
    
    static TermObjectFactory* Instance()
    {
        if(globalInstance == 0)
        {
            globalInstance = new TermObjectFactory();
        }
        return globalInstance;
    }

    // Release And Clear the Singleton
    static void ReleaseInstance()
    {
        if(globalInstance != 0)
        {
            delete globalInstance;
            globalInstance = 0;
        }
        return;
    }
    
    bool registerType(std::string typeID, BaseCreator* globalCreator)
    {
        std::map<std::string, BaseCreator*>::iterator it = creators.find(typeID);
        // if the type is already registered, do nothing
        if(it != creators.end())
        {
            delete globalCreator;
        }
        creators[typeID] = globalCreator;
        return true;
    }
    
    TermObject* create(std::string typeID)
    {
        std::map<std::string, BaseCreator*>::iterator it = creators.find(typeID);
        if(it == creators.end())
        {
            std::cout << "could not find type: " << typeID << "\n";
            return nullptr;
        }
        BaseCreator* globalCreator = (*it).second;
        return globalCreator->createTermObject();
    }
    
private:
    
    TermObjectFactory() {}
    ~TermObjectFactory() {}
    
    std::map<std::string, BaseCreator*> creators;
    static TermObjectFactory* globalInstance;
};

typedef TermObjectFactory TheTermObjectFactory;

#endif
