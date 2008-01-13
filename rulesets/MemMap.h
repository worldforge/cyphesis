// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id: MemMap.h,v 1.54 2008-01-13 01:32:55 alriddoch Exp $

#ifndef RULESETS_MEM_MAP_H
#define RULESETS_MEM_MAP_H

#include "common/OperationRouter.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/ObjectsFwd.h>

#include <string>
#include <list>

class Location;
class MemEntity;
class Script;
class TypeNode;

typedef std::vector<MemEntity *> MemEntityVector;
typedef std::map<long, MemEntity *> MemEntityDict;

/// \brief Class to handle the basic entity memory of a mind
class MemMap {
  private:
    friend class BaseMind;

    static const TypeNode * m_entity_type;

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

    void sendLooks(OpVector &);
    void del(const std::string & id);
    MemEntity * get(const std::string & id) const;
    MemEntity * getAdd(const std::string & id);
    MemEntity * updateAdd(const Atlas::Objects::Entity::RootEntity &, const double &);

    MemEntityVector findByType(const std::string & what);
    MemEntityVector findByLocation(const Location & where, double radius,
                                   const std::string & what);

    const Atlas::Message::Element asMessage();

    void check(const double &);
    void flush();

    std::vector<std::string> & getAddHooks() { return m_addHooks; }
    std::vector<std::string> & getUpdateHooks() { return m_updateHooks; }
    std::vector<std::string> & getDeleteHooks() { return m_deleteHooks; }
};

#endif // RULESETS_MEM_MAP_H
