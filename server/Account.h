// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_ACCOUNT_H
#define SERVER_ACCOUNT_H

#include "common/OOGThing.h"

#include <sigc++/connection.h>

class Connection;

/// \brief This is the base class for storing information about uses who
/// can use this server.
///
/// The majority of functionality relating to user accounts is encapsulated
/// here. Sub-classes control privilege levels by implementing
/// characterError().
class Account : public OOGThing {
  protected:
    typedef std::map<std::string, SigC::Connection *> ConMap;

    EntityDict m_charactersDict;
    ConMap m_destroyedConnections;

    Entity * addNewCharacter(const std::string &,
                             const Atlas::Message::MapType &);
    void characterDestroyed(std::string);

    virtual int characterError(const RootOperation &,
                               const Atlas::Message::MapType &,
                               OpVector &) const = 0;

  public:
    Connection * m_connection;
    std::string m_username;
    std::string m_password;

    Account(Connection * conn, const std::string & username,
                               const std::string & passwd,
                               const std::string & id);
    virtual ~Account();

    virtual const char * getType() const;

    virtual void addToMessage(Atlas::Message::MapType &) const;
    virtual void LogoutOperation(const RootOperation &, OpVector &);
    virtual void CreateOperation(const RootOperation &, OpVector &);
    virtual void SetOperation(const RootOperation &, OpVector &);
    virtual void ImaginaryOperation(const RootOperation &, OpVector &);
    virtual void TalkOperation(const RootOperation &, OpVector &);
    virtual void LookOperation(const RootOperation &, OpVector &);

    void addCharacter(Entity *);

    const EntityDict & getCharacters() const {
        return m_charactersDict;
    }
};

typedef std::map<std::string, Account *> AccountDict;

#endif // SERVER_ACCOUNT_H
