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


#ifndef RULESETS_TASKS_PROPERTY_H
#define RULESETS_TASKS_PROPERTY_H

#include "modules/Ref.h"
#include "common/Property.h"
#include <map>

class Task;

/// \brief Keeps track of any ongoing tasks.
/// \ingroup PropertyClasses
class TasksProperty : public PropertyBase
{
    protected:
        std::map<std::string, Ref<Task>> m_tasks;
    public:
        static constexpr const char* property_name = "tasks";
        static constexpr const char* property_atlastype = "map";

        explicit TasksProperty();

        void install(LocatedEntity*, const std::string&) override;

        void remove(LocatedEntity*, const std::string& name) override;

        int get(Atlas::Message::Element& val) const override;

        void set(const Atlas::Message::Element& val) override;

        TasksProperty* copy() const override;

        int updateTask(LocatedEntity* owner, OpVector& res);

        int startTask(const std::string& id, Ref<Task> task,
                      LocatedEntity* owner,
                      OpVector& res);

        int clearTask(const std::string& id, LocatedEntity* owner, OpVector& res);

        void stopTask(const std::string& id, LocatedEntity* owner, OpVector& res);

        HandlerResult TickOperation(LocatedEntity* owner, const Operation& op, OpVector&);

        HandlerResult UseOperation(LocatedEntity* owner, const Operation& op, OpVector&);

        HandlerResult operation(LocatedEntity* owner,
                                const Operation& op,
                                OpVector&) override;
};

#endif // RULESETS_TASKS_PROPERTY_H
