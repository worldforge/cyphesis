// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef RULESETS_AREA_PROPERTY_H
#define RULESETS_AREA_PROPERTY_H

#include "LineProperty.h"

#include "physics/Vector3D.h"

#include <set>

/// \brief Class to handle Entity terrain property
class AreaProperty : public PropertyBase {
  protected:
    LineProperty m_line;
  public:
    explicit AreaProperty(unsigned int flags);

    virtual void get(Atlas::Message::Element &);
    virtual void set(const Atlas::Message::Element &);
    virtual void add(const std::string &, Atlas::Message::MapType & map);
};

#endif // RULESETS_AREA_PROPERTY_H
