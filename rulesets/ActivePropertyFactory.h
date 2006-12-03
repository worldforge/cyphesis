// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
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

// $Id: ActivePropertyFactory.h,v 1.1 2006-12-03 06:31:44 alriddoch Exp $

#ifndef RULESETS_ACTIVE_PROPERTY_FACTORY_H
#define RULESETS_ACTIVE_PROPERTY_FACTORY_H

#include "common/types.h"
#include "common/PropertyFactory.h"

template <class T>
class ActivePropertyBuilder : public PropertyFactory {
  protected:
    /// The class number of operations this property needs to handle
    int m_operationClassNo;
    /// The handler function used to handle the operation.
    HandlerResult (*m_handler)(const Operation &, OpVector &);
    
  public:
    explicit ActivePropertyBuilder(int, HandlerResult(*)(const Operation &,
                                                         OpVector &));

    virtual PropertyBase * newProperty(Entity *);
};

#endif // RULESETS_ACTIVE_PROPERTY_FACTORY_H
