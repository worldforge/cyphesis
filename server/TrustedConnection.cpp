// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "TrustedConnection.h"

TrustedConnection::TrustedConnection(const std::string & id, CommClient & client,
                      ServerRouting & svr) : Connection(id, client, svr)
{
}

bool TrustedConnection::verifyCredentials(const Account &,
                                          const Atlas::Message::MapType &) const
{
    return true;
}
