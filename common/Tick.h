// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_TICK_H
#define COMMON_TICK_H

namespace Atlas { namespace Objects { namespace Operation {

class Tick : public RootOperation {
  public:
    Tick() : RootOperation("tick", "root_operation") {
    }
    virtual ~Tick() { }
    static Tick Instantiate() {
        Tick value;
        value.SetParents(Message::Object::ListType(1,"tick"));
        value.SetObjtype(string("op"));
        return value;
    }
};

} } }

#endif // COMMON_TICK_H
