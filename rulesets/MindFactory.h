// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_ENTITY_FACTORY_H
#define RULESETS_ENTITY_FACTORY_H

#include <varconf/Config.h>

#include <string>

#include <common/globals.h>

class BaseMind;
class Routing;

class MindFactory {
  private:
    MindFactory();
    static MindFactory * m_instance;

  public:
    static MindFactory * instance() {
        if (m_instance == NULL) {
            m_instance = new MindFactory();
        }
        return m_instance;
    }
    void readRuleset(const std::string & file);
    BaseMind * newMind(const std::string &, const std::string &,
                       const std::string &, const Atlas::Message::Object &,
                       Routing *);
};

#endif // RULESETS_ENTITY_FACTORY_H
