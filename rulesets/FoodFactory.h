// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef FOOD_FACTORY_H
#define FOOD_FACTORY_H

#include "ThingFactory.h"
#include "Food.h"

class FoodFactory : public ThingFactory {
  protected:
    FoodFactory() { }
    static FoodFactory * m_instance;
  public:
    static FoodFactory * instance() {
        if (m_instance == NULL) {
            m_instance = new FoodFactory();
        }
        return m_instance;
    }
    virtual Food * newThing();
};

#endif /* FOOD_FACTORY_H */
