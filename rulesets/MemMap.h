// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_MEM_MAP_H
#define RULESETS_MEM_MAP_H

#include <string>
#include <list>

#include <Atlas/Objects/ObjectsFwd.h>

class MemEntity;
class Script;
class Location;

typedef std::vector<MemEntity *> MemEntityVector;
typedef std::map<long, MemEntity *> MemEntityDict;

/// \brief Class to handle the basic entity memory of a mind
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
    void readEntity(MemEntity *, const Atlas::Objects::Entity::RootEntity &);
    void updateEntity(MemEntity *, const Atlas::Objects::Entity::RootEntity &);
    MemEntity * newEntity(const std::string &, long,
                          const Atlas::Objects::Entity::RootEntity &);
    void addContents(const Atlas::Objects::Entity::RootEntity &);
    MemEntity * addId(const std::string &, long);
  public:
    explicit MemMap(Script *& s);

    bool find(const std::string & id) const;

    bool find(long id) const {
        return (m_entities.find(id) != m_entities.end());
    }

    const MemEntityDict & getEntities() const {
        return m_entities;
    }

    Atlas::Objects::Operation::RootOperation lookId();
    void del(const std::string & id);
    MemEntity * get(const std::string & id) const;
    MemEntity * getAdd(const std::string & id);
    MemEntity * updateAdd(const Atlas::Objects::Entity::RootEntity &, const double &);

    MemEntityVector findByType(const std::string & what);
    MemEntityVector findByLocation(const Location & where, double radius);

    const Atlas::Message::Element asMessage();

    void check(const double &);
    void flush();

    std::vector<std::string> & getAddHooks() { return m_addHooks; }
    std::vector<std::string> & getUpdateHooks() { return m_updateHooks; }
    std::vector<std::string> & getDeleteHooks() { return m_deleteHooks; }
};

#endif // RULESETS_MEM_MAP_H
