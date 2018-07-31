// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
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


#include "TaskRuleHandler.h"

#include "EntityBuilder.h"
#include "TaskFactory.h"
#include "EntityFactory.h"

#include "rulesets/PythonScriptFactory.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/Inheritance.h"
#include "common/compose.hpp"

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;

using String::compose;

static const bool debug_flag = false;

int TaskRuleHandler::populateTaskFactory(const std::string & class_name,
                                         TaskKit * factory,
                                         const Root & class_desc,
                                         std::string & dependent,
                                         std::string & reason,
                                         std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
{
    // assert(class_name == class_desc->getId());

    // Establish that this rule has an associated script.
    Element script_attr;
    if (class_desc->copyAttr("script", script_attr) != 0 ||
        !script_attr.isMap()) {
        log(ERROR, compose("Task \"%1\" has no script.", class_name));
        return -1;
    }
    const MapType & script = script_attr.Map();
    std::string script_package;
    std::string script_class;

    if (getScriptDetails(script, class_name, "Task",
                         script_package, script_class) != 0) {
        return -1;
    }


    // FIXME This is the same code as EntityRuleHandler
//    if (factory->m_scriptFactory == nullptr ||
//        factory->m_scriptFactory->package() != script_package) {
//        auto* ptsf = new PythonScriptFactory<Task>(script_package, script_class);
//
//        if (ptsf->setup() == 0) {
//            delete factory->m_scriptFactory;
//            factory->m_scriptFactory = ptsf;
//        } else {
//            log(ERROR, compose("Python class \"%1.%2\" failed to load",
//                               script_package, script_class));
//            delete ptsf;
//            return -1;
//        }
//    } else {
//        // FIXME If this fails, that's bad.
//        // If this fails the user needs to know, but the old values
//        // should remain in place.
//        factory->m_scriptFactory->refreshClass();
//    }

    return 0;
}

int TaskRuleHandler::installTaskClass(const std::string & class_name,
                                      const std::string & parent,
                                      const Root & class_desc,
                                      std::string & dependent,
                                      std::string & reason,
                                      std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
{
    assert(class_name == class_desc->getId());

    if (m_builder->hasTask(class_name)) {
        log(ERROR, compose("Attempt to install task \"%1\" which is already "
                           "installed.", class_name));
        return -1;
    }
    
    TaskKit * factory = new TaskFactory(class_name);

    int ret = populateTaskFactory(class_name, factory, class_desc,
                                  dependent, reason, changes);
    if (ret != 0) {
        delete factory;
        return ret;
    }
    m_builder->installTaskFactory(class_name, factory);

    Inheritance & i = Inheritance::instance();

    i.addChild(class_desc);

    return 0;
}

int TaskRuleHandler::modifyTaskClass(const std::string & class_name,
                                     const Root & class_desc,
                                     std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
{
    assert(class_name == class_desc->getId());

    TaskKit * factory = m_builder->getTaskFactory(class_name);
    if (factory == nullptr) {
        log(ERROR, compose("Could not find factory for existing task class "
                           "\"%1\"", class_name));
        return -1;
    }

    std::string dependent, reason;
    int ret = populateTaskFactory(class_name, factory, class_desc,
                                  dependent, reason, changes);
    if (ret != 0) {
        if (ret > 0) {
            log(ERROR, reason);
        }
        return -1;
    }

    return 0;
}

int TaskRuleHandler::check(const Atlas::Objects::Root & desc)
{
    assert(!desc->getParent().empty());
    return m_builder->isTask(desc->getParent()) ? 0 : -1;
}

int TaskRuleHandler::install(const std::string & name,
                             const std::string & parent,
                             const Atlas::Objects::Root & description,
                             std::string & dependent,
                             std::string & reason,
                             std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
{
    return installTaskClass(name, parent, description, dependent, reason, changes);
}

int TaskRuleHandler::update(const std::string & name,
                            const Atlas::Objects::Root & desc,
                            std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
{
    return modifyTaskClass(name, desc, changes);
}
