// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "BaseClient.h"

#include "CreatorClient.h"

#include "common/debug.h"
#include "common/BaseEntity.h"

#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Login.h>

using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::RootOperation;

static const bool debug_flag = false;

// This is a template which requires debug flag to be declared.
#include "rulesets/Entity_getLocation.h"

BaseClient::BaseClient()
{
}

BaseClient::~BaseClient()
{
}

MapType BaseClient::createPlayer(const std::string & name,
                                 const std::string & password)
{
    m_playerName = name;
    MapType player_ent;
    player_ent["username"] = name;
    player_ent["password"] = password;
    player_ent["parents"] = ListType(1, "player");
    
    debug(std::cout << "Loggin " << name << " in with " << password << " as password"
               << std::endl << std::flush;);
    
    Login loginAccountOp;
    loginAccountOp.setArgs(ListType(1,player_ent));
    send(loginAccountOp);

    if (m_connection.wait() != 0) {
        Create createAccountOp;
        createAccountOp.setArgs(ListType(1,player_ent));
        send(createAccountOp);
        if (m_connection.wait() != 0) {
            std::cerr << "ERROR: Failed to log into server" << std::endl
                      << std::flush;
            return MapType();
        }
    }

    const MapType & ent = m_connection.getReply();

    MapType::const_iterator I = ent.find("id");
    if (I == ent.end() || !I->second.isString()) {
        std::cerr << "ERROR: Logged in, but account has no id" << std::endl
                  << std::flush;
    } else {
        m_playerId = I->second.asString();
    }
    //if (ent.find("characters") != ent.end()) {
    //}

    return ent;
}

CreatorClient * BaseClient::createCharacter(const std::string & type)
{
    MapType character;
    character["name"] = m_playerName;
    character["parents"] = ListType(1,type);

    Create createOp;
    createOp.setFrom(m_playerId);
    createOp.setArgs(ListType(1,character));
    send(createOp);

    if (m_connection.wait() != 0) {
        std::cerr << "ERROR: Failed to create character type: "
                  << type << std::endl << std::flush;
        return NULL;
    }
    const MapType & body = m_connection.getReply();

    const std::string & id = body.find("id")->second.asString();

    EntityDict tmp;

    CreatorClient * obj = new CreatorClient(id, type, m_connection);
    obj->merge(body);
    obj->getLocation(body, tmp);
    // obj = EntityFactory::instance()->newThing(type, body, tmp);
    // FIXME Do we need to create a local entity for this as is done in
    // the python version? If so, do we need to keep track of a full world
    // model here, or just in the minds (when we become an AI client
    return obj;
}

// I'm making this pure virtual, to see if that is desired.
//void BaseClient::idle() {
//    time.sleep(0.1);
//}

void BaseClient::handleNet()
{
    RootOperation * input;
    while ((input = m_connection.pop()) != NULL) {
        OpVector res;
        m_character->operation(*input, res);
        OpVector::const_iterator Iend = res.end();
        for (OpVector::const_iterator I = res.begin(); I != Iend; ++I) {
            send(*(*I));
        }
        delete input;
    }
}
