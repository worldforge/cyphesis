// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
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

#ifndef RULESETS_TASK_SCRIPT_H
#define RULESETS_TASK_SCRIPT_H

#include "rulesets/Task.h"

class Script;

/// \brief Class for handling tasks which are implemented as scripts
class TaskScript : public Task {
  protected:
    /// \brief The language script that will handle this task
    Script * m_script;
  private:
    /// \brief Private and un-implemented, to make sure slicing is impossible
    TaskScript(const TaskScript & t);
    /// \brief Private and un-implemented, to make sure slicing is impossible
    const TaskScript & operator=(const TaskScript &);
  public:
    explicit TaskScript(Character & chr);
    virtual ~TaskScript();

    /// \brief Accessor for the script that handles this task
    Script * script() const {
        return m_script;
    }

    void setScript(Script * scrpt);

    virtual void irrelevant();

    virtual void initTask(const Operation & op, OpVector & res);
    virtual void TickOperation(const Operation & op, OpVector & res);
};

#endif // RULESETS_TASK_SCRIPT_H
