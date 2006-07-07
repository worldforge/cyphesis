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

#ifndef RULESETS_ENTITY_H
#define RULESETS_ENTITY_H

#include "attributes.h"

#include "modules/Location.h"

#include "common/BaseEntity.h"
#include "common/BaseWorld.h"

#include <iostream>
#include <cassert>

// Work in progress, this will be a way of inferring type relationships,
// and will replace the simple string type currently used.

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
class Entity : public BaseEntity {
  private:
    static std::set<std::string> m_immutable;
    static const std::set<std::string> & immutables();

    int m_refCount;
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
    /// Exists in this world.
    BaseWorld * m_world;
    /// Full details of location
    Location m_location;
    /// List of entities which use this as ref
    EntitySet m_contains;
    /// Flags indicating changes to attributes
    unsigned int m_update_flags;

    explicit Entity(const std::string & id, long intId);
    virtual ~Entity();

    void incRef() {
        ++m_refCount;
    }

    void decRef() {
        if (m_refCount <= 0) {
            assert(m_refCount == 0);
            delete this;
        } else {
            --m_refCount;
        }
    }

    int checkRef() const {
        return m_refCount;
    }

    bool isDestroyed() const {
        return m_destroyed;
    }

    Script * script() const {
        return m_script;
    }

    /// \brief Send an operation to the world for dispatch.
    ///
    /// sendWorld() bipasses serialno assignment, so you must ensure
    /// that serialno is sorted. This allows client serialnos to get
    /// in, so that client gets correct usefull refnos back.
    void sendWorld(const Operation & op) {
        m_world->message(op, *this);
    }

    const int getUpdateFlags() const { return m_update_flags; }
    const int getSeq() const { return m_seq; }
    const double getStatus() const { return m_status; }
    const std::string & getName() const { return m_name; }
    const std::string & getType() const { return m_type; }
    const double getMass() const { return m_mass; }

    const bool isPerceptive() const { return m_perceptive; }

    void clearUpdateFlags() { m_update_flags = 0; }

    void setStatus(const double s) {
        m_status = s;
    }

    void setName(const std::string & n) {
        m_name = n;
    }

    void setType(const std::string & t) {
        m_type = t;
    }

    void setMass(const double w) {
        m_mass = w;
    }

    const Atlas::Message::MapType & getAttributes() const {
        return m_attributes;
    }

    bool hasAttr(const std::string &) const;
    bool getAttr(const std::string &, Atlas::Message::Element &) const;
    void setAttr(const std::string &, const Atlas::Message::Element &);

    PropertyBase * getProperty(const std::string &) const;

    virtual void addToMessage(Atlas::Message::MapType &) const;
    virtual void addToEntity(const Atlas::Objects::Entity::RootEntity &) const;

    void setScript(Script * scrpt);
    void merge(const Atlas::Message::MapType &);
    void changeContainer(Entity *);

    void destroy();

    virtual void scriptSubscribe(const std::string &);

    virtual void externalOperation(const Operation & op);

    virtual void SetupOperation(const Operation & op, OpVector &);
    virtual void TickOperation(const Operation & op, OpVector &);
    virtual void ActionOperation(const Operation & op, OpVector &);
    virtual void ChopOperation(const Operation & op, OpVector &);
    virtual void CreateOperation(const Operation & op, OpVector &);
    virtual void CutOperation(const Operation & op, OpVector &);
    virtual void DeleteOperation(const Operation & op, OpVector &);
    virtual void EatOperation(const Operation & op, OpVector &);
    virtual void BurnOperation(const Operation & op, OpVector &);
    virtual void ImaginaryOperation(const Operation & op, OpVector &);
    virtual void MoveOperation(const Operation & op, OpVector &);
    virtual void NourishOperation(const Operation & op, OpVector &);
    virtual void SetOperation(const Operation & op, OpVector &);
    virtual void SightOperation(const Operation & op, OpVector &);
    virtual void SoundOperation(const Operation & op, OpVector &);
    virtual void TouchOperation(const Operation & op, OpVector &);
    virtual void LookOperation(const Operation & op, OpVector &);
    virtual void AppearanceOperation(const Operation & op, OpVector &);
    virtual void DisappearanceOperation(const Operation & op, OpVector &);
    virtual void AttackOperation(const Operation & op, OpVector &);
    virtual void OtherOperation(const Operation & op, OpVector &);

    /// Signal indicating that this entity has been changed
    sigc::signal<void> updated;

    /// Signal indicating that this entity has changed its LOC
    sigc::signal<void> containered;
};

inline std::ostream & operator<<(std::ostream& s, Location& v)
{
    return s << "{" << v.m_loc->getId() << "," << v.pos() << ","
             << v.m_velocity << "}";
}

#endif // RULESETS_ENTITY_H
