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

    virtual RootOperation * Operation(const Load & op);
    virtual RootOperation * Operation(const Save & op);
    virtual RootOperation * character_error(const Create & op, const Message::Object & ent) { return(NULL); }
    //bad_type code_operation(bad_type op)
};

#endif /* ADMIN_H */
