// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_NOURISH_H
#define COMMON_NOURISH_H

namespace Atlas { namespace Objects { namespace Operation {

class Nourish : public RootOperation {
  public:
    Nourish() : RootOperation("nourish", "root_operation") {
    }
    virtual ~Nourish() { }
    static Nourish Instantiate() {
        Nourish value;
        value.SetParents(Message::Object::ListType(1, std::string("nourish")));
        value.SetObjtype(std::string("op"));
        return value;
    }
};

} } }

#endif // COMMON_NOURISH_H
