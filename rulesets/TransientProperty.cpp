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

// $Id$

#include "TransientProperty.h"

#include "Entity.h"

#include "common/OperationRouter.h"
#include "common/Setup.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Message::Element;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Entity::Anonymous;

TransientProperty::TransientProperty()
{
}

void TransientProperty::install(Entity * ent)
{
    ent->setFlags(entity_ephem);
}

void TransientProperty::apply(Entity * ent)
{
    Set s;
    s->setTo(ent->getId());
    s->setFutureSeconds(m_data);

    Anonymous set_arg;
    set_arg->setId(ent->getId());
    set_arg->setAttr("status", -1);
    s->setArgs1(set_arg);

    ent->sendWorld(s);
}
