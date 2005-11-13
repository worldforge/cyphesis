// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SERVER_PEER_H
#define SERVER_PEER_H

#include "common/OOGThing.h"

class CommClient;
class ServerRouting;

/// \brief Class represening connections from another server that is peered to
/// to this one
///
/// This is the main point of dispatch for any operation from the peer.
class Peer : public OOGThing {
  protected:
    
  public:
    CommClient & m_commClient;
    ServerRouting & m_server;

    Peer(CommClient & client, ServerRouting & svr,
         const std::string & addr, const std::string & id);
    virtual ~Peer();
};

#endif // SERVER_PEER_H
