// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef OOG_THING_H
#define OOG_THING_H

#include <common/BaseEntity.h>

class OOGThing : public BaseEntity {
  public:
    OOGThing() { }
    virtual ~OOGThing() { }
    virtual oplist Operation(const RootOperation & op);
};

#endif /* OOG_THING_H */
