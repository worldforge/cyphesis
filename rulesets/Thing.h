#ifndef THING_H
#define THING_H

#include <Python.h>

#include <string.h>

#include <common/BaseEntity.h>

class Player;

class Thing : public BaseEntity {
  protected:
    PyObject * script_object;

    int script_Operation(const string &, const RootOperation &, oplist &);

    Message::Object::MapType attributes;
  public:
    double status;
    int is_character;

    Thing();
    virtual ~Thing() { }

    Message::Object & operator[](const string &);

    virtual void addObject(Message::Object *);
    oplist send_world(RootOperation * msg);
    virtual oplist Operation(const Setup & op);
    virtual oplist Operation(const Tick & op);
    virtual oplist Operation(const Create & op);
    virtual oplist Operation(const Delete & op);
    virtual oplist Operation(const Move & op);
    virtual oplist Operation(const Set & op);
};

class ThingFactory {
  public:
    static Thing * new_thing(const string & type, const Message::Object & ent);
};

#endif /* THING_H */
