#include "OOG_Thing.h"


OOG_Thing::OOG_Thing()
{
    //OOG_Thing::base_init(kw);
}

bad_type OOG_Thing::undefined_operation(bad_type op)
{
    return OOG_Thing::error(op,"Unknown operation in "+this->id);
}
