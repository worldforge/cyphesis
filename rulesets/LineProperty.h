// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef RULESETS_LINE_PROPERTY_H
#define RULESETS_LINE_PROPERTY_H

#include "common/Property.h"

#include "physics/Vector3D.h"

#include <set>

/// \brief Class to handle Entity terrain property
class LineProperty : public PropertyBase {
  protected:
    CoordList & m_data;
    // PointSet & m_modifiedTerrain; // FIXME We will need this for persistence
    // PointSet & m_createdTerrain;
  public:
    explicit LineProperty(CoordList & data,
                          unsigned int flags);

    virtual void get(Atlas::Message::Element &);
    virtual void set(const Atlas::Message::Element &);
    virtual void add(const std::string &, Atlas::Message::MapType & map);
};

#endif // RULESETS_LINE_PROPERTY_H
