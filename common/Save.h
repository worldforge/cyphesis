// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_SAVE_H
#define COMMON_SAVE_H

namespace Atlas { namespace Objects { namespace Operation {

class Save : public RootOperation {
  public:
    Save() : RootOperation() {
        SetId(string("save"));
        Message::Object::ListType parents;
        parents.push_back(string("root_operation"));
        SetParents(parents);
    }
    virtual ~Save() { }
    static Save Instantiate() {
        Save value;
        Message::Object::ListType parents;
        parents.push_back(string("save"));
        value.SetParents(parents);
        value.SetObjtype(string("op"));
        return value;
    }
};

} } }

#endif // COMMON_SAVE_H
