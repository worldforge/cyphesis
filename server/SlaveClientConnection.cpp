// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "SlaveClientConnection.h"

SlaveClientConnection::SlaveClientConnection(const std::string & id,
                                             CommClient & client,
                                             ServerRouting & svr) :
                                             OOGThing(id),
                                             m_commClient(client),
                                             m_server(svr)
{
}

SlaveClientConnection::~SlaveClientConnection()
{
}
