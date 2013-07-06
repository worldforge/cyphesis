// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2007 Alistair Riddoch
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


#ifndef RULESETS_LOCATED_ENTITY_H
#define RULESETS_LOCATED_ENTITY_H

#include "modules/Location.h"

#include "common/Property.h"
#include "common/Router.h"
#include "common/log.h"
#include "common/compose.hpp"

#include <sigc++/signal.h>

#include <set>

#include <cassert>

class Domain;
class LocatedEntity;
class PropertyBase;
class Script;
class TypeNode;

template <typename T>
class Property;

typedef std::set<LocatedEntity *> LocatedEntitySet;
typedef std::map<std::string, PropertyBase *> PropertyDict;

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
/// \brief Flag indicating entity is visible
/// \ingroup EntityFlags
/// Currently only used on MemEntity
static const unsigned int entity_visible = 1 << 7;
/// \brief Flag indicating entity is asleep
/// \ingroup EntityFlags
/// Currently only used on BaseMind
static const unsigned int entity_asleep = 1 << 8;


/// \brief This is the base class from which in-game and in-memory objects
/// inherit.
///
/// This class should not normally be instantiated directly.
/// This class provides hard-coded attributes that are common to all
/// in-game objects, and objects in the memory on an NPC.
/// It provides a physical location for the entity, and a contains list
/// which lists other entities which specify their location with reference to
/// this one. It also provides the script interface for handling operations
/// in scripts rather than in the C++ code.
class LocatedEntity : public Router {
  private:
    static std::set<std::string> m_immutable;
    static const std::set<std::string> & immutables();

    /// Count of references held by other objects to this entity
    int m_refCount;
  protected:
    /// Map of properties
    PropertyDict m_properties;

    /// Sequence number
    int m_seq;

    /// Script associated with this entity
    Script * m_script;
    /// Class of which this is an instance
    const TypeNode * m_type;
    /// Flags indicating changes to attributes
    unsigned int m_flags;

  public:
    /// Full details of location
    Location m_location;
    /// List of entities which use this as ref
    LocatedEntitySet * m_contains;

    explicit LocatedEntity(const std::string & id, long intId);
    virtual ~LocatedEntity();

    /// \brief Increment the reference count on this entity
    void incRef() {
        ++m_refCount;
    }

    /// \brief Decrement the reference count on this entity
    void decRef() {
        if (--m_refCount < 0) {
            assert(m_refCount == -1);
            delete this;
        }
    }

    /// \brief Check the reference count on this entity
    int checkRef() const {
        return m_refCount;
    }

    /// \brief Check if this entity is flagged as perceptive
    bool isPerceptive() const { return m_flags & entity_perceptive; }

    /// \brief Check if this entity is flagged as destroyed
    bool isDestroyed() const { return m_flags & entity_destroyed; }

    bool isVisible() const { return m_flags & entity_visible; }

    /// \brief Accessor for flags
    const int getFlags() const { return m_flags; }

    void setFlags(unsigned int flags) { m_flags |= flags; }

    void resetFlags(unsigned int flags) { m_flags &= ~flags; }

    /// \brief Accessor for pointer to script object
    Script * script() const {
        return m_script;
    }

    /// \brief Accessor for sequence number
    const int getSeq() const { return m_seq; }
    /// \brief Accessor for entity type property
    const TypeNode * getType() const { return m_type; }
    /// \brief Accessor for properies
    const PropertyDict & getProperties() const { return m_properties; }

    /// \brief Set the value of the entity type property
    void setType(const TypeNode * t) {
        m_type = t;
    }

    virtual bool hasAttr(const std::string & name) const;
    virtual int getAttr(const std::string & name,
                        Atlas::Message::Element &) const;
    virtual int getAttrType(const std::string & name,
                            Atlas::Message::Element &,
                            int type) const;
    virtual PropertyBase* setAttr(const std::string & name,
                                  const Atlas::Message::Element &);
    virtual const PropertyBase * getProperty(const std::string & name) const;
    // FIXME These should be de-visrtualised and, and implementations moved
    // from Entity to here.
    virtual PropertyBase * modProperty(const std::string & name);
    virtual PropertyBase * setProperty(const std::string & name, PropertyBase * prop);

    virtual void installDelegate(int, const std::string &);

    virtual void onContainered(const LocatedEntity* oldLocation);
    virtual void onUpdated();

    virtual void destroy() = 0;

    virtual Domain * getMovementDomain();

    virtual void sendWorld(const Operation & op);

    void setScript(Script * scrpt);
    void makeContainer();
    void changeContainer(LocatedEntity *);
    void merge(const Atlas::Message::MapType &);

    /// \brief Get a property that is required to of a given type.
    template <class PropertyT>
    const PropertyT * getPropertyClass(const std::string & name) const
    {
        const PropertyBase * p = getProperty(name);
        if (p != 0) {
            return dynamic_cast<const PropertyT *>(p);
        }
        return 0;
    }

    /// \brief Get a property that is a generic property of a given type
    template <typename T>
    const Property<T> * getPropertyType(const std::string & name) const
    {
        const PropertyBase * p = getProperty(name);
        if (p != 0) {
            return dynamic_cast<const Property<T> *>(p);
        }
        return 0;
    }

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
            //Assert that the stored property is of the correct type. If not,
            //it needs to be installed into CorePropertyManager.
            //We want to do this here, because allowing for properties to be
            //installed of the wrong type brings instability to the system.
            assert(sp);
        }
        if (sp == 0) {
            // If it is not of the right type, delete it and a new
            // one of the right type will be inserted.
            m_properties[name] = sp = new PropertyT;
            sp->install(this, name);
            if (p != 0) {
                log(WARNING, String::compose("Property %1 on entity with id %2 "
                        "reinstalled with new class."
                        "This might cause instability. Make sure that all "
                        "properties are properly installed in "
                        "CorePropertyManager.", name, getId()));
                Atlas::Message::Element val;
                if (p->get(val)) {
                    sp->set(val);
                }
                delete p;
            } else if (!def_val.isNone()) {
                sp->set(def_val);
            }
            sp->apply(this);
        }
        return sp;
    }

    /// Signal indicating that this entity has been changed
    sigc::signal<void> updated;

    /// Signal indicating that this entity has changed its LOC.
    /// First parameter is the old location.
    sigc::signal<void, const LocatedEntity*> containered;

    /// \brief Signal emitted when this entity is removed from the server
    ///
    /// Note that this is usually well before the object is actually deleted
    /// and marks the conceptual destruction of the concept this entity
    /// represents, not the destruction of this object.
    sigc::signal<void> destroyed;

    friend class LocatedEntitytest;
};

#endif // RULESETS_LOCATED_ENTITY_H
