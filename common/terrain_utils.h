// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef COMMON_TERRAIN_UTILS_H
#define COMMON_TERRAIN_UTILS_H

#include <string>
#include <set>
#include <map>

namespace Mercator {
  class Terrain;
}

typedef std::map<int, std::set<int> > PointSet;

void storeTerrain(const std::string & id, const Mercator::Terrain & terrain);
void loadTerrain(const std::string & id, Mercator::Terrain & terrain);
void updateStoredTerrain(const std::string & id,
                         const Mercator::Terrain & terrain,
                         const PointSet & modified,
                         const PointSet & created);

#endif // COMMON_TERRAIN_UTILS_H
