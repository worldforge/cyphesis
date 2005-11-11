// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Py_CreatorClient.h"

#include "CreatorClient.h"

#include "common/debug.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

static const bool debug_flag = false;

using Atlas::Objects::Root;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

using Atlas::Objects::smart_dynamic_cast;

CreatorClient::CreatorClient(const std::string & id, long intId,
                             const std::string & name,
                             ClientConnection &c) :
                             CharacterClient(id, intId, name, c)
{
}

Entity * CreatorClient::make(const RootEntity & entity)
{
    Create op;
    op->setArgs1(entity);
    op->setFrom(getId());
    op->setTo(getId());
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
    Entity * obj = m_map.updateAdd(created, res->getSeconds());
    return obj;
}


void CreatorClient::sendSet(const std::string & id,
                            const RootEntity & entity)
{
    Set op;
    op->setArgs1(entity);
    op->setFrom(getId());
    op->setTo(id);
    send(op);
}

Entity * CreatorClient::look(const std::string & id)
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

Entity * CreatorClient::lookFor(const RootEntity & ent)
{
    Look op;
    op->setArgs1(ent);
    op->setFrom(getId());
    return sendLook(op);
}

Entity * CreatorClient::sendLook(const Operation & op)
{
    OpVector result;
    if (sendAndWaitReply(op, result) != 0) {
        std::cerr << "No reply to look" << std::endl << std::flush;
        return NULL;
    }
    assert(!result.empty());
    const Operation & res = result.front();
    if (!res.isValid()) {
        std::cerr << "NULL reply to look" << std::endl << std::flush;
        return NULL;
    }
    const std::string & resparents = res->getParents().front();
    if (resparents != "sight") {
        std::cerr << "Reply to look isn't sight" << std::endl << std::flush;
        return NULL;
    }
    if (res->getArgs().empty()) {
        std::cerr << "Reply to look has no args" << std::endl << std::flush;
        return NULL;
    }
    RootEntity seen = smart_dynamic_cast<RootEntity>(res->getArgs().front());
    if (!seen.isValid()) {
        std::cerr << "Sight arg is not an entity" << std::endl << std::flush;
        return NULL;
    }
    if (!seen->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        std::cerr << "Looked at entity has no id" << std::endl << std::flush;
        return NULL;
    }
    const std::string & created_id = seen->getId();
    std::cout << "Seen: " << created_id << std::endl << std::flush;
    Entity * obj = m_map.updateAdd(seen, res->getSeconds());
    return obj;
}

int CreatorClient::runScript(const std::string & package,
                             const std::string & function)
{
    return runClientScript(this, package, function);
}
