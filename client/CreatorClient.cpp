// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "CreatorClient.h"

#include <Atlas/Objects/Operation/Create.h>

using Atlas::Message::Object;

CreatorClient::CreatorClient(const std::string & id, const std::string & name,
                             ClientConnection &c) : CharacterClient(id,name,c)
{
}

Entity * CreatorClient::make(const Object & entity)
{
    if (!entity.IsMap()) {
        std::cerr << "entity is not map" << std::endl << std::flush;
        return NULL;
    }
    Create op(Create::Instantiate());
    op.SetArgs(Object::ListType(1,entity));
    op.SetFrom(getId());
    oplist result = sendAndWaitReply(op);
    // FIXME I am pretty sure this is in practice a redundant check.
    if (result.empty()) {
        std::cerr << "No reply to make" << std::endl << std::flush;
        return NULL;
    }
    RootOperation * res = result.front();
    // FIXME This is probably a redundant check
    if (res == NULL) {
        std::cerr << "NULL reply to make" << std::endl << std::flush;
        return NULL;
    }
    const std::string & resparents = res->GetParents().front().AsString();
    if (resparents != "sight") {
        std::cerr << "Reply to make isn't sight" << std::endl << std::flush;
        return NULL;
    }
    if (res->GetArgs().empty()) {
        std::cerr << "Reply to make has no args" << std::endl << std::flush;
        return NULL;
    }
    const Object::MapType & arg = res->GetArgs().front().AsMap();
    Object::MapType::const_iterator I = arg.find("parents");
    if ((I == arg.end()) || !I->second.IsList() || I->second.AsList().empty()) {
        std::cerr << "Arg of reply to make has no parents"
                  << std::endl << std::flush;
        return NULL;
    }
    const std::string & resargp = I->second.AsList().front().AsString();
    if (resargp != "create") {
        std::cerr << "Reply to make isn't sight of create"
                  << std::endl << std::flush;
        return NULL;
    }
    I = arg.find("args");
    if ((I == arg.end()) || !I->second.IsList() || I->second.AsList().empty()) {
        std::cerr << "Arg of reply to make has no args"
                  << std::endl << std::flush;
        return NULL;
    }
    const Object::MapType & created = I->second.AsList().front().AsMap();
    I = created.find("id");
    if ((I == created.end()) || !I->second.IsString()) {
        std::cerr << "Created entity has no id"
                  << std::endl << std::flush;
        return NULL;
    }
    const std::string & created_id = I->second.AsString();
    std::cout << "Created: " << created_id << std::endl << std::flush;
    Entity * obj = map.add(created);
    return obj;
}
