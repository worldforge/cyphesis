// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef RULESETS_STRUCTURE_H
#define RULESETS_STRUCTURE_H

#include "Thing.h"

// Base class for buildings and other structures
// Essentially represents an object with a complex shape which the character
// can enter.

class Structure : public Thing {
  public:
    explicit Structure(const std::string & id) : Thing(id) {
        m_location.m_solid = false;
    }
};

#endif // RULESETS_STRUCTURE_H
