// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef COMMON_NOURISH_H
#define COMMON_NOURISH_H

namespace Atlas { namespace Objects { namespace Operation {

class Nourish : public RootOperation {
  public:
    Nourish() : RootOperation() {
        SetId(string("nourish"));
        Message::Object::ListType parents;
        parents.push_back(string("root_operation"));
        SetParents(parents);
    }
    virtual ~Nourish() { }
    static Nourish Instantiate() {
        Nourish value;
        Message::Object::ListType parents;
        parents.push_back(string("nourish"));
        value.SetParents(parents);
        value.SetObjtype(string("op"));
        return value;
    }
};

} } }

#endif /* COMMON_NOURISH_H */
