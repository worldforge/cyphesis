// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id$

#include "Creator.h"

#include "BaseMind.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/serialno.h"
#include "common/compose.hpp"

#include "common/Setup.h"
#include "common/Tick.h"
#include "common/Unseen.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Objects::Root;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Unseen;
using Atlas::Objects::Entity::Anonymous;

static const bool debug_flag = false;

Creator::Creator(const std::string & id, long intId) :
         Creator_parent(id, intId)
{
    debug( std::cout << "Creator::Creator" << std::endl << std::flush;);
    setFlags(entity_ephem);
}

void Creator::operation(const Operation & op, OpVector & res)
{
    debug( std::cout << "Creator::operation" << std::endl << std::flush;);
    // FIXME: Switch to using callOperation(), some more op handlers would
    // need to be implemented. Some might need to be blocked
    // to prevent anyone from messing with us, like SetOperation().
    OpNo op_no = op->getClassNo();
    switch(op_no) {
        case Atlas::Objects::Operation::CREATE_NO:
            CreateOperation(op, res);
            break;
        case Atlas::Objects::Operation::LOOK_NO:
            LookOperation(op, res);
            break;
        case Atlas::Objects::Operation::MOVE_NO:
            MoveOperation(op, res);
            break;
        case Atlas::Objects::Operation::DELETE_NO:
            DeleteOperation(op, res);
            break;
        case Atlas::Objects::Operation::TALK_NO:
            TalkOperation(op, res);
            break;
        default:
            if (op_no == Atlas::Objects::Operation::TICK_NO) {
                TickOperation(op, res);
            }
            break;
    }
    sendMind(op, res);
}

void Creator::externalOperation(const Operation & op)
{
    // If an admin connection specifies a TO on the op, we treat
    // it specially, and make sure it goes direct, otherwise
    // we handle it like a normal character.
    debug( std::cout << "Creator::externalOperation("
                     << op->getParents().front() << ")" << std::endl
                     << std::flush;);
    if (op->isDefaultTo()) {
        debug( std::cout << "Creator handling op normally" << std::endl
                         << std::flush;);
        Creator_parent::externalOperation(op);
    } else if (op->getTo() == getId() && op->isDefaultFutureSeconds()) {
        debug( std::cout << "Creator handling op " << std::endl << std::flush;);
        OpVector lres;
        OpVector eres;
        callOperation(op, lres);
        OpVector::const_iterator Iend = lres.end();
        for (OpVector::const_iterator I = lres.begin(); I != Iend; ++I) {
            if (!op->isDefaultSerialno()) {
                (*I)->setRefno(op->getSerialno());
            }
            sendMind(*I, eres);
            sendWorld(*I);
            // Don't delete lres as it has gone into World's queue
            // World will deal with it.
        }
    } else {
        Entity * to = BaseWorld::instance().getEntity(op->getTo());
        if (to != 0) {
            // Make it appear like it came from target itself;
            to->sendWorld(op);

            Sight sight;
            sight->setArgs1(op);
            sight->setTo(getId());
            if (!op->isDefaultSerialno()) {
                sight->setRefno(op->getSerialno());
            }
            OpVector res;
            sendMind(sight, res);
        } else {
            log(ERROR, String::compose("Creator operation from client "
                                       "is to unknown ID \"%1\"",
                                       op->getTo()));

            Anonymous unseen_arg;
            unseen_arg->setId(op->getTo());

            Unseen unseen;
            unseen->setArgs1(unseen_arg);
            unseen->setTo(getId());
            if (!op->isDefaultSerialno()) {
                unseen->setRefno(op->getSerialno());
            }
            OpVector res;
            sendMind(unseen, res);
            // We are not interested in anything the external mind might return
        }
    }
}

void Creator::mindLookOperation(const Operation & op, OpVector & res)
{
    // This overriden version allows the Creator to search the world for
    // entities by type or by name
    debug(std::cout << "Got look up from prived mind from [" << op->getFrom()
               << "] to [" << op->getTo() << "]" << std::endl << std::flush;);
    m_flags |= entity_perceptive;
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        op->setTo(BaseWorld::instance().m_gameWorld.getId());
    } else {
        const Root & arg = args.front();
        if (arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            op->setTo(arg->getId());
        } else if (arg->hasAttrFlag(Atlas::Objects::NAME_FLAG)) {
            // Search by name
            Entity * e = BaseWorld::instance().findByName(arg->getName());
            if (e != NULL) {
                op->setTo(e->getId());
            } else {
                Unseen u;
                u->setTo(getId());
                u->setArgs1(arg);
                if (!op->isDefaultSerialno()) {
                    u->setRefno(op->getSerialno());
                }
                sendMind(u, res);
                return;
            }
        } else if (arg->hasAttrFlag(Atlas::Objects::PARENTS_FLAG)) {
            // Search by name
            if (!arg->getParents().empty()) {
                Entity * e = BaseWorld::instance().findByType(arg->getParents().front());
                if (e != NULL) {
                    op->setTo(e->getId());
                } else {
                    Unseen u;
                    u->setTo(getId());
                    u->setArgs1(arg);
                    if (!op->isDefaultSerialno()) {
                        u->setRefno(op->getSerialno());
                    }
                    sendMind(u, res);
                    return;
                }
            }
        }
        // FIXME Need to ensure that a broadcast Look insn't sent, and
        // an Unseen is sent back, in once place if no match is found.
        // Probably most easlier done by checking TO on op by flag.
    }
    debug( std::cout <<"  now to ["<<op->getTo()<<"]"<<std::endl<<std::flush;);
    res.push_back(op);
}
