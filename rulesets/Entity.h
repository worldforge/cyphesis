// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef ENTITY_H
#define ENTITY_H

#include <modules/Location.h>
#include <common/BaseEntity.h>

class BaseWorld;
class MemMap;
class Script;

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

class Entity : public BaseEntity {
  private:
    static std::set<std::string> m_immutable;
    static const std::set<std::string> & immutables();
  protected:
    Script * script;
    Atlas::Message::Object::MapType attributes;
    int seq;			// Sequence number
    double status;		// Health/damage coeficient
    std::string type;		// Easy access to primary parent
    std::string name;		// Entities name
    double weight;		// Weight in kg
    bool deleted;		// Has this been deleted
    bool omnipresent;		// Is this omnipresent
    bool perceptive;		// Is this perceptive
  public:
    BaseWorld * world;		// Exists in this world.
    Location location;		// Full details of location inc. ref pos and vel
    EntitySet contains;		// List of entities which use this as ref

    Entity();
    virtual ~Entity();

    const int getSeq() const { return seq; }
    const double getStatus() const { return status; }
    const std::string & getName() const { return name; }
    const std::string & getType() const { return type; }
    const double getWeight() const { return weight; }

    const bool isDeleted() const { return deleted; }
    const bool isOmnipresent() const { return omnipresent; }
    const bool isPerceptive() const { return perceptive; }

    void setStatus(const double s) {
        status = s;
    }

    void setName(const std::string & n) {
        name = n;
    }

    void setType(const std::string & t) {
        type = t;
    }

    void setWeight(const double w) {
        weight = w;
    }

    virtual const Atlas::Message::Object & operator[](const std::string & aname);
    virtual void set(const std::string & aname, const Atlas::Message::Object & attr);

    void setScript(Script * scrpt);
    void merge(const Atlas::Message::Object::MapType &);
    void getLocation(const Atlas::Message::Object::MapType &,
                     const EntityDict &);
    Vector3D getXyz() const;
    void destroy();

    virtual void addToObject(Atlas::Message::Object::MapType & obj) const;
    virtual OpVector SetupOperation(const Setup & op);
    virtual OpVector TickOperation(const Tick & op);
    virtual OpVector ActionOperation(const Action & op);
    virtual OpVector ChopOperation(const Chop & op);
    virtual OpVector CreateOperation(const Create & op);
    virtual OpVector CutOperation(const Cut & op);
    virtual OpVector DeleteOperation(const Delete & op);
    virtual OpVector EatOperation(const Eat & op);
    virtual OpVector FireOperation(const Fire & op);
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
};

inline std::ostream & operator<<(std::ostream& s, Location& v)
{
    return s << "{" << v.ref->getId() << "," << v.coords << "," << v.velocity << "}";
}

#endif // ENTITY_H
