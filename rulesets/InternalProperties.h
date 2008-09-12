// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
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

// $Id$

#ifndef RULESETS_INTERNAL_PROPERTIES_H
#define RULESETS_INTERNAL_PROPERTIES_H

#include "common/DynamicProperty.h"

/// \brief Class to handle Entity which requires a Setup operation
/// \ingroup PropertyClasses
class SetupProperty : public DynamicProperty<int> {
  public:
    SetupProperty();

    virtual void install(Entity *);
};

/// \brief Class to handle Entity which requires a Tick operation
/// \ingroup PropertyClasses
class TickProperty : public DynamicProperty<int> {
  public:
    TickProperty();

    virtual void install(Entity *);
};


#endif // RULESETS_INTERNAL_PROPERTIES_H
