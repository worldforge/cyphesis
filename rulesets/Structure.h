// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001-2004 Alistair Riddoch
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

// $Id: Structure.h,v 1.13 2006-10-26 00:48:12 alriddoch Exp $

#ifndef RULESETS_STRUCTURE_H
#define RULESETS_STRUCTURE_H

#include "Thing.h"

typedef Thing Structure_parent;

/// \brief Base class for buildings and other structures.
///
/// Essentially represents an object with a complex shape which the character
/// can enter. All this class really does is ensure that the entity geometry
/// is set up in such a way that the collision code allows the character
/// to pass into it.
class Structure : public Structure_parent {
  public:
    explicit Structure(const std::string & id, long intId);
};

#endif // RULESETS_STRUCTURE_H
