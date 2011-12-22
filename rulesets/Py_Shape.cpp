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

#include "Py_Shape.h"

#include "Py_Message.h"
#include "Py_Point3D.h"

#include "common/log.h"

#include "physics/Shape.h"

#include <Atlas/Message/Element.h>

#include <wfmath/axisbox.h>
#include <wfmath/point.h>

#include <sstream>
#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::MapType;

static PyObject * Shape_area(PyShape * self)
{
#ifndef NDEBUG
    if (self->shape == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Shape in Shape.getattr");
        return NULL;
    }
#endif // NDEBUG
    return PyFloat_FromDouble(self->shape->area());
}

static PyObject * Shape_footprint(PyShape * self)
{
#ifndef NDEBUG
    if (self->shape == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Shape in Shape.getattr");
        return NULL;
    }
#endif // NDEBUG
    PyShape * res = newPyShape();
    if (res != 0) {
        res->shape = new MathShape<WFMath::AxisBox, 2>(self->shape->footprint());
    }
    return (PyObject*)res;
}

static PyObject * Shape_low_corner(PyShape * self)
{
#ifndef NDEBUG
    if (self->shape == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Shape in Shape.getattr");
        return NULL;
    }
#endif // NDEBUG
    PyPoint3D * res = newPyPoint3D();
    if (res != 0) {
        res->coords = self->shape->lowCorner();
    }
    return (PyObject*)res;
}

static PyObject * Shape_high_corner(PyShape * self)
{
#ifndef NDEBUG
    if (self->shape == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Shape in Shape.getattr");
        return NULL;
    }
#endif // NDEBUG
    PyPoint3D * res = newPyPoint3D();
    if (res != 0) {
        res->coords = self->shape->highCorner();
    }
    return (PyObject*)res;
}

static PyObject * Shape_as_data(PyShape * self)
{
#ifndef NDEBUG
    if (self->shape == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Shape in Shape.getattr");
        return NULL;
    }
#endif // NDEBUG
    MapType atlas_data;
    self->shape->toAtlas(atlas_data);
    return MessageElement_asPyObject(atlas_data);
}

static PyMethodDef Shape_methods[] = {
    {"area",               (PyCFunction)Shape_area,              METH_NOARGS},
    {"footprint",          (PyCFunction)Shape_footprint,         METH_NOARGS},
    {"low_corner",         (PyCFunction)Shape_low_corner,        METH_NOARGS},
    {"high_corner",        (PyCFunction)Shape_high_corner,       METH_NOARGS},
    {"as_data",            (PyCFunction)Shape_as_data,           METH_NOARGS},
    {NULL,              NULL}           /* sentinel */
};

static void Shape_dealloc(PyShape *self)
{
    if (self->shape != NULL) {
        delete self->shape;
    }
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject * Shape_getattro(PyShape *self, PyObject * oname)
{
#ifndef NDEBUG
    if (self->shape == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Shape in Shape.getattr");
        return NULL;
    }
#endif // NDEBUG
    // char * name = PyString_AsString(oname);
    return PyObject_GenericGetAttr((PyObject *)self, oname);
}

static int Shape_setattro(PyShape *self, PyObject *oname, PyObject *v)
{
#ifndef NDEBUG
    if (self->shape == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Shape in Shape.setattr");
        return -1;
    }
#endif // NDEBUG
    // char * name = PyString_AsString(oname);
    PyErr_SetString(PyExc_AttributeError, "unknown attribute");
    return -1;
}

static PyObject * Shape_repr(PyShape *self)
{
    std::stringstream r;
    r << *self->shape;
    return PyString_FromString(r.str().c_str());
}

static int Shape_init(PyShape * self, PyObject * args, PyObject * kwds)
{
    PyObject * arg = 0;
    if (!PyArg_ParseTuple(args, "|O", &arg)) {
        return -1;
    }
    if (arg == 0) {
        return 0;
    }
    if (PyDict_Check(arg)) {
        MapType data;
        if (PyDictObject_asElement(arg, data) != 0) {
            PyErr_SetString(PyExc_TypeError, "Error converting dict to atlas");
            return -1;
        }
        self->shape = Shape::newFromAtlas(data);
        if (self->shape == 0) {
            PyErr_SetString(PyExc_TypeError, "Error converting atlas to shape");
            return -1;
        }
        return 0;
    }
    if (PyMessage_Check(arg)) {
        Element * data = ((PyMessage*)arg)->m_obj;
        if (!data->isMap()) {
            PyErr_SetString(PyExc_TypeError, "Error converting dict to atlas");
            return -1;
        }
        self->shape = Shape::newFromAtlas(data->Map());
        if (self->shape == 0) {
            PyErr_SetString(PyExc_TypeError, "Error converting atlas to shape");
            return -1;
        }
        return 0;
    }
    return 0;
}

static PyObject * Shape_new(PyTypeObject * type, PyObject *, PyObject *)
{
    // This looks allot like the default implementation
    PyShape * self = (PyShape *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->shape = NULL;
    }
    return (PyObject *)self;
}

Py_ssize_t Shape_sq_length(PyShape * self)
{
#ifndef NDEBUG
    if (self->shape == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Shape in Shape.copy");
        return 0;
    }
#endif // NDEBUG
    return self->shape->size();
}

static PyObject * Shape_inplace_multiply(PyShape * self, PyObject * other)
{
    if (!PyFloat_CheckExact(other)) {
        PyErr_SetString(PyExc_TypeError, "Can only rotatey with a float");
        return NULL;
    }
    double val = PyFloat_AsDouble(other);
    self->shape->scale(val);
    Py_INCREF(self);
    return (PyObject*)self;
}

static PyNumberMethods Shape_number = {
    0,                                        // nb_add;
    0,                                        // nb_subtract;
    0,                                        // nb_multiply;
    0,                                        // nb_divide;
    0,                                        // nb_remainder;
    0,                                        // nb_divmod;
    0,                                        // nb_power;
    0,                                        // nb_negative;
    0,                                        // nb_positive;
    0,                                        // nb_absolute;
    0,                                        // nb_nonzero;
    0,                                        // nb_invert;
    0,                                        // nb_lshift;
    0,                                        // nb_rshift;
    0,                                        // nb_and;
    0,                                        // nb_xor;
    0,                                        // nb_or;
    0,                                        // nb_coerce;
    0,                                        // nb_int;
    0,                                        // nb_long;
    0,                                        // nb_float;
    0,                                        // nb_oct;
    0,                                        // nb_hex;
    /* Added in release 2.0 */
    0,                                        // nb_inplace_add;
    0,                                        // nb_inplace_subtract;
    (binaryfunc)Shape_inplace_multiply,       // nb_inplace_multiply;
    0,                                        // nb_inplace_divide;
    0,                                        // nb_inplace_remainder;
    0,                                        // nb_inplace_power;
    0,                                        // nb_inplace_lshift;
    0,                                        // nb_inplace_rshift;
    0,                                        // nb_inplace_and;
    0,                                        // nb_inplace_xor;
    0,                                        // nb_inplace_or;

};

static PySequenceMethods Shape_sequence = {
        (lenfunc)Shape_sq_length, // sq_length;
        0, // sq_concat;
        0, // sq_repeat;
        0, // sq_item;
        0, // sq_slice;
        0, // sq_ass_item;
        0, // sq_ass_slice;
        0, // sq_contains;
        /* Added in release 2.0 */
        0, // sq_inplace_concat;
        0, // sq_inplace_repeat;
};

PyTypeObject PyShape_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /*ob_size*/
        "atlas.Shape",                  /*tp_name*/
        sizeof(PyShape),                /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        (destructor)Shape_dealloc,      /*tp_dealloc*/
        0,                              /*tp_print*/
        0,                              /*tp_getattr*/
        0,                              /*tp_setattr*/
        0,                              /*tp_compare*/
        (reprfunc)Shape_repr,           /*tp_repr*/
        &Shape_number,                  /*tp_as_number*/
        &Shape_sequence,                /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
        0,                              // tp_call
        0,                              // tp_str
        (getattrofunc)Shape_getattro,   // tp_getattro
        (setattrofunc)Shape_setattro,   // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT,             // tp_flags
        "Shape objects",                // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        0,                              // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        Shape_methods,                  // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        0,                              // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)Shape_init,           // tp_init
        0,                              // tp_alloc
        Shape_new,                      // tp_new
};

PyShape * newPyShape()
{
    return (PyShape *)PyShape_Type.tp_new(&PyShape_Type, 0, 0);
}
