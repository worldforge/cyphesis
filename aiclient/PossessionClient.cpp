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
#include "config.h"
#endif

#include "PossessionClient.h"
#include "PossessionAccount.h"

#include "rulesets/BaseMind.h"

#include "common/Possess.h"
#include "common/log.h"
#include "common/compose.hpp"
#include "common/sockets.h"
#include "common/id.h"
#include "common/custom.h"
#include "common/SystemTime.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::RootOperation;

PossessionClient::PossessionClient(MindFactory& mindFactory) :
        m_mindFactory(mindFactory), m_account(nullptr), m_operationsDispatcher([&](const Operation & op, LocatedEntity & from) {this->operationFromEntity(op, from);},
                [&]()->double {return getTime();})
{

}

PossessionClient::~PossessionClient()
{
}

bool PossessionClient::idle()
{
    return m_operationsDispatcher.idle();
//
//    OpVector res;
//    for (auto& mind : m_minds) {
//        mind.second->idle(res);
//    }
//
//    for (auto& resOp : res) {
//        m_connection.send(resOp);
//    }
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

void PossessionClient::addMind(BaseMind* mind)
{
    m_minds.insert(std::make_pair(mind->getIntId(), mind));
    mind->incRef();
}

void PossessionClient::removeMind(BaseMind* mind)
{
    m_minds.erase(mind->getIntId());
    mind->decRef();
}

void PossessionClient::createAccount(const std::string& accountId)
{
    m_account = new PossessionAccount(accountId, integerId(accountId), *this, m_mindFactory);
    OpVector res;
    m_account->enablePossession(res);
    for (auto& op : res) {
        m_connection.send(op);
    }
}

void PossessionClient::operationFromEntity(const Operation & op, LocatedEntity& locatedEntity)
{
    if (!locatedEntity.isDestroyed()) {
        OpVector res;
        locatedEntity.operation(op, res);
        for (auto& resOp : res) {
            //All resulting ops should go out to the server, except for Ticks which we'll keep ourselves.
            if (resOp->getClassNo() == Atlas::Objects::Operation::TICK_NO) {
                resOp->setTo(resOp->getFrom());
                m_operationsDispatcher.addOperationToQueue(resOp, locatedEntity);
            } else {
                resOp->setFrom(locatedEntity.getId());
                send(resOp);
            }
        }
    }
}

void PossessionClient::operation(const Operation & op, OpVector & res)
{
    if (op->isDefaultTo() || op->getTo() == m_account->getId()) {
        OpVector accountRes;
        m_account->operation(op, accountRes);
        for (auto& resOp : accountRes) {
            //All resulting ops should go out to the server, except for Ticks which we'll keep ourselves.
            if (resOp->getClassNo() == Atlas::Objects::Operation::TICK_NO) {
                auto I = m_minds.find(integerId(resOp->getFrom()));
                if (I != m_minds.end()) {
                    resOp->setTo(resOp->getFrom());
                    m_operationsDispatcher.addOperationToQueue(resOp, *I->second);
                }
            } else {
                res.push_back(resOp);
            }
        }
    } else {
        auto mindI = m_minds.find(integerId(op->getTo()));
        if (mindI != m_minds.end()) {
            OpVector mindRes;
            mindI->second->operation(op, mindRes);

            for (auto& resOp : mindRes) {
                resOp->setFrom(mindI->second->getId());
                //All resulting ops should go out to the server, except for Ticks which we'll keep ourselves.
                if (resOp->getClassNo() == Atlas::Objects::Operation::TICK_NO) {
                    resOp->setTo(mindI->second->getId());
                    m_operationsDispatcher.addOperationToQueue(resOp, *mindI->second);
                } else {
                    res.push_back(resOp);
                }
            }

        } else {
            m_account->operation(op, res);
            //log(WARNING, String::compose("Got operation addressed to %1 for which there's no registered router.", op->getTo()));
        }
    }

//    if (!op->isDefaultRefno()) {
//        auto entry = m_refNoOperations.find(op->getRefno());
//        if (entry != m_refNoOperations.end()) {
//            //If there's a refno, alter the address of the operation
//            op->setTo(entry->second);
//            m_refNoOperations.erase(op->getRefno());
//        }
//    }

//    if (op->getTo() == m_playerId || op->isDefaultTo()) {
//        if (op->getClassNo() == Atlas::Objects::Operation::POSSESS_NO) {
//            PossessOperation(op, res);
//        } else if (op->getClassNo()
//                == Atlas::Objects::Operation::APPEARANCE_NO) {
//            //Ignore appearance ops, since they just signal other accounts being connected
//        } else if (op->getClassNo()
//                == Atlas::Objects::Operation::DISAPPEARANCE_NO) {
//            //Ignore disappearance ops, since they just signal other accounts being disconnected
//        } else {
//            log(NOTICE,
//                    String::compose("Unknown operation %1 in PossessionClient",
//                            op->getParents().front()));
//        }
//    } else {
//        auto mindI = m_minds.find(op->getTo());
//        if (mindI != m_minds.end()) {
//            mindI->second->operation(op, res);
//            if (mindI->second->isMindDestroyed()) {
//                log(INFO, "Removing AI mind as entity was deleted.");
//                //The mind was destroyed as a result of the operation; we should remove it.
//                m_minds.erase(mindI);
//            }
//        } else {
//            log(ERROR, "Op sent to unrecognized address.");
//        }
//    }
}

double PossessionClient::getTime() const
{
    SystemTime time;
    time.update();
    return (double)(time.seconds()) + (double)time.microseconds() / 1000000.;
}
