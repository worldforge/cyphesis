// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "OOG_Thing.h"

class Connection;
class WorldRouter;
class Thing;

class Account : public OOG_Thing {
    friend class Connection;
    fdict_t characters_dict;
    BaseEntity * add_character(const string &, const Atlas::Message::Object &);
  protected:
    virtual oplist character_error(const Create &, const Atlas::Message::Object &) const = 0;
  public:
    Connection * connection;
    string password;
    string type;

    Account(Connection * conn, const string & username, const string & passwd);
    virtual ~Account();

    virtual void addObject(Atlas::Message::Object *) const;
    virtual oplist Operation(const Logout & op);
    virtual oplist Operation(const Create & op);
};

#endif /* ACCOUNT_H */
