// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "CreatorClient.h"

#include <Atlas/Objects/Operation/Create.h>

using Atlas::Message::Object;

Entity * CreatorClient::make(const Object & entity)
{
    if (!entity.IsMap()) {
        return NULL;
    }
    Create op;
    op.SetArgs(Object::ListType(1,entity));
    op.SetFrom(fullid);
    oplist result = sendAndWaitReply(op);
    RootOperation * res = result.front();
    if (res == NULL) {
        return NULL;
    }
    const std::string & resparents = res->GetParents().front().AsString();
    const std::string & resargp = res->GetArgs().front().AsMap().find("parents")->second.AsList().front().AsString();
    if ((resparents != "sight") || (resargp != "create")) {
        // log.inform("creation failed",result);
        return NULL;
    }
    const std::string & created_id = res->GetArgs().front().AsMap().find("args")->second.AsList().front().AsMap().find("id")->second.AsString();
    const Object & created = res->GetArgs().front().AsMap().find("args")->second.AsList().front();
    Entity * obj = map.add(created);
    return obj;
}
