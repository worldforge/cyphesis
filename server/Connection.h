// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

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
    CommClient & m_commClient;
    bool m_obsolete;
    ConMap m_destroyedConnections;

    Account * addPlayer(const std::string &, const std::string &);
  public:
    ServerRouting & m_server;

    Connection(const std::string & id, CommClient & client,
               ServerRouting & svr);
    virtual ~Connection();

    void addObject(BaseEntity * obj);
    void removeObject(const std::string & id);
    void objectDeleted(std::string id);

    void destroy();
    void close();
    void send(const RootOperation & msg) const;

    virtual bool verifyCredentials(const Account &, const MapType &) const;

    virtual void operation(const RootOperation &, OpVector &);

    virtual void LoginOperation(const Login &, OpVector &);
    virtual void LogoutOperation(const Logout &, OpVector &);
    virtual void CreateOperation(const Create &, OpVector &);
    virtual void GetOperation(const Get &, OpVector &);
};

#endif // SERVER_CONNECTION_H
