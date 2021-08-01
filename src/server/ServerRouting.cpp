// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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


#include "ServerRouting.h"

#include "Account.h"
#include "Lobby.h"
#include "Persistence.h"
#include "Connection.h"

#include "rules/simulation/BaseWorld.h"
#include "common/const.h"
#include "common/id.h"
#include "common/Monitors.h"
#include "common/Variable.h"
#include "common/globals.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/RootEntity.h>


using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Entity::RootEntity;

BOOL_OPTION(restricted_flag, false, CYPHESIS, "restricted",
            "Flag to control restricted mode");

/// \brief Constructor for server object.
///
/// Requires a reference to the World management object, as well as the
/// ruleset and server name. Implicitly creates the Lobby management object.
ServerRouting::ServerRouting(BaseWorld& wrld,
                             Persistence& persistence,
                             std::string ruleset,
                             std::string name,
                             long lobbyId) :
        m_svrRuleset(std::move(ruleset)),
        m_svrName(std::move(name)),
        m_lobby(new Lobby(*this, std::to_string(lobbyId), lobbyId)),
        m_numClients(0),
        m_world(wrld),
        m_persistence(persistence)
{
    Monitors& monitors = Monitors::instance();
    monitors.insert("server", "cyphesis");
    monitors.insert("instance", ::instance);
    monitors.insert("name", m_svrName);
    monitors.insert("ruleset", m_svrRuleset);
    monitors.insert("version", consts::version);
    monitors.insert("buildid", consts::buildId);
    monitors.watch("clients", std::make_unique<Variable<int>>(m_numClients));

}

/// Server destructor, implicitly destroys all OOG objects in the server.
ServerRouting::~ServerRouting()
{
    disconnectAllConnections();
}

void ServerRouting::disconnectAllConnections()
{
    for (auto connection: m_connections) {
        connection->disconnect();
    }
}


/// Add an OOG object to the server.
void ServerRouting::addObject(std::unique_ptr<ConnectableRouter> obj)
{
    assert(!obj->getId().empty());
    assert(integerId(obj->getId()) == obj->getIntId());
    assert(obj->getIntId() > 0);
    m_objects[obj->getIntId()] = std::move(obj);
}

/// Add an Account object to the server.
void ServerRouting::addAccount(std::unique_ptr<Account> a)
{
    m_accounts[a->username()] = a.get();
    a->store();
    addObject(std::move(a));
}

/// Remove an OOG object from the server.
void ServerRouting::delObject(ConnectableRouter* obj)
{
    m_objects.erase(obj->getIntId());
}

/// \brief Find an object with the given id.
///
/// @return a pointer to the object with the given id, or
/// zero if no object with this id is present.
ConnectableRouter* ServerRouting::getObject(const std::string& id) const
{
    auto I = m_objects.find(integerId(id));
    if (I == m_objects.end()) {
        return nullptr;
    } else {
        return I->second.get();
    }
}

/// \brief Find an account with a given username.
///
/// @return a pointer to the Account object with the given
/// username, or zero if the Account is not present. Does
/// not check any external authentication sources, or the
/// database.
Account* ServerRouting::getAccountByName(const std::string& username)
{
    auto I = m_accounts.find(username);
    if (I != m_accounts.end()) {
        return I->second;
    } else {
        auto account = m_persistence.getAccount(username);
        if (account) {
            auto J = m_accounts.emplace(username, account.get());
            addObject(std::move(account));
            return J.first->second;
        }
    }
    return nullptr;
}

void ServerRouting::addToMessage(MapType& omap) const
{
    omap["objtype"] = "obj";
    omap["server"] = "cyphesis";
    omap["ruleset"] = m_svrRuleset;
    omap["name"] = m_svrName;
    omap["parent"] = "server";
    omap["clients"] = m_numClients;
    omap["uptime"] = m_world.upTime();
    omap["buildid"] = consts::buildId;
    omap["version"] = std::string(consts::version);
    omap["protocol_version"] = consts::protocol_version;
    if (restricted_flag) {
        omap["restricted"] = "true";
    }
    omap["entities"] = (Atlas::Message::IntType) m_world.getEntities().size();
    omap["assets"] = Atlas::Message::ListType{"file://" + assets_directory};

    // We could add all sorts of stats here, but I don't know exactly what yet.
}

void ServerRouting::addToEntity(const RootEntity& ent) const
{
    ent->setObjtype("obj");
    ent->setAttr("server", "cyphesis");
    ent->setAttr("ruleset", m_svrRuleset);
    ent->setName(m_svrName);
    ent->setParent("server");
    ent->setAttr("clients", m_numClients);
    ent->setAttr("uptime", m_world.upTime());
    ent->setAttr("buildid", consts::buildId);
    ent->setAttr("version", std::string(consts::version));
    ent->setAttr("protocol_version", consts::protocol_version);
    if (restricted_flag) {
        ent->setAttr("restricted", "true");
    }
    ent->setAttr("entities", (Atlas::Message::IntType) m_world.getEntities().size());

    ent->setAttr("assets", Atlas::Message::ListType{"file://" + assets_directory});

    // We could add all sorts of stats here, but I don't know exactly what yet.
}

size_t ServerRouting::dispatch(size_t numberOfOps)
{
    size_t processed = 0;
    for (auto& entry: m_connections) {
        processed += entry->dispatch(numberOfOps);
    }
    return processed;
}

