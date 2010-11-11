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
                         SHAPE_BALL } ShapeT;
    
    virtual ~InnerTerrainMod();
    
    const std::string& getTypename() const;

protected:
    template <template <int> class Shape>
    bool parseStuff(const WFMath::Point<3> & pos,
                    const WFMath::Quaternion & orientation,
                    const Atlas::Message::MapType& modElement,
                    Shape<2> & shape,
                    const Atlas::Message::Element & shapeElement);

public:

    bool parseData(const WFMath::Point<3> & pos,
                   const WFMath::Quaternion &,
                   const Atlas::Message::MapType &);
    
    /**
     * @brief Accessor for the Mercator::TerrainMod created and held by this instance.
     * If no terrain mod could be created, such as with faulty Atlas data, or if parseData() hasn't been called yet, this will return a null pointer.
     * @return A pointer to the TerrainMod held by this instance, or null if none created.
     */
    Mercator::TerrainMod* getModifier();

    InnerTerrainMod();
    
protected:

    /**
     * @brief The type of mod this instance handles.
     * @see getTypename()
     */
    std::string mTypeName;

    static float parsePosition(const WFMath::Point<3> & pos, const Atlas::Message::MapType& modElement);

    template <template <int> class Shape>
    static bool parseShape(const Atlas::Message::Element& shapeElement,
                           const WFMath::Point<3>& pos,
                           const WFMath::Quaternion& orientation,
                           Shape<2> & shape);

    template <template <template <int> class Shape> class Mod,
              template <int> class Shape>
    bool createInstance(Shape<2> & shape,
                        const WFMath::Point<3>& pos,
                        const Atlas::Message::MapType &,
                        float,
                        float);

    template <template <template <int> class Shape> class Mod,
              template <int> class Shape>
    bool createInstance(Shape<2> & shape,
                        const WFMath::Point<3>& pos,
                        const Atlas::Message::MapType &);

    Mercator::TerrainMod * m_mod;
};

#endif // RULESETS_TERRAIN_MOD_H
