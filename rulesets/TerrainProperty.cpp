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

// $Id: TerrainProperty.cpp,v 1.10 2007-07-29 21:23:43 alriddoch Exp $

#include "TerrainProperty.h"

#include "common/log.h"
#include "common/debug.h"

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>
#include <Mercator/Surface.h>

#include <sstream>

#include <cassert>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;

typedef Mercator::Terrain::Pointstore Pointstore;
typedef Mercator::Terrain::Pointcolumn Pointcolumn;

/// \brief TerrainProperty constructor
///
/// @param data Reference to varaible holding the value of this Property
/// @param modifiedTerrain Reference to a variable storing the set of
/// modified points
/// @param createdTerrain Reference to a variable storing the set of
/// created points
/// @param flags Flags indicating how this Property should be handled
TerrainProperty::TerrainProperty(Mercator::Terrain & data,
                                 PointSet & modifiedTerrain,
                                 PointSet & createdTerrain,
                                 unsigned int flags) :
                                 PropertyBase(flags),
                                 m_data(data),
                                 m_modifiedTerrain(modifiedTerrain),
                                 m_createdTerrain(createdTerrain)
{
}

bool TerrainProperty::get(Element & ent) const
{
    MapType & t = (ent = MapType()).asMap();
    MapType & terrain = (t["points"] = MapType()).asMap();

    const Pointstore & points = m_data.getPoints();
    Pointstore::const_iterator Iend = points.end();
    for (Pointstore::const_iterator I = points.begin(); I != Iend; ++I) {
        const Pointcolumn & pointcol = I->second;
        Pointcolumn::const_iterator J = pointcol.begin();
        Pointcolumn::const_iterator Jend = pointcol.end();
        for (; J != Jend; ++J) {
            std::stringstream key;
            key << I->first << "x" << J->first;
            ListType & point = (terrain[key.str()] = ListType(3)).asList();
            point[0] = (FloatType)(I->first);
            point[1] = (FloatType)(J->first);
            point[2] = (FloatType)(J->second.height());
        }
    }
    return true;
}

void TerrainProperty::set(const Element & ent)
{
    if (!ent.isMap()) {
        return;
    }
    const MapType & t = ent.asMap();
    debug(std::cout << "TerrainProperty::setTerrain()"
                    << std::endl << std::flush;);

    const Pointstore & base_points = m_data.getPoints();

    MapType::const_iterator I = t.find("points");
    if ((I != t.end()) && (I->second.isMap())) {
        const MapType & points = I->second.asMap();
        MapType::const_iterator Iend = points.end();
        for (MapType::const_iterator I = points.begin(); I != Iend; ++I) {
            if (!I->second.isList()) {
                continue;
            }
            const ListType & point = I->second.asList();
            if (point.size() != 3) {
                continue;
            }

            int x = (int)point[0].asNum();
            int y = (int)point[1].asNum();

            Pointstore::const_iterator J = base_points.find(x);
            if ((J == base_points.end()) ||
                (J->second.find(y) == J->second.end())) {
                // Newly added point.
                m_createdTerrain[x].insert(y);
            } else {
                // Modified point
                PointSet::const_iterator K = m_createdTerrain.find(x);
                if ((K == m_createdTerrain.end()) ||
                    (K->second.find(y) == K->second.end())) {
                    // Already in database
                    m_modifiedTerrain[x].insert(y);
                }
                // else do nothing, as its currently waiting to be added.
            }
            
            m_data.setBasePoint(x, y, point[2].asNum());
            // FIXME Add support for roughness and falloff, as done
            // by damien in equator and FIXMEd out by me

            
        }
    }
}

/// \brief Calculate the terrain height at the given x,y coordinates
float TerrainProperty::getHeight(float x, float y)
{
    Mercator::Segment * s = m_data.getSegment(x, y);
    if (s != 0 && !s->isValid()) {
        s->populate();
    }
    return m_data.get(x, y);
}

/// \brief Get a number encoding the surface type at the given x,y coordinates
///
/// @param pos the x,y coordinates of the point on the terrain
/// @param material a reference to the integer to be used to store the
/// material identifier at this location.
int TerrainProperty::getSurface(const Point3D & pos, int & material)
{
    float x = pos.x(),
          y = pos.y();
    Mercator::Segment * segment = m_data.getSegment(x, y);
    if (segment == 0) {
        debug(std::cerr << "No terrain at this point" << std::endl << std::flush;);
        return -1;
    }
    if (!segment->isValid()) {
        segment->populate();
    }
    x = x - segment->getResolution() * segment->getXRef();
    y = y - segment->getResolution() * segment->getYRef();
    const Mercator::Segment::Surfacestore & surfaces = segment->getSurfaces();
    WFMath::Vector<3> normal;
    float height = -23;
    segment->getHeightAndNormal(x, y, height, normal);
    debug(std::cout << "At the point " << x << "," << y
                    << " of the segment the height is " << height << std::endl;
          std::cout << "The segment has " << surfaces.size()
                    << std::endl << std::flush;);
    if (surfaces.size() == 0) {
        log(ERROR, "The terrain has no surface data");
        return -1;
    }
    Mercator::Surface & tile_surface = *surfaces.begin()->second;
    if (!tile_surface.isValid()) {
        tile_surface.populate();
    }
    material = tile_surface((int)x, (int)y, 0);
    return 0;
}
