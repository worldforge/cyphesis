#ifndef ADMIN_H
#define ADMIN_H

#include "Account.h"

class Admin : public Account {
  protected:
    virtual oplist character_error(const Create &, const Message::Object &) const {
        oplist res;
        return(res);
    }
  public:
    Admin(Connection * conn, const string & username, const string & passwd) :
        Account(conn, username, passwd) {
        type = "admin";
    }
    virtual ~Admin() { }

    virtual oplist Operation(const Load & op);
    virtual oplist Operation(const Save & op);
};

#endif /* ADMIN_H */
