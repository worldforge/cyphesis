// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_Vector3D.h"

static PyObject * Vector3D_dot(PyVector3D * self, PyObject * args)
{
    PyVector3D * other;
    if (!PyArg_ParseTuple(args, "O", &other)) {
        return NULL;
    }
    if (!PyVector3D_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Can only dot with Vector3D");
        return NULL;
    }
    return PyFloat_FromDouble(Dot(self->coords, other->coords));
}

static PyObject * Vector3D_cross(PyVector3D * self, PyObject * args)
{
    PyVector3D * other;
    if (!PyArg_ParseTuple(args, "O", &other)) {
        return NULL;
    }
    if (!PyVector3D_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Can only cross with Vector3D");
        return NULL;
    }
    PyVector3D * ret = newPyVector3D();
    if (ret == NULL) {
        return NULL;
    }
    ret->coords = Cross(self->coords, other->coords);
    return (PyObject *)ret;
}

static PyObject * Vector3D_rotatex(PyVector3D * self, PyObject * args)
{
    double angle;
    if (!PyArg_ParseTuple(args, "d", &angle)) {
        return NULL;
    }
    self->coords.rotateX(angle);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Vector3D_rotatey(PyVector3D * self, PyObject * args)
{
    double angle;
    if (!PyArg_ParseTuple(args, "d", &angle)) {
        return NULL;
    }
    self->coords.rotateY(angle);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Vector3D_rotatez(PyVector3D * self, PyObject * args)
{
    double angle;
    if (!PyArg_ParseTuple(args, "d", &angle)) {
        return NULL;
    }
    self->coords.rotateZ(angle);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Vector3D_angle(PyVector3D * self, PyObject * args)
{
    PyVector3D * other;
    if (!PyArg_ParseTuple(args, "O", &other)) {
        return NULL;
    }
    if (!PyVector3D_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Can get angle to Vector3D");
        return NULL;
    }
    return PyFloat_FromDouble(Angle(self->coords, other->coords));
}

static PyObject * Vector3D_sqr_mag(PyVector3D * self)
{
    return PyFloat_FromDouble(self->coords.sqrMag());
}

static PyObject * Vector3D_mag(PyVector3D * self)
{
    return PyFloat_FromDouble(self->coords.mag());
}

static PyObject * Vector3D_is_valid(PyVector3D * self)
{
    PyObject * ret = self->coords.isValid() ? Py_True : Py_False;
    Py_INCREF(ret);
    return ret;
}

static PyObject * Vector3D_unit_vector(PyVector3D * self)
{
    PyVector3D * ret = newPyVector3D();
    if (ret == NULL) {
        return NULL;
    }
    ret->coords = self->coords;
    ret->coords.normalize();
    return (PyObject *)ret;
}

static PyObject *Vector3D_unit_vector_to(PyVector3D * self, PyObject * args)
{
    PyVector3D * other;
    if (!PyArg_ParseTuple(args, "O", &other)) {
        return NULL;
    }
    if (!PyVector3D_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Can get unit vector to Vector3D");
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

static PyMethodDef Vector3D_methods[] = {
    {"dot",             (PyCFunction)Vector3D_dot,      METH_VARARGS},
    {"cross",           (PyCFunction)Vector3D_cross,    METH_VARARGS},
    {"rotatex",         (PyCFunction)Vector3D_rotatex,  METH_VARARGS},
    {"rotatey",         (PyCFunction)Vector3D_rotatey,  METH_VARARGS},
    {"rotatez",         (PyCFunction)Vector3D_rotatez,  METH_VARARGS},
    {"angle",           (PyCFunction)Vector3D_angle,    METH_VARARGS},
    {"square_mag",      (PyCFunction)Vector3D_sqr_mag,  METH_NOARGS},
    {"mag",             (PyCFunction)Vector3D_mag,      METH_NOARGS},
    {"is_valid",        (PyCFunction)Vector3D_is_valid, METH_NOARGS},
    {"unit_vector",     (PyCFunction)Vector3D_unit_vector,      METH_NOARGS},
    {"unit_vector_to_another_vector",   (PyCFunction)Vector3D_unit_vector_to,   METH_VARARGS},
    {NULL,              NULL}           /* sentinel */
};

static void Vector3D_dealloc(PyVector3D *self)
{
    self->coords.~Vector3D();
    PyMem_DEL(self);
}

static PyObject * Vector3D_getattr(PyVector3D *self, char *name)
{
    //if (!self->coords) {
        //PyErr_SetString(PyExc_TypeError, "unset Vector");
        //return NULL;
    //}
    if (strcmp(name, "x") == 0) { return PyFloat_FromDouble(self->coords.x()); }
    if (strcmp(name, "y") == 0) { return PyFloat_FromDouble(self->coords.y()); }
    if (strcmp(name, "z") == 0) { return PyFloat_FromDouble(self->coords.z()); }

    return Py_FindMethod(Vector3D_methods, (PyObject *)self, name);
}

static int Vector3D_setattr(PyVector3D *self, char *name, PyObject *v)
{
    return 0;
}

static int Vector3D_compare(PyVector3D * self, PyVector3D * other)
{
    if (!PyVector3D_Check(other)) {
        return -1;
    }
    if (self->coords == other->coords) {
        return 0;
    }
    return 1;
}

static PyVector3D*Vector3D_num_add(PyVector3D*self,PyVector3D*other)
{
    if (!PyVector3D_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Can only add Vector3D to Vector3D");
        return NULL;
    }
    PyVector3D * ret = newPyVector3D();
    if (ret == NULL) {
        return NULL;
    }
    ret->coords = (self->coords + other->coords);
    return ret;
}

static PyVector3D*Vector3D_num_sub(PyVector3D*self,PyVector3D*other)
{
    if (!PyVector3D_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Can only sub Vector3D from Vector3D");
        return NULL;
    }
    PyVector3D * ret = newPyVector3D();
    if (ret == NULL) {
        return NULL;
    }
    ret->coords = (self->coords - other->coords);
    return ret;
}

static PyVector3D * Vector3D_num_mul(PyVector3D * self, PyObject * _other)
{
    double other;
    if (PyInt_Check(_other)) {
        other = PyInt_AsLong(_other);
    } else if (PyFloat_Check(_other)) {
        other = PyFloat_AsDouble(_other);
    } else {
        PyErr_SetString(PyExc_TypeError, "Vector3D can only be multiplied by numeric value");
        return NULL;
    }
    PyVector3D * ret = newPyVector3D();
    if (ret == NULL) {
        return NULL;
    }
    ret->coords = (self->coords * other);
    return ret;
}

static PyVector3D * Vector3D_num_div(PyVector3D * self, PyObject * _other)
{
    double other;
    if (PyInt_Check(_other)) {
        other = PyInt_AsLong(_other);
    } else if (PyFloat_Check(_other)) {
        other = PyFloat_AsDouble(_other);
    } else {
        PyErr_SetString(PyExc_TypeError, "Vector3D can only be divided by numeric value");
        return NULL;
    }
    PyVector3D * ret = newPyVector3D();
    if (ret == NULL) {
        return NULL;
    }
    ret->coords = (self->coords / other);
    return ret;
}

static int Vector3D_num_coerce(PyObject ** self, PyObject ** other)
{
    Py_INCREF(*self);
    Py_INCREF(*other);
    return 0;
}

static PyNumberMethods Vector3D_num = {
        (binaryfunc)Vector3D_num_add,   /* nb_add */
        (binaryfunc)Vector3D_num_sub,   /* nb_subtract */
        (binaryfunc)Vector3D_num_mul,   /* nb_multiply */
        (binaryfunc)Vector3D_num_div,   /* nb_divide */
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
        Vector3D_num_coerce,            /* nb_coerce */
        0,                              /* nb_int */
        0,                              /* nb_long */
        0,                              /* nb_float */
        0,                              /* nb_oct */
        0                               /* nb_hex */
};

PyTypeObject PyVector3D_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /*ob_size*/
        "Vector3D",                     /*tp_name*/
        sizeof(PyVector3D),         /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        (destructor)Vector3D_dealloc,   /*tp_dealloc*/
        0,                              /*tp_print*/
        (getattrfunc)Vector3D_getattr,  /*tp_getattr*/
        (setattrfunc)Vector3D_setattr,  /*tp_setattr*/
        (cmpfunc)Vector3D_compare,      /*tp_compare*/
        0,                              /*tp_repr*/
        &Vector3D_num,                  /*tp_as_number*/
        0,                              /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
};

PyVector3D * newPyVector3D()
{
        PyVector3D * self;
        self = PyObject_NEW(PyVector3D, &PyVector3D_Type);
        if (self == NULL) {
                return NULL;
        }
        new (&(self->coords)) Vector3D();
        return self;
}
