// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#error This file has been removed from the build

#ifndef SERVER_ROUTING_H
#define SERVER_ROUTING_H

#include <common/OOGThing.h>

class Routing : public OOGThing {
  protected:
    BaseDict objects;
  public:

    Routing() { }
    virtual ~Routing();

    BaseEntity * addObject(BaseEntity * obj) {
        objects[obj->getId()]=obj;
        return obj;
    }

    BaseEntity * getObject(const std::string & fid) const {
        BaseDict::const_iterator I = objects.find(fid);
        if (I == objects.end()) {
            return NULL;
        } else {
            return I->second;
        }
    }
};

#endif // SERVER_ROUTING_H
