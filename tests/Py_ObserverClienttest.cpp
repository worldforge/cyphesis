// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Python.h>

#include "python_testers.h"

#include "client/Python_ClientAPI.h"

#include "rulesets/Python_API.h"

#include <cassert>

int main()
{
    init_python_api();
    extend_client_python_api();

    run_python_string("import server");
    run_python_string("server.ObserverClient()");

    shutdown_python_api();
    return 0;
}

// stubs

#include "client/ObserverClient.h"
#include "client/CreatorClient.h"

#include <Atlas/Objects/Operation.h>

using Atlas::Objects::Entity::RootEntity;

LocatedEntity * CharacterClient::look(const std::string & id)
{
    return 0;
}

LocatedEntity * CharacterClient::lookFor(const RootEntity & ent)
{
    return 0;
}

LocatedEntity * CreatorClient::make(const RootEntity & entity)
{
    return 0;
}

void CreatorClient::sendSet(const std::string & id,
                            const RootEntity & entity)
{
}

void CreatorClient::del(const std::string & id)
{
}

void CharacterClient::send(const Operation & op)
{
}

ObserverClient::ObserverClient()
{
}

ObserverClient::~ObserverClient()
{
}

int ObserverClient::setup(const std::string & account,
                          const std::string & password,
                          const std::string & avatar)
{
    return 0;
}

void ObserverClient::idle()
{
}

BaseClient::BaseClient() : m_character(0)
{
}

BaseClient::~BaseClient()
{
}

Atlas::Objects::Root BaseClient::createSystemAccount()
{
    return Atlas::Objects::Operation::Info();
}

Atlas::Objects::Root BaseClient::createAccount(const std::string & name,
                                               const std::string & password)
{
    return Atlas::Objects::Operation::Info();
}

void BaseClient::send(const Operation & op)
{
}

CreatorClient * BaseClient::createCharacter(const std::string & type)
{
    return 0;
}

ClientConnection::ClientConnection()
{
}

ClientConnection::~ClientConnection()
{
}

int ClientConnection::wait()
{
    return 0;
}

int ClientConnection::sendAndWaitReply(const Operation & op, OpVector & res)
{
    return 0;
}

void ClientConnection::operation(const Operation & op)
{
}
