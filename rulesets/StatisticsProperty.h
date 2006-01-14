// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef RULESETS_STATISTICS_PROPERTY_H
#define RULESETS_STATISTICS_PROPERTY_H

#include "common/Property.h"

class Statistics;

/// \brief Class to handle Entity statistics
class StatisticsProperty : public PropertyBase {
  protected:
    Statistics & m_data;
  public:
    explicit StatisticsProperty(Statistics & data, unsigned int flags);

    Statistics & data() const { return m_data; }

    virtual void get(Atlas::Message::Element &) const;
    virtual void set(const Atlas::Message::Element &);
};

#endif // RULESETS_STATISTICS_PROPERTY_H
