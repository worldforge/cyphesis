// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef RULESETS_PYTHON_SCRIPT_H
#define RULESETS_PYTHON_SCRIPT_H

#include <Python.h>

#include "PythonWrapper.h"

class Entity;

/// \brief Base Script class for Python scripts
class PythonScript : public PythonWrapper {
  protected:
    PyObject * scriptObject;
  public:
    PythonScript(PyObject * object, PyObject * wrapper);
    virtual ~PythonScript();
};

#endif // RULESETS_PYTHON_SCRIPT_H
