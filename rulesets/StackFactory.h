// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef STACK_FACTORY_H
#define STACK_FACTORY_H

#include "ThingFactory.h"
#include "Stackable.h"

class StackFactory : public ThingFactory {
  protected:
    StackFactory() { }
    static StackFactory * m_instance;
  public:
    static StackFactory * instance() {
        if (m_instance == NULL) {
            m_instance = new StackFactory();
        }
        return m_instance;
    }
    virtual Stackable * newThing();
};

#endif /* STACK_FACTORY_H */
