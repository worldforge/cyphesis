// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_SETUP_H
#define COMMON_SETUP_H

namespace Atlas { namespace Objects { namespace Operation {

class Setup : public RootOperation {
  public:
    Setup() : RootOperation() {
        SetId(string("setup"));
        Message::Object::ListType parents;
        parents.push_back(string("root_operation"));
        SetParents(parents);
    }
    virtual ~Setup() { }
    static Setup Instantiate() {
        Setup value;
        Message::Object::ListType parents;
        parents.push_back(string("setup"));
        value.SetParents(parents);
        value.SetObjtype(string("op"));
        return value;
    }
};

} } }

#endif // COMMON_SETUP_H
