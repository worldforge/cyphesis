// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef RULESETS_STATISTICS_H
#define RULESETS_STATISTICS_H

#include "common/types.h"

class Character;

/// \brief This class handles presentation of a characters statistics.
///
/// An instance of this class should be a member of Character, and
/// it should hold a reference to the character to which it belongs.
/// Member functions are provided so that code can determine a character's
/// key statistics, including modifiers due to skills, buffs etc.
class Statistics {
  public:
    typedef std::map<std::string, float> SkillDict;
  protected:
    Character & m_character;

    /// \brief Skill values
    SkillDict m_skills;
  public:
    Statistics(Character & chr);

    /// \brief Maximum weight character can lift
    float strength();
    /// \brief Attack power
    float attack();
    /// \brief Defence power
    float defence();

    /// \brief Increment the characters skill in a certain area
    void increment(const std::string & name, OpVector & res);
};

#endif // RULESETS_STATISTICS_H
