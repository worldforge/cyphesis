// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef CREATOR_FACTORY_H
#define CREATOR_FACTORY_H

#include "ThingFactory.h"
#include "Creator.h"

class CreatorFactory : public ThingFactory {
  protected:
    CreatorFactory() { }
    static CreatorFactory * m_instance;
  public:
    static CreatorFactory * instance() {
        if (m_instance == NULL) {
            m_instance = new CreatorFactory();
        }
        return m_instance;
    }
    virtual Creator * newThing();
};

#endif /* CREATOR_FACTORY_H */
