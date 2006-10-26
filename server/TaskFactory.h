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

// $Id: TaskFactory.h,v 1.6 2006-10-26 00:48:15 alriddoch Exp $

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
    std::string m_target;

    virtual ~TaskFactory();

    virtual Task * newTask(Character & chr) = 0;
};

class PythonTaskScriptFactory : public TaskFactory {
  protected:
    PyObject * m_module;
    PyObject * m_class;

    std::string m_package;
    std::string m_name;

    int getClass();
    int addScript();
  public:
    PythonTaskScriptFactory(const std::string & package,
                            const std::string & name);
    virtual ~PythonTaskScriptFactory();

    virtual Task * newTask(Character & chr);
};

#endif // SERVER_TASK_FACTORY_H
