// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Info.h>
#include <Atlas/Objects/Operation/Get.h>

#include <rulesets/Character.h>
#include <rulesets/ExternalMind.h>
#include <common/debug.h>

#include "Connection.h"
#include "ServerRouting.h"
#include "CommClient.h"
#include "Player.h"

#include "server.h"

static const bool debug_flag = false;

using namespace Atlas;
using namespace Objects;

inline Account * Connection::add_player(string & username, string & password)
{
    Player * player=new Player(this, username, password);
    add_object(player);
    player->connection=this;
    player->world=server->world;
    server->add_object(player);
    return(player);
}

void Connection::destroy()
{
    debug(cout << "destroy called";);
    fdict_t::const_iterator I;
    for(I = fobjects.begin(); I != fobjects.end(); I++) {
        BaseEntity * ent = I->second;
        if (ent->in_game == 0) {
            continue;
        }
        Thing * obj = (Thing*)ent;
        if (obj->is_character == true) {
            Character * character = (Character *)obj;
            if (character->external_mind != NULL) {
                if (character->external_mind != NULL) {
                    character->external_mind=NULL;
                }
            }
        }
    }
    comm_client = NULL;
    BaseEntity::destroy();
}

oplist Connection::operation(const RootOperation & op)
{
    debug(cout << "Connection::operation" << endl << flush;);
    const string & from = op.GetFrom();
    if (0==from.size()) {
        debug(cout << "deliver locally as normal" << endl << flush;);
        return BaseEntity::operation(op);
    } else {
        debug(cout << "Must send on to account" << endl << flush;);
        debug(cout << "[" << from << "]" << endl << flush;);
        if (fobjects.find(from)!=fobjects.end()) {
            BaseEntity * ent = fobjects[from];
            if ((ent->in_game != 0) && (((Thing *)ent)->is_character != 0) &&
                (((Character *)ent)->external_mind == NULL)) {
                Character * pchar = (Character *)ent;
                pchar->external_mind = new ExternalMind(this, pchar->fullid, pchar ->name);
                cout << "Re-connecting existing character to new connection" << endl << flush;
                Info * info = new Info();
                *info = Info::Instantiate();
                Message::Object::ListType args(1,pchar->asObject());
                info->SetArgs(args);
                info->SetRefno(op.GetSerialno());
                oplist res = ent->external_operation(op);
                res.push_front(info);
                return res;
            }
            return ent->external_operation(op);
        } else {
            return error(op, "From is illegal");
        }
    }
    oplist res;
    return(res);
}

oplist Connection::Operation(const Login & op)
{

    debug(cout << "Got login op" << endl << flush;);
    const Message::Object & account = op.GetArgs().front();

    if (account.IsMap()) {
        string account_id = account.AsMap().find("id")->second.AsString();
        string password = account.AsMap().find("password")->second.AsString();

        Player * player = (Player *)server->get_object(account_id);
        if (player && (account_id.size()!=0) && (password==player->password)) {
            add_object(player);
            fdict_t::const_iterator I;
            for (I=player->characters_dict.begin();
                 I!=player->characters_dict.end(); I++) {
                add_object(I->second);
            }
            player->connection=this;
            Info * info = new Info();
            *info = Info::Instantiate();
            Message::Object::ListType args(1,player->asObject());
            info->SetArgs(args);
            info->SetRefno(op.GetSerialno());
            debug(cout << "Good login" << endl << flush;);
            return(oplist(1,info));
        }
    }
    return(error(op, "Login is invalid"));
}

oplist Connection::Operation(const Create & op)
{
    debug(cout << "Got create op" << endl << flush;);
    const Message::Object & account = op.GetArgs().front();

    if (account.IsMap()) {
        string account_id = account.AsMap().find("id")->second.AsString();
        string password = account.AsMap().find("password")->second.AsString();

        if ((NULL==server->get_object(account_id)) && 
            (account_id.size() != 0) && (password.size() != 0)) {
            Account * player = add_player(account_id, password);
            Info * info = new Info();
            *info = Info::Instantiate();
            Message::Object::ListType args(1,player->asObject());
            info->SetArgs(args);
            info->SetRefno(op.GetSerialno());
            debug(cout << "Good create" << endl << flush;);
            return(oplist(1,info));
        }
    }
    return(error(op, "Account creation is invalid"));

}

oplist Connection::Operation(const Logout & op)
{
    const Message::Object & account = op.GetArgs().front();
    
    if (account.IsMap()) {
        string account_id = account.AsMap().find("id")->second.AsString();
        string password = account.AsMap().find("password")->second.AsString();
        Player * player = (Player *)server->get_object(account_id);
        if (player) {
            Logout l = op;
            l.SetFrom(player->fullid);
            debug(cout << "Logout without from. Using " << player->fullid << " instead." << endl << flush;);
            operation(l);
        }
    }
    oplist res;
    return(res);
}

oplist Connection::Operation(const Get & op)
{
    oplist res;

    cout << "Got get" << endl << flush;
    Info * info = new Info();
    *info = Info::Instantiate(); 
    Message::Object::ListType args(1,server->asObject());
    info->SetArgs(args);
    info->SetRefno(op.GetSerialno());
    cout << "Replying to get" << endl << flush;
    
    res.push_back(info);
    return(res);
}
