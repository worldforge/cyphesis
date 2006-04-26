// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004-2006 Alistair Riddoch
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

#include "Fell.h"

#include "rulesets/Character.h"

#include "common/Tick.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

using Atlas::Objects::Operation::Tick;

/// \brief Constructor for Fell task
///
/// @param chr Character peforming the task
/// @param tool Entity to be used as a tool for this task
/// @param target Entity that is the target for this task
Fell::Fell(Character & chr, Entity & tool, Entity & target) : Task(chr)
{
}

Fell::~Fell()
{
}

// FIXME Should this be what the default implemntation of this method does?
void Fell::initTask(const Operation & op, OpVector & res)
{
    Tick t;
    t->setAttr("sub_to", "task");
    t->setTo(m_character.getId());
}

void Fell::TickOperation(const Operation & op, OpVector & res)
{
}
