// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_CREATOR_FACTORY_H
#define RULESETS_CREATOR_FACTORY_H

#include "ThingFactory.h"
#include "Creator.h"

class CreatorFactory : public ThingFactory {
  public:
    CreatorFactory() { }
    virtual Creator * newThing();
};

#endif // RULESETS_CREATOR_FACTORY_H
