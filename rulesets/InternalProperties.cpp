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

#include "InternalProperties.h"

#include "rulesets/Entity.h"

#include "common/Setup.h"
#include "common/Tick.h"

#include <iostream>

using Atlas::Objects::Operation::Setup;
using Atlas::Objects::Operation::Tick;

SetupProperty::SetupProperty()
{
}

void SetupProperty::install(Entity * ent)
{
    Setup s;
    s->setTo(ent->getId());
    ent->sendWorld(s);
}

TickProperty::TickProperty()
{
}

void TickProperty::apply(Entity * ent)
{
    Tick t;
    t->setTo(ent->getId());
    if (m_data > 0) {
        t->setFutureSeconds(m_data);
    }
    ent->sendWorld(t);
}
