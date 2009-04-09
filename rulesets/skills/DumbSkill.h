// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

// $Id$

#ifndef SKILLS_DUMB_SKILL_H
#define SKILLS_DUMB_SKILL_H

#include "Skill.h"

class DumbSkill : public Skill {
  public:
    DumbSkill(Entity & e) : Skill(e) { }
    virtual oplist action(const std::string & skill,
            const Atlas::Objects::Operation::RootOperation & op);
};

#endif // SKILLS_DUMB_SKILL_H
