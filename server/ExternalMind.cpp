// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "ExternalMind.h"

#include "Connection_methods.h"

ExternalMind::ExternalMind(Connection & connection, const std::string & id) :
                           BaseEntity(id), m_connection(connection)
{
}

ExternalMind::~ExternalMind()
{
    m_connection.removeObject(getId());
}

void ExternalMind::operation(const RootOperation & op, OpVector &)
{
    m_connection.send(op);
}
