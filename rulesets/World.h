// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef RULESETS_WORLD_H
#define RULESETS_WORLD_H

#include "Thing.h"

namespace Mercator {
    class Terrain;
}

typedef Thing World_parent;

typedef std::map<int, std::set<int> > PointSet;

/// This is the in-game entity class used to represent the world.
///
/// I added this because I was not happy with the way the old object model
/// used an out of game object of type WorldRouter to represent the world.
class World : public World_parent {
  protected:
    /// Terrain manager for the world.
    Mercator::Terrain & m_terrain;
    /// Set of terrain points which have been changed.
    PointSet m_modifiedTerrain;
    /// Set of terrain points which have been added.
    PointSet m_createdTerrain;

  public:
    explicit World(const std::string & id);
    virtual ~World();

    const Mercator::Terrain & terrain() {
        return m_terrain;
    }

    const PointSet & modifiedTerrain() {
        return m_modifiedTerrain;
    }

    const PointSet & createdTerrain() {
        return m_createdTerrain;
    }

    void clearTerrainFlags() {
        m_modifiedTerrain.clear();
        m_createdTerrain.clear();
    }

    float getHeight(float x, float y);

    virtual void LookOperation(const Operation &, OpVector &);
    virtual void BurnOperation(const Operation &, OpVector &);
    virtual void DeleteOperation(const Operation &, OpVector &);
    virtual void MoveOperation(const Operation &, OpVector &);
    virtual void SetOperation(const Operation &, OpVector &);
};

#endif // RULESETS_WORLD_H
