// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_ACCOUNT_H
#define SERVER_ACCOUNT_H

#include <common/OOGThing.h>

class Connection;
class WorldRouter;

class Account : public OOGThing {
    friend class Connection;
    edict_t charactersDict;
    BaseEntity * addCharacter(const std::string &, const Atlas::Message::Object &);
  protected:
    virtual oplist characterError(const Create &, const Atlas::Message::Object::MapType &) const = 0;

  public:
    WorldRouter * world;
    Connection * connection;
    std::string password;
    std::string type;

    Account(Connection * conn, const std::string & username, const std::string & passwd);
    virtual ~Account();

    virtual void addToObject(Atlas::Message::Object::MapType &) const;
    virtual oplist LogoutOperation(const Logout & op);
    virtual oplist CreateOperation(const Create & op);
    virtual oplist ImaginaryOperation(const Imaginary & op);
    virtual oplist TalkOperation(const Talk & op);
    virtual oplist LookOperation(const Look & op);
};

#endif // SERVER_ACCOUNT_H
