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
    std::string script_package;
    std::string script_class;

    if (getScriptDetails(script, class_name, "Task",
                         script_package, script_class) != 0) {
        return -1;
    }

    Element activation_attr;
    if (class_desc->copyAttr("activation", activation_attr) != 0 ||
        !activation_attr.isMap()) {
        log(ERROR, compose("Task \"%1\" has no activation.", class_name));
        return -1;
    }
    const MapType & activation = activation_attr.Map();

    auto act_end = activation.end();
    auto J = activation.find("tool");
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
            log(ERROR, compose(R"(Task "%1" activation has "%2" target.)",
                               class_name,
                               Element::typeName(J->second.getType())));
            return -1;
        }
        const std::string & target_desc = J->second.String();
        std::string::size_type dot = target_desc.find('.');
        if (dot != 0) {
            // If there is no dot, or the dot is not the first character,
            // then the first part of the string is the class to be matched
            // This works if there is a dot, or if the search returned npos
            std::string target_class = target_desc.substr(0, dot);
            const TypeNode * target_type = i.getType(target_class);
            if (target_type == nullptr) {
                dependent = target_class;
                reason = compose("Task \"%1\" is activated on target \"%2\" "
                                 "which does not exist.", class_name,
                                 target_class);
                return 1;
            }
            factory->setTarget(target_type);
        }
        if (dot != std::string::npos) {
            if (++dot < target_desc.size()) {
                std::string target_property = target_desc.substr(dot);
                factory->setRequireProperty(target_property);
            } else {
                log(WARNING, compose("Task rule \"%1\" provide empty string "
                                     "for activation property", class_name));
            }
        }
    }

    if (!i.hasClass(activation_tool)) {
        dependent = activation_tool;
        reason = compose("Task \"%1\" is activated by tool \"%2\" which "
                         "does not exist.", class_name, activation_tool);
        return 1;
    }
    auto* tool_factory = dynamic_cast<EntityFactoryBase*>(m_builder->getClassFactory(activation_tool));
    if (tool_factory == nullptr) {
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

    // FIXME This is the same code as EntityRuleHandler
    if (factory->m_scriptFactory == nullptr ||
        factory->m_scriptFactory->package() != script_package) {
        auto* ptsf = new PythonScriptFactory<Task>(script_package, script_class);

        if (ptsf->setup() == 0) {
            delete factory->m_scriptFactory;
            factory->m_scriptFactory = ptsf;
        } else {
            log(ERROR, compose("Python class \"%1.%2\" failed to load",
                               script_package, script_class));
            delete ptsf;
            return -1;
        }
    } else {
        // FIXME If this fails, that's bad.
        // If this fails the user needs to know, but the old values
        // should remain in place.
        factory->m_scriptFactory->refreshClass();
    }

    // FIXME This does not check for or remove old activations for this
    // factory
    m_builder->addTaskActivation(activation_tool, activation_op, factory);

    auto L = tool_factory->m_classAttributes.find("operations");
    if (L == tool_factory->m_classAttributes.end()) {
        tool_factory->m_classAttributes["operations"] = ListType(1, activation_op);
        tool_factory->m_attributes["operations"] = ListType(1, activation_op);
        tool_factory->updateProperties();
    } else {
        if (L->second.isList()) {
            ListType::const_iterator M = L->second.List().begin();
            for (; M != L->second.List().end() && *M != activation_op; ++M);
            if (M == L->second.List().end()) {
                L->second.List().push_back(activation_op);
                tool_factory->m_attributes[L->first] = L->second.List();
                tool_factory->updateProperties();
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
    if (factory == nullptr) {
        log(ERROR, compose("Could not find factory for existing task class "
                           "\"%1\"", class_name));
        return -1;
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

int TaskRuleHandler::check(const Atlas::Objects::Root & desc)
{
    assert(!desc->getParent().empty());
    return m_builder->isTask(desc->getParent()) ? 0 : -1;
}

int TaskRuleHandler::install(const std::string & name,
                             const std::string & parent,
                             const Atlas::Objects::Root & description,
                             std::string & dependent,
                             std::string & reason)
{
    return installTaskClass(name, parent, description, dependent, reason);
}

int TaskRuleHandler::update(const std::string & name,
                            const Atlas::Objects::Root & desc)
{
    return modifyTaskClass(name, desc);
}
