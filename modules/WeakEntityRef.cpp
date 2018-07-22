// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
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


#include "WeakEntityRef.h"

#include "rulesets/LocatedEntity.h"

void WeakEntityRef::setup()
{
    if (m_inner) {
        m_inner->destroyed.connect(sigc::mem_fun(this, &WeakEntityRef::onEntityDeleted));
    }
}

WeakEntityRef::WeakEntityRef(LocatedEntity* e) : m_inner(e)
{
    setup();
}

WeakEntityRef::WeakEntityRef(const WeakEntityRef& ref) : m_inner(ref.m_inner)
{
    setup();
}

WeakEntityRef& WeakEntityRef::operator=(const WeakEntityRef& ref)
{
    bool changed = (m_inner != ref.m_inner);
    m_inner = ref.m_inner;

    setup();

    if (changed) Changed.emit();
    return *this;
}

void WeakEntityRef::onEntityDeleted()
{
    m_inner = nullptr;
    Changed();
}
