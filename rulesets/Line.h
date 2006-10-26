// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2002 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id: Line.h,v 1.14 2006-10-26 00:48:09 alriddoch Exp $

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
    IdList m_startIntersections;
    IdList m_endIntersections;
    CoordList m_coords;

  public:

    explicit Line(const std::string & id, long intId);
    virtual ~Line();
};

#endif // RULESETS_LINE_H
