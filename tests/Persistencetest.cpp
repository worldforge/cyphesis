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

// $Id

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "server/Persistence.h"

#include <Atlas/Message/Element.h>

#include <cassert>

using Atlas::Message::MapType;
using Atlas::Objects::Root;

int main()
{
    {
        Persistence * p = Persistence::instance();
        p->shutdown();
    }

    {
        Persistence * p = Persistence::instance();
        int res = p->init();
        assert(res == 0);
        p->shutdown();
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

#include <cstdlib>

const char * CYPHESIS = "cyphesis";

std::string instance("mason");

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

ConnectableRouter::~ConnectableRouter()
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

Account::~Account()
{
}

const char * Account::getType() const
{
    return "account";
}

void Account::store() const
{
}

void Account::createObject(const std::string & type_str,
                           const Root & arg,
                           const Operation & op,
                           OpVector & res)
{
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

ServerAccount::~ServerAccount()
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

Player::~Player() { }

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

Database * Database::m_instance = NULL;

int Database::initConnection()
{
    return 0;
}

Database::Database() : m_rule_db("rules"),
                       m_queryInProgress(false),
                       m_connection(NULL)
{
}

Database::~Database()
{
}

void Database::shutdownConnection()
{
}

int Database::registerRelation(std::string & tablename,
                               const std::string & sourcetable,
                               const std::string & targettable,
                               RelationType kind)
{
    return 0;
}

const DatabaseResult Database::selectSimpleRowBy(const std::string & name,
                                                 const std::string & column,
                                                 const std::string & value)
{
    return DatabaseResult(0);
}

Database * Database::instance()
{
    if (m_instance == NULL) {
        m_instance = new Database();
    }
    return m_instance;
}

int Database::createInstanceDatabase()
{
    return 0;
}

int Database::registerEntityIdGenerator()
{
    return 0;
}

int Database::registerEntityTable(const std::map<std::string, int> & chunks)
{
    return 0;
}

int Database::registerPropertyTable()
{
    return 0;
}

int Database::initRule(bool createTables)
{
    return 0;
}

int Database::registerSimpleTable(const std::string & name,
                                  const MapType & row)
{
    return 0;
}

int Database::createSimpleRow(const std::string & name,
                              const std::string & id,
                              const std::string & columns,
                              const std::string & values)
{
    return 0;
}

const DatabaseResult Database::selectRelation(const std::string & name,
                                              const std::string & id)
{
    return DatabaseResult(0);
}

int Database::createRelationRow(const std::string & name,
                                const std::string & id,
                                const std::string & other)
{
    return 0;
}

int Database::removeRelationRow(const std::string & name,
                                const std::string & id)
{
    return 0;
}

int Database::removeRelationRowByOther(const std::string & name,
                                       const std::string & other)
{
    return 0;
}

bool Database::hasKey(const std::string & table, const std::string & key)
{
    return false;
}

int Database::putObject(const std::string & table,
                        const std::string & key,
                        const MapType & o,
                        const StringVector & c)
{
    return 0;
}

int Database::getTable(const std::string & table,
                       std::map<std::string, Root> & contents)
{
    return 0;
}

int Database::clearPendingQuery()
{
    return 0;
}

int Database::updateObject(const std::string & table,
                           const std::string & key,
                           const MapType & o)
{
    return 0;
}

int Database::clearTable(const std::string & table)
{
    return 0;
}

long Database::newId(std::string & id)
{
    return 0;
}

void Database::cleanup()
{
    if (m_instance != 0) {
        delete m_instance;
    }

    m_instance = 0;
}

const char * DatabaseResult::field(const char * column, int row) const
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

namespace consts {
  const char * defaultAdminPasswordHash = "$1$1A67C3C65EECBBCE$A51127573498DCB08992378D07A36A20";
}
