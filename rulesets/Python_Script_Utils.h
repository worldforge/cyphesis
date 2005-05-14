// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef RULESETS_PYTHON_SCRIPT_UTILS_H
#define RULESETS_PYTHON_SCRIPT_UTILS_H

#include <Python.h>

#include <string>

class Entity;
class BaseMind;

void Create_PyEntity(Entity * entity, const std::string & package,
                                    const std::string & type);
void Create_PyMind(BaseMind * mind, const std::string & package,
                                    const std::string & type);
void Subscribe_Script(Entity *, PyObject *, const std::string &);
PyObject * Create_PyScript(PyObject *, PyObject *);

#endif // RULESETS_PYTHON_SCRIPT_UTILS_H
