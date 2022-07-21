// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2005 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#include "BaseClient.h"

#include "CreatorClient.h"

#include "common/id.h"
#include "common/debug.h"
#include "common/system.h"

#include <Atlas/Objects/Anonymous.h>
#include <common/operations/Possess.h>

using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::Logout;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

using Atlas::Objects::smart_dynamic_cast;

using String::compose;

static const bool debug_flag = false;

BaseClient::BaseClient(boost::asio::io_context& io_context,
                       Atlas::Objects::Factories& factories,
                       TypeStore& typeStore) :
        m_typeStore(typeStore),
        m_connection(io_context, factories),
        m_character(nullptr)
{
}

BaseClient::~BaseClient() = default;

/// \brief Send an operation to the server
void BaseClient::send(const Operation& op)
{
    m_connection.send(op);
}


/// \brief Create a new account on the server
///
/// @param name User name of the new account
/// @param password Password of the new account
Root BaseClient::createSystemAccount()
{
    Anonymous player_ent;
    player_ent->setAttr("username", create_session_username());
    player_ent->setAttr("password", compose("%1%2", ::rand(), ::rand()));
    player_ent->setParent("sys");

    Create createAccountOp;
    createAccountOp->setArgs1(player_ent);
    createAccountOp->setSerialno(m_connection.newSerialNo());
    send(createAccountOp);
    if (m_connection.wait() != 0) {
        std::cerr << "ERROR: Failed to log into server: \""
                  << m_connection.errorMessage() << "\""
                  << std::endl << std::flush;
        return Root(nullptr);
    }

    const Root& ent = m_connection.getInfoReply();

    if (!ent->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        std::cerr << "ERROR: Logged in, but account has no id" << std::endl
                  << std::flush;
    } else {
        m_playerId = ent->getId();
        m_playerName = ent->getName();
    }

    return ent;
}

/// \brief Create a new account on the server
///
/// @param name User name of the new account
/// @param password Password of the new account
Root BaseClient::createAccount(const std::string& name,
                               const std::string& password)
{
    m_playerName = name;

    Anonymous player_ent;
    player_ent->setAttr("username", name);
    player_ent->setAttr("password", password);
    player_ent->setParent("player");

    debug_print("Logging " << name << " in with " << password << " as password");

    Login loginAccountOp;
    loginAccountOp->setArgs1(player_ent);
    loginAccountOp->setSerialno(m_connection.newSerialNo());
    send(loginAccountOp);

    if (m_connection.wait() != 0) {
        Create createAccountOp;
        createAccountOp->setArgs1(player_ent);
        createAccountOp->setSerialno(m_connection.newSerialNo());
        send(createAccountOp);
        if (m_connection.wait() != 0) {
            std::cerr << "ERROR: Failed to log into server" << std::endl
                      << std::flush;
            return Root(nullptr);
        }
    }

    const Root& ent = m_connection.getInfoReply();

    if (!ent->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        std::cerr << "ERROR: Logged in, but account has no id" << std::endl
                  << std::flush;
    } else {
        m_playerId = ent->getId();
    }
    //if (ent.find("characters") != ent.end()) {
    //}

    return ent;
}

Atlas::Objects::Entity::RootEntity BaseClient::extractFirstArg(Atlas::Objects::Operation::RootOperation op)
{
    if (!op->getArgs().empty()) {
        return smart_dynamic_cast<RootEntity>(op->getArgs().front());
    }
    return {};
}


/// \brief Create a new Character or avatar on the server
///
/// Requires that an account is already logged in.
/// @param type The type of avatar to be created
/// @return The CreatorClient object used to directly interact with the avatar
Ref<CreatorClient> BaseClient::createCharacter(const std::string& type)
{

    Anonymous character;
    character->setParent(type);
    character->setObjtype("obj");
    character->setAttr("possess", 1);
    character->setAttr("__account", m_playerName);
    character->setLoc("0");

    Create createOp;
    createOp->setFrom(m_playerId);
    createOp->setArgs1(character);
    createOp->setTo("0");

    auto createRes = sendAndWaitReply(createOp);
    if (!createRes.isValid()) {
        std::cerr << "ERROR: Failed to create character type: "
                  << type << std::endl;
        return nullptr;
    }

    auto arg = extractFirstArg(createRes);
    if (arg->isDefaultId()) {
        std::cerr << "ERROR: Failed to create character type: "
                  << type << std::endl;
        return nullptr;
    }


    Atlas::Objects::Operation::Possess p;

    Atlas::Objects::Entity::Anonymous cmap;
    cmap->setId(arg->getId());
    p->setArgs1(cmap);
    p->setSerialno(m_connection.newSerialNo());
    p->setFrom(m_playerId);
    p->setTo(m_playerId);

    auto possessRes = sendAndWaitReply(p);
    if (!possessRes.isValid()) {
        std::cerr << "ERROR: Failed to create character type: "
                  << type << std::endl;
        return nullptr;
    }

    arg = extractFirstArg(possessRes);

    if (arg->isDefaultId()) {
        std::cerr << "ERROR: Failed to create character type: "
                  << type << std::endl;
        return nullptr;
    }


    std::string entityId;

    Atlas::Message::Element element;
    if (arg->copyAttr("entity", element) == 0 && element.isMap()) {
        auto idElement = element.Map().find("id");
        if (idElement != element.Map().end() && idElement->second.isString()) {
            entityId = idElement->second.String();
        }
    }

    if (entityId.empty()) {
        std::cerr << "ERROR: Failed to create character type: "
                  << type << std::endl;
        return nullptr;
    }

    auto mind_id = idFromString(arg->getId());

    if (!mind_id.isValid()) {
        log(ERROR, String::compose("Invalid character ID \"%1\" from server.", mind_id.m_id));
    }

    Ref<CreatorClient> obj(new CreatorClient(mind_id, entityId, m_connection, m_typeStore));

    Ref<MemEntity> ownEntity = new MemEntity(mind_id);

    ownEntity->merge(arg->asMessage());
    // FIXME We are making no attempt to set LOC, as we have no entity to
    // set it to.
//    ownEntity->m_location.readFromEntity(arg);
    OpVector res; //Ignored
    obj->setOwnEntity(res, std::move(ownEntity));

    // obj = EntityFactory::instance().newThing(type, body, tmp);
    // FIXME Do we need to create a local entity for this as is done in
    // the python version? If so, do we need to keep track of a full world
    // model here, or just in the minds (when we become an AI client
    if (m_character == nullptr) {
        m_character = obj;
    }
    return obj;
}

void BaseClient::logout()
{
    Logout logout;
    send(logout);

    if (m_connection.wait() != 0) {
        std::cerr << "ERROR: Failed to logout" << std::endl << std::flush;
    }
}

// I'm making this pure virtual, to see if that is desired.
//void BaseClient::idle() {
//    time.sleep(0.1);
//}

/// \brief Handle any operations that have arrived from the server
void BaseClient::handleNet()
{
    Operation input;
    while ((input = m_connection.pop()).isValid()) {
        OpVector res;
        m_character->operation(input, res);
        OpVector::const_iterator Iend = res.end();
        for (OpVector::const_iterator I = res.begin(); I != Iend; ++I) {
            send(*I);
        }
    }
}
