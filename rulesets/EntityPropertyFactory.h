// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2007 Alistair Riddoch
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

// $Id: EntityPropertyFactory.h,v 1.3 2007-12-20 18:47:29 alriddoch Exp $

#ifndef RULESETS_ENTITY_PROPERTY_FACTORY_H
#define RULESETS_ENTITY_PROPERTY_FACTORY_H

#include "common/types.h"
#include "common/PropertyFactory.h"

/// \brief Factory class template to create Property objects which require
/// a pointer to their entity
template <class T>
class EntityPropertyFactory : public PropertyKit {
  public:
    virtual PropertyBase * newProperty(Entity *);
};

#endif // RULESETS_ENTITY_PROPERTY_FACTORY_H
