// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/Operation/Login.h>

#include <common/Chop.h>
#include <common/Cut.h>
#include <common/Eat.h>
#include <common/Fire.h>

#include "ExternalMind.h"

#include <server/Connection_methods.h>

ExternalMind::ExternalMind(Connection & connection, string & id, string & name):
		BaseMind(id, name), connection(connection)
{
}

oplist ExternalMind::message(const RootOperation & op)
{
    connection.send(&op);
    return oplist();
}
