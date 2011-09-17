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

#ifndef SERVER_TASK_RULE_HANDLER_H
#define SERVER_TASK_RULE_HANDLER_H

#include "RuleHandler.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>

class EntityBuilder;
class TaskKit;

/// \brief Handle processing and updating of task ruless
class TaskRuleHandler : public RuleHandler {
  protected:
    EntityBuilder * const m_builder;

    int populateTaskFactory(const std::string & class_name,
                            TaskKit * factory,
                            const Atlas::Objects::Root & class_desc,
                            std::string & dependent,
                            std::string & reason);

    int installTaskClass(const std::string & class_name,
                         const std::string & parent,
                         const Atlas::Objects::Root & class_desc,
                         std::string & dependent,
                         std::string & reason);

    int modifyTaskClass(const std::string & class_name,
                        const Atlas::Objects::Root & class_desc);
  public:
    TaskRuleHandler(EntityBuilder * eb) : m_builder(eb) { }

    virtual int check(const Atlas::Objects::Root & desc);
    virtual int install(const std::string &,
                        const std::string &,
                        const Atlas::Objects::Root & desc,
                        std::string &,
                        std::string &);
    virtual int update(const std::string &,
                       const Atlas::Objects::Root & desc);
};

#endif // SERVER_TASK_RULE_HANDLER_H

