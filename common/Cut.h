// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_CUT_H
#define COMMON_CUT_H

namespace Atlas { namespace Objects { namespace Operation {

class Cut : public RootOperation {
  public:
    Cut() : RootOperation("cut", "root_operation") {
    }
    virtual ~Cut() { }
    static Cut Instantiate() {
        Cut value;
        value.SetParents(Message::Object::ListType(1,std::string("cut")));
        value.SetObjtype(std::string("op"));
        return value;
    }
};

} } }

#endif // COMMON_CUT_H
