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


#include "CharacterClient.h"

#include "ClientConnection.h"

#include <Atlas/Objects/Anonymous.h>

using Atlas::Objects::Operation::Look;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

using Atlas::Objects::smart_dynamic_cast;

static const bool debug_flag = false;

/// \brief CharacterClient constructor
///
/// @param id String identifier
/// @param intId Integer identifier
/// @param name The name of the avatar used by this remote agent
/// @param c The network connection to the server used for communication
CharacterClient::CharacterClient(RouterId id,
                                 const std::string& entityId,
                                 ClientConnection& c,
                                 TypeStore& typeStore) :
        BaseMind(std::move(id), entityId, typeStore), m_connection(c)
{
}

/// \brief Send an operation to the server from this avatar
///
/// @param op Operation to be sent
void CharacterClient::send(const Operation& op)
{
    op->setFrom(getId());
    m_connection.send(op);
}

Ref<LocatedEntity> CharacterClient::look(const std::string& id)
{
    Look op;
    if (!id.empty()) {
        Anonymous ent;
        ent->setId(id);
        op->setArgs1(ent);
    }
    op->setFrom(getId());
    return sendLook(op);
}

Ref<LocatedEntity> CharacterClient::lookFor(const RootEntity& ent)
{
    Look op;
    op->setArgs1(ent);
    op->setFrom(getId());
    return sendLook(op);
}

/// \brief Send an operation to the server, and wait for a reply
///
/// Reply is identified as it should have its refno attribute set to
/// the serialno of the operation sent.
/// @param op Operation to be sent
/// @param res Result with correct refno is returned here
int CharacterClient::sendAndWaitReply(const Operation& op, OpVector& res)
{
    op->setFrom(getId());
    return m_connection.sendAndWaitReply(op, res);
}

Ref<LocatedEntity> CharacterClient::sendLook(const Operation& op)
{
    OpVector result;
    if (sendAndWaitReply(op, result) != 0) {
        std::cerr << "No reply to look" << std::endl << std::flush;
        return nullptr;
    }
    assert(!result.empty());
    const Operation& res = result.front();
    if (!res.isValid()) {
        std::cerr << "nullptr reply to look" << std::endl << std::flush;
        return nullptr;
    }
    const std::string& resparent = res->getParent();
    if (resparent == "unseen") {
        return nullptr;
    }
    if (resparent != "sight") {
        std::cerr << "Reply to look is " << resparent << " not sight" << std::endl << std::flush;
        return nullptr;
    }
    if (res->getArgs().empty()) {
        std::cerr << "Reply to look has no args" << std::endl << std::flush;
        return nullptr;
    }
    RootEntity seen = smart_dynamic_cast<RootEntity>(res->getArgs().front());
    if (!seen.isValid()) {
        std::cerr << "Sight arg is not an entity" << std::endl << std::flush;
        return nullptr;
    }
    if (!seen->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        std::cerr << "Looked at entity has no id" << std::endl << std::flush;
        return nullptr;
    }
    const std::string& sight_id = seen->getId();
    if (seen->hasAttrFlag(Atlas::Objects::PARENT_FLAG)) {
        std::cout << "Seen: " << seen->getParent()
                  << "(" << sight_id << ")" << std::endl << std::flush;
    } else {
        std::cout << "Seen: " << sight_id << std::endl << std::flush;
    }
    return m_map.updateAdd(seen, res->getSeconds());
}
