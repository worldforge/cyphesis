// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/Operation/Login.h>

#include "ExternalMind.h"

#include <server/Connection.h>

ExternalMind::ExternalMind(Connection * connection, string & id, string & name):
		BaseMind(id, name), connection(connection)
{
}

oplist ExternalMind::message(const RootOperation & op)
{
    connection->send(&op);
    oplist res;
    return(res);
}
