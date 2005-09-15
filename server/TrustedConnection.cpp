// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "TrustedConnection.h"

TrustedConnection::TrustedConnection(const std::string & id, CommClient & client,
                      ServerRouting & svr) : Connection(id, client, svr)
{
}

int TrustedConnection::verifyCredentials(const Account &,
                                         const Atlas::Objects::Root &) const
{
    return 0;
}
