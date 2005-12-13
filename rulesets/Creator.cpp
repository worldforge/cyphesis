// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Creator.h"

#include "BaseMind.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/const.h"
#include "common/serialno.h"
#include "common/compose.hpp"

#include "common/Setup.h"
#include "common/Tick.h"
#include "common/Unseen.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Objects::Root;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Unseen;
using Atlas::Objects::Entity::Anonymous;

static const bool debug_flag = false;

Creator::Creator(const std::string & id, long intId) : Creator_parent(id, intId)
{
    debug( std::cout << "Creator::Creator" << std::endl << std::flush;);
    if (consts::enable_omnipresence) {
        m_attributes["omnipresent"] = 1;
    }
}

void Creator::sendExternalMind(const Operation & op, OpVector & res)
{
    debug(std::cout << "Creator::sendExternalMind(" << op->getParents().front()
                    << ")" << std::endl << std::flush;);
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
        Delete d;
        Anonymous del_arg;
        del_arg->setId(getId());
        d->setArgs1(del_arg);
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
        case OP_DELETE:
            DeleteOperation(op, res);
            // Prevent Delete op from being sent to mind, so another delete
            // is not created in response.
            return;
            break;
        default:
            if (op_no == OP_SETUP) {
                m_world->addPerceptive(getId());
            } else if (op_no == OP_TICK) {
                TickOperation(op, res);
            }
            break;
    }
    sendExternalMind(op, res);
}

void Creator::externalOperation(const Operation & op)
{
    // If an admin connection specifies a TO on the op, we treat
    // it specially, and make sure it goes direct, otherwise
    // we handle it like a normal character.
    debug( std::cout << "Creator::externalOperation("
                     << op->getParents().front() << ")" << std::endl
                     << std::flush;);
    if (!op->hasAttrFlag(Atlas::Objects::Operation::TO_FLAG)) {
        debug( std::cout << "Creator handling op normally" << std::endl
                         << std::flush;);
        Creator_parent::externalOperation(op);
    } else if (op->getTo() == getId() && !op->hasAttrFlag(Atlas::Objects::Operation::FUTURE_SECONDS_FLAG)) {
        debug( std::cout << "Creator handling op " << std::endl << std::flush;);
        OpVector lres;
        callOperation(op, lres);
        OpVector::const_iterator Iend = lres.end();
        for (OpVector::const_iterator I = lres.begin(); I != Iend; ++I) {
            (*I)->setSerialno(newSerialNo());
            (*I)->setRefno(op->getSerialno());
            sendWorld(*I);
            // Don't delete lres as it has gone into World's queue
            // World will deal with it.
        }
    } else {
        Entity * to = m_world->getEntity(op->getTo());
        if (to == 0) {
            log(ERROR, String::compose("Creator operation from client is to unknown ID %1", op->getTo()).c_str());
            // FIXME Respond with Unseen.
        } else {
            // Make it appear like it came from target itself;
            to->sendWorld(op);
        }
    }
}

void Creator::mindLookOperation(const Operation & op, OpVector & res)
{
    // This overriden version allows the Creator to search the world for
    // entities by type or by name
    debug(std::cout << "Got look up from prived mind from [" << op->getFrom()
               << "] to [" << op->getTo() << "]" << std::endl << std::flush;);
    m_perceptive = true;
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        op->setTo(m_world->m_gameWorld.getId());
    } else {
        const Root & arg = args.front();
        if (arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            op->setTo(arg->getId());
        } else if (arg->hasAttrFlag(Atlas::Objects::NAME_FLAG)) {
            Entity * e = m_world->findByName(arg->getName());
            if (e != NULL) {
                op->setTo(e->getId());
            } else {
                Unseen u;
                u->setTo(getId());
                u->setArgs1(arg);
                if (op->hasAttrFlag(Atlas::Objects::Operation::SERIALNO_FLAG)) {
                    u->setRefno(op->getSerialno());
                }
                sendExternalMind(u, res);
                return;
            }
        } else if (arg->hasAttrFlag(Atlas::Objects::PARENTS_FLAG)) {
            if (!arg->getParents().empty()) {
                Entity * e = m_world->findByType(arg->getParents().front());
                if (e != NULL) {
                    op->setTo(e->getId());
                } else {
                    Unseen u;
                    u->setTo(getId());
                    u->setArgs1(arg);
                    if (op->hasAttrFlag(Atlas::Objects::Operation::SERIALNO_FLAG)) {
                        u->setRefno(op->getSerialno());
                    }
                    sendExternalMind(u, res);
                    return;
                }
            }
        }
    }
    debug( std::cout <<"  now to ["<<op->getTo()<<"]"<<std::endl<<std::flush;);
    res.push_back(op);
}
