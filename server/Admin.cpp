// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

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
#include <rulesets/Character.h>
#include <rulesets/BaseMind.h>

using Atlas::Message::Object;

Admin::Admin(Connection * conn, const std::string& username,
             const std::string& passwd) : Account(conn, username, passwd)
{
    type = "admin";
}

Admin::~Admin()
{
}

oplist Admin::characterError(const Create &, const Object::MapType &) const {
    return oplist();
}

oplist Admin::SaveOperation(const Save & op)
{
    edict_t::const_iterator I;
    Persistance * p = Persistance::instance();
    Object ent;
    DatabaseIterator dbi(p->getWorldDb());
    while (dbi.get(ent)) {
        dbi.del();
    }
    DatabaseIterator dbj(p->getMindDb());
    while (dbj.get(ent)) {
        dbj.del();
    }
    int count = 0;
    int mind_count = 0;
    for(I = world->eobjects.begin(); I != world->eobjects.end(); I++) {
        p->putEntity(*I->second);
        ++count;
        if (I->second->isCharacter) {
            cout << "Dumping character to database" << endl << flush;
            Character * c = (Character *)I->second;
            if (c->mind == NULL) { continue; }
            oplist res = c->mind->SaveOperation(op);
            if ((res.size() != 0) && (res.front()->GetArgs().size() != 0)) {
                cout << "Dumping mind to database" << endl << flush;
                Object & mindmap = res.front()->GetArgs().front();
                p->putMind(c->fullid, mindmap);
                ++mind_count;
            }
        }
    }
    Object::MapType report;
    report["message"] = "Objects saved to database";
    report["object_count"] = count;
    report["mind_count"] = mind_count;
    Info * info = new Info(Info::Instantiate());
    Object::ListType args(1,report);
    info->SetArgs(args);
    info->SetRefno(op.GetSerialno());
    return oplist(1,info);
}

void Admin::load(Persistance * p, const std::string & id, int & count)
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
    I = emap.find("parents");
    std::string type("thing");
    if ((I != emap.end()) && I->second.IsList()) {
        type = I->second.AsList().front().AsString();
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

oplist Admin::LoadOperation(const Load & op)
{
    int count = 0;
    int mind_count = 0;
    Persistance * p = Persistance::instance();

    // Load the world recursively
    load(p, "world_0", count);

    // Load the mind states
    DatabaseIterator dbi(p->getMindDb());
    Object ent;
    while (dbi.get(ent)) {
        const Object::MapType & m = ent.AsMap();
        Object::MapType::const_iterator I = m.find("id");
        if ((I != m.end()) && (I->second.IsString())) {
            const std::string & id = I->second.AsString();
            Entity * ent = world->getObject(id);
            if ((ent == NULL) || (!ent->isCharacter)) {
                continue;
            }
            Character * c = (Character *)ent;
            if (c->mind == NULL) { continue; }
            Load l(op);
            l.SetArgs(Object::ListType(1,ent));
            c->mind->LoadOperation(l);
            ++mind_count;
        }
    }
    Object::MapType report;
    report["message"] = "Objects loaded from database";
    report["object_count"] = count;
    report["mind_count"] = mind_count;
    Info * info = new Info(Info::Instantiate());
    Object::ListType args(1,report);
    info->SetArgs(args);
    info->SetRefno(op.GetSerialno());
    return oplist(1,info);
}

oplist Admin::GetOperation(const Get & op)
{
    const Object & ent = op.GetArgs().front();
    try {
        const Object::MapType & emap = ent.AsMap();
        const std::string & id = emap.find("id")->second.AsString();
        if (id == "server") {
            const std::string & cmd = emap.find("cmd")->second.AsString();
            Object arg;
            Object::MapType::const_iterator I = emap.find("arg");
            if (I != emap.end()) {
                arg = I->second;
            }
            if (cmd == "query") {
                if (!arg.IsString()) {
                    return error(op, "query with no id given");
                }
                const std::string & ent_id = arg.AsString();
                if (ent_id.empty()) {
                    return error(op, "query id invalid");
                }
                dict_t::iterator I = world->server.idDict.find(ent_id);
                if (I == world->server.idDict.end()) {
                    return error(op, "query id not found");
                }
                Info * info = new Info(Info::Instantiate());
                info->SetArgs(Object::ListType(1,I->second->asObject()));
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

oplist Admin::SetOperation(const Set & op)
{
    const Object & ent = op.GetArgs().front();
    try {
        const Object::MapType & emap = ent.AsMap();
        const std::string & id = emap.find("id")->second.AsString();
        if (id == "server") {
            const std::string & cmd = emap.find("cmd")->second.AsString();
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
