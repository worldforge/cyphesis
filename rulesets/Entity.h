// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef ENTITY_H
#define ENTITY_H

#include "attributes.h"

#include "modules/Location.h"

#include "common/BaseEntity.h"

class BaseWorld;
class MemMap;
class Script;

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
        // FIXME
        return false;
    }
};

// This is the base class from which all in-game objects inherit.
// This class should not be instantiated directly.
// This class provides all the static atributes which are common to most
// in game objects, the dynamic attributes map, and a means to access both
// transparantly without needing to know which are which.
// It provides a physical location for the entity, and a contains list
// which lists other entities which specify their location with reference to
// this one.
// It also provides the script interface for handling operations
// in scripts rather than in the C++ code.
//
// 2002-11-28 Al Riddoch
//
// This is now also intended to be the base for in-game persistance.
// It implements the basic types required for persistance.

class Entity : public BaseEntity {
  private:
    static std::set<std::string> m_immutable;
    static const std::set<std::string> & immutables();
  protected:
    Script * m_script;
    Atlas::Message::Element::MapType m_attributes;
    int m_seq;                  // Sequence number
    double m_status;            // Health/damage coeficient
    std::string m_type;         // Easy access to primary parent
    std::string m_name;         // Entities name
    double m_mass;              // Mass in kg
    bool m_perceptive;          // Is this perceptive
  public:
    BaseWorld * m_world;        // Exists in this world.
    Location m_location;        // Full details of location
    EntitySet m_contains;       // List of entities which use this as ref
    unsigned int m_update_flags;

    // Entity();
    explicit Entity(const std::string & id);
    virtual ~Entity();

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

    const Atlas::Message::Element::MapType & getAttributes() const {
        return m_attributes;
    }

    virtual bool get(const std::string &, Atlas::Message::Element &) const;
    virtual void set(const std::string &, const Atlas::Message::Element &);

    void setScript(Script * scrpt);
    void merge(const Atlas::Message::Element::MapType &);
    bool getLocation(const Atlas::Message::Element::MapType &,
                     const EntityDict &);
    Vector3D getXyz() const;
    void destroy();
    void scriptSubscribe(const std::string &);

    virtual void addToObject(Atlas::Message::Element::MapType & obj) const;
    virtual OpVector SetupOperation(const Setup & op);
    virtual OpVector TickOperation(const Tick & op);
    virtual OpVector ActionOperation(const Action & op);
    virtual OpVector ChopOperation(const Chop & op);
    virtual OpVector CreateOperation(const Create & op);
    virtual OpVector CutOperation(const Cut & op);
    virtual OpVector DeleteOperation(const Delete & op);
    virtual OpVector EatOperation(const Eat & op);
    virtual OpVector BurnOperation(const Burn & op);
    virtual OpVector ImaginaryOperation(const Imaginary & op);
    virtual OpVector MoveOperation(const Move & op);
    virtual OpVector NourishOperation(const Nourish & op);
    virtual OpVector SetOperation(const Set & op);
    virtual OpVector SightOperation(const Sight & op);
    virtual OpVector SoundOperation(const Sound & op);
    virtual OpVector TouchOperation(const Touch & op);
    virtual OpVector LookOperation(const Look & op);
    virtual OpVector AppearanceOperation(const Appearance & op);
    virtual OpVector DisappearanceOperation(const Disappearance & op);
    virtual OpVector OtherOperation(const RootOperation & op);

    SigC::Signal0<void> updated;
};

inline std::ostream & operator<<(std::ostream& s, Location& v)
{
    return s << "{" << v.m_loc->getId() << "," << v.m_pos << ","
             << v.m_velocity << "}";
}

#endif // ENTITY_H
