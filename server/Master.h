// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SERVER_MASTER_H
#define SERVER_MASTER_H

#include "common/OOGThing.h"

class CommClient;
class ServerRouting;

class Master : public OOGThing {
  protected:
    
  public:
    CommClient & m_commClient;
    ServerRouting & m_server;

    Master(const std::string & id, CommClient & client, ServerRouting & svr);
    virtual ~Master();
};

#endif // SERVER_MASTER_H
