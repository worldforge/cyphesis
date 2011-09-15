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

// $Id$

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "TaskRuleHandler.h"

#include "EntityBuilder.h"
#include "EntityFactory.h"
#include "ScriptFactory.h"
#include "TaskFactory.h"
#include "TaskScriptFactory.h"
#include "Persistence.h"
#include "Player.h"

#include "rulesets/MindFactory.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/globals.h"
#include "common/const.h"
#include "common/Inheritance.h"
#include "common/AtlasFileLoader.h"
#include "common/compose.hpp"
#include "common/Property.h"
#include "common/TypeNode.h"
#include "common/PropertyManager.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/objectFactory.h>

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
                                         std::string & reason)
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

    MapType::const_iterator J = script.find("name");
    MapType::const_iterator script_end = script.end();
    if (J == script_end || !J->second.isString()) {
        log(ERROR, compose("Task \"%1\" script has no name.", class_name));
        return -1;
    }
    const std::string & script_name = J->second.String();

    J = script.find("language");
    if (J == script_end || !J->second.isString()) {
        log(ERROR, compose("Task \"%1\" script has no language.", class_name));
        return -1;
    }
    const std::string & script_language = J->second.String();

    if (script_language != "python") {
        log(ERROR, compose("Task \"%1\" script has unknown language \"%2\".",
                           class_name, script_language));
        return -1;
    }
    std::string::size_type ptr = script_name.rfind(".");
    if (ptr == std::string::npos) {
        log(ERROR, compose("Task \"%1\" python script has a bad class "
                           "name \"%2\".", class_name, script_name));
        return -1;
    }
    std::string script_package = script_name.substr(0, ptr);
    std::string script_class = script_name.substr(ptr + 1);

    Element activation_attr;
    if (class_desc->copyAttr("activation", activation_attr) != 0 ||
        !activation_attr.isMap()) {
        log(ERROR, compose("Task \"%1\" has no activation.", class_name));
        return -1;
    }
    const MapType & activation = activation_attr.Map();

    MapType::const_iterator act_end = activation.end();
    J = activation.find("tool");
    if (J == act_end || !J->second.isString()) {
        log(ERROR, compose("Task \"%1\" activation has no tool.", class_name));
        return -1;
    }
    const std::string & activation_tool = J->second.String();

    J = activation.find("operation");
    if (J == act_end || !J->second.isString()) {
        log(ERROR, compose("Task \"%1\" activation has no operation.",
                           class_name));
        return -1;
    }

    const std::string & activation_op = J->second.String();

    Inheritance & i = Inheritance::instance();

    J = activation.find("target");
    if (J != act_end) {
        if (!J->second.isString()) {
            log(ERROR, compose("Task \"%1\" activation has \"%2\" target.",
                               class_name,
                               Element::typeName(J->second.getType())));
            return -1;
        }
        const std::string & target_base = J->second.String();
        if (!i.hasClass(target_base)) {
            dependent = target_base;
            reason = compose("Task \"%1\" is activated on target \"%2\" "
                             "which does not exist.", class_name,
                             target_base);
            return 1;
        }
        factory->m_target = target_base;
    }

    if (!i.hasClass(activation_tool)) {
        dependent = activation_tool;
        reason = compose("Task \"%1\" is activated by tool \"%2\" which "
                         "does not exist.", class_name, activation_tool);
        return 1;
    }
    EntityKit * tool_factory = m_builder->getClassFactory(activation_tool);
    if (tool_factory == 0) {
        log(ERROR, compose("Task class \"%1\" is activated by tool \"%2\" "
                           "which is not an entity class.", class_name,
                           activation_tool));
        return -1;
    }

    if (!i.hasClass(activation_op)) {
        dependent = activation_op;
        reason = compose("Task \"%1\" is activated by operation \"%2\" "
                         "which does not exist.", class_name, 
                         activation_op);
        return 1;
    }

    if (factory->m_scriptFactory != 0) {
        if (factory->m_scriptFactory->package() != script_package) {
            delete factory->m_scriptFactory;
            factory->m_scriptFactory = 0;
        }
    }
    if (factory->m_scriptFactory == 0) {
        PythonTaskScriptFactory * ptsf = new PythonTaskScriptFactory(script_package,
                                                                     script_class);
        if (ptsf->isOkay()) {
            factory->m_scriptFactory = ptsf;
        } else {
            delete ptsf;
            return -1;
        }
    }

    // FIXME This does not check for or remove old activations for this
    // factory
    m_builder->addTaskActivation(activation_tool, activation_op, factory);

    MapType::iterator L = tool_factory->m_classAttributes.find("operations");
    if (L == tool_factory->m_classAttributes.end()) {
        tool_factory->m_classAttributes["operations"] = ListType(1, activation_op);
        tool_factory->m_attributes["operations"] = ListType(1, activation_op);
        tool_factory->updateChildren();
        tool_factory->updateChildrenProperties();
    } else {
        if (L->second.isList()) {
            ListType::const_iterator M = L->second.List().begin();
            for (; M != L->second.List().end() && *M != activation_op; ++M);
            if (M == L->second.List().end()) {
                L->second.List().push_back(activation_op);
                tool_factory->m_attributes[L->first] = L->second.List();
                tool_factory->updateChildren();
                tool_factory->updateChildrenProperties();
            }
        }
    }
    
    return 0;
}

int TaskRuleHandler::installTaskClass(const std::string & class_name,
                                      const std::string & parent,
                                      const Root & class_desc,
                                      std::string & dependent,
                                      std::string & reason)
{
    assert(class_name == class_desc->getId());

    if (m_builder->hasTask(class_name)) {
        log(ERROR, compose("Attempt to install task \"%1\" which is already "
                           "installed.", class_name));
        return -1;
    }
    
    TaskKit * factory = new TaskFactory(class_name);

    int ret = populateTaskFactory(class_name, factory, class_desc,
                                  dependent, reason);
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
                                     const Root & class_desc)
{
    assert(class_name == class_desc->getId());

    TaskKit * factory = m_builder->getTaskFactory(class_name);
    if (factory == 0) {
        log(ERROR, compose("Could not find factory for existing task class "
                           "\"%1\"", class_name));
        return -1;
    }

    assert(factory != 0);

    TaskScriptKit * script_factory = factory->m_scriptFactory;
    if (script_factory != 0) {
        script_factory->refreshClass();
    }

    std::string dependent, reason;
    int ret = populateTaskFactory(class_name, factory, class_desc,
                                  dependent, reason);
    if (ret != 0) {
        if (ret > 0) {
            log(ERROR, reason);
        }
        return -1;
    }

    return 0;
}
