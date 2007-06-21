// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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

// $Id: Food.h,v 1.19 2007-06-21 20:26:53 alriddoch Exp $

#ifndef RULESETS_FOOD_H
#define RULESETS_FOOD_H

#include "Thing.h"

typedef Thing Food_parent;

/// \brief This is the base class for edible things.
///
/// Most of the functionality will be common to all food, and most derived
/// classes will probably be in python.
/// \ingroup EntityClasses
class Food : public Food_parent {
  public:
    explicit Food(const std::string & id, long intId);
    virtual ~Food();

    virtual void BurnOperation(const Operation & op, OpVector &);
};

#endif // RULESETS_FOOD_H
