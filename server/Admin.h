#ifndef ADMIN_H
#define ADMIN_H

typedef int bad_type; // Remove this to get unset type reporting

#define None 0 // Remove this to deal with un-initialied vars

#include "Account.h"

//class File {
    //bad_type str;
//
    //public:
    //bad_type write(bad_type str);
    //File();
//};

class Admin : public Account {
  public:
    virtual ~Admin() { }

    virtual oplist Operation(const Load & op);
    virtual oplist Operation(const Save & op);
    virtual oplist character_error(const Create & op, const Message::Object & ent) { oplist res; return(res); }
    //bad_type code_operation(bad_type op)
};

#endif /* ADMIN_H */
