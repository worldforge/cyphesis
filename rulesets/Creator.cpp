// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Sound.h>
#include <Atlas/Objects/Operation/Touch.h>

#include <common/Chop.h>
#include <common/Cut.h>
#include <common/Eat.h>
#include <common/Fire.h>
#include <common/Chop.h>

#include <server/WorldRouter.h>
#include <common/debug.h>

#include "Creator.h"

#include "ExternalMind.h"

static const bool debug_flag = false;

Creator::Creator()
{
    debug( cout << "Creator::Creator" << endl << flush;);
    is_character = true;
    omnipresent = true;
    location.bbox = Vector3D();
}

oplist Creator::send_mind(const RootOperation & msg)
{
    debug( cout << "Creator::send_mind" << endl << flush;);
    // Simpified version of character method send_mind() because local mind
    // of creator is irrelevant
    if ((NULL != external_mind) && (NULL != external_mind->connection)) {
        debug( cout << "Sending to external mind" << endl << flush;);
        return external_mind->message(msg);
        // If there is some kinf of error in the connection, we turn autom on
    }
    return oplist();
}

oplist Creator::operation(const RootOperation & op)
{
    debug( cout << "Creator::operation" << endl << flush;);
    op_no_t op_no = op_enumerate(&op);
    if (op_no == OP_LOOK) {
        return ((BaseEntity *)this)->Operation((Look &)op);
    }
    if (op_no == OP_SETUP) {
        Look look = Look::Instantiate();
        look.SetFrom(fullid);
        return world->Operation(look);
    }
    return send_mind(op);
}

oplist Creator::external_operation(const RootOperation & op)
{
    debug( cout << "Creator::external_operation" << endl << flush;);
    if ((op.GetTo()==fullid) || (op.GetTo()=="")) {
        oplist lres = call_operation(op);
        set_refno(lres, op);
        for(oplist::const_iterator I = lres.begin(); I != lres.end(); I++) {
            send_world(*I);
        }
    } else {
        RootOperation * new_op = new RootOperation(op);
        new_op->SetFrom("cheat"); //make it appear like it came from character itself;
        Creator::send_world(new_op);
    }
    return oplist();
}
