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
    Message::Object::MapType attributes;

    virtual int script_Operation(const string &, const RootOperation &, oplist &, RootOperation * sub_op=NULL);
  public:
    double status;
    string type;
    bool is_character;

    Thing();
    virtual ~Thing() { }

    virtual Object & operator[](const string & aname) {
        if (attributes.find(aname) == attributes.end()) {
            attributes[aname]=Object();
        }
        return(attributes[aname]);
    }

    virtual int set_object(PyObject * obj) {
        script_object = obj;
        return(obj == NULL ? -1 : 0);
    }

    virtual MemMap * getMap() { return NULL; }

    oplist send_world(RootOperation * op) const {
        return world->message(*op, this);
    }

    void merge(const Message::Object::MapType &);
    void getLocation(Message::Object::MapType &, fdict_t &);

    virtual void addObject(Message::Object *) const;
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

    friend PyObject * Thing_getattr(ThingObject *self, char *name);
    friend int Thing_setattr(ThingObject *self, char *name, PyObject *v);
};

#if 0
typedef std::pair<int, string> thing_t;

#define BASE_THING	0
#define BASE_CHARACTER	1
#define BASE_CREATOR	2

class ThingFactory {
    map<string,thing_t> thing_map;
  public:
    ThingFactory();
    Thing * new_thing(const string &, const Message::Object &, Routing *);
};

extern ThingFactory thing_factory;

#endif /* 0 */

#endif /* THING_H */
