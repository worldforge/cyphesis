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
    op.SetFrom(fullid);
    oplist result = sendAndWaitReply(op);
    if (result.empty()) {
        std::cerr << "No reply to make" << std::endl << std::flush;
        return NULL;
    }
    RootOperation * res = result.front();
    if (res == NULL) {
        std::cerr << "No reply to make" << std::endl << std::flush;
        return NULL;
    }
    try {
    const std::string & resparents = res->GetParents().front().AsString();
    if (resparents != "sight") {
        std::cerr << "Reply to make isn't sight" << std::endl << std::flush;
        return NULL;
    }
    const std::string & resargp = res->GetArgs().front().AsMap().find("parents")->second.AsList().front().AsString();
    if (resargp != "create") {
        std::cerr << "Reply to make isn't sight of create" << std::endl << std::flush;
        return NULL;
    }
    cout << 4 << endl << flush;
    const std::string & created_id = res->GetArgs().front().AsMap().find("args")->second.AsList().front().AsMap().find("id")->second.AsString();
    cout << 5 << endl << flush;
    const Object & created = res->GetArgs().front().AsMap().find("args")->second.AsList().front();
    cout << 6 << endl << flush;
    Entity * obj = map.add(created);
    std::cout << "returning from make" << std::endl << std::flush;
    return obj;
    }
    catch (...) {
        std::cerr << "EXCEPTION: While makeing" << std::endl << std::flush;
    }
}
