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
  protected:
    Script * script;
    Atlas::Message::Object::MapType attributes;
    int seq;			// Sequence number
    double status;		// Health/damage coeficient
    std::string type;		// Easy access to primary parent
    std::string name;		// Entities name
    double weight;		// Weight in kg
    bool character;		// Is this a character
    bool deleted;		// Has this been deleted
    bool omnipresent;		// Is this omnipresent
  public:
    BaseWorld * world;		// Exists in this world.
    Location location;		// Full details of location inc. ref pos and vel
    eset_t contains;		// List of entities which use this as ref

    Entity();
    virtual ~Entity();

    const int getSeq() const { return seq; }
    const double getStatus() const { return status; }
    const std::string & getName() const { return name; }
    const std::string & getType() const { return type; }
    const double getWeight() const { return weight; }

    const bool isCharacter() const { return character; }
    const bool isDeleted() const { return deleted; }
    const bool isOmnipresent() const { return omnipresent; }

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
    virtual void destroy();

    void setScript(Script * scrpt);
    void merge(const Atlas::Message::Object::MapType &);
    void getLocation(const Atlas::Message::Object::MapType &, const edict_t &);
    Vector3D getXyz() const;

    virtual void addToObject(Atlas::Message::Object::MapType & obj) const;
    virtual oplist SetupOperation(const Setup & op);
    virtual oplist TickOperation(const Tick & op);
    virtual oplist ActionOperation(const Action & op);
    virtual oplist ChopOperation(const Chop & op);
    virtual oplist CreateOperation(const Create & op);
    virtual oplist CutOperation(const Cut & op);
    virtual oplist DeleteOperation(const Delete & op);
    virtual oplist EatOperation(const Eat & op);
    virtual oplist FireOperation(const Fire & op);
    virtual oplist ImaginaryOperation(const Imaginary & op);
    virtual oplist MoveOperation(const Move & op);
    virtual oplist NourishOperation(const Nourish & op);
    virtual oplist SetOperation(const Set & op);
    virtual oplist SightOperation(const Sight & op);
    virtual oplist SoundOperation(const Sound & op);
    virtual oplist TouchOperation(const Touch & op);
    virtual oplist LookOperation(const Look & op);
    virtual oplist AppearanceOperation(const Appearance & op);
    virtual oplist DisappearanceOperation(const Disappearance & op);
    virtual oplist OtherOperation(const RootOperation & op);
};

inline std::ostream & operator<<(std::ostream& s, Location& v)
{
    return s << "{" << v.ref->getId() << "," << v.coords << "," << v.velocity << "}";
}

#endif // ENTITY_H
