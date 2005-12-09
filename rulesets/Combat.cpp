// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "rulesets/Combat.h"

#include "rulesets/Character.h"

#include "common/log.h"
#include "common/random.h"

#include "common/Attack.h"
#include "common/Tick.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/SmartPtr.h>

using Atlas::Objects::Operation::Attack;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Sound;
using Atlas::Objects::Operation::Talk;
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
void Combat::initTask(const Operation & op, OpVector & res)
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

    if (attacker.getStamina() <= 0.f) {
        log(WARNING, "Combat::Tick: Attacker has no stamina at tick");
        irrelevant();
        return;
    }

    Sight s;
    Attack a;
    a->setTo(defender.getId());
    a->setFrom(attacker.getId());
    s->setArgs1(a);
    res.push_back(s);

    m_attack = !m_attack;

    float damage = (attacker.statistics().attack() / attacker.statistics().defence()) / uniform(2.f, 10.f);

    float stamina = std::max(defender.getStamina() - damage, 0.);

    Anonymous set_arg;
    set_arg->setId(defender.getId());
    set_arg->setAttr("stamina", stamina);

    if (stamina <= 0.f) {
        set_arg->setAttr("status", defender.getStatus() - 0.1);

        Sound s1;
        Talk t1;
        Anonymous say1;
        say1->setAttr("say", "You have been defeated");
        t1->setArgs1(say1);
        s1->setArgs1(t1);
        s1->setTo(defender.getId());
        m_character.m_world->m_gameWorld.sendWorld(s1);

        Sound s2;
        Talk t2;
        Anonymous say2;
        say2->setAttr("say", "You are victorious");
        t2->setArgs1(say2);
        s2->setArgs1(t2);
        s2->setTo(attacker.getId());
        m_character.m_world->m_gameWorld.sendWorld(s2);

        irrelevant();
    }

    Set set;
    set->setArgs1(set_arg);
    set->setTo(defender.getId());
    res.push_back(set);

    attacker.statistics().increment("combat", res);
    // FIXME Handle skill/experience increment?

    // Defend op

    if (obsolete()) {
        return;
    }

    Tick t;
    t->setAttr("sub_to", "task");
    t->setTo(m_character.getId());
    t->setFutureSeconds(1.75);
    res.push_back(t);
}
