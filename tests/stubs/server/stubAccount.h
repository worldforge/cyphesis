/*
 Copyright (C) 2014 Erik Ogenvik

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
#ifndef STUBACCOUNT_H_
#define STUBACCOUNT_H_

#include "common/OperationRouter.h"


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
    return "testaccount";
}

void Account::store() const
{
}

bool Account::isPersisted() const {
    return true;
}

void Account::addToMessage(Atlas::Message::MapType &) const
{
}

void Account::addToEntity(const Atlas::Objects::Entity::RootEntity &) const
{
}

void Account::createObject(const std::string & type_str,
                           const Atlas::Objects::Root & arg,
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

void Account::externalOperation(const Operation &, Link &)
{
}

void Account::operation(const Operation &, OpVector &)
{
}


void Account::LogoutOperation(const Operation &, OpVector &)
{
}

void Account::CreateOperation(const Operation &, OpVector &)
{
}

void Account::SetOperation(const Operation &, OpVector &)
{
}

void Account::ImaginaryOperation(const Operation &, OpVector &)
{
}

void Account::TalkOperation(const Operation &, OpVector &)
{
}

void Account::LookOperation(const Operation &, OpVector &)
{
}

void Account::GetOperation(const Operation &, OpVector &)
{
}

void Account::OtherOperation(const Operation &, OpVector &)
{
}



#endif /* STUBACCOUNT_H_ */
