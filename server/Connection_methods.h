// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_CONNECTION_METHODS_H
#define SERVER_CONNECTION_METHODS_H

#include "Connection.h"

#include "CommClient.h"

inline void Connection::send(const Operation & msg) const
{
    m_commClient.send(msg);
}

#endif // SERVER_CONNECTION_METHODS_H
