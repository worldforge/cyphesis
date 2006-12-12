// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001 Alistair Riddoch
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

// $Id: World.h,v 1.33 2006-12-12 15:54:24 alriddoch Exp $

#ifndef RULESETS_WORLD_H
#define RULESETS_WORLD_H

#include "Thing.h"

namespace Mercator {
    class Terrain;
    class TileShader;
}

typedef Thing World_parent;

typedef std::map<int, std::set<int> > PointSet;

/// \brief This is the in-game entity class used to represent the world.
///
/// I added this because I was not happy with the way the old object model
/// used an out of game object of type WorldRouter to represent the world.
/// \ingroup EntityClasses
class World : public World_parent {
  protected:
    /// Terrain manager for the world.
    Mercator::Terrain & m_terrain;
    /// Terrain shader tracking surface type.
    Mercator::TileShader & m_tileShader;
    /// Set of terrain points which have been changed.
    PointSet m_modifiedTerrain;
    /// Set of terrain points which have been added.
    PointSet m_createdTerrain;

  public:
    explicit World(const std::string & id, long intId);
    virtual ~World();

    /// \brief Accessor for terrain manager
    const Mercator::Terrain & terrain() {
        return m_terrain;
    }

    /// \brief Accessor for set of terrain points which have been changed
    const PointSet & modifiedTerrain() {
        return m_modifiedTerrain;
    }

    /// \brief Accessor for set of terrain points which have been added
    const PointSet & createdTerrain() {
        return m_createdTerrain;
    }

    /// \brief Clear the sets used to track terrain modifications
    void clearTerrainFlags() {
        m_modifiedTerrain.clear();
        m_createdTerrain.clear();
    }

    float getHeight(float x, float y);
    int getSurface(const Point3D &,  int &);

    void delveOperation(const Operation &, OpVector &);
    void digOperation(const Operation &, OpVector &);
    void mowOperation(const Operation &, OpVector &);

    virtual void EatOperation(const Operation &, OpVector &);
    virtual void LookOperation(const Operation &, OpVector &);
    virtual void BurnOperation(const Operation &, OpVector &);
    virtual void DeleteOperation(const Operation &, OpVector &);
    virtual void MoveOperation(const Operation &, OpVector &);
    virtual void OtherOperation(const Operation &, OpVector &);
};

#endif // RULESETS_WORLD_H
