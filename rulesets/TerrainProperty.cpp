// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

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

void TerrainProperty::get(Element & ent)
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
