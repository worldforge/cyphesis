// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_CHOP_H
#define COMMON_CHOP_H

namespace Atlas { namespace Objects { namespace Operation {

class Chop : public RootOperation {
  public:
    Chop() : RootOperation() {
        SetId(string("chop"));
        Message::Object::ListType parents;
        parents.push_back(string("root_operation"));
        SetParents(parents);
    }
    virtual ~Chop() { }
    static Chop Instantiate() {
        Chop value;
        Message::Object::ListType parents;
        parents.push_back(string("chop"));
        value.SetParents(parents);
        value.SetObjtype(string("op"));
        return value;
    }
};

} } }

#endif // COMMON_CHOP_H
