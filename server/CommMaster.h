// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SERVER_COMM_MASTER_H
#define SERVER_COMM_MASTER_H

#include "CommClient.h"

/// \brief Handle an internet socket connected to a remote master server.
class CommMaster : public CommClient {
  public:
    CommMaster(CommServer & svr, const std::string & addr);
    virtual ~CommMaster();

    int connect(const std::string &);
};

#endif // SERVER_COMM_MASTER_H
