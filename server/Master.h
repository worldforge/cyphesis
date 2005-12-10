// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SERVER_MASTER_H
#define SERVER_MASTER_H

#include "common/OOGThing.h"

class CommClient;
class ServerRouting;

/// \brief Class representing connections from a master server to which this
/// program is a slave.
///
/// This is designed to be used from a cyphesis process running as an AI slave.
class Master : public OOGThing {
  protected:
    
  public:
    CommClient & m_commClient;
    ServerRouting & m_server;

    Master(CommClient & client, ServerRouting & svr, const std::string & id);
    virtual ~Master();
};

#endif // SERVER_MASTER_H
