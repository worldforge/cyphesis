// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 Alistair Riddoch

#ifndef RULESETS_AREA_H
#define RULESETS_AREA_H

#include "Thing.h"

// This is the base class for area based geomap features.
// On the atlas side, it inherits from feature, but feature
// does not have any added attributes or functionality over
// thing

typedef Thing Area_parent;

class Area : public Area_parent {
  protected:
    IdList m_segments;

  public:

    Area(const std::string & id);
    virtual ~Area();

    virtual bool get(const std::string &, Atlas::Message::Element &) const;
    virtual void set(const std::string &, const Atlas::Message::Element &);
    virtual void addToObject(Atlas::Message::Element::MapType & obj) const;
};

#endif // RULESETS_AREA_H
