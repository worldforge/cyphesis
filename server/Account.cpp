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

// $Id$

#include "Account.h"

#include "Connection_methods.h"
#include "ServerRouting.h"
#include "Lobby.h"
#include "ExternalMind.h"
#include "Persistence.h"
#include "TeleportAuthenticator.h"

#include "rulesets/Character.h"

#include "common/id.h"
#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/serialno.h"
#include "common/Inheritance.h"
#include "common/compose.hpp"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <sigc++/adaptors/bind.h>
#include <sigc++/functors/mem_fun.h>

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
         Router(id, intId),
         m_username(uname), m_password(passwd), m_connection(conn)
{
}

Account::~Account()
{
}

/// \brief Called when the Character has been removed from the world.
///
/// @param id Integer identifier of the Character destroyed.
void Account::characterDestroyed(long id)
{
    m_charactersDict.erase(id);
    if (consts::enable_persistence) {
        Persistence::instance()->delCharacter(String::compose("%1", id));
    }
}

/// \brief Connect an existing character to this account
///
/// \brief chr The character to connect to this account
/// \return Returns 0 on success and -1 on failure.
int Account::connectCharacter(Entity *chr)
{
    Character * character = dynamic_cast<Character *>(chr);
    if (character != 0) {
        m_connection->connectAvatar(character);
        // Only genuinely playable characters should go in here. Otherwise
        // if a normal entity gets into the account, and connection, it
        // starts getting hard to tell whether or not they exist.
        m_charactersDict[chr->getIntId()] = chr;
        chr->destroyed.connect(sigc::bind(sigc::mem_fun(this, &Account::characterDestroyed), chr->getIntId()));
        m_connection->addEntity(chr);
        if (consts::enable_persistence) {
            Persistence::instance()->addCharacter(*this, *chr);
        }
        return 0;
    }
    return -1;
}

/// \brief Add a Character to those that belong to this Account
///
/// @param chr Character object to be adddded
void Account::addCharacter(Entity * chr)
{
    Character * pchar = dynamic_cast<Character *>(chr);
    if (pchar == 0) {
        return;
    }
    m_charactersDict[chr->getIntId()] = chr;
    chr->destroyed.connect(sigc::bind(sigc::mem_fun(this, &Account::characterDestroyed), chr->getIntId()));
}

/// \brief Create a new Character and add it to this Account
///
/// @param typestr The type name of the Character to be created
/// @param ent Atlas description of the Character to be created
Entity * Account::addNewCharacter(const std::string & typestr,
                                  const RootEntity & ent,
                                  const RootEntity & arg)
{
    if (m_connection == 0) {
        return 0;
    }
    BaseWorld & world = m_connection->m_server.m_world;
    debug(std::cout << "Account::Add_character" << std::endl << std::flush;);
    Entity * chr;
    Element spawn;
    if (arg->copyAttr("spawn_name", spawn) == 0 && spawn.isString()) {
        chr = world.spawnNewEntity(spawn.String(), typestr, ent);
    } else {
        chr = world.addNewEntity(typestr, ent);
    }
    if (chr == 0) {
        return 0;
    }
    debug(std::cout << "Added" << std::endl << std::flush;);
    assert(chr->m_location.isValid());
    debug(std::cout << "Location set to: " << chr->m_location << std::endl << std::flush;);
    Character * character = dynamic_cast<Character *>(chr);
    if (character != 0) {
        m_connection->connectAvatar(character);
        // Only genuinely playable characters should go in here. Otherwise
        // if a normal entity gets into the account, and connection, it
        // starts getting hard to tell whether or not they exist.
        m_charactersDict[chr->getIntId()] = chr;
        chr->destroyed.connect(sigc::bind(sigc::mem_fun(this, &Account::characterDestroyed), chr->getIntId()));
        m_connection->addEntity(chr);
        if (consts::enable_persistence) {
            Persistence::instance()->addCharacter(*this, *chr);
        }
    }

    logEvent(TAKE_CHAR, String::compose("%1 %2 %3 Created character (%4) "
                                        "by account %5",
                                        m_connection->getId(),
                                        getId(),
                                        chr->getId(),
                                        chr->getType(),
                                        m_username));

    return chr;
}

void Account::LogoutOperation(const Operation & op, OpVector & res)
{
    if (m_connection == 0) {
        log(ERROR, "Account::LogoutOperation on account that doesn't seem to "
                   "be connected.");
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
    if (database_flag) {
        Persistence::instance()->putAccount(*this);
    }
}

void Account::addToMessage(MapType & omap) const
{
    omap["username"] = m_username;
    omap["name"] = m_username;
    if (!m_password.empty()) {
        omap["password"] = m_password;
    }
    omap["parents"] = ListType(1,getType());
    if (m_connection != 0) {
        BaseWorld & world = m_connection->m_server.m_world;
        ListType spawn_list;
        if (world.getSpawnList(spawn_list) == 0) {
            omap["spawns"] = spawn_list;
        }
    }
    ListType char_list;
    EntityDict::const_iterator I = m_charactersDict.begin();
    EntityDict::const_iterator Iend = m_charactersDict.end();
    for (; I != Iend; ++I) {
        char_list.push_back(I->first);
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
    ent->setParents(std::list<std::string>(1,getType()));
    if (m_connection != 0) {
        BaseWorld & world = m_connection->m_server.m_world;
        ListType spawn_list;
        if (world.getSpawnList(spawn_list) == 0) {
            ent->setAttr("spawns", spawn_list);
        }
    }
    ListType char_list;
    EntityDict::const_iterator I = m_charactersDict.begin();
    EntityDict::const_iterator Iend = m_charactersDict.end();
    for (; I != Iend; ++I) {
        char_list.push_back(I->second->getId());
    }
    ent->setAttr("characters", char_list);
    ent->setObjtype("obj");
    ent->setId(getId());
}

void Account::operation(const Operation & op, OpVector & res)
{
    const OpNo op_no = op->getClassNo();
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

void Account::CreateOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Account::Operation(create)" << std::endl << std::flush;);
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        return;
    }

    RootEntity arg = smart_dynamic_cast<RootEntity>(args.front());

    if (!arg.isValid()) {
        error(op, "Character creation arg is malformed", res, getId());
        return;
    }

    if (!arg->hasAttrFlag(Atlas::Objects::PARENTS_FLAG)) {
        error(op, "Entity has no type", res, getId());
        return;
    }

    const std::list<std::string> & parents = arg->getParents();
    if (parents.empty()) {
        error(op, "Entity has empty type list.", res, getId());
        return;
    }
    
    const std::string & typestr = parents.front();

    if (characterError(op, arg, res)) {
        return;
    }

    debug( std::cout << "Account creating a " << typestr << " object"
                     << std::endl << std::flush; );

    Anonymous new_character;
    new_character->setParents(std::list<std::string>(1, typestr));
    new_character->setAttr("status", 0.024);
    new_character->setAttr("mind", "");
    if (!arg->isDefaultName()) {
        new_character->setName(arg->getName());
    }

    Entity * entity = addNewCharacter(typestr, new_character, arg);

    if (entity == 0) {
        error(op, "Character creation failed", res, getId());
        return;
    }

    Character * character = dynamic_cast<Character *>(entity);
    if (character != 0) {
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
    ListType::const_iterator I = tasks.begin();
    ListType::const_iterator Iend = tasks.end();
    for (; I != Iend; ++I) {
        if (!I->isMap()) {
            return -1;
        }
        const MapType & task = I->asMap();
        MapType filtered_task;
        MapType::const_iterator J = task.find("name");
        MapType::const_iterator Jend = task.end();
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
        filtered_tasks.push_back(filtered_task);
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

    Entity * e = J->second;
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
            float old_height = bbox.highCorner().z() - bbox.lowCorner().z();
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
    if (m_connection != 0) {
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
    if (m_connection != 0) {
        m_connection->m_server.m_lobby.operation(s, res);
    }
}

void Account::LookOperation(const Operation & op, OpVector & res)
{
    if (m_connection == 0) {
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
        Entity *character;
        character = TeleportAuthenticator::instance()->authenticateTeleport(to, key_str);
        if (character) {
            if (connectCharacter(character) == 0) {
                TeleportAuthenticator::instance()->removeTeleport(to);
            }
        }
    }

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
    AccountDict::const_iterator K = accounts.find(to);
    if (K != accounts.end()) {
        Sight s;
        s->setTo(getId());
        Anonymous sight_arg;
        K->second->addToEntity(sight_arg);
        s->setArgs1(sight_arg);
        res.push_back(s);
        return;
    }
    error(op, "Unknown look target", res, getId());
}

void Account::GetOperation(const Operation & op, OpVector & res)
{
}

void Account::OtherOperation(const Operation & op, OpVector & res)
{
    error(op, "Unknown operation", res);
}
