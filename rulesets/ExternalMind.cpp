// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#error This file has been removed from the build

#include <Atlas/Objects/Operation/RootOperation.h>

#include "ExternalMind.h"

#include <server/Connection_methods.h>

ExternalMind::ExternalMind(Connection & connection, std::string & id,
                           std::string & name):
		BaseMind(id, name), connection(connection)
{
}

OpVector ExternalMind::message(const RootOperation & op)
{
    connection.send(&op);
    return OpVector();
}
