// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001 Alistair Riddoch
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
