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
};

typedef std::pair<int, string> thing_t;

#define BASE_THING	0
#define BASE_CHARACTER	1

class ThingFactory {
    map<string,thing_t> thing_map;
  public:
    ThingFactory();
    Thing * new_thing(const string & type, const Message::Object & ent);
};

extern ThingFactory thing_factory;

#endif /* THING_H */
