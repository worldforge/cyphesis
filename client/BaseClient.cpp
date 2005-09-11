// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2005 Alistair Riddoch

#include "BaseClient.h"

#include "CreatorClient.h"

#include "common/debug.h"
#include "common/BaseEntity.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

using Atlas::Objects::smart_dynamic_cast;

static const bool debug_flag = false;

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

    Anonymous player_ent;
    player_ent->setAttr("username", name);
    player_ent->setAttr("password", password);
    player_ent->setParents(std::list<std::string>(1, "player"));
    
    debug(std::cout << "Loggin " << name << " in with " << password << " as password"
               << std::endl << std::flush;);
    
    Login loginAccountOp;
    loginAccountOp->setArgs1(player_ent);
    send(loginAccountOp);

    if (m_connection.wait() != 0) {
        Create createAccountOp;
        createAccountOp->setArgs1(player_ent);
        send(createAccountOp);
        if (m_connection.wait() != 0) {
            std::cerr << "ERROR: Failed to log into server" << std::endl
                      << std::flush;
            return MapType();
        }
    }

    const Root & ent = m_connection.getReply();

    if (!ent->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        std::cerr << "ERROR: Logged in, but account has no id" << std::endl
                  << std::flush;
    } else {
        m_playerId = ent->getId();
    }
    //if (ent.find("characters") != ent.end()) {
    //}

    return ent->asMessage();
}

CreatorClient * BaseClient::createCharacter(const std::string & type)
{
    Anonymous character;
    character->setName(m_playerName);
    character->setParents(std::list<std::string>(1,type));
    character->setObjtype("obj");

    Create createOp;
    createOp->setFrom(m_playerId);
    createOp->setArgs1(character);
    send(createOp);

    if (m_connection.wait() != 0) {
        std::cerr << "ERROR: Failed to create character type: "
                  << type << std::endl << std::flush;
        return NULL;
    }

    RootEntity ent = smart_dynamic_cast<RootEntity>(m_connection.getReply());

    if (!ent->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        std::cerr << "ERROR: Character created, but has no id" << std::endl
                  << std::flush;
        return 0;
    }

    const std::string & id = ent->getId();

    EntityDict tmp;

    CreatorClient * obj = new CreatorClient(id, type, m_connection);
    obj->merge(ent->asMessage());
    // FIXME We are making no attempt to set LOC, as we have no entity to
    // set it to.
    obj->m_location.readFromEntity(ent);
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
    RootOperation input;
    while ((input = m_connection.pop()).isValid()) {
        OpVector res;
        m_character->operation(input, res);
        OpVector::const_iterator Iend = res.end();
        for (OpVector::const_iterator I = res.begin(); I != Iend; ++I) {
            send(*I);
        }
    }
}
