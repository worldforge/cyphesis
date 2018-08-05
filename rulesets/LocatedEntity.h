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


#include "modules/Ref.h"
#include "modules/Flags.h"
#include "Location.h"

#include "common/Property.h"
#include "common/Router.h"
#include "common/log.h"
#include "common/compose.hpp"

#include <sigc++/signal.h>

#include <boost/any.hpp>

#include <set>

#include <cassert>

class Domain;
class LocatedEntity;
class PropertyBase;
class Script;
class TypeNode;
class _object;

template <typename T>
class Property;

typedef std::set<Ref<LocatedEntity>> LocatedEntitySet;
typedef std::map<std::string, PropertyBase *> PropertyDict;

/// \brief Flag indicating entity has been written to permanent store
/// \ingroup EntityFlags
static const std::uint32_t entity_clean = 1u << 0u;
/// \brief Flag indicating entity POS has been written to permanent store
/// \ingroup EntityFlags
static const std::uint32_t entity_pos_clean = 1u << 1u;
/// \brief Flag indicating entity ORIENT has been written to permanent store
/// \ingroup EntityFlags
static const std::uint32_t entity_orient_clean = 1u << 2u;

static const std::uint32_t entity_clean_mask = entity_clean |
                                              entity_pos_clean |
                                              entity_orient_clean;

/// \brief Flag indicating entity is perceptive
/// \ingroup EntityFlags
static const std::uint32_t entity_perceptive = 1u << 3u;
/// \brief Flag indicating entity has been destroyed
/// \ingroup EntityFlags
static const std::uint32_t entity_destroyed = 1u << 4u;
/// \brief Flag indicating entity has been queued for storage update
/// \ingroup EntityFlags
static const std::uint32_t entity_queued = 1u << 5u;
/// \brief Flag indicaiting entity is ephemeral
/// \ingroup EntityFlags
static const std::uint32_t entity_ephem = 1u << 6u;
/// \brief Flag indicating entity is visible
/// \ingroup EntityFlags
/// Currently only used on MemEntity
static const std::uint32_t entity_visible = 1u << 7u;
/// \brief Flag indicating entity is asleep
/// \ingroup EntityFlags
/// Currently only used on BaseMind
static const std::uint32_t entity_asleep = 1u << 8u;
/// \brief Flag indicating entity contains a Domain, used for movement and sights
/// \ingroup EntityFlags
static const std::uint32_t entity_domain = 1u << 9u;

/// \brief Flag indicating entity has thoughts that needs to be persisted.
/// \ingroup EntityFlags
static const std::uint32_t entity_dirty_thoughts = 1u << 10u;

/// \brief Flag indicating entity has a location which has been changed since last sent to clients,
/// and new location data should be sent on the next Update.
/// \ingroup EntityFlags
static const std::uint32_t entity_dirty_location = 1u << 11u;

/**
 * The entity is an admin character, and has additional capabilities.
 */
static const std::uint32_t entity_admin = 1u << 12u;

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
    static std::set<std::string> s_immutable;
    static const std::set<std::string> & immutables();

    /// Count of references held by other objects to this entity
    int m_refCount;
  protected:
    /// Map of properties
    PropertyDict m_properties;

    /// Sequence number
    int m_seq;
    
    /// Class of which this is an instance
    const TypeNode * m_type;

    void clearProperties();

    /**
     * Collects all observers of the child, i.e. all entities that are currently observing it.
     * This method will walk upwards the entity chain.
     * @param child The child entity that's being observed.
     * @param op
     * @param res
     */
    void collectObserversForChild(const LocatedEntity& child, std::set<const LocatedEntity*>& receivers) const;

    public:

        /**
         * Visibility for broadcasted entities.
         */
        enum class Visibility {
                /**
                 * Sent to all.
                 */
                PUBLIC,
                /**
                 * Sent to the entity itself and admins.
                 */
                PROTECTED,

                /**
                 * Sent only to admins (not even the entity itself).
                 */
                PRIVATE
        };

        /// Flags indicating entity behaviour
        Flags m_flags;


        /// Scripts that are associated with this entity.
    std::vector<Script*> m_scripts;
    /// Full details of location
    Location m_location;
    /// List of entities which use this as ref
    LocatedEntitySet * m_contains;
    // A representation of this instance used by the scripting system. This is opaque to this class.
    boost::any m_scriptEntity;

    explicit LocatedEntity(const std::string & id, long intId);
    ~LocatedEntity() override;

    /// \brief Increment the reference count on this entity
    void incRef() {
        ++m_refCount;
    }

    /// \brief Decrement the reference count on this entity
    void decRef() {
        if (--m_refCount == 0) {
            delete this;
        }
        assert(m_refCount >= 0);
    }

    /// \brief Check the reference count on this entity
    int checkRef() const {
        return m_refCount;
    }

    /// \brief Check if this entity is flagged as perceptive
    bool isPerceptive() const { return m_flags.hasFlags(entity_perceptive); }

    /// \brief Check if this entity is flagged as destroyed
    bool isDestroyed() const { return m_flags.hasFlags(entity_destroyed); }

    bool isVisible() const { return m_flags.hasFlags(entity_visible); }

    /// \brief Accessor for flags
    const Flags& flags() const { return m_flags; }
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

    /// \brief Accessor for sequence number
    const int getSeq() const { return m_seq; }
    /// \brief Accessor for entity type property
    const TypeNode * getType() const { return m_type; }
    /// \brief Accessor for properies
    const PropertyDict & getProperties() const { return m_properties; }

    /// \brief Set the value of the entity type property
    virtual void setType(const TypeNode * t);

    virtual bool hasAttr(const std::string & name) const;
    virtual int getAttr(const std::string & name,
                        Atlas::Message::Element &) const;
    virtual int getAttrType(const std::string & name,
                            Atlas::Message::Element &,
                            int type) const;
    virtual PropertyBase* setAttr(const std::string & name,
                                  const Atlas::Message::Element &);
    virtual const PropertyBase * getProperty(const std::string & name) const;
    // FIXME These should be de-virtualised and, and implementations moved
    // from Entity to here.
    virtual PropertyBase * modProperty(const std::string & name, const Atlas::Message::Element& def_val = Atlas::Message::Element());
    virtual PropertyBase * setProperty(const std::string & name, PropertyBase * prop);

    virtual void installDelegate(int, const std::string &);
    virtual void removeDelegate(int, const std::string &);

    virtual void onContainered(const Ref<LocatedEntity>& oldLocation);
    virtual void onUpdated();

    virtual void destroy() = 0;

    virtual Domain * getDomain();
    virtual const Domain * getDomain() const;

    virtual void sendWorld(const Operation & op);

    virtual void setScript(Script * scrpt);
    void makeContainer();
    void changeContainer(const Ref<LocatedEntity>&);
    void merge(const Atlas::Message::MapType &);

    /**
     * Gets all registered thoughts.
     * @return All registered thoughts.
     */
    virtual std::vector<Atlas::Objects::Root> getThoughts() const;

    /// \brief Adds a child to this entity.
    virtual void addChild(LocatedEntity& childEntity);
    /// \brief Removes a child from this entity.
    virtual void removeChild(LocatedEntity& childEntity);


    /**
     * Collects all entities that are observing this entity.
     * @param observers A set which will be filled with observing entities.
     */
    void collectObservers(std::set<const LocatedEntity*>& observers) const;

    /**
     * Broadcasts an op.
     *
     * If this entity has a domain, the op is broadcast to all observers in the domain.
     * In addition, broadcastFromChild will be called on any parent entity, to make it broadcast to entities in the same domain, or in domains above.
     * @param op
     * @param res
     */
    void broadcast(const Atlas::Objects::Operation::RootOperation& op, OpVector& res, Visibility visibility) const;

    /**
     * Processes appearance and disappearance of this entity for other observing entities. This is done by matching the supplied list of entities that previously
     * observed the entity. When called, a list of entities that are currently observing it will be created, and the two lists will be compared.
     *
     * Any new entity gets a "Appearance" op, and any old entity which wasn't present in the new list will get a "Disappearance" op.
     * @param previousObserving
     * @param res
     */
    void processAppearDisappear(std::set<const LocatedEntity*> previousObserving, OpVector& res) const;

     /**
     * @brief Determines if this entity is visible to another entity.
     *
     * @param watcher The other entity observing this entity, for which we want to determine visibility.
     * @return True if this entity is visible to another entity.
     */
    bool isVisibleForOtherEntity(const LocatedEntity* watcher) const;

    /**
    * @brief Determines if this entity can be reached by another entity (i.e. physically interacted with).
    *
    * @param entityLocation Where we want to reach.
    * @return True if this entity is reachable to another entity.
    */
    bool canReach(const EntityLocation& entityLocation, float extraReach = 0) const;

    /// \brief Get a property that is required to of a given type.
    template <class PropertyT>
    const PropertyT * getPropertyClass(const std::string & name) const
    {
        const PropertyBase * p = getProperty(name);
        if (p != nullptr) {
            return dynamic_cast<const PropertyT *>(p);
        }
        return 0;
    }

    /// \brief Get a property that is required to of a given type.
    ///
    /// The specified class must present the "property_name" trait.
    template <class PropertyT>
    const PropertyT * getPropertyClassFixed() const
    {
        return this->getPropertyClass<PropertyT>(PropertyT::property_name);
    }

    /// \brief Get a property that is a generic property of a given type
    template <typename T>
    const Property<T> * getPropertyType(const std::string & name) const
    {
        const PropertyBase * p = getProperty(name);
        if (p != nullptr) {
            return dynamic_cast<const Property<T> *>(p);
        }
        return 0;
    }

    /// \brief Get a property that is required to of a given type.
    template <class PropertyT>
    PropertyT * modPropertyClass(const std::string & name)
    {
        PropertyBase * p = modProperty(name);
        if (p != nullptr) {
            return dynamic_cast<PropertyT *>(p);
        }
        return 0;
    }

    /// \brief Get a property that is required to of a given type.
    ///
    /// The specified class must present the "property_name" trait.
    template <class PropertyT>
    PropertyT * modPropertyClassFixed()
    {
        return this->modPropertyClass<PropertyT>(PropertyT::property_name);
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
        if (p != nullptr) {
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
        PropertyBase * p = modProperty(name, def_val);
        PropertyT * sp = nullptr;
        if (p != nullptr) {
            sp = dynamic_cast<PropertyT *>(p);
            //Assert that the stored property is of the correct type. If not,
            //it needs to be installed into CorePropertyManager.
            //We want to do this here, because allowing for properties to be
            //installed of the wrong type brings instability to the system.
            assert(sp);
        }
        if (sp == nullptr) {
            // If it is not of the right type, delete it and a new
            // one of the right type will be inserted.
            m_properties[name] = sp = new PropertyT;
            sp->install(this, name);
            if (p != nullptr) {
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

    /// \brief Require that a property of a given type is set, relying on the "property_name" trait.
    ///
    /// If the property is not set on the Entity instance, but has a class
    /// default, the default is copied to the instance, and a pointer is
    /// returned if it is a property of the right type. If it does not
    /// exist, or is not of the right type, a new property is created of
    /// the right type, and installed on the Entity instance.
    ///
    /// The specified class must present the "property_name" trait.
    template <class PropertyT>
    PropertyT * requirePropertyClassFixed(const Atlas::Message::Element & def_val
            = Atlas::Message::Element())
    {
        return this->requirePropertyClass<PropertyT>(PropertyT::property_name, def_val);
    }

    /**
     * Generates a string describing this entity, used for debugging.
     *
     * @return A string describing the entity in more detail.
     */
    std::string describeEntity() const;

    /// Signal indicating that this entity has been changed
    sigc::signal<void> updated;

    /// Signal indicating that this entity has changed its LOC.
    /// First parameter is the old location.
    sigc::signal<void, const Ref<LocatedEntity>&> containered;

    /// \brief Signal emitted when this entity is removed from the server
    ///
    /// Note that this is usually well before the object is actually deleted
    /// and marks the conceptual destruction of the concept this entity
    /// represents, not the destruction of this object.
    sigc::signal<void> destroyed;

    /// @brief Signal emitted whenever a property update is applied.
    ///
    /// The first parameter is the name of the property, the second is the updated property.
    sigc::signal<void, const std::string&, const PropertyBase&> propertyApplied;

    friend class LocatedEntitytest;
};

#endif // RULESETS_LOCATED_ENTITY_H
