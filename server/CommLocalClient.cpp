// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "CommLocalClient.h"

#include "Connection.h"
#include "CommServer.h"

CommLocalClient::CommLocalClient(CommServer & svr, int fd) :
                 CommClient(svr, fd, *new Connection("local", *this, svr.server))
{
}


CommLocalClient::~CommLocalClient()
{
}
