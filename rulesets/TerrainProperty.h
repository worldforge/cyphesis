// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef RULESETS_TERRAIN_PROPERTY_H
#define RULESETS_TERRAIN_PROPERTY_H

#include "common/Property.h"

#include <set>

namespace Mercator {
    class Terrain;
}

typedef std::map<int, std::set<int> > PointSet;

/// \brief Class to handle Entity line property
class TerrainProperty : public PropertyBase {
  protected:
    Mercator::Terrain & m_data;
    PointSet & m_modifiedTerrain;
    PointSet & m_createdTerrain;
  public:
    explicit TerrainProperty(Mercator::Terrain & data,
                             PointSet & modifiedTerrain,
                             PointSet & createdTerrain,
                             unsigned int flags);

    virtual void get(Atlas::Message::Element &);
    virtual void set(const Atlas::Message::Element &);
};

#endif // RULESETS_TERRAIN_PROPERTY_H
