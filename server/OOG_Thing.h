#ifndef OOG_THING_H
#define OOG_THING_H

typedef int bad_type; // Remove this to get unset type reporting

#include <common/BaseEntity.h>

class OOG_Thing : public BaseEntity {
  public:
    OOG_Thing() { }
    virtual ~OOG_Thing() { }
    virtual RootOperation * Operation(const RootOperation & obj) {
        return error(obj, "Unknown operation");
    }
};

#endif /* OOG_THING_H */
