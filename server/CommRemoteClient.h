// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_COMM_LOCAL_CLIENT_H
#define SERVER_COMM_LOCAL_CLIENT_H

#include "CommClient.h"

class CommRemoteClient : public CommClient {
  public:
    CommRemoteClient(CommServer & svr, int fd, const std::string & address);
    virtual ~CommRemoteClient();

};

#endif // SERVER_COMM_LOCAL_CLIENT_H
