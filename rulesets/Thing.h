#ifndef THING_H
#define THING_H

typedef int bad_type; // Remove this to get unset type reporting

#define None 0 // Remove this to deal with un-initialied vars

#include <string.h>

#include <common/BaseEntity.h>

class Player;

class Thing : public BaseEntity {
  public:
    string description;
    string mode;
    double status;
    double weight;
    double age;
    int is_character;

    Thing();
    virtual ~Thing() { }

    virtual void addObject(Message::Object *);
    RootOperation * send_world(RootOperation * msg);
    virtual RootOperation * Operation(const Setup & op);
    virtual RootOperation * Operation(const Tick & op);
    virtual RootOperation * Operation(const Create & op);
    virtual RootOperation * Operation(const Delete & op);
    virtual RootOperation * Operation(const Move & op);
    virtual RootOperation * Operation(const Set & op);
};

class ThingFactory {
  public:
    static Thing * new_thing(const string & type, const Message::Object & ent);
};

#endif /* THING_H */
