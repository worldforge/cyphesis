// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Creator.h"

#include "BaseMind.h"

#include "common/debug.h"
#include "common/refno.h"
#include "common/const.h"

#include <Atlas/Objects/Operation/Delete.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Operation::Delete;

static const bool debug_flag = false;

Creator::Creator(const std::string & id) : Creator_parent(id)
{
    debug( std::cout << "Creator::Creator" << std::endl << std::flush;);
    if (consts::enable_omnipresence) {
        m_attributes["omnipresent"] = 1;
    }
}

void Creator::sendExternalMind(const Operation & op, OpVector & res)
{
    debug(std::cout << "Creator::sendExternalMind" << std::endl << std::flush;);
    // Simpified version of Character method sendMind() because local
    // mind of Creator is irrelevant
    if (0 != m_externalMind) {
        debug( std::cout << "Sending to external mind" << std::endl
                         << std::flush;);
        m_externalMind->operation(op, res);
    } else {
        // If we do not have an external mind, and therefor a connection,
        // there is no purpose to our existance, so we should die.
        debug( std::cout << "NOTICE: Creator self destruct"
                         << std::endl << std::flush;);
        Delete * d = new Delete();
        ListType & dargs = d->getArgs();
        dargs.push_back(MapType());
        dargs.front().asMap()["id"] = getId();
        d->setTo(getId());
        res.push_back(d);
    }
}

void Creator::operation(const Operation & op, OpVector & res)
{
    debug( std::cout << "Creator::operation" << std::endl << std::flush;);
    // FIXME Why not just call callOperation() to handle the type switch?
    // The only real reason is that we avoid passing the Delete op to the
    // mind, so we return early here. Could check for the Delete op in
    // sendExternalMind() when the mind is gone, thus getting rid of the
    // problem.
    // To switch to using callOperation(), some more op handlers would
    // need to be implemented, in particular SetupOperation() would need
    // need to be implemented as below. Some might need to be blocked
    // to prevent anyone from messing with us, like SetOperation().
    OpNo op_no = opEnumerate(op);
    switch(op_no) {
        case OP_CREATE:
            CreateOperation(op, res);
            break;
        case OP_LOOK:
            LookOperation(op, res);
            break;
        case OP_MOVE:
            MoveOperation(op, res);
            break;
        case OP_SETUP:
            m_world->addPerceptive(getId());
            break;
        case OP_DELETE:
            DeleteOperation(op, res);
            // Prevent Delete op from being sent to mind, so another delete
            // is not created in response.
            return;
            break;
        case OP_TICK:
            TickOperation(op, res);
            break;
        default:
            break;
    }
    sendExternalMind(op, res);
}

void Creator::externalOperation(const Operation & op)
{
    // If an admin connection specifies a TO on the op, we treat
    // it specially, and make sure it goes direct, otherwise
    // we handle it like a normal character.
    debug( std::cout << "Creator::externalOperation" << std::endl
                     << std::flush;);
    if (op.getTo().empty()) {
        debug( std::cout << "Creator handling op normally" << std::endl
                         << std::flush;);
        Creator_parent::externalOperation(op);
    } else if (op.getTo() == getId()) {
        debug( std::cout << "Creator handling op " << std::endl << std::flush;);
        OpVector lres;
        callOperation(op, lres);
        setRefno(lres, op);
        OpVector::const_iterator Iend = lres.end();
        for (OpVector::const_iterator I = lres.begin(); I != Iend; ++I) {
            m_world->setSerialnoOp(**I);
            sendWorld(*I);
            // Don't delete lres as it has gone into World's queue
            // World will deal with it.
        }
    } else {
        Operation * new_op = new Operation(op);
        //make it appear like it came from target itself;
        new_op->setFrom("cheat");
        sendWorld(new_op);
    }
}

void Creator::mindLookOperation(const Operation & op, OpVector & res)
{
    // This overriden version allows the Creator to search the world for
    // entities by type or by name
    debug(std::cout << "Got look up from prived mind from [" << op.getFrom()
               << "] to [" << op.getTo() << "]" << std::endl << std::flush;);
    m_perceptive = true;
    Operation * l = new Operation(op);
    if (op.getTo().empty()) {
        const ListType & args = op.getArgs();
        if (args.empty() || !args.front().isMap()) {
            l->setTo(m_world->m_gameWorld.getId());
        } else {
            const MapType & amap = args.front().asMap();
            MapType::const_iterator I = amap.find("id");
            if (I != amap.end() && I->second.isString()) {
                l->setTo(I->second.asString());
            } else if ((I = amap.find("name")) != amap.end()) {
                if (I->second.isString() && !I->second.asString().empty()) {
                    Entity * e = m_world->findByName(I->second.asString());
                    if (e != NULL) {
                        l->setTo(e->getId());
                    }
                }
            } else if ((I = amap.find("parents")) != amap.end()) {
                if (I->second.isList() && !I->second.asList().empty()) {
                    const Element & p = I->second.asList().front();
                    if (p.isString()) {
                        Entity * e = m_world->findByType(p.asString());
                        if (e != NULL) {
                            l->setTo(e->getId());
                        }
                    }
                }
            }
        }
    }
    debug( std::cout <<"    now to ["<<l->getTo()<<"]"<<std::endl<<std::flush;);
    res.push_back(l);
}
