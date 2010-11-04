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

// $Id$

#ifndef RULESETS_TERRAIN_MOD_H
#define RULESETS_TERRAIN_MOD_H

#include <Atlas/Message/Element.h>

#include <wfmath/point.h>

namespace Mercator {
    class TerrainMod;
}

class InnerTerrainMod_impl;

/**
@author Erik Hjortsberg <erik.hjortsberg@iteam.se>
@brief Base class for all terrain mod specific classes.This is not meant to be used directly by anything else than the TerrainMod class.
The TerrainMod class in itself doesn't hold the actual reference to the terrain mod, and doesn't handle the final parsing of Atlas data. This is instead handled by the different subclasses of this class. Since the different kinds of terrain mods found in Mercator behave differently depending on their type and the kind of shape used, we need to separate the code for handling them into different classes.
*/
class InnerTerrainMod
{
public:
    typedef enum shape { SHAPE_UNKNOWN,
                         SHAPE_ROTBOX,
                         SHAPE_POLYGON,
                         SHAPE_BALL } Shape;
    
    virtual ~InnerTerrainMod();
    
    const std::string& getTypename() const;

    /**
     * @brief Tries to parse the Atlas data.
     * It's up to the specific subclasses to provide proper parsing of the data depending on their needs.
     * If the data is successfully parsed, a new Mercator::TerrainMod instance will be created.
     * @param modElement The Atlas element describing the terrainmod. This should in most instances correspond directly to the "terrainmod" element found in the root atlas attribute map.
     * @return If the parsing was successful, true will be returned, and a new Mercator::TerrainMod will have been created, else false.
     */
    virtual bool parseAtlasData(const WFMath::Point<3> & pos, const WFMath::Quaternion & orientation, const Atlas::Message::MapType& modElement) = 0;
    
    /**
     * @brief Accessor for the Mercator::TerrainMod created and held by this instance.
     * If no terrain mod could be created, such as with faulty Atlas data, or if parseAtlasData() hasn't been called yet, this will return a null pointer.
     * @return A pointer to the TerrainMod held by this instance, or null if none created.
     */
    Mercator::TerrainMod* getModifier();

protected:

    InnerTerrainMod(const std::string& typemod);
    
    /**
     * @brief The type of mod this instance handles.
     * @see getTypename()
     */
    std::string mTypeName;

    Shape parseShape(const Atlas::Message::MapType& modElement, Atlas::Message::Element& shapeMap);
    
    float parsePosition(const WFMath::Point<3> & pos, const Atlas::Message::MapType& modElement);

    /**
     * @brief A reference to inner mod implementation.
     * This is separate from this class because of the heavy use of templated shapes.
     * The ownership is ours, so it will be destroyed when this instance is destroyed.
     */
    InnerTerrainMod_impl* m_impl;
};


/**
@author Erik Hjortsberg <erik.hjortsberg@iteam.se>
@brief Inner terrain mod class for handling slope mods.
This will parse and create an instance of Mercator::SlopeTerrainMod, which is a mod which produces a sloped area in the landscape.
The main parsing of the atlas data and creation of the terrain mod occurs in InnerTerrainMod_impl however, as this is a mod that uses templated shapes.
*/
class InnerTerrainModSlope : public InnerTerrainMod
{
public:
    /**
     * @brief Ctor.
     * @param terrainMod The TerrainMod instance to which this instance belongs to.
     */
    InnerTerrainModSlope();
    
    /**
     * @brief Dtor.
     */
    virtual ~InnerTerrainModSlope();
    
    virtual bool parseAtlasData(const WFMath::Point<3> & pos, const WFMath::Quaternion & orientation, const Atlas::Message::MapType& modElement);
};

/**
@author Erik Hjortsberg <erik.hjortsberg@iteam.se>
@author Tamas Bates
@brief Handles a crater terrain mod.
This will parse and create an instance of Mercator::CraterTerrainMod, which is a mod which produces a crater in the landscape.
Note that this will not make use of InnerTerrainMod_impl since there's no templated shapes in use here.
TODO: Should perhaps this also use the same pattern of InnerTerrainMod_impl as the other mods, just to not break the pattern? /ehj
*/
class InnerTerrainModCrater : public InnerTerrainMod
{
public:
    /**
     * @brief Ctor.
     * @param terrainMod The TerrainMod instance to which this instance belongs to.
     */
    InnerTerrainModCrater();
    
    /**
     * @brief Dtor.
     */
    virtual ~InnerTerrainModCrater();
    
    virtual bool parseAtlasData(const WFMath::Point<3> & pos, const WFMath::Quaternion & orientation, const Atlas::Message::MapType& modElement);
};

/**
@author Erik Hjortsberg <erik.hjortsberg@iteam.se>
@author Tamas Bates
@brief Handles a level terrain mod.
This will parse and create an instance of Mercator::LevelTerrainMod, which is a mod which produces a level area in the landscape.
The main parsing of the atlas data and creation of the terrain mod occurs in InnerTerrainMod_impl however, as this is a mod that uses templated shapes.
*/
class InnerTerrainModLevel : public InnerTerrainMod
{
public:
    /**
     * @brief Ctor.
     * @param terrainMod The TerrainMod instance to which this instance belongs to.
     */
    InnerTerrainModLevel();
    
    /**
     * @brief Dtor.
     */
    virtual ~InnerTerrainModLevel();
    
    virtual bool parseAtlasData(const WFMath::Point<3> & pos, const WFMath::Quaternion & orientation, const Atlas::Message::MapType& modElement);
};

/**
@author Erik Hjortsberg <erik.hjortsberg@iteam.se>
@author Tamas Bates
@brief Handles a level terrain mod.
This will parse and create an instance of Mercator::AdjustTerrainMod, which is a mod which adjusts the terrain within an area in the landscape.
The main parsing of the atlas data and creation of the terrain mod occurs in InnerTerrainMod_impl however, as this is a mod that uses templated shapes.
*/
class InnerTerrainModAdjust : public InnerTerrainMod
{
public:
    /**
     * @brief Ctor.
     * @param terrainMod The TerrainMod instance to which this instance belongs to.
     */
    InnerTerrainModAdjust();
    
    /**
     * @brief Dtor.
     */
    virtual ~InnerTerrainModAdjust();
    
    virtual bool parseAtlasData(const WFMath::Point<3> & pos, const WFMath::Quaternion & orientation, const Atlas::Message::MapType& modElement);
};


#endif // RULESETS_TERRAIN_MOD_H
