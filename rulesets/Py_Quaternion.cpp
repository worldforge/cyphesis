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

// $Id$

#include "Py_Quaternion.h"

#include "Py_Vector3D.h"

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

static PyObject * Quaternion_is_valid(PyQuaternion * self)
{
    PyObject * ret = self->rotation.isValid() ? Py_True : Py_False;
    Py_INCREF(ret);
    return ret;
}

static PyObject * Quaternion_rotation(PyQuaternion * self, PyObject * args)
{
    PyObject * axis_arg;
    double angle;
    if (!PyArg_ParseTuple(args, "Od", &axis_arg, &angle)) {
        return NULL;
    }
    if (!PyVector3D_Check(axis_arg)) {
        PyErr_SetString(PyExc_TypeError, "Argument must be a Vector3D");
        return NULL;
    }
    PyVector3D * axis = (PyVector3D *)axis_arg;

    self->rotation.rotation(axis->coords, angle);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef Quaternion_methods[] = {
    {"as_list",         (PyCFunction)Quaternion_as_list, METH_NOARGS},
    {"is_valid",        (PyCFunction)Quaternion_is_valid,METH_NOARGS},
    {"rotation",        (PyCFunction)Quaternion_rotation,METH_VARARGS},
    {NULL,              NULL}           /* sentinel */
};

static void Quaternion_dealloc(PyQuaternion *self)
{
    self->rotation.~Quaternion();
    self->ob_type->tp_free((PyObject*)self);
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

static int Quaternion_init(PyQuaternion * self,
                                  PyObject * args, PyObject * kwds)
{
    PyObject * clist;
    switch (PyTuple_Size(args)) {
        case 0:
            break;
        case 1:
            clist = PyTuple_GetItem(args, 0);
            if (!PyList_Check(clist) || PyList_Size(clist) != 4) {
                PyErr_SetString(PyExc_TypeError, "Quaternion() from single value must a list 4 long");
                return -1;
            }
            {
            float quaternion[4];
            for(int i = 0; i < 4; i++) {
                PyObject * item = PyList_GetItem(clist, i);
                if (PyInt_Check(item)) {
                    quaternion[i] = (WFMath::CoordType)PyInt_AsLong(item);
                } else if (PyFloat_Check(item)) {
                    quaternion[i] = PyFloat_AsDouble(item);
                } else {
                    PyErr_SetString(PyExc_TypeError, "Quaternion() must take list of floats, or ints");
                    return -1;
                }
            }
            self->rotation = Quaternion(quaternion[3], quaternion[0],
                             quaternion[1], quaternion[2]);
            }
            break;
        case 2:
            {
            PyObject * v1 = PyTuple_GetItem(args, 0);
            PyObject * v2 = PyTuple_GetItem(args, 1);
            if (!PyVector3D_Check(v1)) {
                PyErr_SetString(PyExc_TypeError, "Quaternion(a,b) must take a vector");
                return -1;
            }
            PyVector3D * arg1 = (PyVector3D *)v1;
            if (PyVector3D_Check(v2)) {
                PyVector3D * to = (PyVector3D *)v2;
                self->rotation = quaternionFromTo(arg1->coords, to->coords);
            } else if (PyFloat_Check(v2)) {
                float angle = PyFloat_AsDouble(v2);
                self->rotation.rotation(arg1->coords, angle);
            } else {
                PyErr_SetString(PyExc_TypeError, "Quaternion(a,b) must take a vector");
                return -1;
            }
            }
            break;
        case 4:
            {
            float quaternion[4];
            for(int i = 0; i < 4; i++) {
                PyObject * item = PyTuple_GetItem(args, i);
                if (PyInt_Check(item)) {
                    quaternion[i] = (WFMath::CoordType)PyInt_AsLong(item);
                } else if (PyFloat_Check(item)) {
                    quaternion[i] = PyFloat_AsDouble(item);
                } else {
                    PyErr_SetString(PyExc_TypeError, "Quaternion() must take list of floats, or ints");
                    return -1;
                }
            }
            self->rotation = Quaternion(quaternion[3], quaternion[0],
                             quaternion[1], quaternion[2]);
            }
            break;
        default:
            PyErr_SetString(PyExc_TypeError, "Quaternion must take list of floats, or ints, 4 ints or 4 floats");
            return -1;
            break;
    }

    return 0;
}

static PyObject * Quaternion_new(PyTypeObject * type, PyObject *, PyObject *)
{
    // This looks allot like the default implementation, except we call the
    // in-place constructor.
    PyQuaternion * self = (PyQuaternion *)type->tp_alloc(type, 0);
    if (self != NULL) {
        new (&(self->rotation)) Quaternion();
    }
    return (PyObject *)self;
}

PyTypeObject PyQuaternion_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /*ob_size*/
        "physics.Quaternion",           /*tp_name*/
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
        0,                              // tp_call
        0,                              // tp_str
        0,                              // tp_getattro
        0,                              // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT,             // tp_flags
        "Quaternion objects",           // tp_doc
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
        (initproc)Quaternion_init,      // tp_init
        0,                              // tp_alloc
        Quaternion_new,                 // tp_new
};

PyQuaternion * newPyQuaternion()
{
#if 0
        PyQuaternion * self;
        self = PyObject_NEW(PyQuaternion, &PyQuaternion_Type);
        if (self == NULL) {
                return NULL;
        }
        new(&(self->rotation)) Quaternion();
        return self;
#else
    return (PyQuaternion *)PyQuaternion_Type.tp_new(&PyQuaternion_Type, 0, 0);
#endif
}
