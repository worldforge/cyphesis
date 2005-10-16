// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "Statistics.h"

#include "Character.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/SmartPtr.h>

using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;

using Atlas::Objects::Entity::Anonymous;

using Atlas::Message::MapType;

Statistics::Statistics(Character & chr) : m_character(chr)
{
}

float Statistics::strength()
{
    return m_character.getMass();
}

float Statistics::attack()
{
    return 1;
}

float Statistics::defence()
{
    return 1;
}

void Statistics::increment(const std::string & name, OpVector & res)
{
    std::cout << "Incrementing " << name << std::endl << std::flush;

    float oldval, newval;

    // Check if we have this skill already
    SkillDict::iterator I = m_skills.find(name);
    if (I == m_skills.end()) {
        // We dont have this skill yet
        oldval = 0.f;
        newval = 0.01f;
        m_skills.insert(std::make_pair(name, newval));
    } else {
        // We have this skill, increment in a curve which tends towards 1
        oldval = I->second;
        // FIXME Need to replace with a more flexible function which
        // gives the right curve
        newval = oldval + ((1.f - std::max(oldval, 1.f)) / 1000.f);
        I->second = newval;
    }
    std::cout << "Skill change " << oldval << ":" << newval << ":"
              << (int)(oldval * 10) << ":" << (int)(newval * 10)
              << std::endl << std::flush;
    // If value has changed by more than 0.001 then report to character.
    if ((int)(newval * 1000) != (int)(oldval * 1000)) {
        Set set;
        Anonymous set_arg;
        MapType skills;
        skills[name] = newval;
        set_arg->setAttr("skills", skills);
        set_arg->setId(m_character.getId());
        set->setTo(m_character.getId());
        set->setFrom(m_character.getId());
        set->setArgs1(set_arg);
        Sight sight;
        sight->setArgs1(set);
        sight->setTo(m_character.getId());
        res.push_back(sight);
    }
}
