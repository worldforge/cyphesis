#ifndef CREATOR_H
#define CREATOR_H

#include "Character.h"

class Creator : public Character {
  public:
    Creator();
    virtual oplist send_mind(const RootOperation & msg);
    virtual oplist operation(const RootOperation & op);
    virtual oplist external_operation(const RootOperation & op);
};

#endif /* CREATOR_H */
