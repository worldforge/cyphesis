// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef ENTITY_H
#define ENTITY_H

#include <modules/Location.h>
#include <common/BaseEntity.h>

class WorldRouter;
class MemMap;
class Script;

// This is the base class from which all in-game objects inherit.
// This class should not be instantiiated directly.
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
  public:
    WorldRouter * world;	// Exists in this world.
    int seq;			// Sequence number
    Location location;		// Full details of location inc. ref pos and vel
    elist_t contains;		// List of entities which use this as ref
    double status;		// Health/damage coeficient
    string type;		// Easy access to primary parent
    string name;		// Entities name
    double weight;		// Weight in kg
    bool isCharacter;		// Is this a character
    bool deleted;		// Has this been deleted
    bool omnipresent;		// Is this omnipresent

    Entity();
    virtual ~Entity();

    virtual const Atlas::Message::Object & operator[](const string & aname);
    virtual void set(const string & aname, const Atlas::Message::Object & attr);
    virtual void destroy();

    void setScript(Script * scrpt);
    void merge(const Atlas::Message::Object::MapType &);
    void getLocation(const Atlas::Message::Object::MapType &, edict_t &);
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

inline ostream & operator<<(ostream& s, Location& v)
{
    return s << "{" << v.ref->fullid << "," << v.coords << "," << v.velocity << "}";
}

#endif // ENTITY_H
