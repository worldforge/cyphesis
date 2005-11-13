// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003-2004 Alistair Riddoch

#ifndef SERVER_COMM_LOCAL_CLIENT_H
#define SERVER_COMM_LOCAL_CLIENT_H

#include "CommClient.h"

/// \brief Handle a unix socket connected to a client on the local machine.
class CommLocalClient : public CommClient {
  public:
    CommLocalClient(CommServer & svr, int fd, const std::string & id);
    virtual ~CommLocalClient();

};

#endif // SERVER_COMM_LOCAL_CLIENT_H
