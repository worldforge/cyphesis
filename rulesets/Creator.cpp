// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Look.h>

#include <server/WorldRouter.h>
#include <common/debug.h>

#include "Creator.h"
#include "ExternalMind.h"

static const bool debug_flag = false;

Creator::Creator()
{
    debug( cout << "Creator::Creator" << endl << flush;);
    isCharacter = true;
    omnipresent = true;
    location.bbox = Vector3D();
}

oplist Creator::sendMind(const RootOperation & msg)
{
    debug( cout << "Creator::sendMind" << endl << flush;);
    // Simpified version of character method sendMind() because local mind
    // of creator is irrelevant
    if (NULL != externalMind) {
        debug( cout << "Sending to external mind" << endl << flush;);
        return externalMind->message(msg);
        // If there is some kinf of error in the connection, we turn autom on
    }
    return oplist();
}

oplist Creator::operation(const RootOperation & op)
{
    debug( cout << "Creator::operation" << endl << flush;);
    op_no_t op_no = opEnumerate(op);
    if (op_no == OP_LOOK) {
        return ((BaseEntity *)this)->LookOperation((Look &)op);
    }
    if (op_no == OP_SETUP) {
        Look look = Look::Instantiate();
        look.SetFrom(fullid);
        return world->lookOperation(look);
    }
    return sendMind(op);
}

oplist Creator::externalOperation(const RootOperation & op)
{
    debug( cout << "Creator::externalOperation" << endl << flush;);
    if ((op.GetTo()==fullid) || (op.GetTo()=="")) {
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
