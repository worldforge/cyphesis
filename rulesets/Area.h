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

class Area : public Thing {
  protected:
    IdList segments;

  public:

    Area(const std::string & id);
    virtual ~Area();

    virtual const Atlas::Message::Object get(const std::string &) const;
    virtual void set(const std::string & aname,
                     const Atlas::Message::Object & attr);
    virtual void addToObject(Atlas::Message::Object::MapType & obj) const;
};

#endif // RULESETS_AREA_H
