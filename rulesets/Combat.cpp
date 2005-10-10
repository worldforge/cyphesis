// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "rulesets/Combat.h"

#include "rulesets/Character.h"

#include "common/Attack.h"
#include "common/Tick.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

using Atlas::Objects::Operation::Attack;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Tick;

/// \brief Constructor for Fell task
///
/// @param chr Character peforming the task
/// @param tool Entity to be used as a tool for this task
/// @param target Entity that is the target for this task
Combat::Combat(Character & chr, Character & target) : Task(chr),
                                                      m_target(target),
                                                      m_attack(true)
{
}

Combat::~Combat()
{
}

// FIXME Should this be what the default implemntation of this method does?
void Combat::setup(OpVector & res)
{
    Tick t;
    t->setAttr("sub_to", "task");
    t->setTo(m_character.getId());
    res.push_back(t);
}

void Combat::TickOperation(const Operation & op, OpVector & res)
{
    std::cout << "Combat::TickOperation " 
              << (m_attack ? "attack" : "defend") << std::endl << std::flush;
    Tick t;
    t->setAttr("sub_to", "task");
    t->setTo(m_character.getId());
    t->setFutureSeconds(1.75);
    res.push_back(t);

    const std::string & attacker = m_attack ? m_character.getId() : m_target.getId();
    const std::string & defender = m_attack ? m_target.getId() : m_character.getId();

    Sight s;
    Attack a;
    a->setTo(defender);
    a->setFrom(attacker);
    s->setArgs1(a);
    res.push_back(s);

    m_attack = !m_attack;

    // Defend op
}
