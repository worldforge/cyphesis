// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef SERVER_CONNECTION_H
#define SERVER_CONNECTION_H

#include "common/OOGThing.h"

#include <sigc++/connection.h>

class ServerRouting;
class CommClient;
class Account;

/// \brief Class representing connections from a client at the Atlas level.
///
/// This is the first point of dispatch for any operation from the client.
/// It maintains a dictionary of entities which are associated with this
/// connection, such as Account objects that the client has logged into,
/// and any other entities that that are associated with those accounts,
/// like in-game characters. Clients specify which entity should handle
/// an operation using the from attribute.
class Connection : public OOGThing {
    typedef std::map<std::string, SigC::Connection *> ConMap;

    BaseDict m_objects;

    /// \brief Signal connections connected to the destroyed signal of
    /// entities associated with this object. As the entities get deleted
    /// it is necessary to disconnect the signals.
    ConMap m_destroyedConnections;

    /// \brief Flag to indicate if this connection has already been
    /// disconnected from the entities associated with it.
    ///
    /// This ensures that entities that get destroyed are removed as
    /// they are destroyed, but when this connection is destroyed,
    /// it can destroy certain types of entity connected to itself,
    /// without them trying to remove themselves from the connection.
    bool m_obsolete;

    Account * addPlayer(const std::string &, const std::string &);
  protected:
    virtual bool verifyCredentials(const Account &,
                                   const Atlas::Message::MapType &) const;
  public:
    CommClient & m_commClient;
    ServerRouting & m_server;

    Connection(const std::string & id, CommClient &, ServerRouting & svr);
    virtual ~Connection();

    void addObject(BaseEntity * obj);
    void removeObject(const std::string & id);
    void objectDeleted(std::string id);

    void disconnect();
    void send(const Operation & op) const;

    virtual void operation(const Operation &, OpVector &);

    virtual void LoginOperation(const Operation &, OpVector &);
    virtual void LogoutOperation(const Operation &, OpVector &);
    virtual void CreateOperation(const Operation &, OpVector &);
    virtual void GetOperation(const Operation &, OpVector &);
};

#endif // SERVER_CONNECTION_H
