// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2006 Alistair Riddoch
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


#include "Admin.h"

#include "rules/simulation/AdminMind.h"
#include "ServerRouting.h"
#include "Connection.h"
#include "Ruleset.h"
#include "Juncture.h"


#include "rules/LocatedEntity.h"
#include "common/TypeNode.h"

#include "rules/simulation/BaseWorld.h"
#include "common/id.h"
#include "common/Inheritance.h"

#include "common/operations/Monitor.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

using Atlas::Objects::Root;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

using String::compose;

static const bool debug_flag = false;

/// \brief Admin constructor
Admin::Admin(Connection * conn,
             const std::string & username,
             const std::string & passwd,
             RouterId id) :
       Account(conn, username, passwd, std::move(id))
{
}

Admin::~Admin()
{
    if (m_monitorConnection.connected()) {
        m_monitorConnection.disconnect();
    }
}

const char * Admin::getType() const
{
    return "admin";
}

std::unique_ptr<ExternalMind> Admin::createMind(const Ref<LocatedEntity>& entity) const {
    auto id = newId();

    return std::make_unique<AdminMind>(id, entity);
}

/// \brief Function to monitor server operations
///
/// This function is connected to the WorldRouter operation dispatch
/// signal when monitoring is enabled, and relays all in-game operations
/// to the client.
void Admin::opDispatched(const Operation& op)
{
    if (m_connection != nullptr) {
        m_connection->send(op);
    } else {
        if (m_monitorConnection.connected()) {
            m_monitorConnection.disconnect();
        }
    }
}

void Admin::LogoutOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();

    //Check if arg points to another account, and log out that if so.
    if (!args.empty()) {
        const Root & arg = args.front();
        if (arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            const std::string & account_id = arg->getId();

            if (account_id != getId() && m_connection != nullptr) {
                Router * account = m_connection->m_server.getObject(account_id);
                if (account) {
                    log(INFO, String::compose("Admin account %1 is forcefully logging out account %2.", getId(), account_id));
                    account->operation(op, res);
                    return;
                }
            }
        } else {
            error(op, "No id given on logout op", res, getId());
            return;
        }
    }

    Account::LogoutOperation(op, res);
}

void Admin::GetOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        error(op, "Get has no args.", res, getId());
        return;
    }
    const Root & arg = args.front();
    if (!arg->hasAttrFlag(Atlas::Objects::OBJTYPE_FLAG)) {
        error(op, "Get arg has no objtype.", res, getId());
        return;
    }
    const std::string & objtype = arg->getObjtype();
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        error(op, "Get arg has no id.", res, getId());
        return;
    }
    const std::string & id = arg->getId();
    if (id.empty()) {
        error(op, "Get arg id empty", res, getId());
        return;
    }
    Info info;
    if (objtype == "object" || objtype == "obj") {
        if (!m_connection) {
            return;
        }
        long intId = integerId(id);

        const auto& OOGDict = m_connection->m_server.getObjects();
        auto J = OOGDict.find(intId);
        auto& worldDict = m_connection->m_server.m_world.getEntities();
        auto K = worldDict.find(intId);

        if (J != OOGDict.end()) {
            auto& obj = J->second;
            Anonymous info_arg;
            obj->addToEntity(info_arg);
            info->setArgs1(info_arg);
        } else if (K != worldDict.end()) {
            Anonymous info_arg;
            auto& entity = K->second;
            //If the "archive" flag is set the clients wants a format where it can later restore the entity.
            //In that case we need to take any modifiers into account.
            if (arg->hasAttr("archive")) {
                for (auto& entry : entity->getProperties()) {
                    //Don't send any property that's ephemeral, except for "id" and "contains" which are marked such.
                    //TODO: remove this flag from those properties and have the StorageManager look specifically for them instead. They are not ephemeral, they are just stored differently by the StorageManager.
                    if (!entry.second.property->hasFlags(prop_flag_persistence_ephem) || entry.first == "id" || entry.first == "contains") {
                        if (!entry.second.modifiers.empty()) {
                            //If the base value is none, only include it if there's a default value
                            if (!entry.second.baseValue.isNone() || (!entity->getType() || entity->getType()->defaults().find(entry.first) != entity->getType()->defaults().end())) {
                                info_arg->setAttr(entry.first, entry.second.baseValue);
                            }
                        } else {
                            entry.second.property->add(entry.first, info_arg);
                        }
                    }
                }

                info_arg->setStamp(entity->getSeq());
                if (entity->getType()) {
                    info_arg->setParent(entity->getType()->name());
                }
                info_arg->setObjtype("obj");
            } else {
                entity->addToEntity(info_arg);
            }

            info->setArgs1(info_arg);
        } else {
            clientError(op, compose("Unknown object id \"%1\" requested", id),
                        res, getId());
            return;
        }
    } else if (objtype == "class" ||
               objtype == "meta" ||
               objtype == "op_definition") {
        const Root & o = Inheritance::instance().getClass(id, Visibility::PRIVATE);
        if (!o.isValid()) {
            clientError(op, compose("Unknown type definition for \"%1\" "
                                    "requested", id), res);
            return;
        }
        info->setArgs1(o);
    } else {
        error(op, compose(R"(Unknown object type "%1" requested for "%2")",
                          objtype, id), res, getId());
        return;
    }
    res.push_back(info);
}

void Admin::SetOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        error(op, "Set has no args.", res, getId());
        return;
    }
    const Root & arg = args.front();
    if (!arg->hasAttrFlag(Atlas::Objects::OBJTYPE_FLAG)) {
        error(op, "Set arg has no objtype.", res, getId());
        return;
    }
    const std::string & objtype = arg->getObjtype();
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        error(op, "Set arg has no id.", res, getId());
        return;
    }
    const std::string & id = arg->getId();

    if (objtype == "object" || objtype == "obj") {

        long intId = integerId(id);

        if (intId == getIntId()) {
            setAttribute(arg);
        } else {

            if (m_charactersDict.find(intId) != m_charactersDict.end()) {
                Account::SetOperation(op, res);
                return;
            }
            log(WARNING, "Unable to set attributes of non-character yet");
        }
        // Manipulate attributes of existing objects.
    } else if (objtype == "class" || objtype == "op_definition") {
        if (Inheritance::instance().hasClass(id)) {
            if (Ruleset::instance().modifyRule(id, arg) == 0) {
                Info info;
                info->setTo(getId());
                info->setArgs1(arg);
                res.push_back(info);
            } else {
                error(op, "Updating type failed", res, getId());
            }
            return;
        }
        error(op, "Client attempting to use obsolete Set to install new type",
              res, getId());
        return;
    } else {
        error(op, "Unknown object type set", res, getId());
        return;
    }
}


void Admin::CreateOperation(const Operation& op, OpVector& res)
{
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        return;
    }

    auto& arg = args.front();
    auto& type_str = arg->getParent();
    const std::string & objtype = arg->getObjtype();
    if (objtype == "class" || objtype == "op_definition") {
        // New entity type
        if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            error(op, "Set arg has no id.", res, getId());
            return;
        }
        const std::string & id = arg->getId();

        if (Inheritance::instance().hasClass(id)) {
            error(op, "Attempt to install type that already exists", res,
                  getId());
            return;
        }
        const Root & o = Inheritance::instance().getClass(type_str, Visibility::PRIVATE);
        if (!o.isValid()) {
            error(op, compose("Attempt to install type with non-existent "
                              "parent \"%1\"", type_str), res, getId());
            return;
        }
        if (Ruleset::instance().installRule(id, "unknown", arg) == 0) {
            Info info;
            info->setTo(getId());
            info->setArgs1(arg);
            res.push_back(info);
        } else {
            error(op, "Installing new type failed", res, getId());
        }
    } else if (type_str == "juncture") {
        auto junc_id = newId();
        if (junc_id.m_intId < 0) {
            error(op, "Juncture failed as no ID available", res, getId());
            return;
        }

        auto juncture = std::make_unique<Juncture>(m_connection, junc_id);

        Anonymous info_arg;
        juncture->addToEntity(info_arg);

        m_connection->addConnectableRouter(juncture.get());
        m_connection->m_server.addRouter(std::move(juncture));



        Info info;
        info->setTo(getId());
        info->setArgs1(info_arg);
        if (!op->isDefaultSerialno()) {
            info->setRefno(op->getSerialno());
        }
        res.push_back(info);
    } else {
        Account::CreateOperation(op, res);
    }
}

void Admin::OtherOperation(const Operation & op, OpVector & res)
{
    const int op_type = op->getClassNo();
    if (op_type == Atlas::Objects::Operation::MONITOR_NO) {
        customMonitorOperation(op, res);
    }
}

void Admin::setAttribute(const Root& args) {
    //If the attribute "possessive" is set on the account it will control whether this
    //account acts as an external minds connection.
    if (args->hasAttr("possessive")) {
        const Element possessiveElement = args->getAttr("possessive");
        m_connection->setPossessionEnabled(possessiveElement.isInt() && possessiveElement.asInt() != 0, getId());
    }
}


/// \brief Process a Monitor operation
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void Admin::customMonitorOperation(const Operation & op, OpVector & res)
{
    if (!op->getArgs().empty()) {
        if (m_connection != nullptr) {
            if (!m_monitorConnection.connected()) {
                m_monitorConnection = m_connection->m_server.m_world.Dispatching.connect(sigc::mem_fun(*this, &Admin::opDispatched));
            }
        }
    } else {
        if (m_monitorConnection.connected()) {
            m_monitorConnection.disconnect();
        }
    }
}

void Admin::processExternalOperation(const Operation& op, OpVector& res)
{
    //Allow admin accounts to send operations directly to other entities.
    if (!op->isDefaultTo() && op->getTo() != getId()) {
        auto entity = m_connection->m_server.getWorld().getEntity(op->getTo());
        if (entity) {
            entity->operation(op, res);
        }
        return;
    } else {
        Account::processExternalOperation(op, res);
    }
}
