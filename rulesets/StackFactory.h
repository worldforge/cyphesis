// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef STACK_FACTORY_H
#define STACK_FACTORY_H

#include "ThingFactory.h"
#include "Stackable.h"

class StackFactory : public ThingFactory {
  public:
    StackFactory() { }
    virtual Stackable * newThing();
};

#endif // STACK_FACTORY_H
