#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include "Admin.h"

oplist Admin::Operation(const Save & op)
{
    // Obviously does nothing yet. A persistance system needs to be implemented
    oplist res;
    return(res);
}

oplist Admin::Operation(const Load & op)
{
    // Obviously does nothing yet. A persistance system needs to be implemented
    oplist res;
    return(res);
}

// There used to be a code operation handler here. It may become desirable in
// the future for the admind account to be able to send script fragments.
// Think about implementing this.
