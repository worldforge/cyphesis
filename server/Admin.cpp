// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include "Admin.h"

oplist Admin::Operation(const Save & op)
{
    // Obviously does nothing yet. A persistance system needs to be implemented
    return oplist();
}

oplist Admin::Operation(const Load & op)
{
    // Obviously does nothing yet. A persistance system needs to be implemented
    return oplist();
}

// There used to be a code operation handler here. It may become desirable in
// the future for the admind account to be able to send script fragments.
// Think about implementing this.
