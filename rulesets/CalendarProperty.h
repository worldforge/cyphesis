// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef RULESETS_CALENDAR_PROPERTY_H
#define RULESETS_CALENDAR_PROPERTY_H

#include "common/Property.h"

#include <set>

typedef std::map<int, std::set<int> > PointSet;

/// \brief Class to handle World calendar property
class CalendarProperty : public PropertyBase {
  public:
    explicit CalendarProperty(unsigned int flags);

    virtual void get(Atlas::Message::Element &) const;
    virtual void set(const Atlas::Message::Element &);
};

#endif // RULESETS_CALENDAR_PROPERTY_H
