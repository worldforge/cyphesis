// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Get.h>
#include <Atlas/Objects/Operation/Info.h>

#include <common/Load.h>
#include <common/Save.h>

#include <common/persistance.h>

#include "Admin.h"
#include "Connection.h"
#include "ServerRouting.h"
#include "WorldRouter.h"
#include <common/globals.h>
#include <rulesets/Entity.h>

using Atlas::Message::Object;
using Atlas::Objects::Operation::Info;

Admin::Admin(Connection * conn, const string& username, const string& passwd) :
             Account(conn, username, passwd)
{
    type = "admin";
}

Admin::~Admin()
{
}

oplist Admin::characterError(const Create &, const Object::MapType &) const {
    return oplist();
}

oplist Admin::Operation(const Save & op)
{
    edict_t::const_iterator I;
    Persistance * p = Persistance::instance();
    DatabaseIterator dbi(p->getWorld());
    Object ent;
    while (dbi.get(ent)) {
        dbi.del();
    }
    int count = 0;
    for(I = world->eobjects.begin(); I != world->eobjects.end(); I++) {
        p->putEntity(*I->second);
        ++count;
    }
    Object::MapType report;
    report["message"] = "Objects saved to database";
    report["object_count"] = count;
    Info * info = new Info(Info::Instantiate());
    Object::ListType args(1,report);
    info->SetArgs(args);
    info->SetRefno(op.GetSerialno());
    return oplist(1,info);
}

void Admin::load(Persistance * p, const string & id, int & count)
{
    Object entity;
    if (!p->getEntity(id, entity)) {
        return;
    }
    if (!entity.IsMap()) {
        return;
    }
    Object::MapType & emap = entity.AsMap();
    Object::MapType::iterator I;
    I = emap.find("type");
    string type("thing");
    if ((I != emap.end()) && I->second.IsString()) {
        type = I->second.AsString();
    }
    if (id != "world_0") {
        world->addObject(type, entity, id);
        count++;
    }
    I = emap.find("contains");
    if ((I != emap.end()) && I->second.IsList()) {
        Object::ListType & contains = I->second.AsList();
        Object::ListType::iterator J = contains.begin();
        for(;J != contains.end(); ++J) {
            if (J->IsString()) {
                load(p, J->AsString(), count);
            }
        }
    }
}

oplist Admin::Operation(const Load & op)
{
    int count = 0;
    Persistance * p = Persistance::instance();
#if 0
    DatabaseIterator dbi(p->getWorld());
    Object ent;
    while (dbi.get(ent)) {
        const Object::MapType & m = ent.AsMap();
        Object::MapType::const_iterator I = m.find("parents");
        const string & type = (I != m.end()) ? I->second.AsList().front().AsString() : "thing";
        I = m.find("id");
        if ((I != m.end()) && (I->second.IsString())) {
            const string & id = I->second.AsString();
            if (id == "world_0") {
                // Ignore the world entry. No info required at the moment.
            } else {
                world->addObject(type, ent, id);
                ++count;
            }
        }
    }
#else
    load(p, "world_0", count);
#endif
    Object::MapType report;
    report["message"] = "Objects loaded from database";
    report["object_count"] = count;
    Info * info = new Info(Info::Instantiate());
    Object::ListType args(1,report);
    info->SetArgs(args);
    info->SetRefno(op.GetSerialno());
    return oplist(1,info);
}

oplist Admin::Operation(const Get & op)
{
    const Object & ent = op.GetArgs().front();
    try {
        const Object::MapType & emap = ent.AsMap();
        const string & id = emap.find("id")->second.AsString();
        if (id == "server") {
            const string & cmd = emap.find("cmd")->second.AsString();
            Object arg;
            Object::MapType::const_iterator I = emap.find("arg");
            if (I != emap.end()) {
                arg = I->second;
            }
            if (cmd == "query") {
                if (!arg.IsString()) {
                    return error(op, "query with no id given");
                }
                const string & ent_id = arg.AsString();
                if (ent_id.empty()) {
                    return error(op, "query id invalid");
                }
                dict_t::iterator I = world->server.idDict.find(ent_id);
                if (I == world->server.idDict.end()) {
                    return error(op, "query id not found");
                }
                Info * info = new Info(Info::Instantiate());
                Object::ListType args(1,I->second->asObject());
                info->SetArgs(args);
                info->SetRefno(op.GetSerialno());
                return oplist(1,info);
            } else {
                return error(op, "Unknown command");
            }
        }
    }
    catch (...) {
        return error(op, "Invalid get");
    }
    return oplist();
}

oplist Admin::Operation(const Set & op)
{
    const Object & ent = op.GetArgs().front();
    try {
        const Object::MapType & emap = ent.AsMap();
        const string & id = emap.find("id")->second.AsString();
        if (id == "server") {
            const string & cmd = emap.find("cmd")->second.AsString();
            Object arg;
            Object::MapType::const_iterator I = emap.find("arg");
            if (I != emap.end()) {
                arg = I->second;
            }
            if (cmd == "shutdown") {
                exit_flag = true;
                Object::MapType report;
                report["message"] = "Shutdown initiated";
                Info * info = new Info(Info::Instantiate());
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
