// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_ADMIN_H
#define SERVER_ADMIN_H

#include "Account.h"

class Persistance;

class Admin : public Account {
  protected:
    virtual OpVector characterError(const Create &, const Atlas::Message::Element::MapType &) const;
    void load(Persistance *, const std::string &, int &);
  public:
    Admin(Connection * conn, const std::string & username,
                             const std::string & passwd,
                             const std::string & id);
    virtual ~Admin();

    virtual const char * getType() const;

    virtual OpVector LogoutOperation(const Logout & op);
    virtual OpVector GetOperation(const Get & op);
    virtual OpVector SetOperation(const Set & op);
    virtual OpVector CreateOperation(const Create & op);
};

#endif // SERVER_ADMIN_H
