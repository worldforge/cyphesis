// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Admin.h"

#include "Persistance.h"
#include "ServerRouting.h"
#include "WorldRouter.h"
#include "Connection.h"

#include <rulesets/Character.h>
#include <rulesets/BaseMind.h>

#include <common/debug.h>

#include <common/Load.h>
#include <common/Save.h>

#include <Atlas/Objects/Operation/Info.h>
#include <Atlas/Objects/Operation/Logout.h>

static const bool debug_flag = true;

Admin::Admin(Connection * conn, const std::string& username,
             const std::string& passwd) : Account(conn, username, passwd)
{
    type = "admin";

    subscribe("load", OP_LOAD);
    subscribe("save", OP_SAVE);
    subscribe("get", OP_GET);
    subscribe("set", OP_SET);
}

Admin::~Admin()
{
}

OpVector Admin::characterError(const Create &, const Fragment::MapType &) const {
    return OpVector();
}

OpVector Admin::LogoutOperation(const Logout & op)
{
    const Fragment::ListType & args = op.GetArgs();
    
    if (args.empty()) {
        return Account::LogoutOperation(op);
    } else {
        Fragment::MapType::const_iterator I = args.front().AsMap().find("id");
        if ((I == args.front().AsMap().end()) || (!I->second.IsString())) {
            return error(op, "No account id given");
        }
        if (connection == NULL) {
            return error(op, "Disconnected admin account handling explicit logout");
        }
        const std::string & account_id = I->second.AsString();
        if (account_id == getId()) {
           return Account::LogoutOperation(op);
        }
        BaseEntity * player = connection->server.getObject(account_id);
        if (!player) {
            return error(op, "Logout failed");
        }
        return player->operation(op);
    }
}

OpVector Admin::SaveOperation(const Save & op)
{
    EntityDict::const_iterator I;
    Persistance * p = Persistance::instance();
    Fragment::MapType ent;
    // Clear the world database
    //DatabaseIterator dbi(p->getWorldDb());
    //while (dbi.get(ent)) {
        //dbi.del();
    //}
    //DatabaseIterator dbj(p->getMindDb());
    //while (dbj.get(ent)) {
        //dbj.del();
    //}
    int count = 0;
    int mind_count = 0;
    for(I = world->getObjects().begin(); I != world->getObjects().end(); I++) {
        p->putEntity(*I->second);
        ++count;
        Character * c = dynamic_cast<Character *>(I->second);
        
        if (c == NULL) {
            continue;
        }
        debug(std::cout << "Dumping character to database" << std::endl
                        << std::flush;);
        if (c->mind == NULL) { continue; }
        OpVector res = c->mind->SaveOperation(op);
        if ((!res.empty()) && (!res.front()->GetArgs().empty())) {
            debug(std::cout << "Dumping mind to database" << std::endl
                            << std::flush;);
            const Fragment::MapType & mindmap = res.front()->GetArgs().front().AsMap();
            p->putMind(c->getId(), mindmap);
            ++mind_count;
        }
    }
    Fragment::MapType report;
    report["message"] = "Objects saved to database";
    report["object_count"] = count;
    report["mind_count"] = mind_count;
    Info * info = new Info(Info::Instantiate());
    Fragment::ListType args(1,report);
    info->SetArgs(args);
    info->SetRefno(op.GetSerialno());
    info->SetSerialno(connection->server.getSerialNo());
    return OpVector(1,info);
}

void Admin::load(Persistance * p, const std::string & id, int & count)
{
    Fragment::MapType entity;
    if (!p->getEntity(id, entity)) {
        return;
    }
    Fragment::MapType::const_iterator I;
    I = entity.find("parents");
    std::string type("thing");
    if ((I != entity.end()) && I->second.IsList()) {
        type = I->second.AsList().front().AsString();
    }
    if (id != "world_0") {
        world->addObject(type, entity, id);
        count++;
    }
    I = entity.find("contains");
    if ((I != entity.end()) && I->second.IsList()) {
        const Fragment::ListType & contains = I->second.AsList();
        Fragment::ListType::const_iterator J = contains.begin();
        for(;J != contains.end(); ++J) {
            if (J->IsString()) {
                load(p, J->AsString(), count);
            }
        }
    }
}

OpVector Admin::LoadOperation(const Load & op)
{
    int count = 0;
    int mind_count = 0;
    Persistance * p = Persistance::instance();

    // Load the world recursively
    load(p, "world_0", count);

    // Load the mind states
    //DatabaseIterator dbi(p->getMindDb());
    //Fragment::MapType ent;
    //while (dbi.get(ent)) {
        //Fragment::MapType::const_iterator I = ent.find("id");
        //if ((I != ent.end()) && (I->second.IsString())) {
            //const std::string & id = I->second.AsString();
            //Entity * ent = world->getObject(id);
            //if ((ent == NULL) || (!ent->isCharacter())) {
                //continue;
            //}
            //Character * c = (Character *)ent;
            //if (c->mind == NULL) { continue; }
            //Load l(op);
            //l.SetArgs(Fragment::ListType(1,ent));
            //c->mind->LoadOperation(l);
            //++mind_count;
        //}
    //}
    if (connection != NULL) {
        Fragment::MapType report;
        report["message"] = "Objects loaded from database";
        report["object_count"] = count;
        report["mind_count"] = mind_count;
        Info * info = new Info(Info::Instantiate());
        Fragment::ListType args(1,report);
        info->SetArgs(args);
        info->SetRefno(op.GetSerialno());
        info->SetSerialno(connection->server.getSerialNo());
        return OpVector(1,info);
    } else {
        return OpVector();
    }
}

OpVector Admin::GetOperation(const Get & op)
{
    const Fragment & ent = op.GetArgs().front();
    try {
        const Fragment::MapType & emap = ent.AsMap();
        const std::string & id = emap.find("id")->second.AsString();
        if (id == "server") {
            const std::string & cmd = emap.find("cmd")->second.AsString();
            Fragment arg;
            Fragment::MapType::const_iterator I = emap.find("arg");
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
                const BaseDict & OOGDict = world->server.getObjects();
                BaseDict::const_iterator I = OOGDict.find(ent_id);
                const EntityDict & worldDict = world->getObjects();
                EntityDict::const_iterator J = worldDict.find(ent_id);

                if ((I == OOGDict.end()) && (J == worldDict.end())) {
                    return error(op, "query id not found");
                }

                Info * info = new Info(Info::Instantiate());
                if (I != OOGDict.end()) {
                    info->SetArgs(Fragment::ListType(1,I->second->asObject()));
                } else {
                    info->SetArgs(Fragment::ListType(1,J->second->asObject()));
                }
                info->SetRefno(op.GetSerialno());
                info->SetSerialno(connection->server.getSerialNo());
                return OpVector(1,info);
            } else {
                return error(op, "Unknown command");
            }
        }
    }
    catch (...) {
        return error(op, "Invalid get");
    }
    return OpVector();
}

OpVector Admin::SetOperation(const Set & op)
{
    const Fragment & ent = op.GetArgs().front();
    try {
        const Fragment::MapType & emap = ent.AsMap();
        const std::string & id = emap.find("id")->second.AsString();
        const std::string & objtype = emap.find("objtype")->second.AsString();
        if (id == "server") {
            const std::string & cmd = emap.find("cmd")->second.AsString();
            if (cmd == "shutdown") {
                exit_flag = true;
                Fragment::MapType report;
                report["message"] = "Shutdown initiated";
                Info * info = new Info(Info::Instantiate());
                Fragment::ListType args(1,report);
                info->SetArgs(args);
                info->SetRefno(op.GetSerialno());
                info->SetSerialno(connection->server.getSerialNo());
                return OpVector(1,info);
            } else {
                return error(op, "Unknown command");
            }
        } else if (objtype == "class") {
            // Install a new type from the client
            // const std::string & parent = emap.find("parents")->second.AsList().front().AsString();
            // std::string script;
            // Fragment::MapType::const_iterator I = emap.find("script");
            // if ((I != emap.end()) && I->second.IsString()) {
                // script = I->second.AsString();
            // }
        }
    }
    catch (...) {
        return error(op, "Invalid set");
    }
    return OpVector();
}

// There used to be a code operation handler here. It may become desirable in
// the future for the admind account to be able to send script fragments.
// Think about implementing this.
