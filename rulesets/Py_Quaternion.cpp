// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_Quaternion.h"

static PyObject * Quaternion_as_list(PyQuaternion * self, PyObject * args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return NULL;
    }
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
    {"as_list",         (PyCFunction)Quaternion_as_list, METH_VARARGS},
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

static int Quaternion_setattr(PyQuaternion *self, char *name, PyObject *v)
{
    return 0;
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
        sizeof(PyQuaternion),               /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        (destructor)Quaternion_dealloc, /*tp_dealloc*/
        0,                              /*tp_print*/
        (getattrfunc)Quaternion_getattr,        /*tp_getattr*/
        (setattrfunc)Quaternion_setattr,        /*tp_setattr*/
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
