// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_TICK_H
#define COMMON_TICK_H

namespace Atlas { namespace Objects { namespace Operation {

class Tick : public RootOperation {
  public:
    Tick() : RootOperation() {
        SetId(string("tick"));
        Message::Object::ListType parents;
        parents.push_back(string("root_operation"));
        SetParents(parents);
    }
    virtual ~Tick() { }
    static Tick Instantiate() {
        Tick value;
        Message::Object::ListType parents;
        parents.push_back(string("tick"));
        value.SetParents(parents);
        value.SetObjtype(string("op"));
        return value;
    }
};

} } }

#endif // COMMON_TICK_H
