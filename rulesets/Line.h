// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 Alistair Riddoch

#ifndef RULESETS_LINE_H
#define RULESETS_LINE_H

#include "Thing.h"

typedef Thing Line_parent;

/// \brief This is the base class for line based geomap features.
///
/// On the atlas side, it inherits from feature, but feature does not have
/// any added attributes or functionality over thing. This is not yet fully
/// implemented.
class Line : public Line_parent {
  protected:
    IdList startIntersections;
    IdList endIntersections;
    CoordList coords;

  public:

    explicit Line(const std::string & id);
    virtual ~Line();

    virtual bool get(const std::string &, Element &) const;
    virtual void set(const std::string &, const Element &);

    virtual void addToMessage(MapType & obj) const;
};

#endif // RULESETS_LINE_H
