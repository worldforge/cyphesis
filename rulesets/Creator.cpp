// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Delete.h>

#include <common/debug.h>

#include "Creator.h"
#include "BaseMind.h"

static const bool debug_flag = false;

Creator::Creator()
{
    debug( std::cout << "Creator::Creator" << std::endl << std::flush;);
    omnipresent = true;
    // location.bBox = BBox();
}

OpVector Creator::sendMind(const RootOperation & msg)
{
    debug( std::cout << "Creator::sendMind" << std::endl << std::flush;);
    // Simpified version of character method sendMind() because local mind
    // of creator is irrelevant
    if (NULL != externalMind) {
        debug( std::cout << "Sending to external mind" << std::endl
                         << std::flush;);
        return externalMind->message(msg);
        // If there is some kinf of error in the connection, we turn autom on
    } else {
        // If we do not have an external mind, and therefor a connection,
        // there is no purpose to our existance, so we should die.
        debug( std::cout << "NOTICE: Creator self destruct"
                         << std::endl << std::flush;);
        Delete * d = new Delete(Delete::Instantiate());
        d->SetTo(getId());
        return OpVector(1,d);
    }
}

OpVector Creator::operation(const RootOperation & op)
{
    debug( std::cout << "Creator::operation" << std::endl << std::flush;);
    OpNo op_no = opEnumerate(op);
    if (op_no == OP_LOOK) {
        return LookOperation((Look &)op);
    }
    if (op_no == OP_SETUP) {
        Look look = Look::Instantiate();
        look.SetFrom(getId());
        return world->lookOperation(look);
    }
    return sendMind(op);
}

OpVector Creator::externalOperation(const RootOperation & op)
{
    // FIXME Here we have a problem. For ops like move with velocity, and
    // Look, it is necessary for the operation to be hindled by
    // Character::mind2body(). So, how do we handle the difference between
    // ops that must be processed this way, and ones that admin wants to
    // send directly to its avatar. I think we should say that if TO is
    // specified, the op should go directly to the entity concerned
    // including this entity, but if not then it should go via the
    // mind2body interface. This means that some clients will need
    // modification. There are few places in Eris where this is a
    // problem.
    debug( std::cout << "Creator::externalOperation" << std::endl
                     << std::flush;);
    if (op.GetTo().empty()) {
        debug( std::cout << "Creator handling op normally" << std::endl << std::flush;);
        OpVector lres = mind2body(op);
        for(OpVector::const_iterator I = lres.begin(); I != lres.end(); I++) {
            sendWorld(*I);
            // Don't delete lres as it has gone into worlds queue
            // World will deal with it.
        }
    } else if (op.GetTo()==getId()) {
        debug( std::cout << "Creator handling op " << std::endl << std::flush;);
        OpVector lres = callOperation(op);
        setRefno(lres, op);
        for(OpVector::const_iterator I = lres.begin(); I != lres.end(); I++) {
            sendWorld(*I);
            // Don't delete lres as it has gone into worlds queue
            // World will deal with it.
        }
    } else {
        RootOperation * new_op = new RootOperation(op);
        //make it appear like it came from character itself;
        new_op->SetFrom("cheat");
        sendWorld(new_op);
    }
    return OpVector();
}
