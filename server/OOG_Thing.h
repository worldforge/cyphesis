typedef int bad_type; // Remove this to get unset type reporting

#include <common/BaseEntity.h>

class OOG_Thing : public BaseEntity {
  public:
    OOG_Thing();
    bad_type undefined_operation(bad_type op);
};
