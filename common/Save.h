// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_SAVE_H
#define COMMON_SAVE_H

namespace Atlas { namespace Objects { namespace Operation {

class Save : public RootOperation {
  public:
    Save() : RootOperation("save", "root_operation") {
    }
    virtual ~Save() { }
    static Save Instantiate() {
        Save value;
        value.SetParents(Message::Object::ListType(1,"save"));
        value.SetObjtype(string("op"));
        return value;
    }
};

} } }

#endif // COMMON_SAVE_H
