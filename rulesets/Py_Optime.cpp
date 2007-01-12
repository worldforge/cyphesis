// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000 Alistair Riddoch
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

#error This file has been removed from the build
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "Py_Optime.h"

#include <Atlas/Objects/Operation/RootOperation.h>

static PyMethodDef Optime_methods[] = {
    //{"update",                (PyCFunction)Optime_update,     METH_VARARGS},
    {NULL,              NULL}           /* sentinel */
};

static void Optime_dealloc(PyOptime *self)
{
    PyObject_Free(self);
}

static PyObject * Optime_getattr(PyOptime *self, char *name)
{
    return Py_FindMethod(Optime_methods, (PyObject *)self, name);
}

static int Optime_setattr(PyOptime *self, char *name, PyObject *v)
{
#ifndef NDEBUG
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Optime in Optime.setattr");
        return -1;
    }
#endif // NDEBUG
    if (strcmp(name, "sadd") == 0) {
        double fsecs;
        if (PyFloat_Check(v)) {
            fsecs = PyFloat_AsDouble(v);
        } else if (PyInt_Check(v)) {
            fsecs = double(PyInt_AsLong(v));
        } else {
            PyErr_SetString(PyExc_TypeError, "setting time to non number");
            return -1;
        }
        self->operation->setFutureSeconds(fsecs);
        return 0;
    }
    PyErr_SetString(PyExc_AttributeError, "unknown attribute");
    return -1;
}

PyTypeObject PyOptime_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /*ob_size*/
        "Optime",                       /*tp_name*/
        sizeof(PyOptime),               /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        (destructor)Optime_dealloc,     /*tp_dealloc*/
        0,                              /*tp_print*/
        (getattrfunc)Optime_getattr,    /*tp_getattr*/
        (setattrfunc)Optime_setattr,    /*tp_setattr*/
        0,                              /*tp_compare*/
        0,                              /*tp_repr*/
        0,                              /*tp_as_number*/
        0,                              /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
};

PyOptime * newPyOptime()
{
    PyOptime * self;
    self = PyObject_NEW(PyOptime, &PyOptime_Type);
    if (self == NULL) {
        return NULL;
    }
    return self;
}
