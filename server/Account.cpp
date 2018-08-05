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
#include "rulesets/ExternalMind.h"
#include "Persistence.h"
#include "PossessionAuthenticator.h"

#include "rulesets/Character.h"

#include "rulesets/BaseWorld.h"
#include "common/id.h"
#include "common/debug.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <sigc++/adaptors/bind.h>

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
Account::Account(Connection * conn,
                 const std::string & uname,
                 const std::string & passwd,
                 const std::string & id,
                 long intId) :
         ConnectableRouter(id, intId, conn),
         m_username(uname), m_password(passwd)
{
}

/// \brief Called when the Character has been removed from the world.
///
/// @param id Integer identifier of the Character destroyed.
void Account::characterDestroyed(long id)
{
    m_charactersDict.erase(id);
    if (isPersisted()) {
        Persistence::instance().delCharacter(String::compose("%1", id));
    }
}

/// \brief Connect an existing character to this account
///
/// \brief chr The character to connect to this account
/// \return Returns 0 on success and -1 on failure.
int Account::connectCharacter(LocatedEntity *chr)
{
    Character * character = dynamic_cast<Character *>(chr);
    if (character) {
        if (character->linkExternal(m_connection) != 0) {
            log(WARNING, String::compose("Account %1 (%2) could not take character %3 as it "
                "already is connected to an external mind with id %4.",
                                         getId(), m_username, chr->getId(), character->m_externalMind->getLink()));
            return -2;
        }

        // Only genuinely playable characters should go in here. Otherwise
        // if a normal entity gets into the account, and connection, it
        // starts getting hard to tell whether or not they exist.
        m_charactersDict[chr->getIntId()] = chr;
        chr->destroyed.connect(sigc::bind(sigc::mem_fun(this, &Account::characterDestroyed), chr->getIntId()));
        m_connection->addEntity(chr);
        if (isPersisted()) {
            Persistence::instance().addCharacter(*this, *chr);
        }
        return 0;
    }
    return -1;
}

/// \brief Add a Character to those that belong to this Account
///
/// @param chr Character object to be adddded
void Account::addCharacter(LocatedEntity * chr)
{
    Character * pchar = dynamic_cast<Character *>(chr);
    if (pchar == nullptr) {
        return;
    }
    m_charactersDict[chr->getIntId()] = chr;
    chr->destroyed.connect(sigc::bind(sigc::mem_fun(this, &Account::characterDestroyed), chr->getIntId()));
}

/// \brief Create a new Character and add it to this Account
///
/// @param typestr The type name of the Character to be created
/// @param ent Atlas description of the Character to be created
Ref<LocatedEntity> Account::addNewCharacter(const std::string & typestr,
                                         const RootEntity & ent,
                                         const Root & arg)
{
    if (m_connection == nullptr) {
        return nullptr;
    }
    //Any entity created as a character should have it's "mind" property disabled; i.e. we don't want AI to control this character.
    ent->setAttr("mind", Atlas::Message::Element());
    debug(std::cout << "Account::Add_character" << std::endl << std::flush;);
    auto chr = createCharacterEntity(typestr, ent, arg);
    if (!chr) {
        return nullptr;
    }
    debug(std::cout << "Added" << std::endl << std::flush;);
    assert(chr->m_location.isValid());
    debug(std::cout << "Location set to: " << chr->m_location << std::endl << std::flush;);
    connectCharacter(chr.get());

    logEvent(TAKE_CHAR, String::compose("%1 %2 %3 Created character (%4) "
                                        "by account %5",
                                        m_connection->getId(),
                                        getId(),
                                        chr->getId(),
                                        chr->getType(),
                                        m_username));

    return chr;
}

Ref<LocatedEntity> Account::createCharacterEntity(const std::string & typestr,
                                                const RootEntity & ent,
                                                const Root & arg)
{
    BaseWorld & world = m_connection->m_server.m_world;
    Element spawn;
    if (arg->copyAttr("spawn_name", spawn) == 0 && spawn.isString()) {
        return world.spawnNewEntity(spawn.String(), typestr, ent);
    } else {
        return world.addNewEntity(typestr, ent);
    }
}


void Account::LogoutOperation(const Operation & op, OpVector & res)
{
    if (m_connection == nullptr) {
        log(ERROR, String::compose("Account::LogoutOperation on account %1 (%2) that doesn't seem to "
                   "be connected.", getId(), m_username));
        return;
    }

    Info info;
    info->setArgs1(op);
    if (!op->isDefaultSerialno()) {
        info->setRefno(op->getSerialno());
    }
    info->setFrom(getId());
    info->setTo(getId());
    m_connection->send(info);
    m_connection->disconnect();
}

const char * Account::getType() const
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


void Account::addToMessage(MapType & omap) const
{
    omap["username"] = m_username;
    omap["name"] = m_username;
    if (!m_password.empty()) {
        omap["password"] = m_password;
    }
    omap["parent"] = getType();
    if (m_connection != nullptr) {
        BaseWorld & world = m_connection->m_server.m_world;
        ListType spawn_list;
        if (world.getSpawnList(spawn_list) == 0) {
            //We should only send those spawn areas which allows for characters to be created.
            for (auto I = spawn_list.begin(); I != spawn_list.end();) {
                if((*I).isMap() && (*I).asMap().count("character_types") == 0) {
                    I = spawn_list.erase(I);
                }
                else
                {
                    ++I;
                }
            }
            if (!spawn_list.empty()) {
                omap["spawns"] = spawn_list;
            }
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

void Account::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
    ent->setAttr("username", m_username);
    ent->setName(m_username);
    if (!m_password.empty()) {
        ent->setAttr("password", m_password);
    }
    ent->setParent(getType());
    if (m_connection != 0) {
        BaseWorld & world = m_connection->m_server.m_world;
        ListType spawn_list;
        if (world.getSpawnList(spawn_list) == 0) {
            //We should only send those spawn areas which allows for characters to be created.
            for (auto I = spawn_list.begin(); I != spawn_list.end();) {
                if((*I).isMap() && (*I).asMap().count("character_types") == 0) {
                    I = spawn_list.erase(I);
                }
                else
                {
                    ++I;
                }
            }
            if (!spawn_list.empty()) {
                ent->setAttr("spawns", spawn_list);
            }
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

void Account::externalOperation(const Operation & op, Link &)
{
    //External operations must come from a connection.
    assert(m_connection != nullptr);
    OpVector res;

    processExternalOperation(op, res);

    if (!res.empty()) {
        for(auto& replyOp : res) {
            if (!op->isDefaultSerialno()) {
                // Should we respect existing refnos?
                if (replyOp->isDefaultRefno()) {
                    long serialno = op->getSerialno();
                    replyOp->setRefno(serialno);
                }
            }
        }
        // FIXME detect socket failure here
        m_connection->send(res);
    }
}

void Account::processExternalOperation(const Operation & op, OpVector& res) {
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
            OtherOperation(op, res);
            break;
    }
}


void Account::operation(const Operation & op, OpVector & res)
{
    if (m_connection) {
        m_connection->send(op);
    }
}

void Account::CreateOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        return;
    }

    const Root & arg = args.front();
    if (!arg->hasAttrFlag(Atlas::Objects::PARENT_FLAG)) {
        error(op, "Object to be created has no type", res, getId());
        return;
    }
    const std::string & type_str = arg->getParent();

    createObject(type_str, arg, op, res);
}

void Account::createObject(const std::string & type_str,
                           const Root & arg,
                           const Operation & op,
                           OpVector & res)
{
    if (characterError(op, arg, res) != 0) {
        return;
    }

    debug( std::cout << "Account creating a " << type_str << " object"
                     << std::endl << std::flush; );

    Anonymous new_character;
    new_character->setParent(type_str);
    //Disable the AI mind since this will be controlled by a client.
    new_character->setAttr("mind", MapType());
    if (!arg->isDefaultName()) {
        new_character->setName(arg->getName());
    }
    Atlas::Message::Element sexElement;
    if (arg->copyAttr("sex", sexElement) == 0 && sexElement.isString()) {
        if (sexElement.String() == "male" || sexElement.String() == "female") {
            new_character->setAttr("sex", sexElement);
        }
    }

    auto entity = addNewCharacter(type_str, new_character, arg);

    if (entity == nullptr) {
        error(op, "Character creation failed", res, getId());
        return;
    }

    auto character = dynamic_cast<Character *>(entity.get());
    if (character != nullptr) {
        // Inform the client that it has successfully subscribed
        Info info;
        Anonymous info_arg;
        entity->addToEntity(info_arg);
        info->setArgs1(info_arg);
        res.push_back(info);
    }

    // Inform the client of the newly created character
    Sight sight;
    sight->setTo(getId());
    Anonymous sight_arg;
    addToEntity(sight_arg);
    sight->setArgs1(sight_arg);
    res.push_back(sight);
}

int Account::filterTasks(const ListType & tasks,
                         const RootEntity & filtered_arg) const
{
    ListType filtered_tasks;
    auto I = tasks.begin();
    auto Iend = tasks.end();
    for (; I != Iend; ++I) {
        if (!I->isMap()) {
            return -1;
        }
        const MapType & task = I->asMap();
        MapType filtered_task;
        auto J = task.find("name");
        auto Jend = task.end();
        if (J == Jend || !J->second.isString()) {
            log(ERROR, "Task has no name");
            return -1;
        }
        const std::string & task_name = J->second.asString();
        filtered_task["name"] = task_name;
        // FIXME Use the typeinfo to check which attributes can be changed.
        for (J = task.begin(); J != Jend; ++J) {
            if (J->first == "name") {
                continue;
            }
            filtered_task[J->first] = J->second;
        }
        filtered_tasks.emplace_back(filtered_task);
    }
    filtered_arg->setAttr("tasks", filtered_tasks);
    return 0;
}

void Account::SetOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Account::Operation(set)" << std::endl << std::flush;);
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        return;
    }

    const Root & arg = args.front();

    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        error(op, "Set character has no ID", res, getId());
        return;
    }

    const std::string & id = arg->getId();

    long intId = integerId(id);

    EntityDict::const_iterator J = m_charactersDict.find(intId);
    if (J == m_charactersDict.end()) {
        // Client has sent a Set op for an object that is not
        // one of our characters.
        return error(op, "Permission denied.", res, getId());
    }

    LocatedEntity * e = J->second;
    Anonymous new_arg;
    bool argument_valid = false;
    Element guise;
    if (arg->copyAttr("guise", guise) == 0) {
        debug(std::cout << "Got attempt to change characters guise"
                        << std::endl << std::flush;);
        // Apply change to character in-game
        new_arg->setAttr("guise", guise);
        argument_valid = true;
    }
    Element height;
    if (arg->copyAttr("height", height) == 0 && (height.isNum())) {
        debug(std::cout << "Got attempt to change characters height"
                        << std::endl << std::flush;);
        const BBox & bbox = e->m_location.bBox();
        if (bbox.isValid()) {
            float old_height = bbox.highCorner().y() - bbox.lowCorner().y();
            float scale = height.asNum() / old_height;
            BBox newBox(WFMath::Point<3>(bbox.lowCorner().x() * scale,
                                         bbox.lowCorner().y() * scale,
                                         bbox.lowCorner().z() * scale),
                        WFMath::Point<3>(bbox.highCorner().x() * scale,
                                         bbox.highCorner().y() * scale,
                                         bbox.highCorner().z() * scale));
            new_arg->setAttr("bbox", newBox.toAtlas());
            argument_valid = true;
        }
    }
    Element tasks;
    if (arg->copyAttr("tasks", tasks) == 0 && (tasks.isList())) {
        log(NOTICE, "Got as yet unsupported task modification from client");
        if (filterTasks(tasks.asList(), new_arg) == 0) {
            argument_valid = true;
        }
    }

    if (argument_valid) {
        debug(std::cout << "Passing character mods in-game"
                        << std::endl << std::flush;);
        Set s;
        s->setTo(id);
        new_arg->setId(id);
        s->setArgs1(new_arg);
        if (m_connection != 0) {
            m_connection->m_server.m_world.message(s, *e);
        }
    }
}

void Account::ImaginaryOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
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
        m_connection->m_server.m_lobby.operation(s, res);
    }
}

void Account::TalkOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
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
        m_connection->m_server.m_lobby.operation(s, res);
    }
}

void Account::LookOperation(const Operation & op, OpVector & res)
{
    if (m_connection == nullptr) {
        return;
    }
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        Sight s;
        s->setTo(getId());
        Anonymous sight_arg;
        m_connection->m_server.m_lobby.addToEntity(sight_arg);
        s->setArgs1(sight_arg);
        res.push_back(s);
        return;
    }
    // FIXME In the possess case this ID isn't really required
    const Root & arg = args.front();
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        error(op, "No target for look", res, getId());
        return;
    }
    const std::string & to = arg->getId();

    long intId = integerId(to);

    // Check for a possess key attached to the argument of the Look op. If 
    // we have one, this is a request to transfer a character to this account.
    // Authenticate the requested character with the possess key found and if
    // successful, add the character to this account.
    Element key;
    if (arg->copyAttr("possess_key", key) == 0 && key.isString()) {
        const std::string & key_str = key.String();
        auto character = PossessionAuthenticator::instance().authenticatePossession(to, key_str);
        // FIXME Not finding the character should be fatal
        // FIXME TA needs to generate clientError ops for the client
        if (character) {
            // FIXME If we don't succeed in connecting, no need to carry on
            // and we probably need to indicate to the client
            if (connectCharacter(character.get()) == 0) {
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
    }

    // FIXME Avoid this lookup if we just took possession of a character
    EntityDict::const_iterator J = m_charactersDict.find(intId);
    if (J != m_charactersDict.end()) {
        Sight s;
        s->setTo(getId());
        Anonymous sight_arg;
        J->second->addToEntity(sight_arg);
        s->setArgs1(sight_arg);
        res.push_back(s);
        return;
    }
    const AccountDict & accounts = m_connection->m_server.m_lobby.getAccounts();
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

void Account::GetOperation(const Operation & op, OpVector & res)
{
}

void Account::OtherOperation(const Operation & op, OpVector & res)
{
    std::string parent = op->getParent().empty() ? "-" : op->getParent();
    error(op, String::compose("Unknown operation %1 in Account %2 (%3)", parent, getId(), m_username), res);
}
