// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Creator.h"

#include "BaseMind.h"

#include "common/debug.h"
#include "common/refno.h"
#include "common/const.h"

#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Delete.h>

static const bool debug_flag = false;

Creator::Creator(const std::string & id) : Creator_parent(id)
{
    debug( std::cout << "Creator::Creator" << std::endl << std::flush;);
    if (consts::enable_omnipresence) {
        m_attributes["omnipresent"] = 1;
    }
}

void Creator::sendMind(const RootOperation & op, OpVector & res)
{
    debug( std::cout << "Creator::sendMind" << std::endl << std::flush;);
    // Simpified version of character method sendMind() because local mind
    // of creator is irrelevant
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
        d->setTo(getId());
        res.push_back(d);
    }
}

void Creator::operation(const RootOperation & op, OpVector & res)
{
    debug( std::cout << "Creator::operation" << std::endl << std::flush;);
    // FIXME Why is this function here? If we want to block some ops,
    // why not just override them? Set and Burn are perhaps candidates.
    // Talk and Imaginary are currently blocked, and perhaps it is best
    // to leave normal communication unimplemented. Tick is required for
    // movement to function, but is currenlty blocked. Eat and
    // Nourish should remain blocked.
    // One reason this is here so so that all ops from world are sent
    // to the mind, skipping the world2mind() filter.
    OpNo op_no = opEnumerate(op);
    switch(op_no) {
        case OP_CREATE:
            CreateOperation((Create &)op, res);
            break;
        case OP_LOOK:
            LookOperation((Look &)op, res);
            break;
        case OP_MOVE:
            MoveOperation((Move &)op, res);
            break;
        case OP_SETUP:
            m_world->addPerceptive(getId());
            break;
        case OP_DELETE:
            DeleteOperation((Delete &)op, res);
            break;
        case OP_TICK:
            TickOperation((Tick &)op, res);
            break;
        default:
            break;
    }
    sendMind(op, res);
}

void Creator::externalOperation(const RootOperation & op, OpVector & res)
{
    // If an admin connection specifies a TO on the op, we treat
    // it specially, and make sure it goes direct, otherwise
    // we handle it like a normal character.
    debug( std::cout << "Creator::externalOperation" << std::endl
                     << std::flush;);
    if (op.getTo().empty()) {
        debug( std::cout << "Creator handling op normally" << std::endl
                         << std::flush;);
        Creator_parent::externalOperation(op, res);
    } else if (op.getTo() == getId()) {
        debug( std::cout << "Creator handling op " << std::endl << std::flush;);
        OpVector lres;
        callOperation(op, lres);
        setRefno(lres, op);
        for (OpVector::const_iterator I = lres.begin(); I != lres.end(); I++) {
            m_world->setSerialnoOp(**I);
            sendWorld(*I);
            // Don't delete lres as it has gone into World's queue
            // World will deal with it.
        }
    } else {
        RootOperation * new_op = new RootOperation(op);
        //make it appear like it came from target itself;
        new_op->setFrom("cheat");
        sendWorld(new_op);
    }
}

void Creator::mindLookOperation(const Look & op, OpVector & res)
{
    // This overriden version allows the creator to search the world for
    // entities by type or by name
    debug(std::cout << "Got look up from prived mind from [" << op.getFrom()
               << "] to [" << op.getTo() << "]" << std::endl << std::flush;);
    m_perceptive = true;
    Look * l = new Look(op);
    if (op.getTo().empty()) {
        const ListType & args = op.getArgs();
        if (args.empty()) {
            l->setTo(m_world->m_gameWorld.getId());
        } else {
            if (args.front().isMap()) {
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
    }
    debug( std::cout <<"    now to ["<<l->getTo()<<"]"<<std::endl<<std::flush;);
    res.push_back(l);
}
