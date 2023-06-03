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


#include "Account.h"

#include "Connection.h"
#include "ServerRouting.h"
#include "Lobby.h"
#include "rules/simulation/ExternalMind.h"
#include "Persistence.h"
#include "PossessionAuthenticator.h"
#include "common/custom.h"

#include "rules/simulation/BaseWorld.h"
#include "rules/LocatedEntity.h"

#include "common/id.h"
#include "common/debug.h"
#include "common/log.h"
#include "common/compose.hpp"
#include "AccountProperty.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <sigc++/adaptors/bind.h>
#include <rules/simulation/MindsProperty.h>
#include <common/Property.h>


using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Sound;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

using Atlas::Objects::smart_dynamic_cast;

static const bool debug_flag = false;

/// \brief Account constructor
///
/// @param conn Network Connection creating this Account
/// @param uname Username for this account
/// @param passwd Password for this account
/// @param id String identifier for this account
/// @param intId Integer identifier for this account
Account::Account(Connection* conn,
                 std::string uname,
                 std::string passwd,
                 RouterId id) :
        ConnectableRouter(std::move(id)),
        m_connection(conn),
        m_username(std::move(uname)), m_password(std::move(passwd))
{
}

/// \brief Called when the LocatedEntity has been removed from the world.
///
/// @param id Integer identifier of the LocatedEntity destroyed.
void Account::characterDestroyed(long id)
{
    m_charactersDict.erase(id);
}

void Account::setConnection(Connection* connection)
{
    if (!connection) {
        for (auto& entry : m_minds) {
            removeMindFromEntity(entry.second.mind.get());
        }
        m_minds.clear();
    }
    m_connection = connection;
}

Connection* Account::getConnection() const
{
    return m_connection;
}

std::unique_ptr<ExternalMind> Account::createMind(const Ref<LocatedEntity>& entity) const
{
    auto id = newId();
    return std::make_unique<ExternalMind>(id, entity);
}

/// \brief Connect an existing character to this account
///
/// \brief chr The character to connect to this account
/// \return Returns 0 on success and -1 on failure.
int Account::connectCharacter(const Ref<LocatedEntity>& entity, OpVector& res)
{
    if (m_minds.find(entity->getIntId()) != m_minds.end()) {
        log(WARNING, String::compose("Entity %1 is already connected to mind of account %2.", entity->describeEntity(), m_username));
        return 1;
    } else {
        //Create an external mind and hook it up with the entity
        auto mind = createMind(entity);
        auto mindPtr = mind.get();
        auto destroyedConnection = entity->destroyed.connect([this, mindPtr]() {
            removeMindFromEntity(mindPtr);
            m_minds.erase(mindPtr->getEntity()->getIntId());
        });
        mind->linkUp(m_connection);
        m_connection->addRouter(mind.get());

        //Inform the client about the mind.
        Info mindInfo{};
        Anonymous mindEntity;
        mind->addToEntity(mindEntity);
        mindInfo->setArgs1(mindEntity);
        res.push_back(mindInfo);


        auto& mindsProp = entity->requirePropertyClassFixed<MindsProperty>();
        mindsProp.addMind(mind.get());
        entity->applyProperty(mindsProp);

        m_minds.emplace(entity->getIntId(), MindEntry{std::move(mind), AutoCloseConnection(destroyedConnection)});
        return 0;
    }

//    //Now that we're connected we need to send any thoughts that we've been given to the mind client.
//    auto thoughts = m_proxyMind->getThoughts();
//    //We need to clear the existing thoughts since we'll be sending them anew; else we'll end up with duplicates.
//    m_proxyMind->clearThoughts();
//    Atlas::Objects::Operation::Think think;
//    Atlas::Objects::Operation::Set setThoughts;
//    setThoughts->setArgs(thoughts);
//    think->setArgs1(setThoughts);
//    think->setTo(getId());
//    sendWorld(think);
//
//    externalLinkChanged.emit();




}

/// \brief Add a Character to those that belong to this Account
///
/// @param chr Character object to be added
void Account::addCharacter(const Ref<LocatedEntity>& chr)
{
    m_charactersDict[chr->getIntId()] = chr;
    chr->destroyed.connect(sigc::bind(sigc::mem_fun(*this, &Account::characterDestroyed), chr->getIntId()));
}

void Account::sendUpdateToClient()
{
    if (m_connection) {
        Sight s;
        s->setTo(getId());
        Anonymous sight_arg;
        addToEntity(sight_arg);
        s->setArgs1(sight_arg);
        s->setSeconds(BaseWorld::instance().getTimeAsSeconds());
        m_connection->send(s);
    }
}

void Account::removeMindFromEntity(ExternalMind* mind)
{
    //Delete any mind attached to this character
    if (m_connection) {
        m_connection->removeRouter(mind->getIntId());
    } else {
        log(WARNING, "Account still had minds even after connection had been shut down.");
    }
    auto& entity = mind->getEntity();
    if (!entity->isDestroyed()) {
        auto prop = entity->modPropertyClassFixed<MindsProperty>();
        if (prop) {
            prop->removeMind(mind, *entity);
            entity->applyProperty(*prop);
            entity->enqueueUpdateOp();
        }
    }
}

void Account::LogoutOperation(const Operation& op, OpVector& res)
{
    if (m_connection == nullptr) {
        error(op, String::compose("Account::LogoutOperation on account %1 (%2) that doesn't seem to "
                                  "be connected.", getId(), m_username), res, getId());
        return;
    }

    if (!op->getArgs().empty()) {
        //If there are args it means that we should only log out one specific mind.
        auto arg = op->getArgs().front();
        if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            error(op, "No id given on logout op", res, getId());
            return;
        }
        auto id = arg->getId();
        for (auto& entry : m_minds) {
            auto& mind = entry.second.mind;
            if (mind->getId() == id) {
                removeMindFromEntity(mind.get());
                m_minds.erase(mind->getEntity()->getIntId());

                Info info;
                info->setArgs1(op);
                if (!op->isDefaultSerialno()) {
                    info->setRefno(op->getSerialno());
                }
                info->setFrom(getId());
                info->setTo(getId());
                info->setSeconds(BaseWorld::instance().getTimeAsSeconds());
                m_connection->send(info);

                return;
            }
        }
        error(op, "Logout failed", res, getId());
    } else {

        Info info;
        info->setArgs1(op);
        if (!op->isDefaultSerialno()) {
            info->setRefno(op->getSerialno());
        }
        info->setFrom(getId());
        info->setTo(getId());
        info->setSeconds(BaseWorld::instance().getTimeAsSeconds());
        m_connection->send(info);
        m_connection->disconnect();
    }
}

const char* Account::getType() const
{
    return "account";
}

void Account::store() const
{
    if (isPersisted()) {
        Persistence::instance().putAccount(*this);
    }
}

bool Account::isPersisted() const
{
    return true;
}


void Account::addToMessage(MapType& omap) const
{
    omap["username"] = m_username;
    omap["name"] = m_username;
    if (!m_password.empty()) {
        omap["password"] = m_password;
    }
    omap["parent"] = getType();
    if (m_connection != nullptr) {
        BaseWorld& world = m_connection->m_server.m_world;

        ListType spawn_list;
        auto& spawnEntities = world.getSpawnEntities();
        for (auto& id: spawnEntities) {
            auto spawnEntity = world.getEntity(id);
            if (spawnEntity) {
                auto spawnProp = spawnEntity->getProperty("__spawn");
                if (spawnProp) {
                    Element elem;
                    spawnProp->get(elem);
                    spawn_list.emplace_back(elem);
                }
            }
        }
        if (!spawn_list.empty()) {
            omap["spawns"] = std::move(spawn_list);
        }
    }
    ListType char_list;
    auto I = m_charactersDict.begin();
    auto Iend = m_charactersDict.end();
    for (; I != Iend; ++I) {
        char_list.emplace_back(I->second->getId());
    }
    omap["characters"] = char_list;
    omap["objtype"] = "obj";
    omap["id"] = getId();
}

void Account::addToEntity(const Atlas::Objects::Entity::RootEntity& ent) const
{
    ent->setAttr("username", m_username);
    ent->setName(m_username);
    if (!m_password.empty()) {
        ent->setAttr("password", m_password);
    }
    ent->setParent(getType());
    if (m_connection) {
        BaseWorld& world = m_connection->m_server.m_world;
        ListType spawn_list;
        auto& spawnEntities = world.getSpawnEntities();
        for (auto& id: spawnEntities) {
            auto spawnEntity = world.getEntity(id);
            if (spawnEntity) {
                auto spawnProp = spawnEntity->getProperty("__spawn");
                if (spawnProp) {
                    Element elem;
                    spawnProp->get(elem);
                    elem.Map()["id"] = spawnEntity->getId();
                    spawn_list.emplace_back(elem);
                }
            }
        }
        if (!spawn_list.empty()) {
            ent->setAttr("spawns", spawn_list);
        }
    }
    ListType char_list;
    auto I = m_charactersDict.begin();
    auto Iend = m_charactersDict.end();
    for (; I != Iend; ++I) {
        char_list.emplace_back(I->second->getId());
    }
    ent->setAttr("characters", char_list);
    ent->setObjtype("obj");
    ent->setId(getId());
}

void Account::externalOperation(const Operation& op, Link& link)
{
    //External operations must come from a connection.
    assert(m_connection != nullptr);
    OpVector res;

    processExternalOperation(op, res);

    if (!res.empty()) {
        for (auto& replyOp : res) {
            if (!op->isDefaultSerialno()) {
                // Should we respect existing refnos?
                if (replyOp->isDefaultRefno()) {
                    replyOp->setRefno(op->getSerialno());
                }
            }
            replyOp->setSeconds(BaseWorld::instance().getTimeAsSeconds());
        }
        m_connection->send(res);
    }
}

void Account::processExternalOperation(const Operation& op, OpVector& res)
{
    auto op_no = op->getClassNo();
    switch (op_no) {
        case Atlas::Objects::Operation::CREATE_NO:
            CreateOperation(op, res);
            break;
        case Atlas::Objects::Operation::GET_NO:
            GetOperation(op, res);
            break;
        case Atlas::Objects::Operation::IMAGINARY_NO:
            ImaginaryOperation(op, res);
            break;
        case Atlas::Objects::Operation::LOGOUT_NO:
            LogoutOperation(op, res);
            break;
        case Atlas::Objects::Operation::LOOK_NO:
            LookOperation(op, res);
            break;
        case Atlas::Objects::Operation::SET_NO:
            SetOperation(op, res);
            break;
        case Atlas::Objects::Operation::TALK_NO:
            TalkOperation(op, res);
            break;
        case OP_INVALID:
            break;
        default:
            if (op_no == Atlas::Objects::Operation::POSSESS_NO) {
                PossessOperation(op, res);
            } else {
                OtherOperation(op, res);
            }
            break;
    }
}


void Account::operation(const Operation& op, OpVector& res)
{
    if (m_connection) {
        op->setSeconds(BaseWorld::instance().getTimeAsSeconds());
        m_connection->send(op);
    }
}

void Account::CreateOperation(const Operation& op, OpVector& res)
{
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        error(op, "No arguments.", res, getId());
        return;
    }

    auto& arg = args.front();
    if (arg->isDefaultId()) {
        error(op, "No id in first argument.", res, getId());
        return;
    }
    BaseWorld& world = m_connection->m_server.m_world;
    //Check that the destination is registered as a "spawn entity". Only those entities are allowed to receive Create ops from an Account.
    if (world.getSpawnEntities().find(arg->getId()) == world.getSpawnEntities().end()) {
        error(op, "Could not find spawn entity.", res, getId());
        return;
    }
    auto spawnEntity = world.getEntity(arg->getId());
    if (!spawnEntity) {
        error(op, "Could not find spawn entity.", res, getId());
        return;
    }

    //"__account" property is important, since when installed it will call back into this account, informing it of the new entity.
    arg->setAttr(AccountProperty::property_name, m_username);
    Create create;
    create->setTo(spawnEntity->getId());
    create->setArgs1(arg);
    spawnEntity->sendWorld(create);
}


void Account::SetOperation(const Operation& op, OpVector& res)
{
    debug_print("Account::Operation(set)")
    //Nothing to set on account.

}

void Account::ImaginaryOperation(const Operation& op, OpVector& res)
{
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        return;
    }

    Sight s;
    s->setArgs1(op);
    s->setFrom(getId());
    // FIXME Remove this - broadcasting
    if (!op->isDefaultSerialno()) {
        s->setRefno(op->getSerialno());
    }
    RootEntity arg = smart_dynamic_cast<RootEntity>(args.front());

    if (!arg.isValid()) {
        error(op, "Imaginary arg is malformed", res, getId());
        return;
    }

    if (arg->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG)) {
        s->setTo(arg->getLoc());
    } else {
        s->setTo(op->getTo());
    }
    if (m_connection != nullptr) {
        m_connection->m_server.getLobby().operation(s, res);
    }
}

void Account::TalkOperation(const Operation& op, OpVector& res)
{
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        error(op, "Talk has no args", res, getId());
        return;
    }

    RootEntity arg = smart_dynamic_cast<RootEntity>(args.front());

    if (!arg.isValid()) {
        error(op, "Talk arg is malformed", res, getId());
        return;
    }

    Sound s;
    s->setArgs1(op);
    s->setFrom(getId());
    // FIXME Remove this - broadcasting
    if (!op->isDefaultSerialno()) {
        s->setRefno(op->getSerialno());
    }

    if (arg->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG)) {
        s->setTo(arg->getLoc());
    } else {
        s->setTo(op->getTo());
    }
    if (m_connection != nullptr) {
        m_connection->m_server.getLobby().operation(s, res);
    }
}

void Account::PossessOperation(const Operation& op, OpVector& res)
{
    if (!m_connection) {
        return;
    }
    auto& args = op->getArgs();
    if (args.empty()) {
        clientError(op, "Empty args in possess op.", res, getId());
        return;
    }
    auto& arg = args.front();

    // FIXME In the possess case this ID isn't really required
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        error(op, "No target for look", res, getId());
        return;
    }
    const std::string& to = arg->getId();

    long intId = integerId(to);

    // Check for a possess key attached to the argument of the Look op. If
    // we have one, this is a request to transfer a character to this account.
    // Authenticate the requested character with the possess key found and if
    // successful, add the character to this account.
    Element key;
    if (arg->copyAttr("possess_key", key) == 0 && key.isString()) {
        const std::string& key_str = key.String();
        auto character = PossessionAuthenticator::instance().authenticatePossession(to, key_str);
        // FIXME Not finding the character should be fatal
        // FIXME TA needs to generate clientError ops for the client
        if (character) {
            // FIXME If we don't succeed in connecting, no need to carry on
            // and we probably need to indicate to the client
            if (connectCharacter(character.get(), res) == 0) {
                PossessionAuthenticator::instance().removePossession(to);
                logEvent(POSSESS_CHAR,
                         String::compose("%1 %2 %3 Claimed character (%4) "
                                         "by account %5",
                                         m_connection->getId(),
                                         getId(),
                                         character->getId(),
                                         character->getType(),
                                         m_username));
            }
        }
    } else {
        auto J = m_charactersDict.find(intId);
        if (J != m_charactersDict.end()) {
            connectCharacter(J->second, res);
            return;
        }
        clientError(op, String::compose("Could not find character '%1' to possess.", to), res, getId());
    }

}


void Account::LookOperation(const Operation& op, OpVector& res)
{
    if (m_connection == nullptr) {
        return;
    }
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        Sight s;
        s->setTo(getId());
        Anonymous sight_arg;
        m_connection->m_server.getLobby().addToEntity(sight_arg);
        s->setArgs1(sight_arg);
        res.push_back(s);
        return;
    }
    const Root& arg = args.front();
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        error(op, "No target for look", res, getId());
        return;
    }
    const std::string& to = arg->getId();

    long intId = integerId(to);

    auto J = m_charactersDict.find(intId);
    if (J != m_charactersDict.end()) {
        Sight s;
        s->setTo(getId());
        Anonymous sight_arg;
        J->second->addToEntity(sight_arg);
        s->setArgs1(sight_arg);
        res.push_back(s);
        return;
    }
    auto& accounts = m_connection->m_server.getLobby().getAccounts();
    auto K = accounts.find(to);
    if (K != accounts.end()) {
        Sight s;
        s->setTo(getId());
        Anonymous sight_arg;
        K->second->addToEntity(sight_arg);
        s->setArgs1(sight_arg);
        res.push_back(s);
        return;
    }
    error(op, String::compose("Unknown look target '%1'.", to), res, getId());
}

void Account::GetOperation(const Operation& op, OpVector& res)
{
}

void Account::OtherOperation(const Operation& op, OpVector& res)
{
    std::string parent = op->getParent().empty() ? "-" : op->getParent();
    error(op, String::compose("Unknown operation %1 in Account %2 (%3)", parent, getId(), m_username), res);
}

Account::~Account() = default;
