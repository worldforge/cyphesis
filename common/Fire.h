// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_FIRE_H
#define COMMON_FIRE_H

namespace Atlas { namespace Objects { namespace Operation {

class Fire : public RootOperation {
  public:
    Fire() : RootOperation("fire", "root_operation") {
    }
    virtual ~Fire() { }
    static Fire Instantiate() {
        Fire value;
        value.SetParents(Message::Object::ListType(1, "fire"));
        value.SetObjtype(string("op"));
        return value;
    }
};

} } }

#endif // COMMON_FIRE_H
