// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SERVER_SLAVE_CLIENT_CONNECTION_H
#define SERVER_SLAVE_CLIENT_CONNECTION_H

#include "common/OOGThing.h"

class CommClient;
class ServerRouting;

/// \brief Class representing connections from a client to a cyphesis instance
/// running as an AI slave.
///
/// This behaves much like Connection, but handles the different behavoir
/// required because cyphesis is not the authoritative point for logins.
class SlaveClientConnection : public OOGThing {
  public:
    CommClient & m_commClient;
    ServerRouting & m_server;

    SlaveClientConnection(const std::string &, CommClient &, ServerRouting &);
    virtual ~SlaveClientConnection();
};

#endif // SERVER_SLAVE_CLIENT_CONNECTION_H
