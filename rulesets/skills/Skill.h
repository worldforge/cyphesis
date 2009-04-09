// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

// $Id$

#ifndef SKILLS_SKILL_H
#define SKILLS_SKILL_H

#include "common/types.h"

#include <Atlas/Objects/Operation/RootOperation.h>

class Entity;

class Skill {
  protected:
    Entity & entity;
  public:
    Skill(Entity & e) : entity(e) { }
    virtual oplist action(const std::string & skill,
            const Atlas::Objects::Operation::RootOperation & op) = 0;
};

#endif // SKILLS_SKILL_H
