// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "terrain_utils.h"

#include "Database.h"

#include <Mercator/Terrain.h>

#include <Atlas/Message/Element.h>

#include <vector>

typedef Mercator::Terrain::Pointstore Pointstore;
typedef Mercator::Terrain::Pointcolumn Pointcolumn;

void storeTerrain(const std::string & id, const Mercator::Terrain & terrain)
{
    const Pointstore & points = terrain.getPoints();
    for (Pointstore::const_iterator I=points.begin(); I != points.end(); ++I) {
        Pointcolumn::const_iterator J = I->second.begin();
        for (; J != I->second.end(); ++J) {
            std::vector<int> coords(2);

            coords[0] = I->first;
            coords[1] = J->first;

            Atlas::Message::MapType heightPoint;
            heightPoint["height"] = J->second.height();

            Database::instance()->createArrayRow("terrain", id, coords,
                                                 heightPoint);

            std::cout << I->first << ", " << J->first << " = "
                      << J->second.height() << std::endl << std::flush;
        }
    }
}

void loadTerrain(const std::string & id, Mercator::Terrain & terrain)
{
    DatabaseResult res = Database::instance()->selectArrayRows("terrain", id);

    if (res.empty()) {
        std::cout << "No terrain in DB" << std::endl << std::flush;
    }

    for (DatabaseResult::const_iterator I = res.begin(); I != res.end(); ++I) {
        int x = 4, y = 5;
        float h = 3.14159265;

        I.readColumn("i", x);
        I.readColumn("j", y);
        I.readColumn("height", h);

        std::cout << "Heightpoint " << x << "," << y << " = " << h
                  << std::endl << std::flush;

        terrain.setBasePoint(x, y, h);
    }
}
