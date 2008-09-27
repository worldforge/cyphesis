// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2007 Alistair Riddoch
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

#include "StatusProperty.h"

#include "Entity.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Message::Element;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Entity::Anonymous;

StatusProperty::StatusProperty()
{
}

void StatusProperty::apply(Entity * owner)
{
    if (m_data < 0) {
        Delete del;
        Anonymous delete_arg;
        delete_arg->setId(owner->getId());
        del->setArgs1(delete_arg);
        del->setTo(owner->getId());
        owner->sendWorld(del);
    }
}
