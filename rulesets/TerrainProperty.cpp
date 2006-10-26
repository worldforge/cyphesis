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

// $Id: TerrainProperty.cpp,v 1.7 2006-10-26 00:48:12 alriddoch Exp $

#include "TerrainProperty.h"

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>

#include "common/debug.h"

#include <sstream>

#include <cassert>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;

typedef Mercator::Terrain::Pointstore Pointstore;
typedef Mercator::Terrain::Pointcolumn Pointcolumn;

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
    debug(std::cout << "World::setTerrain()" << std::endl << std::flush;);

    const Pointstore & basePoints = m_data.getPoints();

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

            Pointstore::const_iterator J = basePoints.find(x);
            if ((J == basePoints.end()) ||
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
