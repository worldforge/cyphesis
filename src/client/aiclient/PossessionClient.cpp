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


#include "common/operations/Possess.h"
#include "common/id.h"
#include "common/custom.h"
#include "common/Inheritance.h"

#include "common/debug.h"
#include "common/CommSocket.h"

#include <Atlas/Objects/Entity.h>

#include <chrono>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::RootOperation;

PossessionClient::PossessionClient(CommSocket& commSocket,
                                   MindKit& mindFactory,
                                   Atlas::Objects::Factories& factories,
                                   std::function<void()> reconnectFn) :
    BaseClient(commSocket),
    m_mindFactory(mindFactory),
    m_reconnectFn(std::move(reconnectFn)),
    m_account(nullptr),
    m_operationsDispatcher([&](const Operation& op, Ref<BaseMind> from) { this->operationFromEntity(op, std::move(from)); },
                           [&]() -> double { return getTime(); }),
    m_factories(factories),
    m_inheritance(new Inheritance(m_factories)),
    m_dispatcherTimer(commSocket.m_io_context)
{


}


PossessionClient::~PossessionClient()
{
    if (m_reconnectFn) {
        m_reconnectFn();
    }
}

void PossessionClient::notifyAccountCreated(const std::string& accountId)
{
    log(INFO, "Creating possession account on server.");
    m_account = std::make_unique<PossessionAccount>(accountId, integerId(accountId), m_mindFactory, *this);
    OpVector res;
    m_account->enablePossession(res);
    for (auto& op : res) {
        send(op);
    }
}


void PossessionClient::operationFromEntity(const Operation& op, Ref<BaseMind> locatedEntity)
{
    if (!locatedEntity->isDestroyed()) {
        OpVector res;
        operation(op, res);
        send(res);
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
    bool updatedDispatcher = false;

    for (auto& resOp : accountRes) {
        if (debug_flag) {
            std::cout << "PossessionClient::operation return {" << std::endl;
            debug_dump(resOp, std::cout);
            std::cout << "}" << std::endl << std::flush;
        }
        //Any op with both "from" and "to" set should be re-sent.
        if ((!resOp->isDefaultTo() && !resOp->isDefaultFrom())) {
            auto mind = m_account->findMindForId(resOp->getTo());
            if (mind) {
                m_operationsDispatcher.addOperationToQueue(resOp, std::move(mind));
                updatedDispatcher = true;
            } else {
                log(WARNING, String::compose("Resulting op of type '%1' is set to the mind with id '%2', which can't be found.", resOp->getParent(), resOp->getTo()));
            }
        } else {
            //log(INFO, String::compose("Out %1 from %2", resOp->getParent(), resOp->getFrom()));
            res.push_back(resOp);
        }
    }
    if (updatedDispatcher) {
        scheduleDispatch();
    }
}

double PossessionClient::getTime() const
{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() / 1000000.0;
}


const std::unordered_map<std::string, Ref<BaseMind>>& PossessionClient::getMinds() const
{
    return m_account->getMinds();
}


void PossessionClient::scheduleDispatch()
{
    m_dispatcherTimer.cancel();
    auto waitTime = m_operationsDispatcher.timeUntilNextOp();
    m_dispatcherTimer.expires_from_now(waitTime);

    m_dispatcherTimer.async_wait([&](boost::system::error_code ec) {
        if (!ec) {
            m_operationsDispatcher.idle(std::chrono::steady_clock::now() + std::chrono::milliseconds(10));
            scheduleDispatch();
        }
    });

}
