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

// $Id$

#ifndef COMMON_TASK_KIT_H
#define COMMON_TASK_KIT_H

#include <string>

class LocatedEntity;
class Task;
class TypeNode;

template<class T>
class ScriptKit;

/// \brief Factory interface for for factories for creating tasks
///
/// An Entity consists of an instance of one of a number of C++ classes
/// optionally with a script. Stores information about default attributes,
/// script language and class name.
class TaskKit {
  protected:
    TaskKit();

    /// \brief Type of the base entity class this task works on
    const TypeNode * m_target;

    /// \brief Name of a property the target must have
    std::string m_property;
  public:
    ScriptKit<Task> * m_scriptFactory;

    virtual ~TaskKit();

    const TypeNode * setTarget(const TypeNode * t)
    {
        m_target = t;
        return m_target;
    }

    const std::string & setRequireProperty(const std::string & p)
    {
        m_property = p;
        return m_property;
    }

    /// \brief Check target matches
    ///
    /// @param target the entity the task is to be performed on
    virtual int checkTarget(LocatedEntity * target) = 0;

    /// \brief Create a new task
    ///
    /// @param chr the character performing the task
    virtual Task * newTask(LocatedEntity & chr) = 0;
};

#endif // COMMON_TASK_KIT_H
