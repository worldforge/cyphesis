// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_Point3D.h"

#include "Py_Vector3D.h"

static PyObject * Point3D_mag(PyPoint3D * self)
{
    return PyFloat_FromDouble(sqrt(sqrMag(self->coords)));
}

static PyObject *Point3D_unit_vector_to(PyPoint3D * self, PyObject * args)
{
    PyPoint3D * other;
    if (!PyArg_ParseTuple(args, "O", &other)) {
        return NULL;
    }
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

static PyObject * Point3D_distance(PyPoint3D * self, PyObject * args)
{
    PyPoint3D * other;
    if (!PyArg_ParseTuple(args, "O", &other)) {
        return NULL;
    }
    if (!PyPoint3D_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Can get distance to other Point3D");
        return NULL;
    }
    return PyFloat_FromDouble(distance(self->coords, other->coords));
}

static PyMethodDef Point3D_methods[] = {
    {"mag",             (PyCFunction)Point3D_mag,      METH_NOARGS},
    {"unit_vector_to_another_vector",   (PyCFunction)Point3D_unit_vector_to,   METH_VARARGS},
    {"distance",        (PyCFunction)Point3D_distance, METH_VARARGS},
    {NULL,              NULL}           /* sentinel */
};

static void Point3D_dealloc(PyPoint3D *self)
{
    self->coords.~Point3D();
    PyMem_DEL(self);
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

static int Point3D_setattr(PyPoint3D *self, char *name, PyObject *v)
{
    return 0;
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

static PyPoint3D * Point3D_num_sub(PyPoint3D * self, PyVector3D * other)
{
    if (!PyVector3D_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Can only sub Vector3D from Point3D");
        return NULL;
    }
    PyPoint3D * ret = newPyPoint3D();
    if (ret == NULL) {
        return NULL;
    }
    ret->coords = (self->coords - other->coords);
    return ret;
}

static int Point3D_num_coerce(PyObject ** self, PyObject ** other)
{
    Py_INCREF(*self);
    Py_INCREF(*other);
    return 0;
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
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /*ob_size*/
        "Point3D",                     /*tp_name*/
        sizeof(PyPoint3D),         /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        (destructor)Point3D_dealloc,   /*tp_dealloc*/
        0,                              /*tp_print*/
        (getattrfunc)Point3D_getattr,  /*tp_getattr*/
        (setattrfunc)Point3D_setattr,  /*tp_setattr*/
        (cmpfunc)Point3D_compare,      /*tp_compare*/
        0,                              /*tp_repr*/
        &Point3D_num,                  /*tp_as_number*/
        0,                              /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
};

PyPoint3D * newPyPoint3D()
{
        PyPoint3D * self;
        self = PyObject_NEW(PyPoint3D, &PyPoint3D_Type);
        if (self == NULL) {
                return NULL;
        }
        new (&(self->coords)) Point3D();
        return self;
}
