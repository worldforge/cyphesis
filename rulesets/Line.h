// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 Alistair Riddoch

#ifndef RULESETS_LINE_H
#define RULESETS_LINE_H

#include "Thing.h"

// This is the base class for line based geomap features.
// On the atlas side, it inherits from feature, but feature
// does not have any added attributes or functionality over
// thing

class Line : public Thing {
  protected:
    IdList startIntersections;
    IdList endIntersections;
    CoordList coords;

  public:

    Line();
    virtual ~Line();

    virtual const Atlas::Message::Object& operator[](const std::string & aname);
    virtual void set(const std::string & aname,
                     const Atlas::Message::Object & attr);
    virtual void addToObject(Atlas::Message::Object::MapType & obj) const;
};

#endif // RULESETS_LINE_H
