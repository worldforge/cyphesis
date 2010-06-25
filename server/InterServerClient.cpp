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

#include <Atlas/Codec.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include "common/debug.h"
#include "common/id.h"

#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/RootEntity.h>

using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Entity::RootEntity;

using Atlas::Objects::smart_dynamic_cast;

static const bool debug_flag = false;

/// \brief InterServerClient constructor
///
/// @param c The network connection to the server used for communication
InterServerClient::InterServerClient(InterServerConnection & c) : m_connection(c)
{
}

int InterServerClient::connect(const std::string & server, int port) {
    return m_connection.connect(server, port);
}

int InterServerClient::login(const std::string & username, const std::string & password) {
    return m_connection.login(username, password);
}

void InterServerClient::send(const Operation & op)
{
    // We don't have any entity ID right?
    op->setFrom(m_connection.getAccountId());
    m_connection.send(op);
}

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

std::string InterServerClient::addCharacter(const RootEntity & entity)
{
	Create op;
	op->setFrom(m_connection.getAccountId());
	op->setArgs1(entity);
	op->setSerialno(m_connection.newSerialNo());
	send(op);

    if (m_connection.wait() != 0) {
        return NULL;
    }

    RootEntity ent = smart_dynamic_cast<RootEntity>(m_connection.getInfoReply());

    if (!ent->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        std::cerr << "ERROR: Character created, but has no id" << std::endl
                  << std::flush;
        return NULL;
    }

    const std::string & id = ent->getId();

    long intId = integerId(id);

    if (intId == -1) {
        return NULL;
    }
    return id;
}

std::string InterServerClient::injectEntity(const RootEntity & entity)
{
    Create op;
    op->setArgs1(entity);
    // We don't have an ID right?
    op->setFrom(m_connection.getAccountId());
    op->setTo(m_connection.getAccountId());
    OpVector result;
    if (sendAndWaitReply(op, result) != 0) {
        std::cerr << "No reply to make" << std::endl << std::flush;
        return NULL;
    }
    assert(!result.empty());
    const Operation & res = result.front();
    if (!res.isValid()) {
        std::cerr << "NULL reply to make" << std::endl << std::flush;
        return NULL;
    }
    const std::string & resparents = res->getParents().front();
	std::cout << resparents << "\n";
    if (resparents != "sight") {
        std::cerr << "Reply to make isn't sight" << std::endl << std::flush;
        return NULL;
    }
    if (res->getArgs().empty()) {
        std::cerr << "Reply to make has no args" << std::endl << std::flush;
        return NULL;
    }
    RootOperation arg = smart_dynamic_cast<RootOperation>(res->getArgs().front());
    if (!arg.isValid()) {
        std::cerr << "Arg of reply to make is not an operation"
                  << std::endl << std::flush;
        return NULL;
    }
    if (!arg->hasAttrFlag(Atlas::Objects::PARENTS_FLAG) || arg->getParents().empty()) {
        std::cerr << "Arg of reply to make has no parents"
                  << std::endl << std::flush;
        return NULL;
    }
    const std::string & resargp = arg->getParents().front();
    if (resargp != "create") {
        std::cerr << "Reply to make isn't sight of create"
                  << std::endl << std::flush;
        return NULL;
    }
    if (arg->getArgs().empty()) {
        std::cerr << "Arg of reply to make has no args"
                  << std::endl << std::flush;
        return NULL;
    }
    RootEntity created = smart_dynamic_cast<RootEntity>(arg->getArgs().front());
    if (!created.isValid()) {
        std::cerr << "Created argument is not an entity"
                  << std::endl << std::flush;
        return NULL;
    }
    if (!created->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        std::cerr << "Created entity has no id"
                  << std::endl << std::flush;
        return NULL;
    }
    const std::string & created_id = created->getId();
    if (created->getParents().empty()) {
        std::cerr << "Created entity " << created_id << " has no type"
                  << std::endl << std::flush;
        return NULL;
    }
    const std::string & created_type = created->getParents().front();
    std::cout << "Created: " << created_type << "(" << created_id << ")"
              << std::endl << std::flush;
    return created->getId();
}
