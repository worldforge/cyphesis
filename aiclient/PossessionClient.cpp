/*
 Copyright (C) 2013 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef HAVE_CONFIG_H
#endif

#include "PossessionClient.h"
#include "PossessionAccount.h"


#include "common/Possess.h"
#include "common/id.h"
#include "common/custom.h"
#include "common/SystemTime.h"
#include "common/Inheritance.h"

#include "common/debug.h"

#include <Atlas/Objects/Entity.h>

static const bool debug_flag = true;

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::RootOperation;

PossessionClient::PossessionClient(MindKit& mindFactory) :
    m_mindFactory(mindFactory),
    m_account(nullptr),
    m_operationsDispatcher([&](const Operation& op, Ref<BaseMind> from) { this->operationFromEntity(op, std::move(from)); },
                           [&]() -> double { return getTime(); }),
    m_inheritance(new Inheritance())
{
}

bool PossessionClient::idle()
{
    return m_operationsDispatcher.idle();
}

double PossessionClient::secondsUntilNextOp() const
{
    return m_operationsDispatcher.secondsUntilNextOp();
}

bool PossessionClient::isQueueDirty() const
{
    return m_operationsDispatcher.isQueueDirty();
}

void PossessionClient::markQueueAsClean()
{
    m_operationsDispatcher.markQueueAsClean();
}

void PossessionClient::createAccount(const std::string& accountId)
{
    log(INFO, "Creating possession account on server.");
    m_account = new PossessionAccount(accountId, integerId(accountId), m_mindFactory);
    OpVector res;
    m_account->enablePossession(res);
    for (auto& op : res) {
        m_connection.send(op);
    }
}

void PossessionClient::operationFromEntity(const Operation& op, Ref<BaseMind> locatedEntity)
{
    if (!locatedEntity->isDestroyed()) {
        OpVector res;
        locatedEntity->operation(op, res);
        for (auto& resOp : res) {
            //All resulting ops should go out to the server, except for Ticks which we'll keep ourselves.
            if (resOp->getClassNo() == Atlas::Objects::Operation::TICK_NO) {
                resOp->setTo(resOp->getFrom());
                m_operationsDispatcher.addOperationToQueue(resOp, locatedEntity);
            } else {
                resOp->setFrom(locatedEntity->getId());
                send(resOp);
            }
        }
    }
}

void PossessionClient::operation(const Operation& op, OpVector& res)
{
    if (debug_flag) {
        std::cout << "PossessionClient::operation received {" << std::endl;
        debug_dump(op, std::cout);
        std::cout << "}" << std::endl << std::flush;
    }

    OpVector accountRes;
    m_account->operation(op, accountRes);
    for (auto& resOp : accountRes) {
        //Any op with both "from" and "to" set should be re-sent.
        if ((!resOp->isDefaultTo() && !resOp->isDefaultFrom())) {
            auto& minds = getMinds();
            auto I = minds.find(resOp->getTo());
            if (I != minds.end()) {
                m_operationsDispatcher.addOperationToQueue(resOp, I->second);
            }
        } else {
            res.push_back(resOp);
        }
    }

//
//
//    if (op->isDefaultTo() || op->getTo() == m_account->getId()) {
//    } else {
//        auto mindI = m_minds.find(op->getTo());
//        if (mindI != m_minds.end()) {
//            OpVector mindRes;
//            auto mind = mindI->second;
//            mind->operation(op, mindRes);
//            for (auto& resOp : mindRes) {
//                resOp->setFrom(mind->getId());
//                //All resulting ops should go out to the server, except for Ticks which we'll keep ourselves.
//                if (resOp->getClassNo() == Atlas::Objects::Operation::TICK_NO) {
//                    resOp->setTo(mind->getId());
//                    m_operationsDispatcher.addOperationToQueue(resOp, mind);
//                } else {
//                    res.push_back(resOp);
//                }
//            }
//
//            if (mind->isDestroyed()) {
//                removeLocatedEntity(mind);
//            }
//
//        } else {
//            auto pendingMindI = m_pendingMinds.find(op->getTo());
//            if (pendingMindI != m_pendingMinds.end()) {
//                pendingMindI->second.operation(op, res);
//            } else if (op->getTo() == m_account->getId()){
//                m_account->operation(op, res);
//            } else {
//                log(WARNING, String::compose("Got %1 operation with unknown 'to' (%2).", op->getParent(), op->getTo()));
//            }
//        }
//    }

    if (debug_flag) {
        for (auto resOp : res) {
            std::cout << "PossessionClient::operation sent {" << std::endl;
            debug_dump(resOp, std::cout);
            std::cout << "}" << std::endl << std::flush;
        }
    }

}

double PossessionClient::getTime() const
{
    SystemTime time{};
    time.update();
    return (double) (time.seconds()) + (double) time.microseconds() / 1000000.;
}


const std::unordered_map<std::string, Ref<BaseMind>>& PossessionClient::getMinds() const {
    return m_account->getMinds();
};
