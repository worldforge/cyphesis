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

void Connection::send(bad_type msg)
{
    //try {
        //Connection::comm_client->send(msg);
    //}
    //catch (socket.error) {
        //Connection::disconnect();
    //}
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
            add_object(player, None);
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
    return(NULL);
}


bad_type Connection::create_operation(bad_type op)
{
    //ent=op[0];
    //try {
        //password=ent.password;
    //}
    //catch (AttributeError) {
        //return Connection::error(op,"No password specified");
    //}
    //player=Connection::server.get_object(ent.id);
    //if (player) {
        //log.inform("player account creation failed: "+ent.id,op);
        //return Connection::error(op,"Account id already exist");
    //}
    //player=Connection::add_player(Player,ent);
    //log.inform("player account creation OK: "+ent.id,op);
    //return Operation("info",player.as_entity());
    return None;
}

bad_type Connection::login_operation(bad_type op)
{
    //ent=op[0];
    //player=Connection::server.get_object(ent.id);
    //if (player and player.password==ent.password) {
        //player.connection=this;
        //Connection::add_object(player);
        //for (/*id in player.characters*/) {
            //character=Connection::server.world.find_object(id);
            //character.external_mind.connection=this;
            //log.debug(3,"login???: "+str(character));
            //Connection::add_object(character);
        //}
        //log.inform("login OK: "+ent.id,op);
        //return Operation("info",player.as_entity());
    //}
    //log.inform("login FAIL: "+ent.id,op);
    //return Connection::error(op,"Invalid login");
    return None;
}

bad_type Connection::logout_operation(bad_type op)
{
    //ent=op[0];
    //player = Connection::objects.get(ent.id);
    //if (player) {
        //op.from_=player;
        //print "logout without from, using ent.id instead:", player;
        //return Connection::route_operation(op);
    //}
    return None;
}

bad_type Connection::route_operation(bad_type op)
{
    //if (op.from_) {
        //if (Connection::objects.has_key(op.from_.id)) {
            //return op.from_.external_operation(op);
        //}
        //return Connection::error(op,"from is illegal:"+op.from_.id);
    //}
    //else {
        //return Connection::call_operation(op);
    //}
    return None;
}

bad_type Connection::operation(bad_type op)
{
    //res=Connection::check_operation(op);
    //if (res) {
        //return res;
    //}
    //res=op.atlas2internal(Connection::server.id_dict);
    //if (res) {
        //s=[];
        //for (/*item in res*/) {
            //s.append(item[1]+":"+item[2]);
        //}
        //res=string.join(s,",");
        //return Connection::error(op,"Can't convert all ids: "+res);
    //}
    //res=Connection::route_operation(op);
    //if (res) {
        //res.internal2atlas();
    //}
    //return res;
    return None;
}

Account * Connection::add_player(string & username, string & password)
{
    Player * player=new Player(this, username, password);
    add_object(player, None);
    player->connection=this;
    player->world=server->world;
    server->add_object(player);
    return(player);
}
