// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_OOG_THING_H
#define SERVER_OOG_THING_H

#include "BaseEntity.h"

class OOGThing : public BaseEntity {
  protected:
    OOGThing(const std::string & id) : BaseEntity(id) { }
    OOGThing() { }
  public:
    virtual ~OOGThing();
    virtual OpVector OtherOperation(const RootOperation & op);
};

#endif // SERVER_OOG_THING_H
