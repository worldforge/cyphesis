// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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

// $Id$

#ifndef RULESETS_TERRAINMOD_PROPERTY_H
#define RULESETS_TERRAINMOD_PROPERTY_H

#include "common/Property.h"
#include "Entity.h"
#include "TerrainProperty.h"

#include <Mercator/TerrainMod.h>

namespace Mercator {
    class Terrain;
}


/// \brief Class to handle Entity terrain modifier property
/// \ingroup PropertyClasses
class TerrainModProperty : public PropertyBase {
  protected:

    /// \brief Collection of applied terrain mod data
    Atlas::Message::MapType  m_terrainmods;

    /// \brief A pointer to the modifier returned by a call to Terrain::addMod()
    Mercator::TerrainMod *m_modptr;

    /// \brief the entity owner of this property
    Entity * m_owner;

    /// \brief the handlers this property should install
    HandlerMap m_handlers;

    TerrainProperty* getTerrain();

    /**
     *    @brief creates a CraterTerrainMod based on a shape and position
     * @param shapeMap An Atlas MapType containing all the information about the shape defining the mod
     * @param pos A 3D Point containing the position the modifier is to be applied at
     * @returns A pointer to the new TerrainMod on success, or NULL on failure
     * pos is used to generate the shape for the modifier, and can cause the mod to be
     * applied at a position other than the owning entity's position.
     */
    Mercator::TerrainMod * newCraterMod(const Atlas::Message::MapType, const Point3D &);

    /**    @brief Creates a LevelTerrainMod based on a shape and position
     * @param shapeMap An Atlas MapType containing all the information about the shape defining the mod
     * @param pos A 3D Point containing the position the modifier is to be applied at
     * @returns A pointer to the new TerrainMod on success, or NULL on failure
     * In this case the LevelMod only uses 2D coordinates for its position, so pos.z() actually
     * holds the height that the LevelMod will be raised to.
     */
    Mercator::TerrainMod * newLevelMod(const Atlas::Message::MapType, const Point3D &);

    /**    @brief Creates a SlopeTerrainMod based on a shape and position
     * @param shapeMap An Atlas MapType containing all the information about the shape defining the mod
     * @param pos A 3D Point containing the position the modifier is to be applied at
     * @param dx The desired slope on the X-Axis
     * @param dy The desired slope on the Y-Axis
     * @returns A pointer to the new TerrainMod on success, or NULL on failure
     * In this case the SlopeMod only uses 2D coordinates for its position, so pos.z() actually
     * holds the height that the terrain will be raised to.
     */
    Mercator::TerrainMod * newSlopeMod(const Atlas::Message::MapType, const Point3D &, float, float);

    /**    @brief Creates a AdjustTerrainMod based on a shape and position
     * @param shapeMap An Atlas MapType containing all the information about the shape defining the mod
     * @param pos A 3D Point containing the position the modifier is to be applied at
     * @returns A pointer to the new TerrainMod on success, or NULL on failure
     * In this case the LevelMod only uses 2D coordinates for its position, so pos.z() actually
     * holds the value of the height adjustment made to the terrain.
     */
    Mercator::TerrainMod * newAdjustMod(const Atlas::Message::MapType, const Point3D &);

  public:

    explicit TerrainModProperty(const HandlerMap &);

    virtual bool get(Atlas::Message::Element &) const;
    virtual void set(const Atlas::Message::Element &);
    virtual void add(const std::string & key,
                     Atlas::Message::MapType & map) const;
    virtual void install(Entity *);

    Mercator::TerrainMod * getModifier();
    void setPos(const Point3D &);
    void setup(Entity *);

    /// \brief Constructs a Mercator::TerrainMod from Atlas data
    Mercator::TerrainMod * parseModData(const Atlas::Message::Element &);
    /// \brief Constructs a Mercator::TerrainMod from Atlas data, but uses
    /// the given position
    Mercator::TerrainMod * parseModData(const Atlas::Message::Element &,
                                        const Point3D &);

    /// \brief Changes a modifier's position
    void move(Entity*, const Point3D &);

    /// \brief Removes the modifier from the terrain
    void remove();
};


#endif // RULESETS_TERRAINMOD_PROPERTY_H
