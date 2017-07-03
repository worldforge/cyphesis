// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
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


#ifndef RULESETS_AREA_PROPERTY_H
#define RULESETS_AREA_PROPERTY_H

#include "TerrainEffectorProperty.h"

template <int dim>
class Form;

/// \brief Class to handle Entity terrain property
/// \ingroup PropertyClasses
class AreaProperty : public TerrainEffectorProperty {
  protected:
    int m_layer;
    Form<2> * m_shape;

    AreaProperty(const AreaProperty & other);
    // Assignment isn't banned, but it's gotta be implemented before it'll
    // work. Default should not be used.
    AreaProperty & operator=(const AreaProperty &) = delete;
  public:
    explicit AreaProperty();
    virtual ~AreaProperty();

    const Form<2> * shape() const { return m_shape; }

    virtual void apply(LocatedEntity *);

    virtual void set(const Atlas::Message::Element & val);
    virtual AreaProperty * copy() const;
};

#endif // RULESETS_AREA_PROPERTY_H
