// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Look.h>

#include <server/WorldRouter.h>

#include "Creator.h"

#include "ExternalMind.h"

static int debug_creator = 0;

Creator::Creator()
{
    debug_creator && cout << "Creator::Creator" << endl << flush;
    is_character = true;
    omnipresent = true;
}

oplist Creator::send_mind(const RootOperation & msg)
{
    debug_creator && cout << "Creator::send_mind" << endl << flush;
    // Simpified version of character method send_mind() because local mind
    // of creator is irrelevant
    oplist res;
    if ((NULL != external_mind) && (NULL != external_mind->connection)) {
        debug_creator && cout << "Sending to external mind" << endl << flush;
        res = external_mind->message(msg);
        // If there is some kinf of error in the connection, we turn autom on
    }
    return(res);
}

oplist Creator::operation(const RootOperation & op)
{
    debug_creator && cout << "Creator::operation" << endl << flush;
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
    debug_creator && cout << "Creator::external_operation" << endl << flush;
    oplist res;
    if ((op.GetTo()==fullid) || (op.GetTo()=="")) {
        oplist local_res = call_operation(op);
        set_refno(local_res, op);
        while (local_res.size() != 0) {
            RootOperation * br = local_res.front();
            send_world(br);
            local_res.pop_front();
        }
    } else {
        RootOperation * new_op = new RootOperation(op);
        new_op->SetFrom("cheat"); //make it appear like it came from character itself;
        Creator::send_world(new_op);
    }
    return(res);
}
