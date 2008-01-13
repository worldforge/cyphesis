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

// $Id: ActivePropertyFactory.h,v 1.7 2008-01-13 01:32:55 alriddoch Exp $

#ifndef RULESETS_ACTIVE_PROPERTY_FACTORY_H
#define RULESETS_ACTIVE_PROPERTY_FACTORY_H

#include "common/OperationRouter.h"
#include "common/PropertyFactory.h"

/// \brief Factory class template to create active Property objects.
template <class T>
class ActivePropertyFactory : public PropertyKit {
  protected:
    /// The class number of operations this property needs to handle
    int m_operationClassNo;
    /// The handler function used to handle the operation.
    Handler m_handler;
    
  public:
    explicit ActivePropertyFactory(int, Handler);

    virtual PropertyBase * newProperty(Entity *);
};

/// \brief Factory class template to create multi-operation active Property
/// objects
template <class T>
class MultiActivePropertyFactory : public PropertyKit {
  protected:
    /// A map of operation class number to the handler for operations
    HandlerMap m_handlers;

  public:
    explicit MultiActivePropertyFactory(const HandlerMap & handlers);

    virtual PropertyBase * newProperty(Entity *);
};

#endif // RULESETS_ACTIVE_PROPERTY_FACTORY_H
