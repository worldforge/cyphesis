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

// $Id$

#ifndef RULESETS_ENTITY_H
#define RULESETS_ENTITY_H

#include "LocatedEntity.h"

#include "modules/Location.h"

#include "common/BaseWorld.h"
#include "common/Property.h"

#include <iostream>

class Motion;

/// \brief Flag indicating entity has been written to permanent store
/// \ingroup EntityFlags
static const unsigned int entity_clean = 1 << 0;
/// \brief Flag indicating entity POS has been written to permanent store
/// \ingroup EntityFlags
static const unsigned int entity_pos_clean = 1 << 1;
/// \brief Flag indicating entity ORIENT has been written to permanent store
/// \ingroup EntityFlags
static const unsigned int entity_orient_clean = 1 << 2;

static const unsigned int entity_clean_mask = entity_clean |
                                              entity_pos_clean |
                                              entity_orient_clean;

/// \brief Flag indicating entity is perceptive
/// \ingroup EntityFlags
static const unsigned int entity_perceptive = 1 << 3;
/// \brief Flag indicating entity has been destroyed
/// \ingroup EntityFlags
static const unsigned int entity_destroyed = 1 << 4;
/// \brief Flag indicating entity has been queued for storage update
/// \ingroup EntityFlags
static const unsigned int entity_queued = 1 << 5;
/// \brief Flag indicaiting entity is ephemeral
/// \ingroup EntityFlags
static const unsigned int entity_ephem = 1 << 6;

/// \brief This is the base class from which all in-game objects inherit.
///
/// This class should not normally be instantiated directly.
/// This class provides hard-coded attributes which are common to most
/// in game objects, the dynamic attributes map, and a means to access both
/// transparantly without needing to know which are which.
/// This is now also intended to be the base for in-game persistance.
/// It implements the basic types required for persistance.
/// \ingroup EntityClasses
class Entity : public LocatedEntity {
  protected:
    /// Motion behavoir of this entity
    Motion * m_motion;
    /// Map of operation handlers
    HandlerMap m_operationHandlers;
    /// Flags indicating changes to attributes
    unsigned int m_flags;

  public:
    explicit Entity(const std::string & id, long intId);
    virtual ~Entity();

    /// \brief Accessor for pointer to motion object
    Motion * motion() const {
        return m_motion;
    }

    /// \brief Send an operation to the world for dispatch.
    ///
    /// sendWorld() bipasses serialno assignment, so you must ensure
    /// that serialno is sorted. This allows client serialnos to get
    /// in, so that client gets correct usefull refnos back.
    void sendWorld(const Operation & op) {
        BaseWorld::instance().message(op, *this);
    }

    /// \brief Check if this entity is flagged as perceptive
    const bool isPerceptive() const { return m_flags & entity_perceptive; }

    /// \brief Check if this entity is flagged as destroyed
    bool isDestroyed() const { return m_flags & entity_destroyed; }

    /// \brief Accessor for flags
    const int getFlags() const { return m_flags; }

    void setFlags(unsigned int flags) { m_flags |= flags; }

    void resetFlags(unsigned int flags) { m_flags &= ~flags; }

    virtual void setAttr(const std::string & name,
                         const Atlas::Message::Element &);
    virtual const PropertyBase * getProperty(const std::string & name) const;

    PropertyBase * modProperty(const std::string & name);
    void setProperty(const std::string & name, PropertyBase * prop);

    /// \brief Get a property that is required to of a given type.
    template <class PropertyT>
    PropertyT * modPropertyClass(const std::string & name)
    {
        PropertyBase * p = modProperty(name);
        if (p != 0) {
            return dynamic_cast<PropertyT *>(p);
        }
        return 0;
    }

    /// \brief Get a modifiable property that is a generic property of a type
    ///
    /// If the property is not set on the Entity instance, but has a class
    /// default, the default is copied to the instance, and a pointer is
    /// returned if it is a property of the right type.
    template <typename T>
    Property<T> * modPropertyType(const std::string & name)
    {
        PropertyBase * p = modProperty(name);
        if (p != 0) {
            return dynamic_cast<Property<T> *>(p);
        }
        return 0;
    }

    /// \brief Require that a property of a given type is set.
    ///
    /// If the property is not set on the Entity instance, but has a class
    /// default, the default is copied to the instance, and a pointer is
    /// returned if it is a property of the right type. If it does not
    /// exist, or is not of the right type, a new property is created of
    /// the right type, and installed on the Entity instance.
    template <class PropertyT>
    PropertyT * requirePropertyClass(const std::string & name,
                                     const Atlas::Message::Element & def_val
                                     = Atlas::Message::Element())
    {
        PropertyBase * p = modProperty(name);
        PropertyT * sp = 0;
        if (p != 0) {
            sp = dynamic_cast<PropertyT *>(p);
        }
        if (sp == 0) {
            m_properties[name] = sp = new PropertyT;
            if (p != 0) {
                // If it is not of the right type, delete it and a new
                // one of the right type will be inserted.
                Atlas::Message::Element val;
                if (p->get(val)) {
                    sp->set(val);
                }
                delete p;
            } else if (!def_val.isNone()) {
                sp->set(def_val);
            }
        }
        return sp;
    }

    void installHandler(int, Handler);

    void destroy();

    virtual void addToMessage(Atlas::Message::MapType &) const;
    virtual void addToEntity(const Atlas::Objects::Entity::RootEntity &) const;

    virtual void ActuateOperation(const Operation &, OpVector &);
    virtual void AppearanceOperation(const Operation &, OpVector &);
    virtual void AttackOperation(const Operation &, OpVector &);
    virtual void CombineOperation(const Operation &, OpVector &);
    virtual void CreateOperation(const Operation &, OpVector &);
    virtual void DeleteOperation(const Operation &, OpVector &);
    virtual void DisappearanceOperation(const Operation &, OpVector &);
    virtual void DivideOperation(const Operation &, OpVector &);
    virtual void EatOperation(const Operation &, OpVector &);
    virtual void ImaginaryOperation(const Operation &, OpVector &);
    virtual void LookOperation(const Operation &, OpVector &);
    virtual void MoveOperation(const Operation &, OpVector &);
    virtual void NourishOperation(const Operation &, OpVector &);
    virtual void SetOperation(const Operation &, OpVector &);
    virtual void SightOperation(const Operation &, OpVector &);
    virtual void SoundOperation(const Operation &, OpVector &);
    virtual void TalkOperation(const Operation &, OpVector &);
    virtual void TickOperation(const Operation &, OpVector &);
    virtual void TouchOperation(const Operation &, OpVector &);
    virtual void UpdateOperation(const Operation &, OpVector &);
    virtual void WieldOperation(const Operation &, OpVector &);

    virtual void externalOperation(const Operation & op);
    virtual void operation(const Operation &, OpVector &);

    void callOperation(const Operation &, OpVector &);

    virtual void onContainered();
    virtual void onUpdated();

    /// Signal indicating that this entity has been changed
    sigc::signal<void> updated;

    /// Single shot signal indicating that this entity has changed its LOC
    sigc::signal<void> containered;

    /// \brief Signal emitted when this entity is removed from the server
    ///
    /// Note that this is usually well before the object is actually deleted
    /// and marks the conceptual destruction of the concept this entity
    /// represents, not the destruction of this object.
    sigc::signal<void> destroyed;
};

inline std::ostream & operator<<(std::ostream& s, Location& v)
{
    return s << "{" << v.m_loc->getId() << "," << v.pos() << ","
             << v.m_velocity << "}";
}

#endif // RULESETS_ENTITY_H
