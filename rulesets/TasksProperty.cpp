// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

#include "TasksProperty.h"

#include "Character.h"

#include "common/compose.hpp"
#include "common/log.h"
#include "common/TypeNode.h"

TasksProperty::TasksProperty() : m_task(0)
{
}

bool TasksProperty::get(Atlas::Message::Element & val) const
{
}

void TasksProperty::set(const Atlas::Message::Element & val)
{
}

void TasksProperty::install(Entity * owner)
{
    if (flags() & flag_class) {
        return;
    }
    Character * c = dynamic_cast<Character *>(owner);
    if (c != 0) {
        m_task = c->monitorTask();
        log(NOTICE, String::compose("Bound task property to %1", c->getId()));
    }
}

void TasksProperty::apply(Entity * owner)
{
}
