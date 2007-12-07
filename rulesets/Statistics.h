// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
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

// $Id: Statistics.h,v 1.9 2007-12-07 01:19:16 alriddoch Exp $

#ifndef RULESETS_STATISTICS_H
#define RULESETS_STATISTICS_H

#include "common/types.h"

class ArithmeticScript;
class Character;

/// \brief This class handles presentation of a characters statistics.
///
/// An instance of this class should be a member of Character, and
/// it should hold a reference to the character to which it belongs.
/// Member functions are provided so that code can determine a character's
/// key statistics, including modifiers due to skills, buffs etc.
/// The Character reference passed in at construction time is under
/// construction itself, so should not be used at this time, just stored.
class Statistics {
  public:
    /// \brief Dictionary of skill values
    typedef std::map<std::string, float> SkillDict;

    /// \brief Script that handles number crunching for statistics
    ArithmeticScript * m_script;
  protected:
    /// \brief The Character these are the statistics for
    Character & m_character;

    /// \brief Skill values
    SkillDict m_skills;
  public:
    Statistics(Character & chr);

    /// \brief Get the value for a named statistic
    float get(const std::string & name);

    /// \brief Return a default for a statistic not supported by this model
    float get_default(const std::string & name);

    /// \brief Increment the characters skill in a certain area
    void increment(const std::string & name, OpVector & res);
};

#endif // RULESETS_STATISTICS_H
