// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include "accountbase.h"

AccountBase * AccountBase::m_instance = NULL;

bool AccountBase::putAccount(const Atlas::Message::Object::MapType & o,
                             const std::string & account)
{
    return false;
}

bool AccountBase::modAccount(const Atlas::Message::Object::MapType & o,
                             const std::string & account)
{
    return false;
}

bool AccountBase::delAccount(const std::string & account)
{
    return false;
}

bool AccountBase::getAccount(const std::string & account,
                             Atlas::Message::Object::MapType & o)
{
    return false;
}
