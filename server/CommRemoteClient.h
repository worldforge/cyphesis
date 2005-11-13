// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003-2004 Alistair Riddoch

#ifndef SERVER_COMM_REMOTE_CLIENT_H
#define SERVER_COMM_REMOTE_CLIENT_H

#include "CommClient.h"

/// \brief Handle an internet socket connected to a remote client.
class CommRemoteClient : public CommClient {
  public:
    CommRemoteClient(CommServer & svr, int fd, const std::string & address,
                                               const std::string & id);
    virtual ~CommRemoteClient();

};

#endif // SERVER_COMM_REMOTE_CLIENT_H
