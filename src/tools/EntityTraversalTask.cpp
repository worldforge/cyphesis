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

#include "EntityTraversalTask.h"

#include "common/serialno.h"

#include <Atlas/Objects/Operation.h>

using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Get;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Set;
using Atlas::Message::Element;

EntityTraversalTask::EntityTraversalTask(const std::string& accountId,
        std::function<bool(const Atlas::Objects::Entity::RootEntity&)>& visitor) :
        mAccountId(accountId), mVisitor(visitor), mSerial(0)
{
}

EntityTraversalTask::~EntityTraversalTask() = default;

void EntityTraversalTask::setup(const std::string & arg, OpVector & res)
{
    Anonymous get_arg;
    get_arg->setId("0");
    get_arg->setObjtype("obj");

    Get get;
    get->setArgs1(get_arg);
    get->setFrom(mAccountId);
    get->setSerialno(newSerialNo());
    mSerial = get->getSerialno();
    res.push_back(get);
}

void EntityTraversalTask::operation(const Operation & op, OpVector & res)
{
    if (!op->isDefaultRefno() && op->getRefno() == mSerial) {
        if (op->getClassNo() == Atlas::Objects::Operation::INFO_NO) {
            if (!op->getArgs().empty()) {
                auto arg = smart_dynamic_cast<RootEntity>(
                        op->getArgs().front());
                if (arg.isValid()) {
                    bool result = mVisitor(arg);
                    if (result) {
                        if (!arg->isDefaultContains()
                                && !arg->getContains().empty()) {
                            mStack.emplace_back();
                            mStack.back().children = arg->getContains();
                            mStack.back().currentChildIterator =
                                    mStack.back().children.begin();
                            getEntity(*mStack.back().currentChildIterator, res);
                            return;
                        } else {
                            while (!mStack.empty()) {
                                StackEntry& stackEntry = mStack.back();
                                ++stackEntry.currentChildIterator;
                                if (stackEntry.currentChildIterator
                                        == stackEntry.children.end()) {
                                    mStack.pop_back();
                                } else {
                                    getEntity(*stackEntry.currentChildIterator,
                                            res);
                                    return;
                                }
                            }
                            m_complete = true;
                        }
                    } else {
                        m_complete = true;
                    }
                }
            }
        }
    }
}

void EntityTraversalTask::getEntity(const std::string & id, OpVector & res)
{
    Anonymous get_arg;
    get_arg->setId(id);
    get_arg->setObjtype("obj");

    Get get;
    get->setArgs1(get_arg);
    get->setFrom(mAccountId);
    get->setSerialno(newSerialNo());
    mSerial = get->getSerialno();
    res.push_back(get);
}

