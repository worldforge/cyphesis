// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef FOOD_FACTORY_H
#define FOOD_FACTORY_H

#include "ThingFactory.h"
#include "Food.h"

class FoodFactory : public ThingFactory {
  public:
    FoodFactory() { }
    virtual Food * newThing();
};

#endif // FOOD_FACTORY_H
