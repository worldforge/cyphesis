// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "ExternalMind.h"
#include "Connection_methods.h"

ExternalMind::ExternalMind(Connection & connection, const std::string & id,
                           const std::string & name) :
		BaseMind(id, name), connection(connection)
{
}

ExternalMind::~ExternalMind()
{
    connection.removeObject(getId());
}

OpVector ExternalMind::message(const RootOperation & op)
{
    connection.send(op);
    return OpVector();
}
