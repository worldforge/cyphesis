#ifndef OOG_THING_H
#define OOG_THING_H

#include <common/BaseEntity.h>

class OOG_Thing : public BaseEntity {
  public:
    OOG_Thing() { }
    virtual ~OOG_Thing() { }
    virtual oplist Operation(const RootOperation & obj) {
        return(error(obj, "Unknown operation"));
    }
};

#endif /* OOG_THING_H */
