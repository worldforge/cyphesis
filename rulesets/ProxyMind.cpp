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

#include "ProxyMind.h"
#include "Script.h"

#include "common/custom.h"
#include "common/Think.h"
#include "common/debug.h"

#include <Atlas/Objects/Operation.h>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

using Atlas::Objects::smart_dynamic_cast;

static const bool debug_flag = false;

ProxyMind::ProxyMind(const std::string & id, long intId) :
        BaseMind(id, intId)
{

}

ProxyMind::~ProxyMind()
{
}

void ProxyMind::thinkSetOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        debug(std::cout << " no args!" << std::endl << std::flush
        ;);
        return;
    }
    for (const Root& arg : args) {
        if (arg->isDefaultId()) {
            m_randomThoughts.push_back(arg);
        } else {
            m_thoughtsWithId[arg->getId()] = arg;
        }
    }
}

void ProxyMind::thinkDeleteOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        //No args means "delete all"
        m_thoughtsWithId.clear();
        m_randomThoughts.clear();
    } else {
        for (const Root& arg : args) {
            if (arg->isDefaultId()) {
                log(WARNING, "Thought in Delete operation had no id set, ignoring.");
            } else {
                m_thoughtsWithId.erase(arg->getId());
            }
        }
    }
}

void ProxyMind::thinkGetOperation(const Operation & op, OpVector & res)
{
    Atlas::Objects::Operation::Think think;
    Atlas::Objects::Operation::Set set;
    if (op->getSerialno()) {
        think->setRefno(op->getSerialno());
    }
    std::vector<Root> thoughts;
    if (op->getArgs().empty()) {
        thoughts = getThoughts();
    } else {
        //If there's any arguments, we'll use the key of the first entry to filter on predicates
        auto frontArg = op->getArgs().front();
        Atlas::Message::MapType frontArgMap;
        frontArg->addToMessage(frontArgMap);
        if (!frontArgMap.empty()) {
            auto searchTerm = frontArgMap.begin()->first;
            for (auto& thought : m_thoughtsWithId) {
                if (thought.second->hasAttr(searchTerm)) {
                    thoughts.push_back(thought.second);
                }
            }
            for (auto& thought : m_randomThoughts) {
                if (thought->hasAttr(searchTerm)) {
                    thoughts.push_back(thought);
                }
            }
        }
    }
    set->setArgs(thoughts);
    think->setArgs1(set);
    res.push_back(think);
}

void ProxyMind::thinkLookOperation(const Operation & op, OpVector & res)
{
    Atlas::Objects::Operation::Think think;
    Atlas::Objects::Operation::Info info;
    if (op->getSerialno()) {
        think->setRefno(op->getSerialno());
    }
    std::vector<Root> thoughts;
    if (op->getArgs().empty()) {
        thoughts = getThoughts();
    } else {
        //If there's any arguments, we'll use the key of the first entry to filter on predicates
        auto frontArg = op->getArgs().front();
        Atlas::Message::MapType frontArgMap;
        frontArg->addToMessage(frontArgMap);
        if (!frontArgMap.empty()) {
            auto searchTerm = frontArgMap.begin()->first;
            for (auto& thought : m_thoughtsWithId) {
                if (thought.second->hasAttr(searchTerm)) {
                    thoughts.push_back(thought.second);
                }
            }
            for (auto& thought : m_randomThoughts) {
                if (thought->hasAttr(searchTerm)) {
                    thoughts.push_back(thought);
                }
            }
        }
    }
    info->setArgs(thoughts);
    think->setArgs1(info);
    res.push_back(think);
}

std::vector<Atlas::Objects::Root> ProxyMind::getThoughts() const
{
    std::vector<Atlas::Objects::Root> thoughts = m_randomThoughts;
    for (auto& thought : m_thoughtsWithId) {
        thoughts.push_back(thought.second);
    }
    return thoughts;
}

void ProxyMind::clearThoughts()
{
    m_randomThoughts.clear();
    m_thoughtsWithId.clear();
}

