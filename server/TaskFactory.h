// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SERVER_TASK_FACTORY_H
#define SERVER_TASK_FACTORY_H

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

class TaskScriptFactory {
  public:
    ScriptFactory * m_scriptFactory;

    TaskScriptFactory();
    virtual ~TaskScriptFactory();

    virtual Task * newTask(Character & chr);
};

#endif // SERVER_TASK_FACTORY_H
