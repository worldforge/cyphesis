// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Info.h>

#include "Admin.h"
#include "Connection.h"
#include "ServerRouting.h"
#include "server.h"

using Atlas::Message::Object;
using Atlas::Objects::Operation::Info;

oplist Admin::Operation(const Save & op)
{
    // Obviously does nothing yet. A persistance system needs to be implemented
    return oplist();
}

oplist Admin::Operation(const Load & op)
{
    // Obviously does nothing yet. A persistance system needs to be implemented
    return oplist();
}

oplist Admin::Operation(const Set & op)
{
    const Object & ent = op.GetArgs().front();
    try {
        Object::MapType emap = ent.AsMap();
        const string & id = emap["id"].AsString();
        if (id == "server") {
            const string & cmd = emap["cmd"].AsString();
            Object arg;
            if (emap.find("arg") != emap.end()) {
                arg = emap["arg"];
            }
            if (cmd == "shutdown") {
                exit_flag = true;
            } else if (cmd == "stat") {
                Info * info = new Info();
                *info = Info::Instantiate();
                Object::ListType args(1,connection->server->asObject());
                info->SetArgs(args);
                info->SetRefno(op.GetSerialno());
                return oplist(1,info);
            } else {
                return error(op, "Unknown command");
            }
        }
    }
    catch (...) {
        return error(op, "Invalid set");
    }
    return oplist();
}

// There used to be a code operation handler here. It may become desirable in
// the future for the admind account to be able to send script fragments.
// Think about implementing this.
