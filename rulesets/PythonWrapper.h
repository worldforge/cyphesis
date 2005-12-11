// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef RULESETS_PYTHON_WRAPPER_H
#define RULESETS_PYTHON_WRAPPER_H

#include <Python.h>

#include "Script.h"

/// \brief Wrapper class for entities without scripts with wrappers
class PythonWrapper : public Script {
  protected:
    PyObject * m_wrapper;
  public:
    explicit PythonWrapper(PyObject * wrapper);
    virtual ~PythonWrapper();

    PyObject * wrapper() const { return m_wrapper; }
};

#endif // RULESETS_PYTHON_WRAPPER_H
