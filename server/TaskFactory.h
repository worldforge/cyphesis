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

// $Id: TaskFactory.h,v 1.9 2006-12-24 17:18:55 alriddoch Exp $

#ifndef SERVER_TASK_FACTORY_H
#define SERVER_TASK_FACTORY_H

#include <Python.h>

#include <string>

class Task;
class Character;

class ScriptFactory;

/// \brief Base class for for factories for creating task
///
/// An Entity consists of an instance of one of a number of C++ classes
/// optionally with a script. Stores information about default attributes,
/// script language and class name.
class TaskFactory {
  public:
    /// \brief Type name of the base entity class this task works on
    std::string m_target;

    virtual ~TaskFactory();

    /// \brief Create a new task
    ///
    /// @param chr the character performing the task
    virtual Task * newTask(Character & chr) = 0;
};

/// \brief Factory for creating tasks implemented as python scripts.
class PythonTaskScriptFactory : public TaskFactory {
  protected:
    /// \brief Python module object containing the script type
    PyObject * m_module;
    /// \brief Python class object for the script type
    PyObject * m_class;

    /// \brief Name of the package containing the script
    std::string m_package;
    /// \brief Name of the type within the package for the script
    std::string m_name;

    // FIXME #3 Work out what was intended by these methods, currently
    // unimplemented.
    int getClass();
    int addScript();
  public:
    PythonTaskScriptFactory(const std::string & package,
                            const std::string & name);
    virtual ~PythonTaskScriptFactory();

    virtual Task * newTask(Character & chr);
};

#endif // SERVER_TASK_FACTORY_H
