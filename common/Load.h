// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_LOAD_H
#define COMMON_LOAD_H

namespace Atlas { namespace Objects { namespace Operation {

class Load : public RootOperation {
  public:
    Load() : RootOperation("load", "root_operation") {
    }
    virtual ~Load() { }
    static Load Instantiate() {
        Load value;
        value.SetParents(Message::Object::ListType(1, "load"));
        value.SetObjtype(string("op"));
        return value;
    }
};

} } }

#endif // COMMON_LOAD_H
