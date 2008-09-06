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

// $Id$

#ifndef COMMON_PROPERTY_H
#define COMMON_PROPERTY_H

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/ObjectsFwd.h>

#include <sigc++/trackable.h>
#include <sigc++/signal.h>

class Entity;

/// \brief Interface for Entity properties
///
/// \ingroup PropertyClasses
class PropertyBase {
  protected:
    /// \brief Flags indicating how this Property should be handled
    unsigned int m_flags;
    explicit PropertyBase(unsigned int);
  public:
    virtual ~PropertyBase();

    /// \brief Accessor for Property flags
    unsigned int flags() const { return m_flags; }
    /// \brief Accessor for Property flags
    unsigned int & flags() { return m_flags; }

    void setFlags(unsigned int flags) { m_flags |= flags; }

    void resetFlags(unsigned int flags) { m_flags &= ~flags; }

    /// \brief Install this property on an entity
    ///
    /// Called whenever an Entity gains this property for the first time
    virtual void install(Entity *);
    /// \brief Apply whatever effect this property has on an Entity
    ///
    /// Called whenever the value of this property should affect an Entity
    virtual void apply(Entity *);

    /// \brief Copy the value of the property into an Atlas Message
    virtual bool get(Atlas::Message::Element & val) const = 0;
    /// \brief Read the value of the property from an Atlas Message
    virtual void set(const Atlas::Message::Element & val) = 0;
    /// \brief Add the value as an attribute to an Atlas map
    virtual void add(const std::string & key, Atlas::Message::MapType & map) const;
    /// \brief Add the value as an attribute to an Atlas entity
    virtual void add(const std::string & key, const Atlas::Objects::Entity::RootEntity & ent) const;
};

/// \brief Flag indicating data has been written to permanent store
static const unsigned int per_clean = 1 << 0;
/// \brief Flag indicating data should never be persisted
static const unsigned int per_ephem = 1 << 1;
/// \brief Flag indicating data has been stored initially
static const unsigned int per_seen = 1 << 2;

/// \brief Flag mask indicating data should not be written to store
static const unsigned int per_mask = per_clean | per_ephem;

/// \brief Flag indicating data is not visible
static const unsigned int vis_hidden = 1 << 3;
/// \brief Flag indicating data is server internal
static const unsigned int vis_internal = 1 << 4;

/// \brief Flag mask indicating data should be be perceptable
static const unsigned int vis_mask = vis_hidden | vis_internal;

/// \brief Flag set to indicate this is a class property, and has no instance
static const unsigned int flag_class = 1 << 5;

/// \brief Flag used for boolean properties
static const unsigned int flag_bool = 1 << 6;

/// \brief Entity property template for properties with single data values
/// that cannot be modified directly.
///
/// Properties like CONTAINS, LOC and POS are accessed this way, as they
/// are only ever modified as a result of a move operation.
/// \ingroup PropertyClasses
template <typename T>
class ImmutableProperty : public PropertyBase {
  protected:
    /// \brief Reference to variable holding the value of this Property
    const T & m_data;
  public:
    explicit ImmutableProperty(const T & data, unsigned int flags = 0);

    virtual bool get(Atlas::Message::Element & val) const;
    virtual void set(const Atlas::Message::Element & val);
    virtual void add(const std::string & key, Atlas::Message::MapType & map) const;
    virtual void add(const std::string & key, const Atlas::Objects::Entity::RootEntity & ent) const;
};

/// \brief Entity property template for properties with single data values
/// \ingroup PropertyClasses
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
/// \ingroup PropertyClasses
template <typename T>
class SignalProperty : public Property<T>, virtual public sigc::trackable {
  public:
    explicit SignalProperty(T & data, unsigned int flags);

    virtual void set(const Atlas::Message::Element & val);

    /// \brief Signal that is emitted when this Property is modified.
    sigc::signal<void> modified;
};

class SoftProperty : public PropertyBase {
  protected:
    Atlas::Message::Element m_data;
  public:
    explicit SoftProperty(const Atlas::Message::Element & data);

    /// \brief Copy the value of the property into an Atlas Message
    virtual bool get(Atlas::Message::Element & val) const;
    /// \brief Read the value of the property from an Atlas Message
    virtual void set(const Atlas::Message::Element & val);
};

#endif // COMMON_PROPERTY_H
