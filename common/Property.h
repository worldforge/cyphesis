// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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

#ifndef COMMON_PROPERTY_H
#define COMMON_PROPERTY_H

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/ObjectsFwd.h>

#include <sigc++/trackable.h>
#include <sigc++/signal.h>

/// \brief Interface for Entity properties
class PropertyBase {
  protected:
    /// \brief Flags indicating how this Property should be handled
    const unsigned int m_flags;
    explicit PropertyBase(unsigned int);
  public:
    virtual ~PropertyBase();

    /// \brief Accessor for Property flags
    unsigned int flags() const { return m_flags; }

    /// \brief Copy the value of the property into an Atlas Message
    virtual bool get(Atlas::Message::Element &) const = 0;
    /// \brief Read the value of the property from an Atlas Message
    virtual void set(const Atlas::Message::Element &) = 0;
    /// \brief Add the value as an attribute to an Atlas map
    virtual void add(const std::string &, Atlas::Message::MapType & map) const;
    /// \brief Add the value as an attribute to an Atlas entity
    virtual void add(const std::string &, const Atlas::Objects::Entity::RootEntity &) const;
};

/// \brief Entity property template for properties with single data values
/// that cannot be modified directly.
///
/// Properties like CONTAINS, LOC and POS are accessed this way, as they
/// are only ever modified as a result of a move operation.
template <typename T>
class ImmutableProperty : public PropertyBase {
  protected:
    /// \brief Reference to variable holding the value of this Property
    const T & m_data;
  public:
    explicit ImmutableProperty(const T & data, unsigned int flags = 0);

    virtual bool get(Atlas::Message::Element &) const;
    virtual void set(const Atlas::Message::Element &);
    virtual void add(const std::string &, Atlas::Message::MapType & map) const;
    virtual void add(const std::string &, const Atlas::Objects::Entity::RootEntity &) const;
};

/// \brief Entity property template for properties with single data values
template <typename T>
class Property : public ImmutableProperty<T> {
  protected:
    /// \brief Reference to variable holding the value of this Property
    T & m_modData;
  public:
    explicit Property(T & data, unsigned int flags);

    virtual void set(const Atlas::Message::Element &);
};

/// \brief Entity property template for properties with single data values
template <typename T>
class SignalProperty : public Property<T>, virtual public sigc::trackable {
  public:
    explicit SignalProperty(T & data, unsigned int flags);

    virtual void set(const Atlas::Message::Element &);

    sigc::signal<void> modified;
};

#endif // COMMON_PROPERTY_H
