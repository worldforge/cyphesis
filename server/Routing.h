// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_ROUTING_H
#define SERVER_ROUTING_H

#include <common/OOGThing.h>

class Routing : public OOGThing {
  protected:
    dict_t objects;
  public:

    Routing() { }
    virtual ~Routing();

    BaseEntity * addObject(BaseEntity * obj) {
        objects[obj->fullid]=obj;
        return obj;
    }

    void delObject(BaseEntity * obj) {
        objects.erase(obj->fullid);
        delete obj;
    }

    BaseEntity * getObject(const std::string & fid) const {
        return Routing::objects[fid];
    }

    BaseEntity * findObject(const std::string & fid) const {
        return Routing::objects[fid];
    }
};

#endif // SERVER_ROUTING_H
