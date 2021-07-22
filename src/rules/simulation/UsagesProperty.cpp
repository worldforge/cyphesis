/*
 Copyright (C) 2018 Erik Ogenvik

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

#include "UsagesProperty.h"
#include "rules/simulation/BaseWorld.h"

#include "rules/entityfilter/ProviderFactory.h"

#include "common/debug.h"
#include "common/AtlasQuery.h"
#include "ScriptUtils.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>

#include <wfmath/atlasconv.h>
#include <rules/simulation/python/CyPy_UsageInstance.h>
#include "pythonbase/Python_API.h"
#include <common/Inheritance.h>

static const bool debug_flag = false;
using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Action;
using Atlas::Objects::Operation::Use;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;


using Atlas::Objects::smart_dynamic_cast;

void UsagesProperty::set(const Atlas::Message::Element& val)
{
    Property::set(val);

    m_usages.clear();

    for (auto& usageEntry : m_data) {
        if (!usageEntry.first.empty()) {
            try {
                //TODO: check that the op is a subtype of "action"?
                if (usageEntry.second.isMap()) {
                    auto& map = usageEntry.second.Map();

                    Usage usage;

                    AtlasQuery::find<std::string>(map, "handler", [&](const std::string& value) {
                        usage.handler = value;
                    });
                    if (usage.handler.empty()) {
                        continue;
                    }

                    AtlasQuery::find<std::string>(map, "description", [&](const std::string& value) {
                        usage.description = value;
                    });
                    AtlasQuery::find<std::string>(map, "constraint", [&](const std::string& value) {
                        //TODO: should be a usage constraint provider factory
                        usage.constraint.reset(new EntityFilter::Filter(value, EntityFilter::ProviderFactory()));
                    });
                    AtlasQuery::find<Atlas::Message::MapType>(map, "params", [&](const Atlas::Message::MapType& value) {
                        for (auto& entry : value) {
                            auto parameter = UsageParameter::parse(entry.second);
                            usage.params.emplace(entry.first, std::move(parameter));
                        }
                    });

                    m_usages.emplace(usageEntry.first, std::move(usage));
                }
            } catch (const std::invalid_argument& e) {
                log(ERROR, String::compose("Could not install usage '%1' : %2", usageEntry.first, e.what()));
            }
        }
    }
}

void UsagesProperty::install(LocatedEntity& owner, const std::string& name)
{
    owner.installDelegate(Atlas::Objects::Operation::USE_NO, name);
}


void UsagesProperty::remove(LocatedEntity& owner, const std::string& name)
{
    owner.removeDelegate(Atlas::Objects::Operation::USE_NO, name);
}

HandlerResult UsagesProperty::operation(LocatedEntity& e,
                                        const Operation& op, OpVector& res)
{
    return use_handler(e, op, res);
}

HandlerResult UsagesProperty::use_handler(LocatedEntity& e,
                                          const Operation& op, OpVector& res)
{

    if (!op->getArgs().empty()) {
        auto& arg = op->getArgs().front();
        auto argOp = smart_dynamic_cast<Atlas::Objects::Operation::RootOperation>(arg);
        if (!argOp) {
            //This op is not for us
            return OPERATION_IGNORED;
        }

        auto actor = BaseWorld::instance().getEntity(op->getFrom());
        if (!actor) {
            e.error(op, "Could not find 'from' entity.", res, e.getId());
            return OPERATION_IGNORED;
        }

        if (op->isDefaultFrom()) {
            actor->error(op, "Top op has no 'from' attribute.", res, actor->getId());
            return OPERATION_IGNORED;
        }

        if (!argOp->hasAttrFlag(Atlas::Objects::PARENT_FLAG)) {
            actor->error(op, "Use arg op has malformed parent", res, actor->getId());
            return OPERATION_IGNORED;
        }
        auto op_type = argOp->getParent();
        debug_print("Got op type " << op_type << " from arg")

        auto obj = Inheritance::instance().getFactories().createObject(op_type);
        if (!obj.isValid()) {
            log(ERROR, String::compose("Character::UseOperation Unknown op type "
                                       "\"%1\".", op_type));
            return OPERATION_IGNORED;
        }

        auto rop = smart_dynamic_cast<Operation>(obj);
        if (!rop.isValid()) {
            log(ERROR, String::compose("Character::UseOperation Op type "
                                       "\"%1\" but it is not an operation type. ", op_type));
            return OPERATION_IGNORED;
        }
        rop->setFrom(actor->getId());
        rop->setTo(e.getId());
        rop->setSeconds(op->getSeconds());

        if (argOp->getArgs().empty()) {
            actor->error(op, "Use arg op has no arguments; one expected.", res, actor->getId());
            return OPERATION_IGNORED;
        }

        auto arguments = argOp->getArgs().front();

        //Check that there's an action registered for this operation
        auto usagesI = m_usages.find(op_type);
        if (usagesI != m_usages.end()) {
            auto& usage = usagesI->second;

            //Populate the usage arguments
            std::map<std::string, std::vector<UsageParameter::UsageArg>> usage_instance_args;

            for (auto& param : usage.params) {
                Atlas::Message::Element element;
                if (arguments->copyAttr(param.first, element) != 0 || !element.isList()) {
                    actor->clientError(op, String::compose("Could not find required list argument '%1'.", param.first), res, actor->getId());
                    return OPERATION_IGNORED;
                }

                auto& argVector = usage_instance_args[param.first];

                for (auto& argElement : element.List()) {
                    switch (param.second.type) {
                        case UsageParameter::Type::ENTITY:
                        case UsageParameter::Type::ENTITYLOCATION: {
                            if (!argElement.isMap()) {
                                actor->clientError(op, String::compose("Inner argument in list of arguments for '%1' was not a map.", param.first), res, actor->getId());
                                return OPERATION_IGNORED;
                            }
                            //The arg is for an RootEntity, expressed as a message. Extract id and pos.
                            auto idI = argElement.Map().find("id");
                            if (idI == argElement.Map().end() || !idI->second.isString()) {
                                actor->clientError(op, String::compose("Inner argument in list of arguments for '%1' had no id string.", param.first), res, actor->getId());
                                return OPERATION_IGNORED;
                            }

                            auto involved = BaseWorld::instance().getEntity(idI->second.String());
                            if (!involved) {
                                actor->error(op, "Involved entity does not exist", res, actor->getId());
                                return OPERATION_IGNORED;
                            }

                            auto posI = argElement.Map().find("pos");
                            if (posI != argElement.Map().end() && posI->second.isList()) {
                                argVector.emplace_back(EntityLocation(involved, WFMath::Point<3>(posI->second)));
                            } else {
                                argVector.emplace_back(EntityLocation(involved));
                            }
                        }
                            break;
                        case UsageParameter::Type::POSITION:
                            argVector.emplace_back(WFMath::Point<3>(argElement));
                            break;
                        case UsageParameter::Type::DIRECTION:
                            //Normalize the entry just to make sure.
                            argVector.emplace_back(WFMath::Vector<3>(argElement).normalize());
                            break;
                    }
                }

            }


            UsageInstance usageInstance{usage, actor, &e, std::move(usage_instance_args), rop};
            //Check that the usage is valid before continuing
            auto validRes = usageInstance.isValid();
            if (!validRes.first) {
                actor->clientError(op, validRes.second, res, actor->getId());
            } else {
                auto lastSeparatorPos = usage.handler.find_last_of('.');
                if (lastSeparatorPos != std::string::npos) {
                    auto moduleName = usage.handler.substr(0, lastSeparatorPos);
                    auto functionName = usage.handler.substr(lastSeparatorPos + 1);
                    //Py::Module module(moduleName);
                    Py::Module module(PyImport_Import(Py::String(moduleName).ptr()));
                    //PyImport_ReloadModule(module.ptr());
                    auto functionObject = module.getDict()[functionName];
                    if (!functionObject.isCallable()) {
                        actor->error(op, String::compose("Could not find Python function %1", usage.handler), res, actor->getId());
                        return OPERATION_IGNORED;
                    }

                    try {

                        PythonLogGuard logGuard([functionName, actor]() {
                            return String::compose("Usage '%1', entity %2: ", functionName, actor->describeEntity());
                        });
                        auto ret = Py::Callable(functionObject).apply(Py::TupleN(UsageInstance::scriptCreator(std::move(usageInstance))));
                        return ScriptUtils::processScriptResult(usage.handler, ret, res, e);
                    } catch (const Py::BaseException& py_ex) {
                        log(ERROR, String::compose("Python error calling \"%1\" for entity %2", usage.handler, e.describeEntity()));
                        if (PyErr_Occurred()) {
                            PyErr_Print();
                        }
                    }
                }
            }
            return OPERATION_BLOCKED;

        }
    }
    //We couldn't find any suitable task.
    return OPERATION_IGNORED;
}

UsagesProperty* UsagesProperty::copy() const {
    return new UsagesProperty(*this);
}

