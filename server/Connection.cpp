// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Info.h>
#include <Atlas/Objects/Operation/Get.h>
#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Disappearance.h>

#include <rulesets/Character.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/inheritance.h>

#include "Connection.h"
#include "ServerRouting.h"
#include "Lobby.h"
#include "CommClient.h"
#include "CommServer.h"
#include "Player.h"
#include "ExternalMind.h"
#include "Persistance.h"

static const bool debug_flag = false;

using Atlas::Message::Object;

Connection::Connection(CommClient & client) : commClient(client),
                       server(commClient.commServer.server)
{
}

Connection::~Connection()
{
}

void Connection::send(const RootOperation * msg) const
{
    commClient.send(msg);
}

Account * Connection::addPlayer(const std::string& username,
                                const std::string& password)
{
    Player * player=new Player(this, username, password);
    addObject(player);
    player->connection=this;
    player->world=&server.getWorld();
    server.addObject(player);
    server.lobby.addObject(player);
    return player;
}

void Connection::destroy()
{
    debug(std::cout << "destroy called";);
    BaseDict::const_iterator I;
    for(I = objects.begin(); I != objects.end(); I++) {
        BaseEntity * ent = I->second;
        if (!ent->inGame()) {
            server.lobby.delObject((Account *)ent);
            continue;
        }
        Thing * obj = (Thing*)ent;
        if (obj->isCharacter()) {
            Character * character = (Character *)obj;
            if (character->externalMind != NULL) {
                delete character->externalMind;
                character->externalMind = NULL;
            }
        }
    }
    BaseEntity::destroy();
}

OpVector Connection::operation(const RootOperation & op)
{
    debug(std::cout << "Connection::operation" << std::endl << std::flush;);
    const std::string & from = op.GetFrom();
    if (from.empty()) {
        debug(std::cout << "deliver locally as normal" << std::endl << std::flush;);
        return BaseEntity::operation(op);
    } else {
        debug(std::cout << "Must send on to account" << std::endl << std::flush;);
        debug(std::cout << "[" << from << "]" << std::endl << std::flush;);
        BaseDict::const_iterator I = objects.find(from);
        if (I != objects.end()) {
            BaseEntity * ent = I->second;
            if (ent->inGame() && ((Thing *)ent)->isCharacter() &&
                (((Character *)ent)->externalMind == NULL)) {
                Character * pchar = (Character *)ent;
                pchar->externalMind = new ExternalMind(*this, pchar->getId(), pchar->getName());
                debug(std::cout << "Re-connecting existing character to new connection" << std::endl << std::flush;);
                Info * info = new Info(Info::Instantiate());
                info->SetArgs(Object::ListType(1,pchar->asObject()));
                info->SetRefno(op.GetSerialno());
                info->SetSerialno(server.getSerialNo());
                OpVector res = ent->externalOperation(op);
                res.insert(res.begin(), info);
                return res;
            }
            return ent->externalOperation(op);
        } else {
            std::cout << from;
            return error(op, "From is illegal");
        }
    }
    return OpVector();
}

OpVector Connection::LoginOperation(const Login & op)
{

    debug(std::cout << "Got login op" << std::endl << std::flush;);
    const Object & account = op.GetArgs().front();

    if (account.IsMap()) {
        const std::string account_id = account.AsMap().find("id")->second.AsString();
        const std::string password = account.AsMap().find("password")->second.AsString();

        Account * player = (Player *)server.getObject(account_id);
        if (player == NULL) {
            player = Persistance::instance()->getAccount(account_id);
            if (player != NULL) {
                player->world=&server.getWorld();
                server.addObject(player);
            }
        }
        if (player && !account_id.empty() && (password==player->password)) {
            addObject(player);
            EntityDict::const_iterator I;
            for (I=player->charactersDict.begin();
                 I!=player->charactersDict.end(); I++) {
                addObject(I->second);
            }
            player->connection=this;
            server.lobby.addObject(player);
            Info * info = new Info(Info::Instantiate());
            info->SetArgs(Object::ListType(1,player->asObject()));
            info->SetRefno(op.GetSerialno());
            info->SetSerialno(server.getSerialNo());
            debug(std::cout << "Good login" << std::endl << std::flush;);
            return OpVector(1,info);
        }
    }
    return error(op, "Login is invalid");
}

OpVector Connection::CreateOperation(const Create & op)
{
    debug(std::cout << "Got create op" << std::endl << std::flush;);
    const Object & account = op.GetArgs().front();

    if (Persistance::restricted) {
        return error(op, "Account creation on this server is restricted");
    }

    if (account.IsMap()) {
        const std::string & account_id = account.AsMap().find("id")->second.AsString();
        const std::string & password = account.AsMap().find("password")->second.AsString();

        if ((NULL==server.getObject(account_id)) && 
            (!Persistance::instance()->findAccount(account_id)) &&
            (!account_id.empty()) && (!password.empty())) {
            Account * player = addPlayer(account_id, password);
            Persistance::instance()->putAccount(*player);
            Info * info = new Info(Info::Instantiate());
            info->SetArgs(Object::ListType(1,player->asObject()));
            info->SetRefno(op.GetSerialno());
            info->SetSerialno(server.getSerialNo());
            debug(std::cout << "Good create" << std::endl << std::flush;);
            return OpVector(1,info);
        }
    }
    return error(op, "Account creation is invalid");

}

OpVector Connection::LogoutOperation(const Logout & op)
{
    const Object & account = op.GetArgs().front();
    
    if (account.IsMap()) {
        Object::MapType::const_iterator I = account.AsMap().find("id");
        if ((I == account.AsMap().end()) || (!I->second.IsString())) {
            return error(op, "No account id given");
        }
        const std::string & account_id = I->second.AsString();
        I = account.AsMap().find("password");
        if ((I == account.AsMap().end()) || (!I->second.IsString())) {
            return error(op, "No account password given");
        }
        const std::string & password = I->second.AsString();
        Account * player = (Account *)server.getObject(account_id);
        if ((!player) || (password != player->password)) {
            return error(op, "Logout failed");
        }
        Logout l = op;
        l.SetFrom(player->getId());
        operation(l);
    }
    return OpVector();
}

OpVector Connection::GetOperation(const Get & op)
{
    const Object::ListType & args = op.GetArgs();

    Info * info;
    if (args.empty()) {
        info = new Info(Info::Instantiate());
        info->SetArgs(Object::ListType(1,server.asObject()));
        info->SetRefno(op.GetSerialno());
        info->SetSerialno(server.getSerialNo());
        debug(std::cout << "Replying to empty get" << std::endl << std::flush;);
    } else {
        Object::MapType::const_iterator I = args.front().AsMap().find("id");
        if ((I == args.front().AsMap().end()) || (!I->second.IsString())) {
            return error(op, "Type definition requested with no id");
        }
        const std::string & id = I->second.AsString();
        debug(std::cout << "Get got for " << id << std::endl << std::flush;);
        Atlas::Objects::Root * o = Inheritance::instance().get(id);
        if (o == NULL) {
            return error(op, "Unknown type definition requested");
        }
        info = new Info(Info::Instantiate());
        info->SetArgs(Object::ListType(1,o->AsObject()));
        info->SetRefno(op.GetSerialno());
        info->SetSerialno(server.getSerialNo());
    }
    
    return OpVector(1,info);
}
