// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef CHARACTER_FACTORY_H
#define CHARACTER_FACTORY_H

#include "ThingFactory.h"
#include "Character.h"

class CharacterFactory : public ThingFactory {
  public:
    CharacterFactory() { }
    virtual Character * newThing();
};

#endif // CHARACTER_FACTORY_H
