// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "Statistics.h"

#include "Character.h"
#include "ArithmeticScript.h"

#include "common/log.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/SmartPtr.h>

using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;

using Atlas::Objects::Entity::Anonymous;

using Atlas::Message::MapType;

Statistics::Statistics(Character & chr) : m_script(0), m_character(chr)
{
}

float Statistics::get(const std::string & name)
{
    if (m_script != 0) {
        float val;
        int res = m_script->attribute(name, val);
        if (res == 0) {
            return val;
        } else {
            log(ERROR, "Statistics::get: Error reading value from script");
        }
    }
    // FIXME query the script
    return get_default(name);
}

float Statistics::get_default(const std::string & name)
{
    if (name == "strength") {
        return m_character.getMass();
    } else if (name == "attack") {
        return 1.f;
    } else if (name == "defence") {
        return 1.f;
    } else {
        return 0.f;
    }
}

void Statistics::increment(const std::string & name, OpVector & res)
{
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
        newval = oldval + ((1.f - std::min(oldval, 1.f)) / 1000.f);
        I->second = newval;
    }

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
