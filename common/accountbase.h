// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef COMMON_ACCOUNT_BASE_H
#define COMMON_ACCOUNT_BASE_H

#include <Atlas/Message/Object.h>

#include <common/database.h>

using Atlas::Message::Object;

class AccountBase : public Database {
  protected:
    AccountBase() { }

  public:
    static AccountBase * instance() {
        if (m_instance == NULL) {
            m_instance = new AccountBase();
            m_instance->initAccount();
        }
        return (AccountBase *)m_instance;
    }

    bool putAccount(const Object::MapType & o, const std::string & account) {
        return putObject(account_db, o, account.c_str());
    }
    bool delAccount(const std::string & account) {
        return delObject(account_db, account.c_str());
    }
    bool getAccount(const std::string & account, Object::MapType & o) {
        return getObject(account_db, account.c_str(), o);
    }

};

#endif // COMMON_ACCOUNT_BASE_H
