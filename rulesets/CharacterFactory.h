// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef CHARACTER_FACTORY_H
#define CHARACTER_FACTORY_H

#include "ThingFactory.h"
#include "Character.h"

class CharacterFactory : public ThingFactory {
  protected:
    CharacterFactory() { }
    static CharacterFactory * m_instance;
  public:
    static CharacterFactory * instance() {
        if (m_instance == NULL) {
            m_instance = new CharacterFactory();
        }
        return m_instance;
    }
    virtual Character * newThing();
};

#endif /* CHARACTER_FACTORY_H */
