// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef THING_H
#define THING_H

#include <Python.h>


#include <string.h>

#include <common/BaseEntity.h>
#include <server/WorldRouter.h>

#include "Python_API.h"
#include "MemMap.h"

class Player;
class Routing;

class Thing : public BaseEntity {
  protected:
    PyObject * script_object;
    Atlas::Message::Object::MapType attributes;

    virtual int script_Operation(const string &, const RootOperation &, oplist &, RootOperation * sub_op=NULL);
  public:
    bool perceptive;
    double status;
    string type;
    bool is_character;
    double weight;

    Thing();
    virtual ~Thing() { }

    virtual const Atlas::Message::Object & operator[](const string & aname);
    virtual void set(const string & aname, const Atlas::Message::Object & attr);

    virtual int set_object(PyObject * obj) {
        script_object = obj;
        return(obj == NULL ? -1 : 0);
    }

    virtual MemMap * getMap() { return NULL; }

    oplist send_world(RootOperation * op) const {
        return world->message(*op, this);
    }

    void merge(const Atlas::Message::Object::MapType &);
    void getLocation(Atlas::Message::Object::MapType &, fdict_t &);

    virtual void addObject(Atlas::Message::Object *) const;
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

#endif /* THING_H */
