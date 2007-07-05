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

// $Id: Area.h,v 1.19 2007-07-05 17:51:41 alriddoch Exp $

#error This file has been removed from the build
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifndef RULESETS_AREA_H
#define RULESETS_AREA_H

#include "Thing.h"

typedef Thing Area_parent;

/// \brief This is the base class for area based geomap features.
///
/// On the atlas side, it inherits from feature, but feature does not have
/// any added attributes or functionality over thing. This is not yet fully
/// implemented.
/// \ingroup EntityClasses
class Area : public Area_parent {
  protected:
    /// \brief List if identifiers of entities which make up area segments
    IdList m_segments;

  public:

    explicit Area(const std::string & id, long intId);
    virtual ~Area();
};

#endif // RULESETS_AREA_H
