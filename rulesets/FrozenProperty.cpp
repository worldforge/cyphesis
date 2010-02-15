// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Erik Hjortsberg
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

// $Id$

#include "FrozenProperty.h"
#include "common/BaseWorld.h"
#include "Entity.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/RootEntity.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::RootEntity;

FrozenProperty::FrozenProperty() : m_data(false)
{
}

void FrozenProperty::apply(Entity * ent)
{
	//Only apply if the entity which this is set on is the root world entity.
	if (ent->getIntId() == 0) {
		BaseWorld::instance().setIsFrozen(m_data);
	}
}

bool FrozenProperty::get(Element & val) const
{
    if (m_data) {
        val = 1;
        return true;
    }
    return false;
}

void FrozenProperty::set(const Element & val)
{
	if (val.isInt()) {
		if (val.asInt() != 0) {
			m_data = true;
		} else {
			m_data = false;
		}
	}
}

void FrozenProperty::add(const std::string & key,
                       MapType & map) const
{
    if (m_data) {
        map[key] = 1;
    }
}

void FrozenProperty::add(const std::string & key,
                       const RootEntity & ent) const
{
    if (m_data) {
        ent->setAttr(key, 1);
    }
}
