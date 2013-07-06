// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#include "Link.h"

#include "common/CommSocket.h"

#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Operation.h>

#include <cassert>

static const bool debug_flag = false;

Link::Link(CommSocket & socket, const std::string & id, long iid) :
            Router(id, iid), m_encoder(0), m_commSocket(socket)
{
}

Link::~Link()
{
}

void Link::send(const Operation & op) const
{
    if (m_encoder != 0) {
        m_encoder->streamObjectsMessage(op);
        m_commSocket.flush();
    }
}

void Link::sendError(const Operation & op,
                     const std::string & errstring,
                     const std::string & to) const
{
    Atlas::Objects::Operation::Error e;

    buildError(op, errstring, e, to);

    send(e);
}

void Link::disconnect()
{
    m_commSocket.disconnect();
}
