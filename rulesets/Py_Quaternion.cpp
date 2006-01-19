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

#include "Py_Quaternion.h"

static PyObject * Quaternion_as_list(PyQuaternion * self)
{
    PyObject * r = PyList_New(0);
    PyObject * i = PyFloat_FromDouble(self->rotation.vector().x());
    PyList_Append(r, i);
    Py_DECREF(i);
    i = PyFloat_FromDouble(self->rotation.vector().y());
    PyList_Append(r, i);
    Py_DECREF(i);
    i = PyFloat_FromDouble(self->rotation.vector().z());
    PyList_Append(r, i);
    Py_DECREF(i);
    i = PyFloat_FromDouble(self->rotation.scalar());
    PyList_Append(r, i);
    Py_DECREF(i);
    return r;
}

static PyMethodDef Quaternion_methods[] = {
    {"as_list",         (PyCFunction)Quaternion_as_list, METH_NOARGS},
    {NULL,              NULL}           /* sentinel */
};

static void Quaternion_dealloc(PyQuaternion *self)
{
    self->rotation.~Quaternion();
    PyMem_DEL(self);
}

static PyObject * Quaternion_getattr(PyQuaternion *self, char *name)
{
    if (strcmp(name, "x") == 0) { return PyFloat_FromDouble(self->rotation.vector().x()); }
    if (strcmp(name, "y") == 0) { return PyFloat_FromDouble(self->rotation.vector().y()); }
    if (strcmp(name, "z") == 0) { return PyFloat_FromDouble(self->rotation.vector().z()); }
    if (strcmp(name, "w") == 0) { return PyFloat_FromDouble(self->rotation.scalar()); }

    return Py_FindMethod(Quaternion_methods, (PyObject *)self, name);
}

static int Quaternion_compare(PyQuaternion * self, PyQuaternion * other)
{
    if (!PyQuaternion_Check(other)) {
        return -1;
    }
    if (self->rotation == other->rotation) {
        return 0;
    }
    return 1;
}

PyTypeObject PyQuaternion_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /*ob_size*/
        "Quaternion",                   /*tp_name*/
        sizeof(PyQuaternion),           /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        (destructor)Quaternion_dealloc, /*tp_dealloc*/
        0,                              /*tp_print*/
        (getattrfunc)Quaternion_getattr,/*tp_getattr*/
        0,                              /*tp_setattr*/
        (cmpfunc)Quaternion_compare,    /*tp_compare*/
        0,                              /*tp_repr*/
        0,                              /*tp_as_number*/
        0,                              /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
};

PyQuaternion * newPyQuaternion()
{
        PyQuaternion * self;
        self = PyObject_NEW(PyQuaternion, &PyQuaternion_Type);
        if (self == NULL) {
                return NULL;
        }
        new(&(self->rotation)) Quaternion();
        return self;
}
