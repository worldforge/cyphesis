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
