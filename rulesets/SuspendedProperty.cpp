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


#include "SuspendedProperty.h"
#include "common/BaseWorld.h"
#include "Entity.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/RootEntity.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::RootEntity;

SuspendedProperty::SuspendedProperty()
{
}

SuspendedProperty * SuspendedProperty::copy() const
{
    return new SuspendedProperty(*this);
}

void SuspendedProperty::apply(LocatedEntity * ent)
{
	//Only apply if the entity which this is set on is the root world entity.
	if (ent->getIntId() == 0) {
		BaseWorld::instance().setIsSuspended(m_data);
	}
}

