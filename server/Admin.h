// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_ADMIN_H
#define SERVER_ADMIN_H

#include "Account.h"

#include <sigc++/connection.h>

class Persistance;

class Admin : public Account {
  protected:
    virtual int characterError(const RootOperation &,
                               const Atlas::Message::MapType &,
                               OpVector &) const;
    void opDispatched(RootOperation * op);

    SigC::Connection m_monitorConnection;
  public:
    Admin(Connection * conn, const std::string & username,
                             const std::string & passwd,
                             const std::string & id);
    virtual ~Admin();

    virtual const char * getType() const;

    virtual void LogoutOperation(const RootOperation &, OpVector &);
    virtual void GetOperation(const RootOperation &, OpVector &);
    virtual void SetOperation(const RootOperation &, OpVector &);
    virtual void CreateOperation(const RootOperation &, OpVector &);
    virtual void OtherOperation(const RootOperation &, OpVector &);
};

#endif // SERVER_ADMIN_H
