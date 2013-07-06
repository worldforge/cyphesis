// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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


#include "WorldLoader.h"

#include "tools/MultiLineListFormatter.h"
#include "tools/ObjectContext.h"

#include "common/AtlasFileLoader.h"
#include "common/compose.hpp"
#include "common/log.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <iostream>

using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Get;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Set;

using boost::shared_ptr;

StackEntry::StackEntry(const Atlas::Objects::Entity::RootEntity & o,
                       const std::list<std::string>::const_iterator & c) :
            obj(o), child(c)
{
}

StackEntry::StackEntry(const Atlas::Objects::Entity::RootEntity & o) :
            obj(o)
{
    child = obj->getContains().end();
}

void WorldLoader::getEntity(const std::string & id, OpVector & res)
{
    std::map<std::string, Root>::const_iterator I = m_objects.find(id);
    if (I == m_objects.end()) {
        std::cerr << "Inconsistency in dump file: "
                  << id << " missing"
                  << std::endl << std::flush;
        return;
    }
    RootEntity obj = smart_dynamic_cast<RootEntity>(I->second);
    if (!obj.isValid()) {
        std::cerr << "Corrupt dump - non entity found " << id
                  << std::endl << std::flush;
        return;
    }

    m_state = WALKING;
    m_treeStack.push_back(StackEntry(obj));

    Anonymous get_arg;
    get_arg->setId(id);
    get_arg->setObjtype("obj");

    Get get;
    get->setArgs1(get_arg);
    get->setFrom(m_account);
    ++m_lastSerialNo;
    get->setSerialno(m_lastSerialNo);
    res.push_back(get);
}

void WorldLoader::walk(OpVector & res)
{
    assert(!m_treeStack.empty());
    StackEntry & current = m_treeStack.back();
    if (current.obj->getContains().empty()) {
        // Pop: Go back to WALKING parent
        assert(!m_treeStack.empty());
        m_treeStack.pop_back();
        while (!m_treeStack.empty()) {
            StackEntry & se = m_treeStack.back();
            ++se.child;
            if (se.child != se.obj->getContains().end()) {
                getEntity(*se.child, res);
                break;
            }
            m_treeStack.pop_back();
        }
        if (m_treeStack.empty()) {
            std::cout << "Restore done" << std::endl << std::flush;
            std::cout << "Restored " << m_count
                      << ", created(" << m_createCount
                      << ", updated(" << m_updateCount
                      << std::endl << std::flush;
            m_complete = true;
        }
    } else {
        // Start WALKING the current entity
        assert(current.child == current.obj->getContains().end());
        current.child = current.obj->getContains().begin();
        assert(current.child != current.obj->getContains().end());

        getEntity(*current.child, res);
    }
}

void WorldLoader::create(const RootEntity & obj, OpVector & res)
{
    ++m_count;
    ++m_createCount;

    m_state = CREATING;

    assert (m_treeStack.size() > 1);
    std::deque<StackEntry>::reverse_iterator I = m_treeStack.rbegin();
    ++I;
    assert(I != m_treeStack.rend());
    const std::string & loc = I->restored_id;

    RootEntity create_arg = obj.copy();

    create_arg->removeAttrFlag(Atlas::Objects::Entity::CONTAINS_FLAG);
    create_arg->removeAttrFlag(Atlas::Objects::Entity::VELOCITY_FLAG);
    create_arg->removeAttrFlag(Atlas::Objects::ID_FLAG);
    create_arg->setLoc(loc);

    Create create;
    create->setArgs1(create_arg);
    m_agent_context->setFromContext(create);
    ++m_lastSerialNo;
    create->setSerialno(m_lastSerialNo);

    res.push_back(create);
}

void WorldLoader::errorArrived(const Operation & op, OpVector & res)
{
    if (op->isDefaultRefno() || op->getRefno() != m_lastSerialNo) {
        return;
    }
    switch (m_state) {
      case WALKING:
        {
            assert(!m_treeStack.empty());
            StackEntry & current = m_treeStack.back();
            RootEntity obj = current.obj;

            assert(obj.isValid());

            create(obj, res);
        }
        break;
      case CREATING:
        std::cerr << "Could not create" << std::endl << std::flush;
        m_complete = true;
        break;
      default:
        std::cerr << "Unexpected state in state machine"
                  << std::endl << std::flush;
        break;
    };
}

void WorldLoader::infoArrived(const Operation & op, OpVector & res)
{
    if (op->isDefaultRefno() || op->getRefno() != m_lastSerialNo) {
        return;
    }
    if (op->isDefaultArgs() || op->getArgs().empty()) {
        std::cerr << "Info with no arg" << std::endl << std::flush;
        return;
    }
    if (m_state != WALKING) {
        return;
    }
    const Root & arg = op->getArgs().front();
    RootEntity ent = smart_dynamic_cast<RootEntity>(arg);
    if (!ent.isValid()) {
        std::cerr << "Info response is not entity"
                  << std::endl << std::flush;
        return;
    }
    if (arg->isDefaultId()) {
        std::cerr << "Corrupted info response: no id"
                  << std::endl << std::flush;
    }
    const std::string & id = arg->getId();

    StackEntry & current = m_treeStack.back();
    RootEntity obj = current.obj;

    assert(id == obj->getId());

    if (m_newIds.find(id) != m_newIds.end() ||
        (m_treeStack.size() != 1 && ent->isDefaultLoc()) ||
        ent->getParents().front() != obj->getParents().front()) {
        create(obj, res);
    } else {

        Root update = obj.copy();

        current.restored_id = id;

        std::cout << "Updating: " << obj->getId() << ","
                  << obj->getParents().front()
                  << std::endl << std::flush;

        // assert(id == "0");

        update->removeAttrFlag(Atlas::Objects::Entity::CONTAINS_FLAG);
        update->removeAttrFlag(Atlas::Objects::STAMP_FLAG);

        Set set;
        set->setArgs1(update);
        m_agent_context->setFromContext(set);
        set->setTo(id);
        ++m_lastSerialNo;
        set->setSerialno(m_lastSerialNo);
    
        res.push_back(set);

        ++m_count;
        ++m_updateCount;

        m_state = UPDATING;
    }
}

void WorldLoader::sightArrived(const Operation & op, OpVector & res)
{
    if (op->isDefaultArgs() || op->getArgs().empty()) {
        std::cerr << "No arg" << std::endl << std::flush;
        return;
    }
    const Root & arg = op->getArgs().front();
    switch (m_state) {
      case INIT:
        if (op->isDefaultRefno() || op->getRefno() != m_lastSerialNo) {
            break;
        }
        if (arg->isDefaultId()) {
            std::cerr << "Corrupted top level entity: no id"
                      << std::endl << std::flush;
        }

        getEntity(arg->getId(), res);

        // Expecting sight of world root
        break;
      case UPDATING:
        {
            Operation sub_op = smart_dynamic_cast<Operation>(arg);
            if (!sub_op.isValid()) {
                break;
            }
            if (sub_op->getClassNo() != Atlas::Objects::Operation::SET_NO ||
                sub_op->getArgs().empty() ||
                sub_op->isDefaultSerialno() ||
                sub_op->getSerialno() != m_lastSerialNo) {
                std::cerr << "This is not our entity update response"
                          << std::endl << std::flush;
                break;
            }
            walk(res);
        }
        break;
      case CREATING:
        {
            Operation sub_op = smart_dynamic_cast<Operation>(arg);
            if (!sub_op.isValid()) {
                break;
            }
            if (sub_op->getClassNo() != Atlas::Objects::Operation::CREATE_NO ||
                sub_op->getArgs().empty() ||
                sub_op->isDefaultSerialno() ||
                sub_op->getSerialno() != m_lastSerialNo) {
                std::cerr << "This is not our entity create response"
                          << std::endl << std::flush;
                break;
            }
            Root created = sub_op->getArgs().front();
            m_newIds.insert(created->getId());
            StackEntry & current = m_treeStack.back();
            current.restored_id = created->getId();
            std::cout << "Created: " << created->getParents().front()
                      << "(" << created->getId() << ")"
                      << std::endl << std::flush;
            walk(res);
        }
        break;
      default:
        std::cerr << "Unexpected state in state machine"
                  << std::endl << std::flush;
        break;
    };
}

WorldLoader::WorldLoader(const std::string & accountId,
                         const shared_ptr<ObjectContext> & context) :
                                                        m_account(accountId),
                                                        m_context(context),
                                                        m_lastSerialNo(-1),
                                                        m_count(0),
                                                        m_updateCount(0),
                                                        m_createCount(0),
                                                        m_state(INIT)
{
}

WorldLoader::~WorldLoader()
{
}

void WorldLoader::setup(const std::string & arg, OpVector & res)
{
    m_agent_context = m_context.lock();

    if (!m_agent_context) {
        m_complete = true;
        return;
    }

    std::string filename("world.xml");
    if (!arg.empty()) {
        filename = arg;
    }

    AtlasFileLoader loader(filename, m_objects);

    if (!loader.isOpen()) {
        log(ERROR, String::compose("Unable to open %1", filename));
        m_complete = true;
        return;
    }
    loader.read();
    std::cout << "LOADED " << m_objects.size() << std::endl << std::flush;
    m_description = String::compose("restoring %1", filename);

    Look l;

    m_agent_context->setFromContext(l);
    ++m_lastSerialNo;
    l->setSerialno(m_lastSerialNo);

    res.push_back(l);

    m_agent_context.reset();
}

void WorldLoader::operation(const Operation & op, OpVector & res)
{
    m_agent_context = m_context.lock();

    if (!m_agent_context) {
        m_complete = true;
        return;
    }

    if (op->getClassNo() == Atlas::Objects::Operation::INFO_NO) {
        infoArrived(op, res);
    } else if (op->getClassNo() == Atlas::Objects::Operation::ERROR_NO) {
        errorArrived(op, res);
    } else if (op->getClassNo() == Atlas::Objects::Operation::SIGHT_NO) {
        sightArrived(op, res);
    }

    m_agent_context.reset();
}
