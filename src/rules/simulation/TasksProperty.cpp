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


#include "common/operations/Tick.h"
#include <Atlas/Objects/Operation.h>
#include "TasksProperty.h"

#include "rules/LocatedEntity.h"
#include "Task.h"

#include "common/debug.h"
#include "common/operations/Update.h"
#include "BaseWorld.h"

#include <wfmath/atlasconv.h>
#include <common/Inheritance.h>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Update;

static const bool debug_flag = false;

static const std::string SERIALNO = "serialno";

TasksProperty::TasksProperty()
        : PropertyBase(prop_flag_persistence_ephem)
{
}

int TasksProperty::get(Atlas::Message::Element& val) const
{
    MapType tasks;
    for (const auto& entry : m_tasks) {
        auto& task = entry.second.task;
        MapType taskMap;
        taskMap.emplace("name", task->name());
        auto progress = task->progress();
        if (progress > 0) {
            taskMap.emplace("progress", progress);
        }
        if (task->m_duration) {
            taskMap.emplace("rate", 1.0f / *task->m_duration);
        }
        if (!task->usages().empty()) {
            ListType usagesList;
            for (auto& usage : task->usages()) {

                MapType paramsMap;
                for (auto& param : usage.params) {
                    MapType paramMap;
                    if (param.second.max != 1) {
                        paramMap.emplace("max", param.second.max);
                    }
                    if (param.second.min != 1) {
                        paramMap.emplace("min", param.second.min);
                    }
                    if (param.second.constraint) {
                        paramMap.emplace("constraint", param.second.constraint->getDeclaration());
                    }
                    switch (param.second.type) {
                        case UsageParameter::Type::DIRECTION:
                            paramMap.emplace("type", "direction");
                            break;
                        case UsageParameter::Type::ENTITY:
                            paramMap.emplace("type", "entity");
                            break;
                        case UsageParameter::Type::ENTITYLOCATION:
                            paramMap.emplace("type", "entitylocation");
                            break;
                        case UsageParameter::Type::POSITION:
                            paramMap.emplace("type", "position");
                            break;
                    }
                    paramsMap.emplace(param.first, std::move(paramMap));
                }

                usagesList.emplace_back(Atlas::Message::MapType{{"name",   usage.name},
                                                                {"params", std::move(paramsMap)}});
            }
            taskMap.emplace("usages", std::move(usagesList));
        }
        tasks.emplace(entry.first, std::move(taskMap));
    }

    val = std::move(tasks);
    return 0;
}

void TasksProperty::set(const Atlas::Message::Element& val)
{
    log(ERROR, "Cannot set 'tasks' property.");
}

TasksProperty* TasksProperty::copy() const
{
    //No copy ctor for this.
    return new TasksProperty();
}

int TasksProperty::updateTask(LocatedEntity& owner, OpVector& res)
{
    m_flags.addFlags(prop_flag_unsent);

    owner.enqueueUpdateOp(res);

    return 0;
}

int TasksProperty::startTask(const std::string& id, Ref<Task> task,
                             LocatedEntity& owner,
                             OpVector& res)
{
    bool update_required = false;

    auto tasksI = m_tasks.find(id);

    if (tasksI != m_tasks.end()) {
        update_required = true;
        m_tasks.erase(id);
    }

    task->initTask(id, res);

    if (!task->obsolete()) {
        assert(!res.empty());
        m_tasks.emplace(id, TaskEntry{task});
        update_required = true;
    } else {
        task = nullptr;
    }

    if (update_required) {
        updateTask(owner, res);
    }

    return task ? 0 : -1;

}

int TasksProperty::clearTask(const std::string& id, LocatedEntity& owner, OpVector& res)
{
    if (m_tasks.empty()) {
        // This function should never be called when there is no task,
        // except during Entity destruction
        assert(owner.hasFlags(entity_destroyed));
        return -1;
    }
    m_tasks.erase(id);

    return updateTask(owner, res);
}

void TasksProperty::stopTask(const std::string& id, LocatedEntity& owner, OpVector& res)
{
    // This is just clearTask without an assert
    if (m_tasks.find(id) == m_tasks.end()) {
        log(ERROR, "Tasks property stop when no task");
        return;
    }

    m_tasks.erase(id);

    updateTask(owner, res);
}

HandlerResult TasksProperty::TickOperation(LocatedEntity& owner,
                                           const Operation& op,
                                           OpVector& res)
{

    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        return OPERATION_BLOCKED;
    }

    const Root& arg = args.front();

    if (arg->isDefaultId()) {
        return OPERATION_BLOCKED;
    }

    auto taskI = m_tasks.find(arg->getId());
    if (taskI == m_tasks.end()) {
        return OPERATION_BLOCKED;
    }

    //Make a copy, because the task might be removed as a result of the tick, and the ref count will make it still be alive.
    auto id = taskI->first;
    auto task = taskI->second.task;

    Element serialno;
    if (arg->copyAttr(SERIALNO, serialno) == 0 && (serialno.isInt())) {
        if (serialno.asInt() != task->serialno()) {
            debug_print("Old tick")
            return OPERATION_BLOCKED;
        }
    } else {
        log(ERROR, "Character::TickOperation: No serialno in tick arg");
        return OPERATION_BLOCKED;
    }
    bool hadChange = task->tick(id, op, res);
    if (task->obsolete()) {
        clearTask(id, owner, res);
    } else {
        if (hadChange) {
            updateTask(owner, res);
        }
    }

    if (task != nullptr && res.empty()) {
        log(WARNING,
            String::compose("Character::%1: Task %2 on entity %3 has stalled, i.e. it's still active but didn't return anything from latest Tick op.", __func__, task->name(), owner.describeEntity()));
    }
    return OPERATION_BLOCKED;
}

namespace {
    std::pair<bool, std::string> areUsageParamsValid(const std::map<std::string, UsageParameter>& params,
                                                     const std::map<std::string, std::vector<UsageParameter::UsageArg>>& args,
                                                     const UsageInstance& usageInstance)
    {

        for (auto& param : params) {
            auto I = args.find(param.first);
            if (I == args.end()) {
                return {false, String::compose("Could not find required '%1' argument.", param.first)};
            }

            std::vector<std::string> errorMessages;
            int count = param.second.countValidArgs(I->second, usageInstance.actor, usageInstance.tool, errorMessages);

            if (count < param.second.min) {
                if (!errorMessages.empty()) {
                    return {false, *errorMessages.begin()};
                } else {
                    return {false, String::compose("Too few '%1' arguments. Should be minimum %2, got %3.", param.first, param.second.min, count)};
                }
            }
            if (count > param.second.max) {
                return {false, String::compose("Too many '%1' arguments. Should be maximum %2, got %3.", param.first, param.second.max, count)};
            }

        }


        return {true, ""};
    }
}

HandlerResult TasksProperty::UseOperation(LocatedEntity& e,
                                          const Operation& op,
                                          OpVector& res)
{

    if (!op->getArgs().empty()) {
        auto& arg = op->getArgs().front();
        if (arg->isDefaultObjtype() || arg->getObjtype() != "task") {
            //This op is not for us
            return OPERATION_IGNORED;
        }

        auto actor = BaseWorld::instance().getEntity(op->getFrom());
        if (!actor) {
            e.error(op, "Could not find 'from' entity.", res, e.getId());
            return OPERATION_BLOCKED;
        }

        if (op->isDefaultFrom()) {
            actor->error(op, "Top op has no 'from' attribute.", res, actor->getId());
            return OPERATION_BLOCKED;
        }

        if (arg->isDefaultId()) {
            actor->error(op, "Use arg for task has no id", res, actor->getId());
            return OPERATION_BLOCKED;
        }
        auto taskId = arg->getId();

        auto taskI = m_tasks.find(taskId);
        if (taskI == m_tasks.end()) {
            //Task doesn't exist anymore, just ignore.
            return OPERATION_IGNORED;
        }

        auto task = taskI->second.task;

        if (!arg->hasAttr("args")) {
            actor->error(op, "Use arg for task has no args", res, actor->getId());
            return OPERATION_BLOCKED;
        }

        auto argsElem = arg->getAttr("args");
        if (!argsElem.isList()) {
            actor->error(op, "Use arg for task has invalid args", res, actor->getId());
            return OPERATION_BLOCKED;
        }

        auto innerArgs = Inheritance::instance().getFactories().parseListOfObjects(argsElem.asList());

        if (innerArgs.empty()) {
            actor->error(op, "Use arg for task has empty args", res, actor->getId());
            return OPERATION_BLOCKED;
        }

        auto innerArg = innerArgs.front();

        if (innerArg->isDefaultId()) {
            actor->error(op, "Use arg for task has arg without id", res, actor->getId());
            return OPERATION_BLOCKED;
        }

        auto usageId = innerArg->getId();

        auto usageI = std::find_if(task->usages().begin(), task->usages().end(), [&](const TaskUsage& it) -> bool { return it.name == usageId; });
        if (usageI == task->usages().end()) {
            actor->error(op, String::compose("Usage does %1 not exist in task %2.", usageId, taskId), res, actor->getId());
            return OPERATION_BLOCKED;
        }

        auto& usage = *usageI;
        std::map<std::string, std::vector<UsageParameter::UsageArg>> usage_instance_args;


        for (auto& param : usage.params) {
            Atlas::Message::Element argumentElement;
            auto result = innerArg->copyAttr(param.first, argumentElement);

            if (result != 0 || !argumentElement.isList()) {
                actor->clientError(op, String::compose("Could not find required list argument '%1'.", param.first), res, actor->getId());
                return OPERATION_IGNORED;
            }

            auto& argVector = usage_instance_args[param.first];

            for (auto& argElement : argumentElement.List()) {
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


        auto validRes = areUsageParamsValid(usage.params, usage_instance_args, task->m_usageInstance);

        //Call a script method named after the usage, with "_usage" as suffix.
        task->callUsageScriptFunction(usageId + "_usage", usage_instance_args, res);

        if (task->obsolete()) {
            clearTask(taskId, e, res);
        } else {
            updateTask(e, res);
        }

        return OPERATION_BLOCKED;


    }
    //We couldn't find any suitable task.
    return OPERATION_IGNORED;

}


HandlerResult TasksProperty::operation(LocatedEntity& owner,
                                       const Operation& op,
                                       OpVector& res)
{
    if (op->getClassNo() == Atlas::Objects::Operation::TICK_NO) {
        auto& args = op->getArgs();
        if (!args.empty()) {
            auto& arg = args.front();
            if (arg->getName() == "task") {
                TickOperation(owner, op, res);
                return OPERATION_BLOCKED;
            }
        }
    } else if (op->getClassNo() == Atlas::Objects::Operation::USE_NO) {
        return UseOperation(owner, op, res);
    }

    return OPERATION_IGNORED;
}

void TasksProperty::install(LocatedEntity& owner, const std::string& name)
{
    owner.installDelegate(Atlas::Objects::Operation::TICK_NO, name);
    owner.installDelegate(Atlas::Objects::Operation::USE_NO, name);
}

void TasksProperty::remove(LocatedEntity& owner, const std::string& name)
{
    owner.removeDelegate(Atlas::Objects::Operation::TICK_NO, name);
    owner.removeDelegate(Atlas::Objects::Operation::USE_NO, name);
}
