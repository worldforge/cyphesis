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
#include "BaseWorld.h"

#include "entityfilter/Providers.h"

#include "rulesets/python/CyPy_LocatedEntity.h"
#include "rulesets/python/CyPy_Operation.h"
#include "rulesets/python/CyPy_Oplist.h"
#include "common/debug.h"
#include "common/AtlasQuery.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>

#include <wfmath/atlasconv.h>
#include <rulesets/python/CyPy_Point3D.h>
#include <rulesets/python/CyPy_EntityLocation.h>
#include <rulesets/python/CyPy_UsageInstance.h>

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
            //TODO: check that the op is a subtype of "action"?
            if (usageEntry.second.isMap()) {
                auto map = usageEntry.second.Map();

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
                    usage.constraint.reset(new EntityFilter::Filter(value, new EntityFilter::ProviderFactory()));
                });
                AtlasQuery::find<Atlas::Message::ListType>(map, "targets", [&](const Atlas::Message::ListType& value) {
                    for (auto& entry : value) {
                        if (entry.isString()) {
                            usage.targets.emplace_back(new EntityFilter::Filter(entry.String(), new EntityFilter::ProviderFactory()));
                        }
                    }
                });
                AtlasQuery::find<Atlas::Message::ListType>(map, "consumes", [&](const Atlas::Message::ListType& value) {
                    for (auto& entry : value) {
                        if (entry.isString()) {
                            usage.consumed.emplace_back(new EntityFilter::Filter(entry.String(), new EntityFilter::ProviderFactory()));
                        }
                    }
                });
                m_usages.emplace(usageEntry.first, std::move(usage));
            }
        }
    }
}

void UsagesProperty::install(LocatedEntity* owner, const std::string& name)
{
    owner->installDelegate(Atlas::Objects::Operation::USE_NO, name);
}


void UsagesProperty::remove(LocatedEntity* owner, const std::string& name)
{
    owner->removeDelegate(Atlas::Objects::Operation::USE_NO, name);
}

HandlerResult UsagesProperty::operation(LocatedEntity* e,
                                        const Operation& op, OpVector& res)
{
    return use_handler(e, op, res);
}

HandlerResult UsagesProperty::use_handler(LocatedEntity* e,
                                          const Operation& op, OpVector& res)
{


    auto actor = BaseWorld::instance().getEntity(op->getFrom());
    if (!actor) {
        e->error(op, "Could not find 'from' entity.", res, e->getId());
        return OPERATION_IGNORED;
    }
    
    if (op->isDefaultFrom()) {
        actor->error(op, "Top op has no 'from' attribute.", res, actor->getId());
        return OPERATION_IGNORED;
    }
    
    if (!op->getArgs().empty()) {
        auto& arg = op->getArgs().front();
        auto argOp = smart_dynamic_cast<Atlas::Objects::Operation::RootOperation>(arg);
        if (!argOp) {
            actor->error(op, "First arg wasn't an operation.", res, actor->getId());
            return OPERATION_IGNORED;
        }


        if (!argOp->hasAttrFlag(Atlas::Objects::PARENT_FLAG)) {
            actor->error(op, "Use arg op has malformed parent", res, actor->getId());
            return OPERATION_IGNORED;
        }
        auto op_type = argOp->getParent();
        debug_print("Got op type " << op_type << " from arg");

        auto obj = Atlas::Objects::Factories::instance()->createObject(op_type);
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
        rop->setTo(e->getId());
        rop->setSeconds(op->getSeconds());

        //Optionally extract any involved entities
        std::vector<EntityLocation> involvedEntities;
        auto& arg_op_args = argOp->getArgs();
        for (const auto& arg_op_arg : arg_op_args) {
            auto entity_arg = smart_dynamic_cast<RootEntity>(arg_op_arg);
            if (!entity_arg.isValid()) {
                actor->error(op, "Use op involved is malformed", res, actor->getId());
                return OPERATION_IGNORED;
            }

            if (!entity_arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
                actor->error(op, "Involved entity has no ID", res, actor->getId());
                return OPERATION_IGNORED;
            }

            auto involved = BaseWorld::instance().getEntity(entity_arg->getId());
            if (!involved) {
                actor->error(op, "Involved entity does not exist", res, actor->getId());
                return OPERATION_IGNORED;
            }
            if (!entity_arg->isDefaultPos()) {
                involvedEntities.emplace_back(involved, WFMath::Point<3>(Element(entity_arg->getPosAsList()).List()));
            } else {
                involvedEntities.emplace_back(involved, WFMath::Point<3>());
            }

        }

        //Check that there's an action registered for this operation
        auto usagesI = m_usages.find(op_type);
        if (usagesI != m_usages.end()) {
            auto& usage = usagesI->second;

            //Check that the tool is ready
            auto toolReadyAtProp = e->getPropertyType<double>("ready_at");
            if (toolReadyAtProp) {
                if (toolReadyAtProp->data() > BaseWorld::instance().getTime()) {
                    actor->clientError(op, "Tool is not ready yet.", res, actor->getId());
                    return OPERATION_IGNORED;
                }
            }

            //Check if the tools is attached, and if so the attachment is ready
            auto actorReadyAtProp = actor->getPropertyType<MapType>("_ready_at_attached");
            if (actorReadyAtProp) {
                //FIXME: don't hardcode this, instead get it from the tools "planted_on" prop
                auto attachI = actorReadyAtProp->data().find("right_hand_wield");
                if (attachI != actorReadyAtProp->data().end()) {
                    if (attachI->second.isFloat() && attachI->second.Float() > BaseWorld::instance().getTime()) {
                        actor->clientError(op, "Actor is not ready yet.", res, actor->getId());
                        return OPERATION_IGNORED;
                    }
                }
            }

            //Check that the number of involved entities match targets and consumed.
            if (usage.targets.size() + usage.consumed.size() == involvedEntities.size()) {
                std::vector<EntityLocation> targets;
                std::vector<EntityLocation> consumed;
                for (size_t i = 0; i < usage.targets.size(); ++i) {
                    targets.push_back(involvedEntities[i]);
                }
                for (size_t i = 0; i < usage.consumed.size(); ++i) {
                    consumed.push_back(involvedEntities[usage.targets.size() + i]);
                }

                UsageInstance usageInstance{usage, actor, e, std::move(targets), std::move(consumed), rop};
                //Check that the usage is valid before continuing
                auto validRes = usageInstance.isValid();
                if (!validRes.first) {
                    actor->clientError(op, validRes.second, res, actor->getId());
                } else {
                    auto lastSeparatorPos = usage.handler.find_last_of('.');
                    if (lastSeparatorPos != std::string::npos) {
                        auto moduleName = usage.handler.substr(0, lastSeparatorPos);
                        auto functionName = usage.handler.substr(lastSeparatorPos + 1);
                        Py::Module module(PyImport_Import(Py::String(moduleName).ptr()));
                        PyImport_ReloadModule(module.ptr());
                        auto functionObject = module.getDict()[functionName];
                        if (!functionObject.isCallable()) {
                            actor->error(op, String::compose("Could not find Python function %1", usage.handler), res, actor->getId());
                            return OPERATION_IGNORED;
                        }

                        try {
                            auto ret = Py::Callable(functionObject).apply(Py::TupleN(CyPy_UsageInstance::wrap(std::move(usageInstance))));
                            return processScriptResult(usage.handler, ret, res, e);
                        } catch (const Py::BaseException& py_ex) {
                            log(ERROR, String::compose("Python error calling \"%1\" for entity %2", usage.handler, e->describeEntity()));
                            if (PyErr_Occurred()) {
                                PyErr_Print();
                            }
                        }
                    }
                }
                return OPERATION_BLOCKED;

            }
        }
    }
    //We couldn't find any suitable task.
    return OPERATION_IGNORED;
}


HandlerResult UsagesProperty::processScriptResult(const std::string& scriptName, const Py::Object& ret, OpVector& res, LocatedEntity* e)
{
    HandlerResult result = OPERATION_IGNORED;

    auto processPythonResultFn = [&](const Py::Object& pythonResult) {
        if (pythonResult.isLong()) {
            auto numRet = Py::Long(pythonResult).as_long();
            if (numRet == 0) {
                result = OPERATION_IGNORED;
            } else if (numRet == 1) {
                result = OPERATION_HANDLED;
            } else if (numRet == 2) {
                result = OPERATION_BLOCKED;
            } else {
                log(ERROR, String::compose("Unrecognized return code %1 for script '%2' attached to entity '%3'", numRet, scriptName, e->describeEntity()));
            }

        } else if (CyPy_Operation::check(pythonResult)) {
            auto operation = CyPy_Operation::value(pythonResult);
            assert(operation);
            operation->setFrom(e->getId());
            res.push_back(std::move(operation));
        } else if (CyPy_Oplist::check(pythonResult)) {
            auto& o = CyPy_Oplist::value(pythonResult);
            for (auto& opRes : o) {
                opRes->setFrom(e->getId());
                res.push_back(opRes);
            }
        } else {
            log(ERROR, String::compose("Python script \"%1\" returned an invalid "
                                       "result.", scriptName));
        }
    };

    if (ret.isNone()) {
        debug(std::cout << "Returned none" << std::endl << std::flush;);
    } else {
        //Check if it's a tuple and process it.
        if (ret.isTuple()) {
            for (auto item : Py::Tuple(ret)) {
                processPythonResultFn(item);
            }
        } else {
            processPythonResultFn(ret);
        }
    }

    return result;
}
