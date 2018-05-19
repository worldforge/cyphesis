// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Alistair Riddoch
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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "server/Persistence.h"
#include "DatabaseNull.h"

#include <Atlas/Message/Element.h>

#include <cassert>
#include <common/Database.h>

using Atlas::Message::MapType;
using Atlas::Objects::Root;

int main()
{
    {
        DatabaseNull database;
        Persistence p(database);
    }

    {
        DatabaseNull database;
        Persistence p(database);
        int res = p.init();
        assert(res == 0);
    }

    return 0;
}

// stubs

#include "server/Admin.h"
#include "server/Player.h"
#include "server/ServerAccount.h"

#include "common/Database.h"
#include "common/globals.h"
#include "common/log.h"
#include "common/id.h"
#include "common/Shaker.h"


#define STUB_Database_selectSimpleRowBy
DatabaseResult Database::selectSimpleRowBy(const std::string& name,
                                           const std::string& column,
                                           const std::string& value)
{
    return DatabaseResult(std::unique_ptr<DatabaseNullResultWorker>(new DatabaseNullResultWorker()));
}


#include "stubs/common/stubDatabase.h"

const char * const CYPHESIS = "cyphesis";

std::string instance("deeds");

Router::Router(const std::string & id, long intId) : m_id(id), m_intId(intId)
{
}

Router::~Router()
{
}

void Router::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Router::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

ConnectableRouter::ConnectableRouter(const std::string & id,
                                 long iid,
                                 Connection *c) :
                 Router(id, iid),
                 m_connection(c)
{
}

Account::Account(Connection * conn,
                 const std::string & uname,
                 const std::string & passwd,
                 const std::string & id,
                 long intId) :
         ConnectableRouter(id, intId, conn),
         m_username(uname), m_password(passwd)
{
}


const char * Account::getType() const
{
    return "account";
}

void Account::store() const
{
}

bool Account::isPersisted() const {
    return true;
}

void Account::createObject(const std::string & type_str,
                           const Root & arg,
                           const Operation & op,
                           OpVector & res)
{
}

LocatedEntity * Account::createCharacterEntity(const std::string &,
                                const Atlas::Objects::Entity::RootEntity &,
                                const Atlas::Objects::Root &)
{
    return 0;
}

void Account::addCharacter(LocatedEntity * chr)
{
}

void Account::externalOperation(const Operation & op, Link &)
{
}

void Account::operation(const Operation & op, OpVector & res)
{
}

void Account::LogoutOperation(const Operation & op, OpVector & res)
{
}

void Account::CreateOperation(const Operation & op, OpVector & res)
{
}

void Account::ImaginaryOperation(const Operation & op, OpVector & res)
{
}

void Account::TalkOperation(const Operation & op, OpVector & res)
{
}

void Account::GetOperation(const Operation & op, OpVector & res)
{
}

void Account::SetOperation(const Operation & op, OpVector & res)
{
}

void Account::LookOperation(const Operation & op, OpVector & res)
{
}

void Account::OtherOperation(const Operation & op, OpVector & res)
{
}

void Account::addToMessage(MapType & omap) const
{
}

void Account::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

ServerAccount::ServerAccount(Connection * conn,
             const std::string & username,
             const std::string & passwd,
             const std::string & id,
             long intId) :
       Account(conn, username, passwd, id, intId)
{
}

const char * ServerAccount::getType() const
{
    return "server";
}

int ServerAccount::characterError(const Operation & op,
                                  const Root & ent,
                                  OpVector & res) const
{
    return -1;
}

void ServerAccount::createObject(const std::string & type_str,
                                 const Root & arg,
                                 const Operation & op,
                                 OpVector & res)
{
}

Player::Player(Connection * conn,
               const std::string & username,
               const std::string & passwd,
               const std::string & id,
               long intId) :
        Account(conn, username, passwd, id, intId)
{
}

const char * Player::getType() const
{
    return "player";
}

void Player::addToMessage(MapType & omap) const
{
}

void Player::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

int Player::characterError(const Operation & op,
                           const Root & ent, OpVector & res) const
{
    return 0;
}

Admin::Admin(Connection * conn,
             const std::string & username,
             const std::string & passwd,
             const std::string & id,
             long intId) :
       Account(conn, username, passwd, id, intId)
{
}

Admin::~Admin()
{
}

const char * Admin::getType() const
{
    return "admin";
}

void Admin::addToMessage(MapType & omap) const
{
}

void Admin::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

int Admin::characterError(const Operation & op,
                          const Root & ent, OpVector & res) const
{
    return 0;
}

void Admin::createObject(const std::string & type_str,
                           const Root & arg,
                           const Operation & op,
                           OpVector & res)
{
}

LocatedEntity * Admin::createCharacterEntity(const std::string &,
                                const Atlas::Objects::Entity::RootEntity &,
                                const Atlas::Objects::Root &)
{
    return 0;
}


void Admin::LogoutOperation(const Operation & op, OpVector & res)
{
}

void Admin::SetOperation(const Operation & op, OpVector & res)
{
}

void Admin::GetOperation(const Operation & op, OpVector & res)
{
}

void Admin::OtherOperation(const Operation & op, OpVector & res)
{
}

Shaker::Shaker() {}

std::string Shaker::generateSalt(size_t)
{
    return "";
}

void log(LogLevel lvl, const std::string & msg)
{
}

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}

