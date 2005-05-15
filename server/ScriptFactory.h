// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

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
