#include <Atlas/Message/Object.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Info.h>

#include "Connection.h"
#include "Player.h"

#include "server.h"

using namespace Atlas;
using namespace Objects;

Connection::Connection(CommClient * client) :
	comm_client(client)
{
    server=comm_client->server->server;
}

void Connection::destroy()
{
    cout << "destroy called";
    //for (/*obj in Connection::objects.values()*/) {
        //if (hasattr(obj,"external_mind")) {
            //em=obj.external_mind;
            //if (em.connection) {
                //em.connection=None;
            //}
        //}
    //}
    //comm_client->destroy();
    //super().destroy(this);
}

void Connection::disconnect()
{
    //cout << "Connection died for " << str(Connection::objects) << endl;
    cout << "Connection died " << endl;
    destroy();
    //raise ConnectionError,"connection died";
}

void Connection::send(const RootOperation * msg)
{
    comm_client->send(msg);
}

RootOperation * Connection::operation(const RootOperation & op)
{
    cout << "Connection::operation" << endl << flush;
    const string & from = op.GetFrom();
    if (0==from.size()) {
        cout << "deliver locally as normal" << endl << flush;
        return BaseEntity::operation(op);
    } else {
        cout << "Must send on to account" << endl << flush;
        cout << "[" << from << "]" << endl << flush;
        if (fobjects.find(from)!=fobjects.end()) {
            return fobjects[from]->external_operation(op);
        } else {
            return error(op, "From is illegal");
        }
    }
    //return call_operation(op);
}

RootOperation * Connection::Operation(const Login & obj)
{

    cout << "Got login op" << endl << flush;
    const Message::Object & account = obj.GetArgs().front();

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
            cout << "Good login" << endl << flush;
            return(info);
        }
    }
    return(error(obj, "Login is invalid"));
}

RootOperation * Connection::Operation(const Create & obj)
{
    cout << "Got create op" << endl << flush;
    const Message::Object & account = obj.GetArgs().front();

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
            cout << "Good create" << endl << flush;
            return(info);
        }
    }
    return(error(obj, "Account creation is invalid"));

}

RootOperation * Connection::Operation(const Logout & obj)
{
    const Message::Object & account = obj.GetArgs().front();
    
    if (account.IsMap()) {
        string account_id = account.AsMap().find("id")->second.AsString();
        string password = account.AsMap().find("password")->second.AsString();
        Player * player = (Player *)server->get_object(account_id);
        if (player) {
            Logout l = obj;
            l.SetFrom(player->fullid);
            cout << "Logout without from. Using " << player->fullid << " instead." << endl << flush;
            operation(l);
        }
    }
    return(NULL);
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
