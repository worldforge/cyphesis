// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef RULESETS_PYTHON_SCRIPT_H
#define RULESETS_PYTHON_SCRIPT_H

#include "Script.h"

#include <Python.h>

class Entity;

class PythonScript : public Script {
  protected:
    PyObject * scriptObject;
    Entity & entity;
  public:
    PythonScript(PyObject *, Entity &);
    virtual ~PythonScript();
    virtual bool Operation(const std::string &, const RootOperation &,
                           OpVector &, RootOperation * sub_op = 0) = 0;
    virtual void hook(const std::string &, Entity *) = 0;
};

#endif // RULESETS_PYTHON_SCRIPT_H
