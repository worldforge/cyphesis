// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_MEM_MAP_H
#define RULESETS_MEM_MAP_H

#include "common/types.h"

#include <string>
#include <list>

namespace Atlas {
  namespace Message {
    class Object;
  }
  namespace Objects { namespace Operation {
    class RootOperation;
  } }
}

class Entity;
class Script;
class Location;

class MemMap {
  private:
    friend class BaseMind;

    EntityDict m_entities;
    std::list<std::string> m_additionsById;
    std::vector<std::string> m_addHooks;
    std::vector<std::string> m_updateHooks;
    std::vector<std::string> m_deleteHooks;
    Script *& m_script;

    Entity * addEntity(Entity *);
    void readEntity(Entity *, const Atlas::Message::Element::MapType &);
    void updateEntity(Entity *, const Atlas::Message::Element::MapType &);
    Entity * newEntity(const std::string &,
                       const Atlas::Message::Element::MapType &);
    void addContents(const Atlas::Message::Element::MapType &);
    Entity * addId(const std::string & id);
  public:
    explicit MemMap(Script *& s) : m_script(s)  { }

    bool find(const std::string & id) {
        return (m_entities.find(id) != m_entities.end());
    }

    Atlas::Objects::Operation::RootOperation * lookId();
    void del(const std::string & id);
    Entity * get(const std::string & id);
    Entity * getAdd(const std::string & id);
    Entity * updateAdd(const Atlas::Message::Element::MapType &);

    EntityVector findByType(const std::string & what);
    EntityVector findByLocation(const Location & where, double radius);

    const Atlas::Message::Element asObject();
    void flush();

    std::vector<std::string> & getAddHooks() { return m_addHooks; }
    std::vector<std::string> & getUpdateHooks() { return m_updateHooks; }
    std::vector<std::string> & getDeleteHooks() { return m_deleteHooks; }
};

#endif // RULESETS_MEM_MAP_H
