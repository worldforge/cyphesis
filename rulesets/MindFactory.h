// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include <varconf/Config.h>

#include <string>

#include <common/globals.h>

class BaseMind;
class Routing;

class MindFactory {
    MindFactory();
    static MindFactory * m_instance;

  public:
    static MindFactory * instance() {
        if (m_instance == NULL) {
            m_instance = new MindFactory();
        }
        return m_instance;
    }
    void readRuleset(const string & file);
    BaseMind * newMind(const string &, const string &, const string &,
                       const Atlas::Message::Object &, Routing *);
};

#endif /* ENTITY_FACTORY_H */
