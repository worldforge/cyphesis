// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "ExternalMind.h"

#include "Connection_methods.h"

ExternalMind::ExternalMind(Connection & connection, const std::string & id,
                           const std::string & name) :
                BaseMind(id, name), m_connection(connection)
{
}

ExternalMind::~ExternalMind()
{
    m_connection.removeObject(getId());
}

OpVector ExternalMind::message(const RootOperation & op)
{
    m_connection.send(op);
    return OpVector();
}
