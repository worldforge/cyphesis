// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#error This file has been removed from the build

#ifndef SERVER_OOG_THING_H
#define SERVER_OOG_THING_H

#include <common/BaseEntity.h>

class OOGThing : public BaseEntity {
  public:
    OOGThing() { }
    virtual ~OOGThing();
    virtual oplist OtherOperation(const RootOperation & op);
};

#endif // SERVER_OOG_THING_H
