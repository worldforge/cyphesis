// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "Peer.h"

#include "common/id.h"

Peer::Peer(CommClient & client,
           ServerRouting & svr,
           const std::string & addr,
           const std::string & id) :
      OOGThing(id, forceIntegerId(id)), m_commClient(client), m_server(svr)
{
}

Peer::~Peer()
{
}
