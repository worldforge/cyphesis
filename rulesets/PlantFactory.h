// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef PLANT_FACTORY_H
#define PLANT_FACTORY_H

#include "ThingFactory.h"
#include "Plant.h"

class PlantFactory : public ThingFactory {
  protected:
    PlantFactory() { }
    static PlantFactory * m_instance;
  public:
    static PlantFactory * instance() {
        if (m_instance == NULL) {
            m_instance = new PlantFactory();
        }
        return m_instance;
    }
    virtual Plant * newThing();
};

#endif /* PLANT_FACTORY_H */
