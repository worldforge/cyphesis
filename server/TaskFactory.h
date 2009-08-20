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

#ifndef SERVER_TASK_FACTORY_H
#define SERVER_TASK_FACTORY_H

#include <Python.h>

#include <string>

class Character;
class ScriptFactory;
class Task;
class TaskScriptKit;

/// \brief Factory interface for for factories for creating tasks
///
/// An Entity consists of an instance of one of a number of C++ classes
/// optionally with a script. Stores information about default attributes,
/// script language and class name.
class TaskKit {
  protected:
    TaskKit();
  public:
    TaskScriptKit * m_scriptFactory;
    /// \brief Type name of the base entity class this task works on
    std::string m_target;

    virtual ~TaskKit();

    /// \brief Create a new task
    ///
    /// @param chr the character performing the task
    virtual Task * newTask(Character & chr) = 0;
};

/// \brief Factory for creating tasks implemented as python scripts.
class TaskFactory : public TaskKit {
  protected:
    /// \brief Name of the class within the package for the script
    std::string m_name;

  public:
    TaskFactory(const std::string & name);
    virtual ~TaskFactory();

    virtual Task * newTask(Character & chr);
};

#endif // SERVER_TASK_FACTORY_H
