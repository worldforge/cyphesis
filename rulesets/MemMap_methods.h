// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_MEM_MAP_METHODS_H
#define RULESETS_MEM_MAP_METHODS_H

#include "MemMap.h"

#include "Entity.h"
#include "Script.h"

#include "modules/Location.h"

#include "common/debug.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Look.h>

static const bool debug_flag = false;

inline Entity * MemMap::addObject(Entity * object)
{
    debug(std::cout << "MemMap::addObject " << object << " " << object->getId()
                    << std::endl << std::flush;);
    m_entities[object->getId()] = object;

    debug( std::cout << this << std::endl << std::flush;);
    std::vector<std::string>::const_iterator I;
    for(I = m_addHooks.begin(); I != m_addHooks.end(); I++) {
        m_script->hook(*I, object);
    }
    return object;
}

inline RootOperation * MemMap::lookId()
{
    debug( std::cout << "MemMap::lookId" << std::endl << std::flush;);
    if (!m_additionsById.empty()) {
        const std::string & id = m_additionsById.front();
        Look * l = new Look(Look::Instantiate());
        Atlas::Message::Element::MapType m;
        m["id"] = Atlas::Message::Element(id);
        l->setArgs(Atlas::Message::Element::ListType(1, m));
        l->setTo(id);
        m_additionsById.pop_front();
        return l;
    }
    return NULL;
}

inline Entity * MemMap::addId(const std::string & id)
{
    debug( std::cout << "MemMap::add_id" << std::endl << std::flush;);
    m_additionsById.push_back(id);
    Atlas::Message::Element::MapType m;
    m["id"] = Atlas::Message::Element(id);
    return add(m);
}

inline void MemMap::del(const std::string & id)
{
    EntityDict::iterator I = m_entities.find(id);
    if (I != m_entities.end()) {
        Entity * obj = I->second;
        m_entities.erase(I);
        std::vector<std::string>::const_iterator J;
        for(J = m_deleteHooks.begin(); J != m_deleteHooks.end(); J++) {
            m_script->hook(*J, obj);
        }
        delete obj;
    }
}

inline Entity * MemMap::get(const std::string & id)
{
    debug( std::cout << "MemMap::get" << std::endl << std::flush;);
    if (id.empty()) { return NULL; }
    EntityDict::const_iterator I = m_entities.find(id);
    if (I != m_entities.end()) {
        return I->second;
    }
    return NULL;
}

inline Entity * MemMap::getAdd(const std::string & id)
{
    debug( std::cout << "MemMap::getAdd" << std::endl << std::flush;);
    if (id.empty()) { return NULL; }
    EntityDict::const_iterator I = m_entities.find(id);
    if (I != m_entities.end()) {
        return I->second;
    }
    return addId(id);
}

#endif // RULESETS_MEM_MAP_METHODS_H
