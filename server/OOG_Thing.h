#ifndef OOG_THING_H
#define OOG_THING_H

#include <common/BaseEntity.h>

class OOG_Thing : public BaseEntity {
  public:
    OOG_Thing() { }
    virtual ~OOG_Thing() { }
    virtual oplist Operation(const RootOperation & op) {
        return(error(op, "Unknown operation"));
    }
};

#endif /* OOG_THING_H */
