// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef COMMON_CUT_H
#define COMMON_CUT_H

namespace Atlas { namespace Objects { namespace Operation {

class Cut : public RootOperation {
  public:
    Cut() : RootOperation() {
        SetId(string("cut"));
        Message::Object::ListType parents;
        parents.push_back(string("root_operation"));
        SetParents(parents);
    }
    virtual ~Cut() { }
    static Cut Instantiate() {
        Cut value;
        Message::Object::ListType parents;
        parents.push_back(string("cut"));
        value.SetParents(parents);
        value.SetObjtype(string("op"));
        return value;
    }
};

} } }

#endif /* COMMON_CUT_H */
