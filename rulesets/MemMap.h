// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_MEM_MAP_H
#define RULESETS_MEM_MAP_H

#include "common/types.h"

#include <string>
#include <list>

namespace Atlas {
  namespace Objects { namespace Operation {
    class RootOperation;
  } }
}

class MemEntity;
class Script;
class Location;

typedef std::vector<MemEntity *> MemEntityVector;
typedef std::map<std::string, MemEntity *> MemEntityDict;

class MemMap {
  private:
    friend class BaseMind;

    MemEntityDict m_entities;
    MemEntityDict::iterator m_checkIterator;
    std::list<std::string> m_additionsById;
    std::vector<std::string> m_addHooks;
    std::vector<std::string> m_updateHooks;
    std::vector<std::string> m_deleteHooks;
    Script *& m_script;

    MemEntity * addEntity(MemEntity *);
    void readEntity(MemEntity *, const MapType &);
    void updateEntity(MemEntity *, const MapType &);
    MemEntity * newEntity(const std::string &, const MapType &);
    void addContents(const MapType &);
    MemEntity * addId(const std::string & id);
  public:
    explicit MemMap(Script *& s);

    bool find(const std::string & id) const {
        return (m_entities.find(id) != m_entities.end());
    }

    const MemEntityDict & getEntities() const {
        return m_entities;
    }

    Atlas::Objects::Operation::RootOperation * lookId();
    void del(const std::string & id);
    MemEntity * get(const std::string & id) const;
    MemEntity * getAdd(const std::string & id);
    MemEntity * updateAdd(const MapType &, const double &);

    MemEntityVector findByType(const std::string & what);
    MemEntityVector findByLocation(const Location & where, double radius);

    const Element asObject();

    void check(const double &);
    void flush();

    std::vector<std::string> & getAddHooks() { return m_addHooks; }
    std::vector<std::string> & getUpdateHooks() { return m_updateHooks; }
    std::vector<std::string> & getDeleteHooks() { return m_deleteHooks; }
};

#endif // RULESETS_MEM_MAP_H
