#include <Atlas/Message/Object.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Info.h>

#include <rulesets/Character.h>

#include "Connection.h"
#include "Player.h"

#include "server.h"

static int debug_server = 1;

using namespace Atlas;
using namespace Objects;

Connection::Connection(CommClient * client) :
	comm_client(client)
{
    server=comm_client->server->server;
}

void Connection::destroy()
{
    debug_server && cout << "destroy called";
    fdict_t::const_iterator I;
    for(I = fobjects.begin(); I != fobjects.end(); I++) {
        BaseEntity * ent = I->second;
        if (ent->in_game = 0) {
            continue;
        }
        Thing * obj = (Thing*)ent;
        if (obj->is_character != 0) {
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

void Connection::disconnect()
{
    //cout << "Connection died for " << str(Connection::objects) << endl;
    debug_server && cout << "Connection died " << endl;
    destroy();
    //raise ConnectionError,"connection died";
}

void Connection::send(const RootOperation * msg)
{
    if (comm_client != NULL) {
        comm_client->send(msg);
    }
}

oplist Connection::operation(const RootOperation & op)
{
    debug_server && cout << "Connection::operation" << endl << flush;
    const string & from = op.GetFrom();
    if (0==from.size()) {
        debug_server && cout << "deliver locally as normal" << endl << flush;
        return BaseEntity::operation(op);
    } else {
        debug_server && cout << "Must send on to account" << endl << flush;
        debug_server && cout << "[" << from << "]" << endl << flush;
        if (fobjects.find(from)!=fobjects.end()) {
            return fobjects[from]->external_operation(op);
        } else {
            return error(op, "From is illegal");
        }
    }
    oplist res;
    return(res);
}

oplist Connection::Operation(const Login & op)
{

    debug_server && cout << "Got login op" << endl << flush;
    const Message::Object & account = op.GetArgs().front();

    if (account.IsMap()) {
        string account_id = account.AsMap().find("id")->second.AsString();
        string password = account.AsMap().find("password")->second.AsString();

        Player * player = (Player *)server->get_object(account_id);
        if (player && (account_id.size()!=0) && (password==player->password)) {
            add_object(player);
            player->connection=this;
            Info * info = new Info();
            *info = Info::Instantiate();
            Message::Object::ListType args(1,player->asObject());
            info->SetArgs(args);
            info->SetRefno(op.GetSerialno());
            debug_server && cout << "Good login" << endl << flush;
            return(oplist(1,info));
        }
    }
    return(error(op, "Login is invalid"));
}

oplist Connection::Operation(const Create & op)
{
    debug_server && cout << "Got create op" << endl << flush;
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
            debug_server && cout << "Good create" << endl << flush;
            return(oplist(1,info));
        }
    }
    return(error(op, "Account creation is invalid"));

}

oplist Connection::Operation(const Logout & obj)
{
    const Message::Object & account = obj.GetArgs().front();
    
    if (account.IsMap()) {
        string account_id = account.AsMap().find("id")->second.AsString();
        string password = account.AsMap().find("password")->second.AsString();
        Player * player = (Player *)server->get_object(account_id);
        if (player) {
            Logout l = obj;
            l.SetFrom(player->fullid);
            debug_server && cout << "Logout without from. Using " << player->fullid << " instead." << endl << flush;
            operation(l);
        }
    }
    oplist res;
    return(res);
}

Account * Connection::add_player(string & username, string & password)
{
    Player * player=new Player(this, username, password);
    add_object(player);
    player->connection=this;
    player->world=server->world;
    server->add_object(player);
    return(player);
}
