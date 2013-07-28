/*
 Copyright (C) 2013 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "MindClient.h"

#include "common/log.h"
#include "common/compose.hpp"
#include "common/debug.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>

using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

static const bool debug_flag = false;

MindClient::MindClient()
{

}

MindClient::~MindClient()
{
}

void MindClient::idle()
{
    handleNet();
}

void MindClient::takePossession(const std::string& possessEntityId, const std::string& possessKey)
{
    log(INFO, "Taking possession.");

    Anonymous what;
    what->setId(possessEntityId);
    what->setAttr("possess_key", possessKey);

    Look l;
    l->setFrom(m_playerId);
    l->setArgs1(what);
//    l->setSerialno(getNewSerialno());
    m_connection.send(l);

}

Root MindClient::login(const std::string& username, const std::string& password)
{
    m_username = username;
    m_password = password;

    Anonymous player_ent;
    player_ent->setAttr("username", username);
    player_ent->setAttr("password", password);
    player_ent->setParents(std::list<std::string>(1, "sys"));

    debug(std::cout << "Logging " << username << " in with " << password << " as password"
               << std::endl << std::flush;);

    Login loginAccountOp;
    loginAccountOp->setArgs1(player_ent);
    loginAccountOp->setSerialno(m_connection.newSerialNo());
    send(loginAccountOp);

    if (m_connection.wait() != 0) {
        std::cerr << "ERROR: Failed to log into server" << std::endl
                  << std::flush;
        return Root(0);
    }

    const Root & ent = m_connection.getInfoReply();

    if (!ent->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        std::cerr << "ERROR: Logged in, but account has no id" << std::endl
                  << std::flush;
    } else {
        m_playerId = ent->getId();
    }

    return ent;

}

void MindClient::operation(const Operation & op, OpVector & res)
{
    if (op->getClassNo() == Atlas::Objects::Operation::INFO_NO) {
        InfoOperation(op, res);
    } else {
        log(NOTICE, String::compose("Unknown operation %1 in MindClient",
                                        op->getParents().front()));
    }
}

void MindClient::InfoOperation(const Operation & op, OpVector & res)
{

}
