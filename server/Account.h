#ifndef ACCOUNT_H
#define ACCOUNT_H

typedef int bad_type; // Remove this to get unset type reporting

#define None 0 // Remove this to deal with un-initialied vars

#include "OOG_Thing.h"

class Connection;
class WorldRouter;
class Thing;

class Account : public OOG_Thing {
  public:
    WorldRouter * world;
    Connection * connection;
    dict_t characters;
    string password;

    Account() { }
    Account(Connection * conn, string & username, string & passwd) :
        connection(conn), password(passwd) {
        fullid = username;
    }

    virtual void addObject(Message::Object *);
    virtual RootOperation * Operation(const Logout & op);
    virtual RootOperation * Operation(const Create & op);
    Thing * add_character(string character_class, const Message::Object & ent);
    virtual RootOperation * character_error(const Create & op, const Message::Object & ent) = 0;
};

#endif /* ACCOUNT_H */
