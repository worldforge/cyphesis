// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "CreatorClient.h"
#include "Py_CreatorClient.h"

#include "common/debug.h"

#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Set.h>

static const bool debug_flag = false;

using Atlas::Objects::Operation::RootOperation;

CreatorClient::CreatorClient(const std::string & id, const std::string & name,
                             ClientConnection &c) : CharacterClient(id,name,c)
{
}

Entity * CreatorClient::make(const Element & entity)
{
    if (!entity.isMap()) {
        std::cerr << "make: entity is not map" << std::endl << std::flush;
        return NULL;
    }
    Create op;
    op.setArgs(ListType(1,entity));
    op.setFrom(getId());
    op.setTo(getId());
    OpVector result = sendAndWaitReply(op);
    if (result.empty()) {
        std::cerr << "No reply to make" << std::endl << std::flush;
        return NULL;
    }
    RootOperation * res = result.front();
    if (res == NULL) {
        std::cerr << "NULL reply to make" << std::endl << std::flush;
        return NULL;
    }
    const std::string & resparents = res->getParents().front().asString();
    if (resparents != "sight") {
        std::cerr << "Reply to make isn't sight" << std::endl << std::flush;
        return NULL;
    }
    if (res->getArgs().empty()) {
        std::cerr << "Reply to make has no args" << std::endl << std::flush;
        return NULL;
    }
    if (!res->getArgs().front().isMap()) {
        std::cerr << "Reply to make has malformed args" << std::endl << std::flush;
        return NULL;
    }
    const MapType & arg = res->getArgs().front().asMap();
    MapType::const_iterator I = arg.find("parents");
    if ((I == arg.end()) || !I->second.isList() || I->second.asList().empty()) {
        std::cerr << "Arg of reply to make has no parents"
                  << std::endl << std::flush;
        return NULL;
    }
    const std::string & resargp = I->second.asList().front().asString();
    if (resargp != "create") {
        std::cerr << "Reply to make isn't sight of create"
                  << std::endl << std::flush;
        return NULL;
    }
    I = arg.find("args");
    if ((I == arg.end()) || !I->second.isList() || I->second.asList().empty()) {
        std::cerr << "Arg of reply to make has no args"
                  << std::endl << std::flush;
        return NULL;
    }
    const MapType & created = I->second.asList().front().asMap();
    I = created.find("id");
    if ((I == created.end()) || !I->second.isString()) {
        std::cerr << "Created entity has no id"
                  << std::endl << std::flush;
        return NULL;
    }
    const std::string & created_id = I->second.asString();
    I = created.find("parents");
    if ((I == created.end()) || !I->second.isList() ||
        I->second.asList().empty() || !I->second.asList().front().isString()) {
        std::cerr << "Created entity " << created_id << " has no type"
                  << std::endl << std::flush;
        return NULL;
    }
    const std::string & created_type = I->second.asList().front().asString();
    std::cout << "Created: " << created_type << "(" << created_id << ")"
              << std::endl << std::flush;
    Entity * obj = m_map.updateAdd(created, res->getSeconds());
    return obj;
}


void CreatorClient::sendSet(const std::string & id, const Element & entity)
{
    if (!entity.isMap()) {
        std::cerr << "set: " << id << " entity is not map" << std::endl << std::flush;
        return;
    }
    Set op;
    op.setArgs(ListType(1,entity));
    op.setFrom(getId());
    op.setTo(id);
    send(op);
}

Entity * CreatorClient::look(const std::string & id)
{
    Look op;
    if (!id.empty()) {
        MapType ent;
        ent["id"] = id;
        op.setArgs(ListType(1,ent));
    }
    op.setFrom(getId());
    return sendLook(op);
}

Entity * CreatorClient::lookFor(const Element & ent)
{
    Look op;
    op.setArgs(ListType(1,ent));
    op.setFrom(getId());
    return sendLook(op);
}

Entity * CreatorClient::sendLook(RootOperation & op)
{
    OpVector result = sendAndWaitReply(op);
    if (result.empty()) {
        std::cerr << "No reply to look" << std::endl << std::flush;
        return NULL;
    }
    RootOperation * res = result.front();
    if (res == NULL) {
        std::cerr << "NULL reply to look" << std::endl << std::flush;
        return NULL;
    }
    const std::string & resparents = res->getParents().front().asString();
    if (resparents != "sight") {
        std::cerr << "Reply to look isn't sight" << std::endl << std::flush;
        return NULL;
    }
    if (res->getArgs().empty()) {
        std::cerr << "Reply to look has no args" << std::endl << std::flush;
        return NULL;
    }
    if (!res->getArgs().front().isMap()) {
        std::cerr << "Reply to look has malformed args" << std::endl << std::flush;
        return NULL;
    }
    const MapType & seen = res->getArgs().front().asMap();
    MapType::const_iterator I = seen.find("id");
    if ((I == seen.end()) || !I->second.isString()) {
        std::cerr << "Looked at entity has no id"
                  << std::endl << std::flush;
        return NULL;
    }
    const std::string & created_id = I->second.asString();
    std::cout << "Seen: " << created_id << std::endl << std::flush;
    Entity * obj = m_map.updateAdd(seen, res->getSeconds());
    return obj;
}

bool CreatorClient::runScript(const std::string & package,
                              const std::string & function)
{
    return runClientScript(this, package, function);
}
