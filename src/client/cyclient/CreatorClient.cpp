// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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


#include "CreatorClient.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

static const bool debug_flag = false;

using Atlas::Objects::Root;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

using Atlas::Objects::smart_dynamic_cast;

CreatorClient::CreatorClient(RouterId mindId,
                             const std::string& entityId,
                             ClientConnection& c,
                             TypeStore& typeStore) :
        CharacterClient(std::move(mindId), entityId, c, typeStore)
{
}

LocatedEntity* CreatorClient::handleMakeResponse(const RootOperation& op,
                                                 double create_time)
{
    if (op->getArgs().empty()) {
        std::cerr << "Arg of reply to make has no args"
                  << std::endl << std::flush;
        return nullptr;
    }
    RootEntity created = smart_dynamic_cast<RootEntity>(op->getArgs().front());
    if (!created.isValid()) {
        std::cerr << "Created argument is not an entity"
                  << std::endl << std::flush;
        return nullptr;
    }
    if (!created->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        std::cerr << "Created entity has no id"
                  << std::endl << std::flush;
        return nullptr;
    }
    const std::string& created_id = created->getId();
    if (created->getParent().empty()) {
        std::cerr << "Created entity " << created_id << " has no type"
                  << std::endl << std::flush;
        return nullptr;
    }
    const std::string& created_type = created->getParent();
    std::cout << "Created: " << created_type << "(" << created_id << ")"
              << std::endl << std::flush;
    return m_map.updateAdd(created, create_time).get();
}

Ref<LocatedEntity> CreatorClient::make(const RootEntity& entity)
{
    Create op;
    op->setArgs1(entity);
    op->setFrom(getId());
    op->setTo(getId());
    OpVector result;
    if (sendAndWaitReply(op, result) != 0) {
        std::cerr << "No reply to make" << std::endl << std::flush;
        return nullptr;
    }
    assert(!result.empty());
    const Operation& res = result.front();
    if (!res.isValid()) {
        std::cerr << "nullptr reply to make" << std::endl << std::flush;
        return nullptr;
    }
    // FIXME Make this more robust against an info response
    if (res->getClassNo() == Atlas::Objects::Operation::SIGHT_NO) {
        if (res->getArgs().empty()) {
            std::cerr << "Reply to make has no args" << std::endl << std::flush;
            return nullptr;
        }
        RootOperation arg = smart_dynamic_cast<RootOperation>(res->getArgs().front());
        if (!arg.isValid()) {
            std::cerr << "Arg of reply to make is not an operation"
                      << std::endl << std::flush;
            return nullptr;
        }
        if (arg->getClassNo() != Atlas::Objects::Operation::CREATE_NO) {
            std::cerr << "Reply to make isn't sight of create"
                      << std::endl << std::flush;
            return nullptr;
        }
        return handleMakeResponse(arg, res->getSeconds());
    } else if (res->getClassNo() == Atlas::Objects::Operation::INFO_NO) {
        return handleMakeResponse(res, res->getSeconds());
    } else {
        std::cerr << "Reply to make isn't sight or info"
                  << std::endl << std::flush;
        return nullptr;
    }
}


void CreatorClient::sendSet(const std::string& id,
                            const RootEntity& entity)
{
    Set op;
    op->setArgs1(entity);
    op->setFrom(getId());
    op->setTo(id);
    send(op);
}

void CreatorClient::del(const std::string& id)
{
    Delete op;
    Anonymous ent;
    ent->setId(id);
    op->setArgs1(ent);
    op->setFrom(getId());
    op->setTo(id);
    return send(op);
}
