// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_EAT_H
#define COMMON_EAT_H

namespace Atlas { namespace Objects { namespace Operation {

class Eat : public RootOperation {
  public:
    Eat() : RootOperation("eat", "root_operation") {
    }
    virtual ~Eat() { }
    static Eat Instantiate() {
        Eat value;
        value.SetParents(Message::Object::ListType(1,std::string("eat")));
        value.SetObjtype(std::string("op"));
        return value;
    }
};

} } }

#endif // COMMON_EAT_H
