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

static PyObject * Quaternion_valid(PyQuaternion * self)
{
    PyObject * ret = self->rotation.isValid() ? Py_True : Py_False;
    Py_INCREF(ret);
    return ret;
}

static PyMethodDef Quaternion_methods[] = {
    {"as_list",         (PyCFunction)Quaternion_as_list, METH_NOARGS},
    {"valid",           (PyCFunction)Quaternion_valid,   METH_NOARGS},
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

static PyObject* Quaternion_repr(PyQuaternion * self)
{
    char buf[128];
    ::snprintf(buf, 128, "(%f, (%f, %f, %f))", self->rotation.scalar(),
               self->rotation.vector().x(), self->rotation.vector().y(),
               self->rotation.vector().z());
    return PyString_FromString(buf);
}

PyObject * Quaternium_num_mult(PyQuaternion * self, PyQuaternion * other)
{
    if (!PyQuaternion_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Quaternion must be multiplied by Quaternion");
        return NULL;
    }
    PyQuaternion * ret = newPyQuaternion();
    ret->rotation = self->rotation * other->rotation;
    return (PyObject *)ret;
}

static PyNumberMethods Quaternion_as_number = {
        0,                                           // nb_add;
        0,                                           // nb_subtract;
        (binaryfunc)Quaternium_num_mult,             // nb_multiply;
        0,                                           // nb_divide;
        0,                                           // nb_remainder;
        0,                                           // nb_divmod;
        0,                                           // nb_power;
        0,                                           // nb_negative;
        0,                                           // nb_positive;
        0,                                           // nb_absolute;
        0,                                           // nb_nonzero;
        0,                                           // nb_invert;
        0,                                           // nb_lshift;
        0,                                           // nb_rshift;
        0,                                           // nb_and;
        0,                                           // nb_xor;
        0,                                           // nb_or;
        0,                                           // nb_coerce;
        0,                                           // nb_int;
        0,                                           // nb_long;
        0,                                           // nb_float;
        0,                                           // nb_oct;
        0,                                           // nb_hex;
};


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
        (reprfunc)Quaternion_repr,      /*tp_repr*/
        &Quaternion_as_number,          /*tp_as_number*/
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
