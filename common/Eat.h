// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_EAT_H
#define COMMON_EAT_H

namespace Atlas { namespace Objects { namespace Operation {

class Eat : public RootOperation {
  public:
    Eat() : RootOperation() {
        SetId(string("eat"));
        Message::Object::ListType parents;
        parents.push_back(string("root_operation"));
        SetParents(parents);
    }
    virtual ~Eat() { }
    static Eat Instantiate() {
        Eat value;
        Message::Object::ListType parents;
        parents.push_back(string("eat"));
        value.SetParents(parents);
        value.SetObjtype(string("op"));
        return value;
    }
};

} } }

#endif // COMMON_EAT_H
