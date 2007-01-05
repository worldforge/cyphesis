// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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

// $Id: Entity.h,v 1.85 2007-01-05 17:19:27 alriddoch Exp $

#ifndef RULESETS_ENTITY_H
#define RULESETS_ENTITY_H

#include "attributes.h"

#include "modules/Location.h"

#include "common/BaseEntity.h"
#include "common/BaseWorld.h"

#include <sigc++/connection.h>

#include <iostream>
#include <cassert>

// Work in progress, this will be a way of inferring type relationships,
// and will replace the simple string type currently used.

/// \brief Experimental class in development to handle the inheritance tree
class EntityType {
  protected:
    std::string m_typeName;
    EntityType * const m_parent;
  public:
    explicit EntityType(const std::string & n,
                        EntityType * const parent = 0) : m_typeName(n),
                                                         m_parent(parent) {

    }

    bool isA(const EntityType & other) {
        return false;
    }
};

class Motion;
class Script;
class PropertyBase;

typedef std::map<std::string, PropertyBase *> PropertyDict;

/// \brief Classes that model in world entities
///
/// These classes are used to model all in world entities or objects.
/// \defgroup EntityClasses In World Entity Classes

/// \brief This is the base class from which all in-game objects inherit.
///
/// This class should not normally be instantiated directly.
/// This class provides hard-coded attributes which are common to most
/// in game objects, the dynamic attributes map, and a means to access both
/// transparantly without needing to know which are which.
/// It provides a physical location for the entity, and a contains list
/// which lists other entities which specify their location with reference to
/// this one. It also provides the script interface for handling operations
/// in scripts rather than in the C++ code.
/// This is now also intended to be the base for in-game persistance.
/// It implements the basic types required for persistance.
/// \ingroup EntityClasses
class Entity : public BaseEntity {
  private:
    static std::set<std::string> m_immutable;
    static const std::set<std::string> & immutables();

    /// Count of references held by other objects to this entity
    int m_refCount;
    /// Flag indicating that this entity has been destroyed
    bool m_destroyed;
  protected:
    /// Script associated with this entity
    Script * m_script;
    /// Motion behavoir of this entity
    Motion * m_motion;
    /// Map of non-hardcoded attributes
    Atlas::Message::MapType m_attributes;
    /// Map of properties
    PropertyDict m_properties;
    /// Map of operation handlers
    HandlerMap m_operationHandlers;

    /// Sequence number
    int m_seq;
    /// Health/damage coeficient
    double m_status;
    /// Easy access to primary parent
    std::string m_type;
    /// Entities name
    std::string m_name;
    /// Mass in kg
    double m_mass;
    /// Is this perceptive
    bool m_perceptive;
  public:
    /// Full details of location
    Location m_location;
    /// List of entities which use this as ref
    EntitySet m_contains;
    /// Flags indicating changes to attributes
    unsigned int m_update_flags;

    explicit Entity(const std::string & id, long intId);
    virtual ~Entity();

    /// \brief Increment the reference count on this entity
    void incRef() {
        ++m_refCount;
    }

    /// \brief Decrement the reference count on this entity
    void decRef() {
        if (m_refCount <= 0) {
            assert(m_refCount == 0);
            delete this;
        } else {
            --m_refCount;
        }
    }

    /// \brief Check the reference count on this entity
    int checkRef() const {
        return m_refCount;
    }

    /// \brief Check if this entity is flagged as destroyed
    bool isDestroyed() const {
        return m_destroyed;
    }

    /// \brief Accessor for pointer to script object
    Script * script() const {
        return m_script;
    }

    /// \brief Send an operation to the world for dispatch.
    ///
    /// sendWorld() bipasses serialno assignment, so you must ensure
    /// that serialno is sorted. This allows client serialnos to get
    /// in, so that client gets correct usefull refnos back.
    void sendWorld(const Operation & op) {
        BaseWorld::instance().message(op, *this);
    }

    /// \brief Accessor for update flags
    const int getUpdateFlags() const { return m_update_flags; }
    /// \brief Accessor for sequence number
    const int getSeq() const { return m_seq; }
    /// \brief Accessor for status property
    const double getStatus() const { return m_status; }
    /// \brief Accessor for name property
    const std::string & getName() const { return m_name; }
    /// \brief Accessor for entity type property
    const std::string & getType() const { return m_type; }
    /// \brief Accessor for mass property
    const double getMass() const { return m_mass; }

    /// \brief Check if this entity is flagged as perceptive
    const bool isPerceptive() const { return m_perceptive; }

    /// \brief Reset the update flags
    void clearUpdateFlags() { m_update_flags = 0; }

    /// \brief Set the value of the status property
    void setStatus(const double s) {
        m_status = s;
    }

    /// \brief Set the value of the status property
    void setName(const std::string & n) {
        m_name = n;
    }

    /// \brief Set the value of the entity type property
    void setType(const std::string & t) {
        m_type = t;
    }

    /// \brief Set the value of the mass property
    void setMass(const double w) {
        m_mass = w;
    }

    /// \brief Accessor for soft attribute map
    const Atlas::Message::MapType & getAttributes() const {
        return m_attributes;
    }

    bool hasAttr(const std::string &) const;
    bool getAttr(const std::string &, Atlas::Message::Element &) const;
    void setAttr(const std::string &, const Atlas::Message::Element &);

    PropertyBase * getProperty(const std::string &) const;

    void installHandler(int, Handler);

    void setScript(Script * scrpt);
    void merge(const Atlas::Message::MapType &);
    void changeContainer(Entity *);

    void destroy();

    virtual void scriptSubscribe(const std::string &);

    virtual void addToMessage(Atlas::Message::MapType &) const;
    virtual void addToEntity(const Atlas::Objects::Entity::RootEntity &) const;

    virtual void externalOperation(const Operation & op);
    virtual void operation(const Operation &, OpVector &);

    /// Signal indicating that this entity has been changed
    sigc::signal<void> updated;

    /// Signal indicating that this entity has changed its LOC
    sigc::signal<void> containered;

    /// Signal connections to be removed after firing
    std::list<sigc::connection> containered_oneshots;
};

inline std::ostream & operator<<(std::ostream& s, Location& v)
{
    return s << "{" << v.m_loc->getId() << "," << v.pos() << ","
             << v.m_velocity << "}";
}

#endif // RULESETS_ENTITY_H
