/*
 Copyright (C) 2015 Erik Ogenvik

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

#include "RuleTraversalTask.h"

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
using Atlas::Message::ListType;

RuleTraversalTask::RuleTraversalTask(std::string accountId, std::function<bool(const Atlas::Objects::Root&)>& visitor)
    :
    mAccountId(std::move(accountId)),
    mVisitor(visitor),
    mSerial(0)
{
}

RuleTraversalTask::~RuleTraversalTask() = default;

void RuleTraversalTask::setup(const std::string& arg, OpVector& res)
{
    getRule(arg, res);
}

void RuleTraversalTask::operation(const Operation& op, OpVector& res)
{
    if (!op->isDefaultRefno() && op->getRefno() == mSerial) {
        if (op->getClassNo() == Atlas::Objects::Operation::INFO_NO) {
            if (!op->getArgs().empty()) {
                const auto& arg = op->getArgs().front();
                if (arg.isValid()) {
                    bool result = mVisitor(arg);
                    if (result) {
                        Element childrenElement;
                        if (arg->copyAttr("children", childrenElement) == 0
                            && childrenElement.isList()
                            && !childrenElement.List().empty()) {
                            const ListType& childrenList = childrenElement.asList();
                            mStack.emplace_back();
                            for (auto& childElement : childrenList) {
                                if (childElement.isString()) {
                                    mStack.back().children.push_back(childElement.String());
                                }
                            }
                            mStack.back().currentChildIterator = mStack.back().children.begin();
                            getRule(*mStack.back().currentChildIterator, res);
                            return;
                        } else {
                            while (!mStack.empty()) {
                                StackEntry& stackEntry = mStack.back();
                                ++stackEntry.currentChildIterator;
                                if (stackEntry.currentChildIterator == stackEntry.children.end()) {
                                    mStack.pop_back();
                                } else {
                                    getRule(*stackEntry.currentChildIterator, res);
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

void RuleTraversalTask::getRule(const std::string& id, OpVector& res)
{
    Get get;
    Anonymous arg;
    arg->setId(id);
    get->setArgs1(arg);
    get->setObjtype("op");
    get->setSerialno(newSerialNo());
    mSerial = get->getSerialno();
    res.push_back(get);
}

