// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef ADMIN_H
#define ADMIN_H

#include "Account.h"

class Persistance;

class Admin : public Account {
  protected:
    virtual oplist characterError(const Create &, const Atlas::Message::Object::MapType &) const;
    void load(Persistance *, const string &, int &);
  public:
    Admin(Connection * conn, const string & username, const string & passwd);
    virtual ~Admin();

    virtual oplist LoadOperation(const Load & op);
    virtual oplist SaveOperation(const Save & op);
    virtual oplist GetOperation(const Get & op);
    virtual oplist SetOperation(const Set & op);
};

#endif // ADMIN_H
