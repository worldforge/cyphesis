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

#include "terrain_utils.h"

#include "Database.h"
#include "debug.h"

#include <Mercator/Terrain.h>

#include <Atlas/Message/Element.h>

#include <vector>

static const bool debug_flag = false;

typedef Mercator::Terrain::Pointstore Pointstore;
typedef Mercator::Terrain::Pointcolumn Pointcolumn;

void storeTerrain(const std::string & id, const Mercator::Terrain & terrain)
{
    const Pointstore & points = terrain.getPoints();
    Pointstore::const_iterator Iend = points.end();
    for (Pointstore::const_iterator I = points.begin(); I != Iend; ++I) {
        Pointcolumn::const_iterator J = I->second.begin();
        Pointcolumn::const_iterator Jend = I->second.end();
        for (; J != Jend; ++J) {
            std::vector<int> coords(2);

            coords[0] = I->first;
            coords[1] = J->first;

            Atlas::Message::MapType heightPoint;
            heightPoint["height"] = J->second.height();

            Database::instance()->createArrayRow("terrain", id, coords,
                                                 heightPoint);

            debug(std::cout << I->first << ", " << J->first << " = "
                            << J->second.height() << std::endl << std::flush;);
        }
    }
}

void loadTerrain(const std::string & id, Mercator::Terrain & terrain)
{
    DatabaseResult res = Database::instance()->selectArrayRows("terrain", id);

    DatabaseResult::const_iterator Iend = res.end();
    for (DatabaseResult::const_iterator I = res.begin(); I != Iend; ++I) {
        int x = 4, y = 5;
        float h = 3.14159265;

        I.readColumn("i", x);
        I.readColumn("j", y);
        I.readColumn("height", h);

        debug(std::cout << "Heightpoint " << x << "," << y << " = " << h
                        << std::endl << std::flush;);

        terrain.setBasePoint(x, y, h);
    }
    res.clear();
}

void updateStoredTerrain(const std::string & id,
                         const Mercator::Terrain & terrain,
                         const PointSet & modified,
                         const PointSet & created)
{
    Mercator::BasePoint bp;
    debug(std::cout << "Update Stored Terrain" << std::endl << std::flush;);

    PointSet::const_iterator Iend = modified.end();
    for (PointSet::const_iterator I = modified.begin(); I != Iend; ++I) {
        std::set<int>::const_iterator J = I->second.begin();
        std::set<int>::const_iterator Jend = I->second.end();
        for (; J != Jend; ++J) {
            debug(std::cout << "Modified " << I->first << *J
                            << std::endl << std::flush;);
            std::vector<int> coords(2);

            coords[0] = I->first;
            coords[1] = *J;

            Atlas::Message::MapType heightPoint;
            assert(terrain.getBasePoint(I->first, *J, bp));
            heightPoint["height"] = bp.height();

            Database::instance()->updateArrayRow("terrain", id, coords,
                                                 heightPoint);
        }
    }

    Iend = created.end();
    for (PointSet::const_iterator I = created.begin(); I != Iend; ++I) {
        std::set<int>::const_iterator J = I->second.begin();
        std::set<int>::const_iterator Jend = I->second.end();
        for (; J != Jend; ++J) {
            std::cout << "Created " << I->first << "," << *J << std::endl << std::flush;
            std::vector<int> coords(2);

            coords[0] = I->first;
            coords[1] = *J;

            Atlas::Message::MapType heightPoint;
            assert(terrain.getBasePoint(I->first, *J, bp));
            heightPoint["height"] = bp.height();

            Database::instance()->createArrayRow("terrain", id, coords,
                                                 heightPoint);
        }
    }
}
