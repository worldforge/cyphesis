// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef RULESETS_PYTHON_SCRIPT_H
#define RULESETS_PYTHON_SCRIPT_H

#include <Python.h>

#include "Script.h"

class Entity;

/// \brief Base Script class for Python scripts
class PythonScript : public Script {
  protected:
    PyObject * scriptObject;
  public:
    PythonScript(PyObject *);
    virtual ~PythonScript();
    virtual bool operation(const std::string &,
                           const Atlas::Objects::Operation::RootOperation &,
                           OpVector &,
                           const Atlas::Objects::Operation::RootOperation * = 0) = 0;
    virtual void hook(const std::string &, Entity *) = 0;
};

#endif // RULESETS_PYTHON_SCRIPT_H
