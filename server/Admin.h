// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef ADMIN_H
#define ADMIN_H

#include "Account.h"

class Admin : public Account {
  protected:
    virtual oplist characterError(const Create &, const Atlas::Message::Object &) const;
  public:
    Admin(Connection * conn, const string & username, const string & passwd);
    virtual ~Admin();

    virtual oplist Operation(const Load & op);
    virtual oplist Operation(const Save & op);
    virtual oplist Operation(const Get & op);
    virtual oplist Operation(const Set & op);
};

#endif /* ADMIN_H */
