// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef PLANT_FACTORY_H
#define PLANT_FACTORY_H

#include "ThingFactory.h"
#include "Plant.h"

class PlantFactory : public ThingFactory {
  public:
    PlantFactory() { }
    virtual Plant * newThing();
};

#endif // PLANT_FACTORY_H
