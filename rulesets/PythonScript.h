// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef PYTHON_SCRIPT_H
#define PYTHON_SCRIPT_H

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
    virtual bool Operation(const string&,
                      const Atlas::Objects::Operation::RootOperation&, oplist&,
                      Atlas::Objects::Operation::RootOperation * sub_op=NULL) = 0;
    virtual void hook(const string &, Entity *) = 0;
};

#endif // PYTHON_SCRIPT_H
