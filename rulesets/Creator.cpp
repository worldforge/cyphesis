// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Creator.h"

#include "BaseMind.h"

#include <common/debug.h>

#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Delete.h>

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
        return world->LookOperation(look);
    }
    return sendMind(op);
}

OpVector Creator::externalOperation(const RootOperation & op)
{
    // If an admin connection specifies a TO on the op, we treat
    // it specially, and make sure it goes direct, otherwise
    // we handle it like a normal character.
    debug( std::cout << "Creator::externalOperation" << std::endl
                     << std::flush;);
    if (op.GetTo().empty()) {
        debug( std::cout << "Creator handling op normally" << std::endl << std::flush;);
        Character::externalOperation(op);
    } else if (op.GetTo()==getId()) {
        debug( std::cout << "Creator handling op " << std::endl << std::flush;);
        OpVector lres = callOperation(op);
        setRefno(lres, op);
        for(OpVector::const_iterator I = lres.begin(); I != lres.end(); I++) {
            world->setSerialnoOp(**I);
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
