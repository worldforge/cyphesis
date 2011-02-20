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

#include "physics/Shape.h"

#include <sstream>

static PyMethodDef Shape_methods[] = {
    {NULL,              NULL}           /* sentinel */
};

static void Shape_dealloc(PyShape *self)
{
    if (self->shape != NULL) {
        delete self->shape;
    }
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject * Shape_getattr(PyShape *self, char *name)
{
#ifndef NDEBUG
    if (self->shape == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Shape in Shape.getattr");
        return NULL;
    }
#endif // NDEBUG
    return Py_FindMethod(Shape_methods, (PyObject *)self, name);
}

static int Shape_setattr(PyShape *self, char *name, PyObject *v)
{
#ifndef NDEBUG
    if (self->shape == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Shape in Shape.setattr");
        return -1;
    }
#endif // NDEBUG
    PyErr_SetString(PyExc_AttributeError, "unknown attribute");
    return -1;
}

static PyObject * Shape_repr(PyShape *self)
{
    std::stringstream r;
    r << "Shape repr";
    // r << *self->shape;
    return PyString_FromString(r.str().c_str());
}

static int Shape_init(PyShape * self, PyObject * args, PyObject * kwds)
{
    // TODO Init from something?
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
        return NULL;
    }
#endif // NDEBUG
    return self->shape->size();
}

static PyNumberMethods Shape_number = {
    0,                   // nb_add;
    0,                   // nb_subtract;
    0,                   // nb_multiply;
    0,                   // nb_divide;
    0,                   // nb_remainder;
    0,                   // nb_divmod;
    0,                   // nb_power;
    0,                   // nb_negative;
    0,                   // nb_positive;
    0,                   // nb_absolute;
    0,                   // nb_nonzero;
    0,                   // nb_invert;
    0,                   // nb_lshift;
    0,                   // nb_rshift;
    0,                   // nb_and;
    0,                   // nb_xor;
    0,                   // nb_or;
    0,                   // nb_coerce;
    0,                   // nb_int;
    0,                   // nb_long;
    0,                   // nb_float;
    0,                   // nb_oct;
    0,                   // nb_hex;
    /* Added in release 2.0 */
    0,                   // nb_inplace_add;
    0,                   // nb_inplace_subtract;
    0,                   // nb_inplace_multiply;
    0,                   // nb_inplace_divide;
    0,                   // nb_inplace_remainder;
    0,                   // nb_inplace_power;
    0,                   // nb_inplace_lshift;
    0,                   // nb_inplace_rshift;
    0,                   // nb_inplace_and;
    0,                   // nb_inplace_xor;
    0,                   // nb_inplace_or;

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
        (getattrfunc)Shape_getattr,     /*tp_getattr*/
        (setattrfunc)Shape_setattr,     /*tp_setattr*/
        0,                              /*tp_compare*/
        (reprfunc)Shape_repr,           /*tp_repr*/
        &Shape_number,                  /*tp_as_number*/
        &Shape_sequence,                /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
        0,                              // tp_call
        0,                              // tp_str
        0,                              // tp_getattro
        0,                              // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT,             // tp_flags
        "Shape objects",             // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        0,                              // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        0,                              // tp_methods
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
