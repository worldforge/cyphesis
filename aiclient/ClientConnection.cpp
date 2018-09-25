// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001 Alistair Riddoch
// Copyright (C) 2013 Erik Ogenvik
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


#include "ClientConnection.h"

#include "common/debug.h"

#include <Atlas/Codec.h>
#include <Atlas/Objects/Anonymous.h>

#include <Atlas/Message/QueuedDecoder.h>
#include <Atlas/Objects/Encoder.h>

#include <iostream>
#include <Atlas/Codecs/Bach.h>
#include <Atlas/PresentationBridge.h>

using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::RootOperation;

static bool debug_flag = false;


ClientConnection::ClientConnection(CommSocket & commSocket, const std::string & id, long iid)
    : Link(commSocket, id, iid)
{

}
