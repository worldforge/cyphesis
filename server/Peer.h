// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SERVER_PEER_H
#define SERVER_PEER_H

#include "common/OOGThing.h"

class CommClient;
class ServerRouting;

class Peer : public OOGThing {
  protected:
    
  public:
    CommClient & m_commClient;
    ServerRouting & m_server;

    Peer(const std::string & id, CommClient & client, ServerRouting & svr);
    virtual ~Peer();
};

#endif // SERVER_PEER_H
