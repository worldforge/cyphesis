// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef ENTITY_H
#define ENTITY_H

#include "attributes.h"

#include "modules/Location.h"

#include "common/BaseEntity.h"
#include "common/BaseWorld.h"

#include <iostream>

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
  protected:
    Script * m_script;
    MapType m_attributes;
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

    explicit Entity(const std::string & id);
    virtual ~Entity();

    /// \brief Send an operation to the world for dispatch.
    ///
    /// sendWorld() bipasses serialno assignment, so you must ensure
    /// that serialno is sorted. This allows client serialnos to get
    /// in, so that client gets correct usefull refnos back.
    void sendWorld(RootOperation * op) const {
        m_world->message(*op, this);
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

    const MapType & getAttributes() const {
        return m_attributes;
    }

    virtual bool get(const std::string &, Element &) const;
    virtual void set(const std::string &, const Element &);

    void setScript(Script * scrpt);
    void merge(const MapType &);

    template <class EntityType>
    bool getLocation(const MapType &,
                     const std::map<std::string, EntityType *> &);

    Vector3D getXyz() const;
    void destroy();
    void scriptSubscribe(const std::string &);

    virtual void addToMessage(MapType & obj) const;

    virtual void externalOperation(const RootOperation & op);

    virtual void SetupOperation(const Setup & op, OpVector &);
    virtual void TickOperation(const Tick & op, OpVector &);
    virtual void ActionOperation(const Action & op, OpVector &);
    virtual void ChopOperation(const Chop & op, OpVector &);
    virtual void CreateOperation(const Create & op, OpVector &);
    virtual void CutOperation(const Cut & op, OpVector &);
    virtual void DeleteOperation(const Delete & op, OpVector &);
    virtual void EatOperation(const Eat & op, OpVector &);
    virtual void BurnOperation(const Burn & op, OpVector &);
    virtual void ImaginaryOperation(const Imaginary & op, OpVector &);
    virtual void MoveOperation(const Move & op, OpVector &);
    virtual void NourishOperation(const Nourish & op, OpVector &);
    virtual void SetOperation(const Set & op, OpVector &);
    virtual void SightOperation(const Sight & op, OpVector &);
    virtual void SoundOperation(const Sound & op, OpVector &);
    virtual void TouchOperation(const Touch & op, OpVector &);
    virtual void LookOperation(const Look & op, OpVector &);
    virtual void AppearanceOperation(const Appearance & op, OpVector &);
    virtual void DisappearanceOperation(const Disappearance & op, OpVector &);
    virtual void OtherOperation(const RootOperation & op, OpVector &);

    SigC::Signal0<void> updated;
};

inline std::ostream & operator<<(std::ostream& s, Location& v)
{
    return s << "{" << v.m_loc->getId() << "," << v.m_pos << ","
             << v.m_velocity << "}";
}

#endif // ENTITY_H
