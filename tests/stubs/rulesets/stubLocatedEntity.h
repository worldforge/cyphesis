/*
 Copyright (C) 2014 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef STUBLOCATEDENTITY_H_
#define STUBLOCATEDENTITY_H_

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>

LocatedEntity::LocatedEntity(const std::string & id, long intId) :
               Router(id, intId),
               m_refCount(0), m_seq(0),
               m_script(0), m_type(0), m_flags(0), m_contains(0)
{
}

LocatedEntity::~LocatedEntity()
{
}

bool LocatedEntity::hasAttr(const std::string & name) const
{
    return false;
}

int LocatedEntity::getAttr(const std::string & name,
                           Atlas::Message::Element & attr) const
{
    return -1;
}

int LocatedEntity::getAttrType(const std::string & name,
                               Atlas::Message::Element & attr,
                               int type) const
{
    return -1;
}

PropertyBase * LocatedEntity::setAttr(const std::string & name,
                                      const Atlas::Message::Element & attr)
{
    return 0;
}

const PropertyBase * LocatedEntity::getProperty(const std::string & name) const
{
    return 0;
}

PropertyBase * LocatedEntity::modProperty(const std::string & name)
{
    return 0;
}

PropertyBase * LocatedEntity::setProperty(const std::string & name,
                                          PropertyBase * prop)
{
    return 0;
}

void LocatedEntity::installDelegate(int, const std::string &)
{
}

void LocatedEntity::removeDelegate(int class_no, const std::string & delegate)
{
}

void LocatedEntity::destroy()
{
}

Domain * LocatedEntity::getDomain()
{
    return 0;
}

const Domain * LocatedEntity::getDomain() const
{
    return 0;
}

bool LocatedEntity::isVisibleForOtherEntity(const LocatedEntity* watcher) const
{
    return true;
}


void LocatedEntity::sendWorld(const Operation & op)
{
}

void LocatedEntity::onContainered(const LocatedEntity*)
{
}

void LocatedEntity::onUpdated()
{
}

void LocatedEntity::addChild(LocatedEntity& childEntity)
{
}

void LocatedEntity::removeChild(LocatedEntity& childEntity)
{
}

void LocatedEntity::makeContainer()
{
    if (m_contains == 0) {
        m_contains = new LocatedEntitySet;
    }
}

void LocatedEntity::merge(const Atlas::Message::MapType &)
{

}

void LocatedEntity::setType(const TypeNode * t) {
    m_type = t;
}

std::vector<Atlas::Objects::Root> LocatedEntity::getThoughts() const
{
    return std::vector<Atlas::Objects::Root>();
}

std::string LocatedEntity::describeEntity() const
{
    return "";
}

void LocatedEntity::broadcast(const Atlas::Objects::Operation::RootOperation& op, OpVector& res) const
{
}

void LocatedEntity::collectObservers(std::set<const LocatedEntity*>& observers) const
{

}

void LocatedEntity::processAppearDisappear(std::set<const LocatedEntity*> previousObserving, OpVector& res) const
{

}



#endif /* STUBLOCATEDENTITY_H_ */
