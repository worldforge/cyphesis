// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef COMMON_FIRE_H
#define COMMON_FIRE_H

namespace Atlas { namespace Objects { namespace Operation {

class Fire : public RootOperation {
  public:
    Fire() : RootOperation() {
        SetId(string("fire"));
        Message::Object::ListType parents;
        parents.push_back(string("root_operation"));
        SetParents(parents);
    }
    virtual ~Fire() { }
    static Fire Instantiate() {
        Fire value;
        Message::Object::ListType parents;
        parents.push_back(string("fire"));
        value.SetParents(parents);
        value.SetObjtype(string("op"));
        return value;
    }
};

} } }

#endif /* COMMON_FIRE_H */
