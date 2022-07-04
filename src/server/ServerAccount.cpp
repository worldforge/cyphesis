// Cyphesis Online RPG ServerAccount and AI Engine
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


#include "ServerAccount.h"

#include "ServerRouting.h"
#include "Connection.h"
#include "PossessionAuthenticator.h"

#include "rules/LocatedEntity.h"

#include "rules/simulation/BaseWorld.h"
#include "common/debug.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

using Atlas::Objects::Root;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

using Atlas::Objects::smart_dynamic_cast;

using String::compose;

static const bool debug_flag = false;

/// \brief ServerAccount constructor
ServerAccount::ServerAccount(Connection * conn,
             const std::string & username,
             const std::string & passwd,
             RouterId id) :
       Account(conn, username, passwd, std::move(id))
{
}


/// \brief Get the type of account ("server" in this case)
///
/// \return The type string
const char * ServerAccount::getType() const
{
    return "server";
}
