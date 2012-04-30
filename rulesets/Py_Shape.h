// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
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

#ifndef RULESETS_PY_SHAPE_H
#define RULESETS_PY_SHAPE_H

#include <Python.h>

class Shape;
template <int dim>
class Form;

/// \brief Wrapper for Shape in Python
/// \ingroup PythonWrappers
typedef struct {
    PyObject_HEAD
    /// \brief Shape object handled by this wrapper
    union {
      Shape * s;
      Form<2> * p;
      Form<3> * b;
    } shape;
} PyShape;

extern PyTypeObject PyShape_Type;

extern PyTypeObject PyArea_Type;
extern PyTypeObject PyBody_Type;

extern PyTypeObject PyBox_Type;
extern PyTypeObject PyCourse_Type;
extern PyTypeObject PyLine_Type;
extern PyTypeObject PyPolygon_Type;

#define PyShape_Check(_o) PyObject_TypeCheck(_o, &PyShape_Type)
#define PyShape_CheckExact(_o) (Py_TYPE(_o) == &PyShape_Type)

#define PyArea_Check(_o) PyObject_TypeCheck(_o, &PyArea_Type)
#define PyBody_Check(_o) PyObject_TypeCheck(_o, &PyBody_Type)

#define PyBox_Check(_o) PyObject_TypeCheck(_o, &PyBox_Type)
#define PyBox_CheckExact(_o) (Py_TYPE(_o) == &PyBox_Type)

#define PyCourse_Check(_o) PyObject_TypeCheck(_o, &PyCourse_Type)
#define PyCourse_CheckExact(_o) (Py_TYPE(_o) == &PyCourse_Type)

#define PyLine_Check(_o) PyObject_TypeCheck(_o, &PyLine_Type)
#define PyLine_CheckExact(_o) (Py_TYPE(_o) == &PyLine_Type)

#define PyPolygon_Check(_o) PyObject_TypeCheck(_o, &PyPolygon_Type)
#define PyPolygon_CheckExact(_o) (Py_TYPE(_o) == &PyPolygon_Type)

PyShape * wrapShape(Shape *);

PyShape * newPyShape();

PyShape * newPyArea();
PyShape * newPyBody();

PyShape * newPyBox();

#endif // RULESETS_PY_SHAPE_H
