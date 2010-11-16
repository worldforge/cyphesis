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

#ifndef SERVER_TASK_SCRIPT_FACTORY_H
#define SERVER_TASK_SCRIPT_FACTORY_H

#include <string>

class TaskScript;

/// \brief Factory interface for creating scripts to attach to in game
/// task objects.
class TaskScriptKit {
  protected:
    /// \brief Name of the script package where the script type is
    std::string m_package;
    /// \brief Name of the script type instanced by this factory
    std::string m_type;

    TaskScriptKit(const std::string & package, const std::string & type);
  public:
    virtual ~TaskScriptKit();

    /// \brief Accessor for package name
    const std::string & package() { return m_package; }

    /// \brief Add a script to an task
    virtual int addScript(TaskScript * task) = 0;
    /// \brief Reload the underlying class object from the script on disk
    virtual int refreshClass() = 0;
};

/// \brief Factory implementation for creating python script objects to attach
/// to in game task objects.
class PythonTaskScriptFactory : public TaskScriptKit {
  protected:
    /// \brief Module object which contains the script class
    struct _object * m_module;
    /// \brief Class object to be instanced when creating scripts
    struct _object * m_class;

    int getClass();
  public:
    PythonTaskScriptFactory(const std::string & package, const std::string & type);
    ~PythonTaskScriptFactory();

    int addScript(TaskScript * task);
    int refreshClass();

    bool isOkay() const {
        return m_class != 0;
    }
};

#endif // SERVER_TASK_SCRIPT_FACTORY_H
