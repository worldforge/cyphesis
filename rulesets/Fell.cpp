// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "rulesets/Fell.h"

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
void Fell::setup(OpVector & res)
{
    Tick t;
    t->setAttr("sub_to", "task");
    t->setTo(m_character.getId());
}

void Fell::TickOperation(const Operation & op, OpVector & res)
{
}
