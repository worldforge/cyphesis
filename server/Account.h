// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_ACCOUNT_H
#define SERVER_ACCOUNT_H

#include <common/OOGThing.h>

class Connection;
class WorldRouter;

class Account : public OOGThing {
  private:
    typedef std::map<std::string, SigC::Connection *> ConMap;

    EntityDict charactersDict;
    ConMap destroyedConnections;

    BaseEntity * addCharacter(const std::string &,
                              const Atlas::Message::Object::MapType &);
  protected:
    void characterDestroyed(std::string);

    virtual OpVector characterError(const Create &, const Atlas::Message::Object::MapType &) const = 0;

  public:
    Connection * connection;
    std::string username;
    std::string password;

    Account(Connection * conn, const std::string & username,
                               const std::string & passwd,
                               const std::string & id = "");
    virtual ~Account();

    virtual const char * getType() const;

    virtual void addToObject(Atlas::Message::Object::MapType &) const;
    virtual OpVector LogoutOperation(const Logout & op);
    virtual OpVector CreateOperation(const Create & op);
    virtual OpVector ImaginaryOperation(const Imaginary & op);
    virtual OpVector TalkOperation(const Talk & op);
    virtual OpVector LookOperation(const Look & op);

    const EntityDict & getCharacters() {
        return charactersDict;
    }
};

#endif // SERVER_ACCOUNT_H
