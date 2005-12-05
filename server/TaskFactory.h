// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SERVER_TASK_FACTORY_H
#define SERVER_TASK_FACTORY_H

#include <Python.h>

class Task;
class Character;

class ScriptFactory;

/// \brief Base class for for factories for creating task
///
/// An Entity consists of an instance of one of a number of C++ classes
/// optionally with a script. Stores information about default attributes,
/// script language and class name.
class TaskFactory {
  protected:
    TaskFactory();
  public:
    virtual ~TaskFactory();

    virtual Task * newTask(Character & chr) = 0;
};

class PythonTaskScriptFactory {
  public:
    PyObject * m_module;
    PyObject * m_class;

    int getClass();
    int addScript();
  public:
    PythonTaskScriptFactory();
    virtual ~PythonTaskScriptFactory();

    virtual Task * newTask(Character & chr);
};

#endif // SERVER_TASK_FACTORY_H
