// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004-2006 Alistair Riddoch
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

// $Id: Py_Point3D.cpp,v 1.13 2007-07-30 18:12:51 alriddoch Exp $

#include "Py_Point3D.h"

#include "Py_Vector3D.h"
#include "Py_Object.h"

static PyObject * Point3D_mag(PyPoint3D * self)
{
    return PyFloat_FromDouble(sqrt(sqrMag(self->coords)));
}

static PyObject *Point3D_unit_vector_to(PyPoint3D * self, PyPoint3D * other)
{
    if (!PyPoint3D_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Can get unit vector to Point3D");
        return NULL;
    }
    PyVector3D * ret = newPyVector3D();
    if (ret == NULL) {
        return NULL;
    }
    ret->coords = (other->coords - self->coords);
    ret->coords.normalize();
    return (PyObject *)ret;
}

static PyObject * Point3D_distance(PyPoint3D * self, PyPoint3D * other)
{
    if (!PyPoint3D_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Can get distance to other Point3D");
        return NULL;
    }
    return PyFloat_FromDouble(distance(self->coords, other->coords));
}

static PyObject * Point3D_is_valid(PyPoint3D * self)
{
    PyObject * ret = self->coords.isValid() ? Py_True : Py_False;
    Py_INCREF(ret);
    return ret;
}

static PyMethodDef Point3D_methods[] = {
    {"mag",             (PyCFunction)Point3D_mag,              METH_NOARGS},
    {"unit_vector_to",  (PyCFunction)Point3D_unit_vector_to,   METH_O},
    {"distance",        (PyCFunction)Point3D_distance,         METH_O},
    {"is_valid",        (PyCFunction)Point3D_is_valid,         METH_NOARGS},
    {NULL,              NULL}           /* sentinel */
};

static void Point3D_dealloc(PyPoint3D *self)
{
    self->coords.~Point3D();
    self->ob_type->tp_free(self);
}

static int Point3D_print(PyPoint3D * self, FILE * fp, int)
{
    // if (flags & Py_PRINT_RAW) {
    // }
    fprintf(fp, "(%lf %lf %lf", self->coords.x(), self->coords.y(), self->coords.z());
    return 0;
}

static PyObject* Point3D_repr(PyPoint3D * self)
{
    char buf[64];
    ::snprintf(buf, 64, "(%f, %f, %f)", self->coords.x(), self->coords.y(), self->coords.z());
    return PyString_FromString(buf);
}

static PyObject * Point3D_getattr(PyPoint3D *self, char *name)
{
    //if (!self->coords) {
        //PyErr_SetString(PyExc_TypeError, "unset Point");
        //return NULL;
    //}
    if (strcmp(name, "x") == 0) { return PyFloat_FromDouble(self->coords.x()); }
    if (strcmp(name, "y") == 0) { return PyFloat_FromDouble(self->coords.y()); }
    if (strcmp(name, "z") == 0) { return PyFloat_FromDouble(self->coords.z()); }

    return Py_FindMethod(Point3D_methods, (PyObject *)self, name);
}

static int Point3D_compare(PyPoint3D * self, PyPoint3D * other)
{
    if (!PyPoint3D_Check(other)) {
        return -1;
    }
    if (self->coords == other->coords) {
        return 0;
    }
    return 1;
}

static PyPoint3D * Point3D_num_add(PyPoint3D * self, PyVector3D*other)
{
    if (!PyVector3D_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Can only add Vector3D to Point3D");
        return NULL;
    }
    PyPoint3D * ret = newPyPoint3D();
    if (ret == NULL) {
        return NULL;
    }
    ret->coords = (self->coords + other->coords);
    return ret;
}

static PyObject * Point3D_num_sub(PyPoint3D * self, PyObject * other)
{
    if (PyVector3D_Check(other)) {
        PyVector3D * ovec = (PyVector3D *)other;
        PyPoint3D * ret = newPyPoint3D();
        if (ret == NULL) {
            return NULL;
        }
        ret->coords = (self->coords - ovec->coords);
        return (PyObject *)ret;
    } else if (PyPoint3D_Check(other)) {
        PyPoint3D * opoint = (PyPoint3D *)other;
        PyVector3D * ret = newPyVector3D();
        if (ret == NULL) {
            return NULL;
        }
        ret->coords = (self->coords - opoint->coords);
        return (PyObject *)ret;
    } else {
        PyErr_SetString(PyExc_TypeError, "Can only subtract Vector3D or Point3D from Point3D");
        return NULL;
    }
}

static int Point3D_num_coerce(PyObject ** self, PyObject ** other)
{
    Py_INCREF(*self);
    Py_INCREF(*other);
    return 0;
}

static int Point3D_init(PyPoint3D * self, PyObject * args, PyObject * kwds)
{
    PyObject * clist;
    new (&(self->coords)) Point3D();
    switch (PyTuple_Size(args)) {
        case 0:
            break;
        case 1:
            clist = PyTuple_GetItem(args, 0);
            if (!PyList_Check(clist) || PyList_Size(clist) != 3) {
                PyErr_SetString(PyExc_TypeError, "Point3D() from single value must a list 3 long");
                return -1;
            }
            for(int i = 0; i < 3; i++) {
                PyObject * item = PyList_GetItem(clist, i);
                if (PyInt_Check(item)) {
                    self->coords[i] = (float)PyInt_AsLong(item);
                } else if (PyFloat_Check(item)) {
                    self->coords[i] = PyFloat_AsDouble(item);
                } else if (PyMessageElement_Check(item)) {
                    PyMessageElement * mitem = (PyMessageElement*)item;
                    if (!mitem->m_obj->isNum()) {
                        PyErr_SetString(PyExc_TypeError, "Point3D() must take list of floats, or ints");
                        return -1;
                    }
                    self->coords[i] = mitem->m_obj->asNum();
                } else {
                    PyErr_SetString(PyExc_TypeError, "Point3D() must take list of floats, or ints");
                    return -1;
                }
            }
            self->coords.setValid();
            break;
        case 3:
            for(int i = 0; i < 3; i++) {
                PyObject * item = PyTuple_GetItem(args, i);
                if (PyInt_Check(item)) {
                    self->coords[i] = (float)PyInt_AsLong(item);
                } else if (PyFloat_Check(item)) {
                    self->coords[i] = PyFloat_AsDouble(item);
                } else {
                    PyErr_SetString(PyExc_TypeError, "Point3D() must take list of floats, or ints");
                    return -1;
                }
            }
            self->coords.setValid();
            break;
        default:
            PyErr_SetString(PyExc_TypeError, "Point3D must take list of floats, or ints, 3 ints or 3 floats");
            return -1;
            break;
    }
        
    return 0;
}

PyObject * Point3D_new(PyTypeObject * type, PyObject *, PyObject *)
{
    // This looks allot like the default implementation, except we call the
    // in-place constructor.
    PyPoint3D * self = (PyPoint3D *)type->tp_alloc(type, 0);
    if (self != NULL) {
        new (&(self->coords)) Point3D();
    }
    return (PyObject *)self;
}

static PyNumberMethods Point3D_num = {
        (binaryfunc)Point3D_num_add,    /* nb_add */
        (binaryfunc)Point3D_num_sub,    /* nb_subtract */
        0,                              /* nb_multiply */
        0,                              /* nb_divide */
        0,                              /* nb_remainder */
        0,                              /* nb_divmod */
        0,                              /* nb_power */
        0,                              /* nb_negative */
        0,                              /* nb_positive */
        0,                              /* nb_absolute */
        0,                              /* nb_nonzero */
        0,                              /* nb_invert */
        0,                              /* nb_lshift */
        0,                              /* nb_rshift */
        0,                              /* nb_and */
        0,                              /* nb_xor */
        0,                              /* nb_or */
        Point3D_num_coerce,             /* nb_coerce */
        0,                              /* nb_int */
        0,                              /* nb_long */
        0,                              /* nb_float */
        0,                              /* nb_oct */
        0                               /* nb_hex */
};

PyTypeObject PyPoint3D_Type = {
        PyObject_HEAD_INIT(0)
        0,                              // ob_size
        "Point3D.Point3D",              // tp_name
        sizeof(PyPoint3D),              // tp_basicsize
        0,                              // tp_itemsize
        //  methods 
        (destructor)Point3D_dealloc,    // tp_dealloc
        (printfunc)Point3D_print,       // tp_print
        (getattrfunc)Point3D_getattr,   // tp_getattr
        0,                              // tp_setattr
        (cmpfunc)Point3D_compare,       // tp_compare
        (reprfunc)Point3D_repr,         // tp_repr
        &Point3D_num,                   // tp_as_number
        0,                              // tp_as_sequence
        0,                              // tp_as_mapping
        0,                              // tp_hash
        0,                              // tp_call
        0,                              // tp_str
        0,                              // tp_getattro
        0,                              // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT,             // tp_flags
        "Point3D objects",              // tp_doc
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
        (initproc)Point3D_init,         // tp_init
        0,                              // tp_alloc
        Point3D_new,                    // tp_new
};

PyPoint3D * newPyPoint3D()
{
#if 0
    PyPoint3D * self;
    self = PyObject_NEW(PyPoint3D, &PyPoint3D_Type);
    if (self == NULL) {
            return NULL;
    }
    new (&(self->coords)) Point3D();
    return self;
#else
    return (PyPoint3D *)PyPoint3D_Type.tp_new(&PyPoint3D_Type, 0, 0);
#endif
}
