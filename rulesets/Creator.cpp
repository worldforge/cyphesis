// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Creator.h"

#include "BaseMind.h"

#include "common/debug.h"
#include "common/const.h"

#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Delete.h>

static const bool debug_flag = false;

Creator::Creator(const std::string & id) : Character(id)
{
    debug( std::cout << "Creator::Creator" << std::endl << std::flush;);
    if (consts::enable_omnipresence) {
        attributes["omnipresent"] = 1;
    }
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

OpVector Creator::mindLookOperation(const Look & op)
{
    debug(std::cout << "Got look up from prived mind from [" << op.GetFrom()
               << "] to [" << op.GetTo() << "]" << std::endl << std::flush;);
    perceptive = true;
    Look * l = new Look(op);
    if (op.GetTo().empty()) {
        const Element::ListType & args = op.GetArgs();
        if (args.empty()) {
            l->SetTo(world->getId());
        } else {
            if (args.front().IsMap()) {
                const Element::MapType & amap = args.front().AsMap();
                Element::MapType::const_iterator I = amap.find("id");
                if (I != amap.end() && I->second.IsString()) {
                    l->SetTo(I->second.AsString());
                } else if ((I = amap.find("name")) != amap.end()) {
                    if (I->second.IsString() && !I->second.AsString().empty()) {
                        Entity * e = world->findByName(I->second.AsString());
                        if (e != NULL) {
                            l->SetTo(e->getId());
                        }
                    }
                } else if ((I = amap.find("parents")) != amap.end()) {
                    if (I->second.IsList() && !I->second.AsList().empty()) {
                        const Element & p = I->second.AsList().front();
                        if (p.IsString()) {
                            Entity * e = world->findByType(p.AsString());
                            if (e != NULL) {
                                l->SetTo(e->getId());
                            }
                        }
                    }
                }
            }
        }
    }
    debug( std::cout <<"    now to ["<<l->GetTo()<<"]"<<std::endl<<std::flush;);
    return OpVector(1,l);
}
