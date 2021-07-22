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

#include "modules/Flags.h"
#include "OperationRouter.h"
#include "common/ModifierType.h"

#include <Atlas/Message/Element.h>

class LocatedEntity;
class TypeNode;

struct PropertyUtil {

    /**
     * Extract the property visibility flags from the name.
     * Names that starts with "__" are "private". Only visible to the simulation and to administrators.
     * Names that starts with "_" are "protected". Only visible to the entity it belongs, the simulation and to administrators.
     * All other properties are "public", i.e. visible to everyone.
     * @param name A property name.
     * @return
     */
    static std::uint32_t flagsForPropertyName(const std::string& name);

    /**
     * Checks if the name supplied is a valid property name.
     *
     * It should not be more than 32 characters, and can only contain ascii characters or numbers, dollar sign ("$"), underscores ("_") or hyphens ("-").
     */
    static bool isValidName(const std::string& name);

    static std::pair<ModifierType, std::string> parsePropertyModification(const std::string& propertyName);
};


/// \brief Classes that define properties on in world entities
///
/// Property classes handle the values of Atlas attributes on in
/// game entities, ensuring type safety, and encapsulating certain
/// behaviors related to the presence and value of the attribute.
/// A property instance can be associated with an Entity instance
/// or a class, so it should not store any data specific to any of
/// the Entity instances it has been applied to. When it is taking effect
/// on an Entity for the first time, PropertyBase::install() is called
/// to allow the property to do any setup required, such as install one
/// or more operation handlers. When the property value must be applied
/// to an Entity, PropertyBase::apply() is called so that any side effect
/// of the value can be taken care of.
/// \defgroup PropertyClasses Entity Property Classes

/// \brief Flags used to control properties
///
/// The base class PropertyBase has a flag member which can be used to
/// control or track the property in various ways. The constants in this
/// group define the masks for these flags.
/// \defgroup PropertyFlags Entity Property Flags


/// \brief Interface for Entity properties
///
/// \ingroup PropertyClasses
template<typename EntityT>
class PropertyCore : public OperationsListener {
  protected:
    /// \brief Flags indicating how this Property should be handled
    Flags m_flags;
    /// \brief Constructor called from classes which inherit from Property
    /// @param flags default value for the Property flags
    explicit PropertyCore(std::uint32_t flags = 0);
    PropertyCore(const PropertyCore &) = default;
  public:
    virtual ~PropertyCore() = default;

    /// \brief Accessor for Property flags
    const Flags& flags() const { return m_flags; }
    /// \brief Accessor for Property flags
    Flags& flags() { return m_flags; }

    void addFlags(std::uint32_t flags)
    {
        m_flags.addFlags(flags);
    }

    void removeFlags(std::uint32_t flags)
    {
        m_flags.removeFlags(flags);
    }

    bool hasFlags(std::uint32_t flags) const
    {
        return m_flags.hasFlags(flags);
    }

    /// \brief Install this property on an entity
    ///
    /// Called whenever an Entity gains this property for the first time
    virtual void install(EntityT &, const std::string &);
    /// \brief Install this property on a type
    ///
    /// Called whenever a TypeNode gains this property for the first time
    virtual void install(TypeNode &, const std::string &);
    /// \brief Remove this property from an entity.
    ///
    /// Called whenever the property is removed or the entity is shutting down.
    virtual void remove(EntityT &, const std::string & name);
    /// \brief Apply whatever effect this property has on an Entity
    ///
    /// Called whenever the value of this property should affect an Entity
    virtual void apply(EntityT &);


    /// \brief Copy the value of the property into an Atlas Message
    virtual int get(Atlas::Message::Element & val) const = 0;
    /// \brief Read the value of the property from an Atlas Message
    virtual void set(const Atlas::Message::Element & val) = 0;
    /// \brief Add the value as an attribute to an Atlas map
    //TODO: remove this as it's not used
    virtual void add(const std::string & key, Atlas::Message::MapType & map) const;
    /// \brief Add the value as an attribute to an Atlas entity
    virtual void add(const std::string & key, const Atlas::Objects::Entity::RootEntity & ent) const;
    /// \brief Handle an operation
    HandlerResult operation(LocatedEntity &,
                                    const Operation &,
                                    OpVector &) override;
    /// \brief Create a copy of this instance
    ///
    /// The copy should have exactly the same type, and the same value
    virtual PropertyCore * copy() const = 0;

    bool operator==(const PropertyCore& rhs) const;
    bool operator!=(const PropertyCore& rhs) const;
};

typedef PropertyCore<LocatedEntity> PropertyBase;

/// \brief Flag indicating data has been written to permanent store
/// \ingroup PropertyFlags
static const std::uint32_t prop_flag_persistence_clean = 1u << 0u;
/// \brief Flag indicating data should never be persisted
/// \ingroup PropertyFlags
static const std::uint32_t prop_flag_persistence_ephem = 1u << 1u;
/// \brief Flag indicating data has been stored initially
/// \ingroup PropertyFlags
static const std::uint32_t prop_flag_persistence_seen = 1u << 2u;

/// \brief Flag mask indicating data should not be written to store
/// \ingroup PropertyFlags
static const std::uint32_t prop_flag_persistence_mask = prop_flag_persistence_clean | prop_flag_persistence_ephem;

/// \brief Flag indicating property is "private", i.e. only available to the simulation.
/// \ingroup PropertyFlags
static const std::uint32_t prop_flag_visibility_private = 1u << 3u;
/// \brief Flag indicating property is "protected", i.e. only available to the entity itself.
/// \ingroup PropertyFlags
static const std::uint32_t prop_flag_visibility_protected = 1u << 4u;

/// \brief Flag mask indicating property is private or protected.
/// \ingroup PropertyFlags
static const std::uint32_t prop_flag_visibility_non_public = prop_flag_visibility_private | prop_flag_visibility_protected;

/// \brief Flag set to indicate this is a class property, and has no instance
/// \ingroup PropertyFlags
static const std::uint32_t prop_flag_class = 1u << 5u;

/// \brief Flag used for boolean properties
/// \ingroup PropertyFlags
static const std::uint32_t prop_flag_bool = 1u << 6u;

/// \brief Flag used to mark properties whose state has not been broadcast
/// \ingroup PropertyFlags
static const std::uint32_t prop_flag_unsent = 1u << 7u;

/// \brief Flag used to mark properties which must be instance properties
/// \ingroup PropertyFlags
/// Typically this will be because they have per-entity state which cannot
/// be handled on a class property.
static const std::uint32_t prop_flag_instance = 1u << 8u;

/**
 * \brief The property won't allow any kind of modifiers to affect it.
 * \ingroup PropertyFlags
 */
static const std::uint32_t prop_flag_modifiers_not_allowed = 1u << 9u;


/// \brief Entity property template for properties with single data values
/// \ingroup PropertyClasses
template <typename T>
class Property : public PropertyBase {
  protected:
    /// \brief Reference to variable holding the value of this Property
    T m_data;
    Property(const Property<T> &) = default;
  public:
    static const std::string property_atlastype;

    explicit Property(unsigned int flags = 0);

    const T & data() const { return this->m_data; }
    T & data() { return this->m_data; }

    int get(Atlas::Message::Element & val) const override;

    void set(const Atlas::Message::Element &) override;

    void add(const std::string & key, Atlas::Message::MapType & map) const override;
    void add(const std::string & key, const Atlas::Objects::Entity::RootEntity & ent) const override;
    Property<T> * copy() const override;
};

/// \brief Entity property that can store any Atlas value
/// \ingroup PropertyClasses
class SoftProperty : public PropertyBase {
  protected:
    Atlas::Message::Element m_data;
  public:
    explicit SoftProperty() = default;
    explicit SoftProperty(Atlas::Message::Element  data);

    int get(Atlas::Message::Element & val) const override ;
    void set(const Atlas::Message::Element & val) override ;
    SoftProperty * copy() const override;

    Atlas::Message::Element& data();
    const Atlas::Message::Element& data() const;
};

class BoolProperty : public PropertyBase {
public:
    static constexpr const char* property_atlastype = "int";

    explicit BoolProperty() = default;

    int get(Atlas::Message::Element & val) const override;
    void set(const Atlas::Message::Element & val) override;
    BoolProperty * copy() const override;

    bool isTrue() const;

};

#endif // COMMON_PROPERTY_H
