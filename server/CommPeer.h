// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SERVER_COMM_PEER_H
#define SERVER_COMM_PEER_H

#include "CommClient.h"

/// \brief Handle an internet socket connected to a remote peer server.
class CommPeer : public CommClient {
  public:
    CommPeer(CommServer & svr, const std::string & addr);
    CommPeer(CommServer & svr, int fd, const std::string & addr);
    virtual ~CommPeer();

    int connect(const std::string &);
};

#endif // SERVER_COMM_PEER_H
