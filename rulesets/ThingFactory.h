// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef THING_FACTORY_H
#define THING_FACTORY_H

#include "Thing.h"

class ThingFactory {
  protected:
    ThingFactory() { }
    static ThingFactory * m_instance;
  public:
    static ThingFactory * instance() {
        if (m_instance == NULL) {
            m_instance = new ThingFactory();
        }
        return m_instance;
    }
    virtual Thing * newThing();
};

#endif /* THING_FACTORY_H */
