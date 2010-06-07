// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001 Alistair Riddoch
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

// $Id$

#include "InterServerClient.h"
#include "InterServerConnection.h"

#include "common/debug.h"

#include <Atlas/Objects/RootOperation.h>

static const bool debug_flag = false;

/// \brief InterServerClient constructor
///
/// @param id String identifier
/// @param intId Integer identifier
/// @param name The name of the avatar used by this remote agent
/// @param c The network connection to the server used for communication
InterServerClient::InterServerClient(InterServerConnection & c) : m_connection(c)
{
}

/// \brief Send an operation to the server from this avatar
///
/// @param op Operation to be sent
void InterServerClient::send(const Operation & op)
{
    op->setFrom(getId());
    m_connection.send(op);
}

/// \brief Send an operation to the server, and wait for a reply
///
/// Reply is identified as it should have its refno attribute set to
/// the serialno of the operation sent.
/// @param op Operation to be sent
/// @param res Result with correct refno is returned here
int InterServerClient::sendAndWaitReply(const Operation & op, OpVector & res)
{
    long no = m_connection.newSerialNo();
    op->setSerialno(no);
    send(op);
    debug(std::cout << "Waiting for reply to " << op->getParents().front()
                    << std::endl << std::flush;);
    while (true) {
        if (m_connection.pending()) {
            Operation input = m_connection.pop();
            assert(input.isValid());
            if (input.isValid()) {
                if (input->getRefno() == no) {
                    debug(std::cout << "Got reply" << std::endl << std::flush;);
                    res.push_back(input);
                    return 0;
                } else {
                    debug(std::cout << "Not reply" << std::endl << std::flush;);
                }
            } else {
                debug(std::cout << "Not op" << std::endl << std::flush;);
            }
        } else if (m_connection.wait() != 0) {
            return -1;
        }
    }
}
