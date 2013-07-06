// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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


#ifndef MODULES_TERRAIN_CONTEXT_H
#define MODULES_TERRAIN_CONTEXT_H

#include "modules/EntityRef.h"

#include <Mercator/TerrainMod.h>

class TerrainContext : public Mercator::Effector::Context
{
  protected:
    EntityRef m_entity;

  public:
    TerrainContext();

    explicit TerrainContext(LocatedEntity*);

    virtual ~TerrainContext();

    EntityRef & entity() { return m_entity; }
};

#endif // MODULES_TERRAIN_CONTEXT_H
