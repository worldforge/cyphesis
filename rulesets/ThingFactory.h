// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef THING_FACTORY_H
#define THING_FACTORY_H

#include "Thing.h"

class ThingFactory {
  public:
    ThingFactory() { }
    virtual Thing * newThing();
};

#endif // THING_FACTORY_H
