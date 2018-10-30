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


#include "InternalProperties.h"

#include "rulesets/LocatedEntity.h"

#include "common/operations/Setup.h"
#include "common/operations/Tick.h"

#include <iostream>

using Atlas::Message::Element;
using Atlas::Objects::Operation::Setup;
using Atlas::Objects::Operation::Tick;

SetupProperty * SetupProperty::copy() const
{
    return new SetupProperty(*this);
}

void SetupProperty::install(LocatedEntity * ent, const std::string & name)
{
    Setup s;
    s->setTo(ent->getId());
    ent->sendWorld(s);
}

TickProperty * TickProperty::copy() const
{
    return new TickProperty(*this);
}

void TickProperty::apply(LocatedEntity * ent)
{
    Tick t;
    t->setTo(ent->getId());
    if (m_data > 0) {
        t->setFutureSeconds(m_data);
    }
    ent->sendWorld(t);
}

SimpleProperty * SimpleProperty::copy() const
{
    return new SimpleProperty(*this);
}

void SimpleProperty::apply(LocatedEntity * owner)
{
    owner->m_location.setSimple(isTrue());
}
