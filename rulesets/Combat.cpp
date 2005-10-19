// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "rulesets/Combat.h"

#include "rulesets/Character.h"

#include "common/random.h"

#include "common/Attack.h"
#include "common/Tick.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/SmartPtr.h>

using Atlas::Objects::Operation::Attack;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Tick;

using Atlas::Objects::Entity::Anonymous;

const double Combat::minStamina = 0.1;

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

// FIXME Should this be the default implemntation of this method in Task?
void Combat::setup(OpVector & res)
{
    if (m_character.getStamina() < minStamina ||
        m_target.getStamina() < minStamina) {
        irrelevant();
        std::cout << "Too weak to fight" << std::endl << std::flush;
        return;
    }


    Tick t;
    t->setAttr("sub_to", "task");
    t->setTo(m_character.getId());
    res.push_back(t);
}

void Combat::irrelevant()
{
    m_target.clearTask();
    Task::irrelevant();
}

void Combat::TickOperation(const Operation & op, OpVector & res)
{
    Character & attacker = m_attack ? m_character : m_target;
    Character & defender = m_attack ? m_target : m_character;

    if (attacker.getStamina() <= 0.f || defender.getStamina() <= 0.f) {
        irrelevant();
        std::cout << "A character has been defeated" << std::endl << std::flush;
        // FIXME Make the right person obviously lose.
        return;
    }

    std::cout << "Combat::TickOperation " 
              << (m_attack ? "attack" : "defend") << std::endl << std::flush;
    Tick t;
    t->setAttr("sub_to", "task");
    t->setTo(m_character.getId());
    t->setFutureSeconds(1.75);
    res.push_back(t);

    Sight s;
    Attack a;
    a->setTo(defender.getId());
    a->setFrom(attacker.getId());
    s->setArgs1(a);
    res.push_back(s);

    m_attack = !m_attack;

    float damage = (attacker.statistics().attack() / attacker.statistics().defence()) / uniform(2.f, 10.f);

    float stamina = std::max(defender.getStamina() - damage, 0.);

    Set set;
    Anonymous set_arg;
    set_arg->setId(defender.getId());
    set_arg->setAttr("stamina", stamina);
    set->setArgs1(set_arg);
    set->setTo(defender.getId());
    res.push_back(set);
    std::cout << "Defender: " << defender.getStamina() << std::endl << std::flush;

    attacker.statistics().increment("combat", res);
    // FIXME Handle skill/experience increment?

    // Defend op
}
