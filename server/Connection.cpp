// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Connection.h"

#include "ServerRouting.h"
#include "WorldRouter.h"
#include "Lobby.h"
#include "CommClient.h"
#include "CommServer.h"
#include "Player.h"
#include "ExternalMind.h"
#include "Persistance.h"

#include <rulesets/Character.h>

#include <common/log.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/inheritance.h>

#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Info.h>
#include <Atlas/Objects/Operation/Get.h>

#include <sigc++/bind.h>
#include <sigc++/object_slot.h>

static const bool debug_flag = false;

Connection::Connection(const std::string & id, CommClient & client) : OOGThing(id),
                       commClient(client), obsolete(false),
                       server(commClient.commServer.server)
{
    subscribe("login", OP_LOGIN);
    subscribe("logout", OP_LOGOUT);
    subscribe("create", OP_CREATE);
    subscribe("get", OP_GET);
}

Connection::~Connection()
{
}

void Connection::send(const RootOperation & msg) const
{
    commClient.send(msg);
}

Account * Connection::addPlayer(const std::string& username,
                                const std::string& password)
{
    Player * player = new Player(this, username, password);
    addObject(player);
    player->connection = this;
    server.addAccount(player);
    server.lobby.addObject(player);
    return player;
}

void Connection::addObject(BaseEntity * obj)
{
    objects[obj->getId()] = obj;
    SigC::Connection * con = new SigC::Connection(obj->destroyed.connect(SigC::bind<std::string>(SigC::slot(*this, &Connection::objectDeleted), obj->getId())));
    destroyedConnections[obj->getId()] = con;
}

void Connection::removeObject(const std::string & id)
{
    if (!obsolete) {
        objects.erase(id);
        ConMap::iterator I = destroyedConnections.find(id);
        if (I != destroyedConnections.end()) {
            delete I->second;
            destroyedConnections.erase(I);
        }
    }
}

void Connection::objectDeleted(std::string id) {
    removeObject(id);
}

void Connection::destroy()
{
    // Once we are obsolete, ExternalMind can no longer affect contents
    // of objects when we delete it.
    assert(!obsolete);
    obsolete = true;
    ConMap::const_iterator J = destroyedConnections.begin();
    for(; J != destroyedConnections.end(); J++) {
        J->second->disconnect();
        delete J->second;
    }

    debug(std::cout << "destroy called" << std::endl << std::flush;);
    BaseDict::const_iterator I;
    for(I = objects.begin(); I != objects.end(); I++) {
        Account * ac = dynamic_cast<Account *>(I->second);
        if (ac != NULL) {
            server.lobby.delObject(ac);
            ac->connection = NULL;
            continue;
        }
        Character * character = dynamic_cast<Character *>(I->second);
        if (character != NULL) {
            if (character->externalMind != NULL) {
                delete character->externalMind;
                character->externalMind = NULL;
            }
        }
    }
}

void Connection::close()
{
    commClient.close();
}

OpVector Connection::operation(const RootOperation & op)
{
    debug(std::cout << "Connection::operation" << std::endl << std::flush;);
    const std::string & from = op.GetFrom();
    if (from.empty()) {
        debug(std::cout << "deliver locally as normal" << std::endl << std::flush;);
        return callOperation(op);
    } else {
        debug(std::cout << "Must send on to account" << std::endl << std::flush;);
        debug(std::cout << "[" << from << "]" << std::endl << std::flush;);
        BaseDict::const_iterator I = objects.find(from);
        if (I == objects.end()) {
            std::cout << "Illegal from \"" << from << "\" in " << op.GetParents().front().AsString() << " op from client" << std::endl << std::flush;
            std::string err = "From [";
            err += from;
            err += "] is illegal";
            return error(op, err.c_str());
        }
        BaseEntity * ent = I->second;
        Character * character = dynamic_cast<Character *>(ent);
        if ((character != NULL) && (character->externalMind == NULL)) {
            character->externalMind = new ExternalMind(*this,
                       character->getId(), character->getName());
            debug(std::cout << "Re-connecting existing character to new connection" << std::endl << std::flush;);
            Info * info = new Info(Info::Instantiate());
            info->SetArgs(Fragment::ListType(1,character->asObject()));
            info->SetRefno(op.GetSerialno());
            info->SetSerialno(server.getSerialNo());
            OpVector res = ent->externalOperation(op);
            res.insert(res.begin(), info);
            return res;
        }
        return ent->externalOperation(op);
    }
    return OpVector();
}

OpVector Connection::LoginOperation(const Login & op)
{

    debug(std::cout << "Got login op" << std::endl << std::flush;);
    if (op.GetArgs().empty()) {
        return error(op, "Login has no argument");
    }
    if (!op.GetArgs().front().IsMap()) {
        return error(op, "Login arg is malformed");
    }
    // Account should be the first argument of the op
    const Fragment::MapType & account = op.GetArgs().front().AsMap();
    // Check for username, and if its not there, then check for
    // id in case we are dealing with an old client.
    Fragment::MapType::const_iterator I = account.find("username");
    if ((I == account.end()) || !I->second.IsString()) {
        log(WARNING, "Got Login with no username. Checking for old style Login");
        I = account.find("id");
        if ((I == account.end()) || !I->second.IsString()) {
            return error(op, "No username provided for Login");
        }
    }
    const std::string & username = I->second.AsString();
    if (username.empty()) {
        return error(op, "Empty username provided for Login");
    }
    I = account.find("password");
    if ((I == account.end()) || !I->second.IsString()) {
        return error(op, "No password provided for Login");
    }
    const std::string & passwd = account.find("password")->second.AsString();
    // We now have username and password, so can check whether we know this
    // account, either from existing account ....
    Account * player = server.getAccountByName(username);
    // or if not, from the database
    if (player == 0) {
        debug(std::cout << "No " << username << " account in server. Checking in database." << std::endl << std::flush;);
        player = Persistance::instance()->getAccount(username);
        if (player != 0) {
            Persistance::instance()->registerCharacters(*player,
                                                     server.world.getObjects());
            server.addAccount(player);
        }
    }
    if ((player == 0) || (passwd != player->password)) {
        return error(op, "Login is invalid");
    }
    // Account appears to be who they say they are
    if (player->connection) {
        // Internals don't allow player to log in more than once.
        return error(op, "This account is already logged in");
    }
    // Connect everything up
    addObject(player);
    EntityDict::const_iterator J = player->getCharacters().begin();
    for (; J != player->getCharacters().end(); J++) {
        addObject(J->second);
    }
    player->connection = this;
    server.lobby.addObject(player);
    // Let the client know they have logged in
    Info * info = new Info(Info::Instantiate());
    info->SetArgs(Fragment::ListType(1,player->asObject()));
    info->SetRefno(op.GetSerialno());
    info->SetSerialno(server.getSerialNo());
    debug(std::cout << "Good login" << std::endl << std::flush;);
    return OpVector(1,info);
}

OpVector Connection::CreateOperation(const Create & op)
{
    debug(std::cout << "Got create op" << std::endl << std::flush;);
    if (op.GetArgs().empty()) {
        return error(op, "Create has no argument");
    }
    if (!op.GetArgs().front().IsMap()) {
        return error(op, "Create is malformed");
    }
    const Fragment::MapType & account = op.GetArgs().front().AsMap();

    if (restricted_flag) {
        return error(op, "Account creation on this server is restricted");
    }
    Fragment::MapType::const_iterator I = account.find("username");
    if ((I == account.end()) || !I->second.IsString()) {
        log(WARNING, "Got Create for account with no username. Checking for old style Create.");
        I = account.find("id");
        if ((I == account.end()) || !I->second.IsString()) {
            return error(op, "Account creation with no username");
        }
    }

    const std::string & username = I->second.AsString();
    I = account.find("password");
    if ((I == account.end()) || !I->second.IsString()) {
        return error(op, "Account creation with no password");
    }
    const std::string & password = I->second.AsString();

    if ((0 != server.getAccountByName(username)) ||
        (Persistance::instance()->findAccount(username)) ||
        (username.empty()) || (password.empty())) {
        // Account exists, or creation data is duff
        return error(op, "Account creation is invalid");
    }
    Account * player = addPlayer(username, password);
    Persistance::instance()->putAccount(*player);
    Info * info = new Info(Info::Instantiate());
    info->SetArgs(Fragment::ListType(1,player->asObject()));
    info->SetRefno(op.GetSerialno());
    info->SetSerialno(server.getSerialNo());
    debug(std::cout << "Good create" << std::endl << std::flush;);
    return OpVector(1,info);
}

OpVector Connection::LogoutOperation(const Logout & op)
{
    if (op.GetArgs().empty()) {
        // Logging self out
        Info info = Info(Info::Instantiate());
        info.SetArgs(Fragment::ListType(1,op.AsObject()));
        info.SetRefno(op.GetSerialno());
        info.SetSerialno(server.getSerialNo());
        send(info);
        close();
        return OpVector();
    }
    if (!op.GetArgs().front().IsMap()) {
        return error(op, "Create arg is not a map");
    }
    const Fragment::MapType & account = op.GetArgs().front().AsMap();
    
    Fragment::MapType::const_iterator I = account.find("username");
    if ((I == account.end()) || !I->second.IsString()) {
        log(WARNING, "Got Logout with no username. Checking for old style Logout.");
        I = account.find("id");
        if ((I == account.end()) || !I->second.IsString()) {
            return error(op, "Logout is invalid");
        }
    }
    const std::string & username = I->second.AsString();
    I = account.find("password");
    if ((I == account.end()) || (!I->second.IsString())) {
        return error(op, "No account password given");
    }
    const std::string & password = I->second.AsString();
    Account * player = server.getAccountByName(username);
    if ((!player) || (password != player->password)) {
        return error(op, "Logout failed");
    }
    Logout l = op;
    l.SetFrom(player->getId());
    operation(l);

    return OpVector();
}

OpVector Connection::GetOperation(const Get & op)
{
    const Fragment::ListType & args = op.GetArgs();

    Info * info;
    if (args.empty()) {
        info = new Info(Info::Instantiate());
        info->SetArgs(Fragment::ListType(1,server.asObject()));
        info->SetRefno(op.GetSerialno());
        info->SetSerialno(server.getSerialNo());
        debug(std::cout << "Replying to empty get" << std::endl << std::flush;);
    } else {
        if (!args.front().IsMap()) {
            return error(op, "Get op arg is not a map");
        }
        Fragment::MapType::const_iterator I = args.front().AsMap().find("id");
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
        info->SetArgs(Fragment::ListType(1,o->AsObject()));
        info->SetRefno(op.GetSerialno());
        info->SetSerialno(server.getSerialNo());
    }
    
    return OpVector(1,info);
}
