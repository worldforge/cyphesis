// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2005 Alistair Riddoch
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


#include "rulesets/MemMap.h"

const TypeNode * MemMap::m_entity_type = 0;

MemEntity * MemMap::addEntity(MemEntity * entity)
{
    return nullptr;
}

void MemMap::readEntity(MemEntity * entity, const RootEntity & ent)
{

}

void MemMap::updateEntity(MemEntity * entity, const RootEntity & ent)
{
}

MemEntity * MemMap::newEntity(const std::string & id, long int_id,
                              const RootEntity & ent)
{
    return nullptr;
}

MemMap::MemMap(Script *& s) : m_script(s)
{
}

void MemMap::sendLooks(OpVector & res)
{
}

MemEntity * MemMap::addId(const std::string & id, long int_id)
{
    return nullptr;
}

void MemMap::del(const std::string & id)
{

}

MemEntity * MemMap::get(const std::string & id) const
{
    return NULL;
}

MemEntity * MemMap::getAdd(const std::string & id)
{
    return NULL;

}

void MemMap::addContents(const RootEntity & ent)
{

}

MemEntity * MemMap::updateAdd(const RootEntity & ent, const double & d)
{
    return NULL;
}

void MemMap::addEntityMemory(const std::string& id,
                             const std::string& memory,
                             const Element& value)
{
}

void MemMap::recallEntityMemory(const std::string& id,
                                const std::string& memory,
                                Element& value) const
{
}

const std::map<std::string, std::map<std::string, Element>>& MemMap::getEntityRelatedMemory() const
{
    return m_entityRelatedMemory;
}


EntityVector MemMap::findByType(const std::string & what)
{
    EntityVector res;
    return res;
}

EntityVector MemMap::findByLocation(const Location & loc,
                                       WFMath::CoordType radius,
                                       const std::string & what)
{
    EntityVector res;
    return res;
}

void MemMap::check(const double & time)
{

}

void MemMap::flush()
{
}
