#ifndef THING_H
#define THING_H

#include <Python.h>


#include <string.h>

#include <common/BaseEntity.h>

#include "Python_API.h"
#include "MemMap.h"

class Player;
class Routing;

class Thing : public BaseEntity {
  protected:
    PyObject * script_object;

    int script_Operation(const string &, const RootOperation &, oplist &, RootOperation * sub_op=NULL);

    Message::Object::MapType attributes;
    MemMap map;
  public:
    double status;
    string type;
    int is_character;

    Thing();
    virtual ~Thing() { }

    Message::Object & operator[](const string &);

    int set_object(PyObject * obj) {
        script_object = obj;
        return(obj == NULL ? -1 : 0);
    }

    oplist send_world(RootOperation * msg);
    void merge(const Message::Object::MapType &);
    void getLocation(Message::Object::MapType &);

    virtual void addObject(Message::Object *);
    virtual oplist Operation(const Setup & op);
    virtual oplist Operation(const Tick & op);
    virtual oplist Operation(const Create & op);
    virtual oplist Operation(const Delete & op);
    virtual oplist Operation(const Move & op);
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
