// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Look.h>

#include <common/debug.h>

#include "Creator.h"
#include "BaseMind.h"

static const bool debug_flag = false;

Creator::Creator()
{
    debug( std::cout << "Creator::Creator" << std::endl << std::flush;);
    character = true;
    omnipresent = true;
    location.bBox = BBox();
}

oplist Creator::sendMind(const RootOperation & msg)
{
    debug( std::cout << "Creator::sendMind" << std::endl << std::flush;);
    // Simpified version of character method sendMind() because local mind
    // of creator is irrelevant
    if (NULL != externalMind) {
        debug( std::cout << "Sending to external mind" << std::endl
                         << std::flush;);
        return externalMind->message(msg);
        // If there is some kinf of error in the connection, we turn autom on
    }
    return oplist();
}

oplist Creator::operation(const RootOperation & op)
{
    debug( std::cout << "Creator::operation" << std::endl << std::flush;);
    op_no_t op_no = opEnumerate(op);
    if (op_no == OP_LOOK) {
        return ((BaseEntity *)this)->LookOperation((Look &)op);
    }
    if (op_no == OP_SETUP) {
        Look look = Look::Instantiate();
        look.SetFrom(getId());
        return world->lookOperation(look);
    }
    return sendMind(op);
}

oplist Creator::externalOperation(const RootOperation & op)
{
    debug( std::cout << "Creator::externalOperation" << std::endl
                     << std::flush;);
    if ((op.GetTo()==getId()) || (op.GetTo()=="")) {
        debug( std::cout << "Creator handling op " << std::endl << std::flush;);
        oplist lres = callOperation(op);
        setRefno(lres, op);
        for(oplist::const_iterator I = lres.begin(); I != lres.end(); I++) {
            sendWorld(*I);
        }
    } else {
        RootOperation * new_op = new RootOperation(op);
        //make it appear like it came from character itself;
        new_op->SetFrom("cheat");
        sendWorld(new_op);
    }
    return oplist();
}
