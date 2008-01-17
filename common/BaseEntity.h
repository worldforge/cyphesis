// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2004 Alistair Riddoch
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

// $Id: BaseEntity.h,v 1.92 2008-01-17 18:54:05 alriddoch Exp $

#ifndef COMMON_BASE_ENTITY_H
#define COMMON_BASE_ENTITY_H

#include "OperationRouter.h"

#include <Atlas/Message/Element.h>

#include <sigc++/trackable.h>
#include <sigc++/signal.h>

#include <string>

/// \brief This is the base class from which all other entity like classes
/// inherit, both in game and out of game.
///
/// This class basically provides a means of delivering operations to
/// an object, and the structure required to process those operations.
/// It also provides a mechanism to create an Atlas representation of
/// the entity, and an operation enumeration and subscription mechanism
/// which ensures that operations are handled as efficiently as
/// possible.
/// It has an id member which is typically used to store it in a STL map or
/// dictionary as they are called elsewhere in this code.
class BaseEntity : public OperationRouter, virtual public sigc::trackable {
  private:
    /// \brief String identifier
    const std::string m_id;
    /// \brief Integer identifier
    const long m_intId;

    /// \brief Copy constructor private and un-implemented to prevent slicing
    BaseEntity(const BaseEntity &);
    /// \brief Assignment operator private and un-implemented to prevent slicing
    const BaseEntity & operator=(const BaseEntity &);
  protected:
    explicit BaseEntity(const std::string & id, long intId);
  public:
    virtual ~BaseEntity();

    /// \brief Read only accessor for string identity
    const std::string & getId() const {
        return m_id;
    }

    /// \brief Read only accessor for Integer identity
    long getIntId() const {
        return m_intId;
    }

    virtual void addToMessage(Atlas::Message::MapType &) const;
    virtual void addToEntity(const Atlas::Objects::Entity::RootEntity &) const;

    void error(const Operation &, const std::string & errstring, OpVector &,
               const std::string & to = "") const;
    void clientError(const Operation &, const std::string & errstring,
                     OpVector &, const std::string & to = "") const;

    /// \brief Signal emitted when this entity is removed from the server
    ///
    /// Note that this is usually well before the object is actually deleted
    /// and marks the conceptual destruction of the concept this entity
    /// represents, not the destruction of this object.
    sigc::signal<void> destroyed;
};

#endif // COMMON_BASE_ENTITY_H
