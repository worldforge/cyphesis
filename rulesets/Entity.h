// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef ENTITY_H
#define ENTITY_H

#include <string>

#include <common/BaseEntity.h>

class Player;
class Routing;
class MemMap;
class Script;

class Entity : public BaseEntity {
  protected:
    Script * script;
    Atlas::Message::Object::MapType attributes;
  public:
    double status;
    string type;
    bool is_character;
    double weight;

    Entity();
    virtual ~Entity();

    virtual const Atlas::Message::Object & operator[](const string & aname);
    virtual void set(const string & aname, const Atlas::Message::Object & attr);
    virtual int set_script(Script * scrpt);
    virtual MemMap * getMap();

    void merge(const Atlas::Message::Object::MapType &);
    void getLocation(Atlas::Message::Object::MapType &, fdict_t &);

    virtual void addObject(Object * obj) const;
    virtual oplist Operation(const Setup & op);
    virtual oplist Operation(const Tick & op);
    virtual oplist Operation(const Chop & op);
    virtual oplist Operation(const Create & op);
    virtual oplist Operation(const Cut & op);
    virtual oplist Operation(const Delete & op);
    virtual oplist Operation(const Eat & op);
    virtual oplist Operation(const Fire & op);
    virtual oplist Operation(const Move & op);
    virtual oplist Operation(const Nourish & op);
    virtual oplist Operation(const Set & op);
    virtual oplist Operation(const Sight & op);
    virtual oplist Operation(const Sound & op);
    virtual oplist Operation(const Touch & op);
    virtual oplist Operation(const Look & op);
    virtual oplist Operation(const Appearance & op);
    virtual oplist Operation(const Disappearance & op);
};

#endif /* ENTITY_H */
