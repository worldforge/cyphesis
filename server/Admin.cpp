// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Info.h>

#include <common/Load.h>
#include <common/Save.h>

#include <common/persistance.h>

#include "Admin.h"
#include "Connection.h"
#include "ServerRouting.h"
#include "server.h"

using Atlas::Message::Object;
using Atlas::Objects::Operation::Info;

oplist Admin::Operation(const Save & op)
{
    fdict_t::const_iterator I;
    Persistance * p = Persistance::instance();
    DatabaseIterator dbi(p->getWorld());
    Object ent;
    while (dbi.get(ent)) {
        dbi.del();
    }
    int count = 0;
    for(I = world->fobjects.begin(); I != world->fobjects.end(); I++) {
        p->putEntity(I->second);
        ++count;
    }
    Object::MapType report;
    report["message"] = "Objects saved to database";
    report["object_count"] = count;
    Info * info = new Info();
    *info = Info::Instantiate();
    Object::ListType args(1,report);
    info->SetArgs(args);
    info->SetRefno(op.GetSerialno());
    return oplist(1,info);
}

oplist Admin::Operation(const Load & op)
{
    int count = 0;
    Persistance * p = Persistance::instance();
    DatabaseIterator dbi(p->getWorld());
    Object ent;
    while (dbi.get(ent)) {
        Object::MapType m = ent.AsMap();
        bool p = (m.find("parents") != m.end());
        const string & type = p ? m["parents"].AsList().front().AsString()
                                : "thing";
        if (m.find("id") != m.end()) {
            const string & id = m["id"].AsString();
            if (id == "world_0") {
                // Ignore the world entry. No info required at the moment.
            } else {
                world->add_object(type, ent, id);
                ++count;
            }
        }
    }
    Object::MapType report;
    report["message"] = "Objects loaded from database";
    report["object_count"] = count;
    Info * info = new Info();
    *info = Info::Instantiate();
    Object::ListType args(1,report);
    info->SetArgs(args);
    info->SetRefno(op.GetSerialno());
    return oplist(1,info);
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
                Object::MapType report;
                report["message"] = "Shutdown initiated";
                Info * info = new Info();
                *info = Info::Instantiate();
                Object::ListType args(1,report);
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
