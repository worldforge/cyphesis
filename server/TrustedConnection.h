// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef SERVER_TRUSTED_CONNECTION_H
#define SERVER_TRUSTED_CONNECTION_H

#include "Connection.h"

// This class represents a connection from a client that is inherently
// trusted, and can thus login without a password. This allows us to
// clients login automatically without needing to store the admin password
// in plain text anywhere. The client might be trusted because it has
// connected with a known certificate of some kind, or because it has
// connected over a unix domain socket.

class TrustedConnection : public Connection {
  public:
    TrustedConnection(const std::string & id,
                      CommClient & client,
                      ServerRouting & svr);

    virtual bool verifyCredentials(const Account &,
                              const Atlas::Message::Element::MapType &) const;
};

#endif // SERVER_TRUSTED_CONNECTION_H
