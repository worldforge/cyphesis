// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
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


#include "TransientProperty.h"

#include "LocatedEntity.h"

#include "common/Setup.h"
#include "common/log.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Message::Element;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Entity::Anonymous;

TransientProperty::TransientProperty()
{
}

TransientProperty::~TransientProperty()
{
}

TransientProperty * TransientProperty::copy() const
{
    return new TransientProperty(*this);
}

void TransientProperty::install(LocatedEntity * ent, const std::string & name)
{
    ent->setFlags(entity_ephem);
}

void TransientProperty::apply(LocatedEntity * ent)
{
    // If data is less than zero we don't ever delete it.
    if (m_data < 0) {
        return;
    }
    Set s;
    s->setTo(ent->getId());
    s->setFutureSeconds(m_data);

    Anonymous set_arg;
    set_arg->setId(ent->getId());
    set_arg->setAttr("status", -1);
    s->setArgs1(set_arg);

    ent->sendWorld(s);
}
