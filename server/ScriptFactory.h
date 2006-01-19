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

#ifndef SERVER_SCRIPT_FACTORY_H
#define SERVER_SCRIPT_FACTORY_H

#include <Python.h>

#include <string>

class Entity;

class ScriptFactory {
  protected:
    std::string m_package;
    std::string m_type;

    ScriptFactory(const std::string & package, const std::string & type);
  public:
    virtual ~ScriptFactory();

    const std::string & package() { return m_package; }

    virtual int addScript(Entity * entity) = 0;
    virtual int refreshClass() = 0;
};

class PythonScriptFactory : public ScriptFactory {
  protected:
    PyObject * m_module;
    PyObject * m_class;

    int getClass();
  public:
    PythonScriptFactory(const std::string & package, const std::string & type);
    ~PythonScriptFactory();

    int addScript(Entity * entity);
    int refreshClass();

};

#endif // SERVER_SCRIPT_FACTORY_H
