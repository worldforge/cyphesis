// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_Vector3D.h"

static PyObject * Vector3D_dot(Vector3DObject * self, PyObject * args)
{
    Vector3DObject * other;
    if (!PyArg_ParseTuple(args, "O", &other)) {
        return NULL;
    }
    if (!PyVector3D_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Can only dot with Vector3D");
        return NULL;
    }
    return PyFloat_FromDouble(Dot(self->coords, other->coords));
}

static PyObject * Vector3D_cross(Vector3DObject * self, PyObject * args)
{
    Vector3DObject * other;
    if (!PyArg_ParseTuple(args, "O", &other)) {
        return NULL;
    }
    if (!PyVector3D_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Can only cross with Vector3D");
        return NULL;
    }
    Vector3DObject * ret = newVector3DObject(NULL);
    if (ret == NULL) {
        return NULL;
    }
    ret->coords = Cross(self->coords, other->coords);
    return (PyObject *)ret;
}

static PyObject * Vector3D_rotatex(Vector3DObject * self, PyObject * args)
{
    double angle;
    if (!PyArg_ParseTuple(args, "d", &angle)) {
        return NULL;
    }
    self->coords.rotateX(angle);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Vector3D_rotatey(Vector3DObject * self, PyObject * args)
{
    double angle;
    if (!PyArg_ParseTuple(args, "d", &angle)) {
        return NULL;
    }
    self->coords.rotateY(angle);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Vector3D_rotatez(Vector3DObject * self, PyObject * args)
{
    double angle;
    if (!PyArg_ParseTuple(args, "d", &angle)) {
        return NULL;
    }
    self->coords.rotateZ(angle);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Vector3D_angle(Vector3DObject * self, PyObject * args)
{
    Vector3DObject * other;
    if (!PyArg_ParseTuple(args, "O", &other)) {
        return NULL;
    }
    if (!PyVector3D_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Can get angle to Vector3D");
        return NULL;
    }
    return PyFloat_FromDouble(Angle(self->coords, other->coords));
}

static PyObject * Vector3D_mag(Vector3DObject * self, PyObject * args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return NULL;
    }
    return PyFloat_FromDouble(self->coords.mag());
}

static PyObject * Vector3D_unit_vector(Vector3DObject * self, PyObject * args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return NULL;
    }
    Vector3DObject * ret = newVector3DObject(NULL);
    if (ret == NULL) {
        return NULL;
    }
    ret->coords = self->coords;
    ret->coords.normalize();
    return (PyObject *)ret;
}

static PyObject *Vector3D_unit_vector_to(Vector3DObject * self, PyObject * args)
{
    Vector3DObject * other;
    if (!PyArg_ParseTuple(args, "O", &other)) {
        return NULL;
    }
    if (!PyVector3D_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Can get unit vector to Vector3D");
        return NULL;
    }
    Vector3DObject * ret = newVector3DObject(NULL);
    if (ret == NULL) {
        return NULL;
    }
    ret->coords = (other->coords - self->coords);
    ret->coords.normalize();
    return (PyObject *)ret;
}

static PyObject * Vector3D_distance(Vector3DObject * self, PyObject * args)
{
    Vector3DObject * other;
    if (!PyArg_ParseTuple(args, "O", &other)) {
        return NULL;
    }
    if (!PyVector3D_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Can get distance to other Vector3D");
        return NULL;
    }
    return PyFloat_FromDouble(distance(self->coords, other->coords));
}

static PyMethodDef Vector3D_methods[] = {
    {"dot",             (PyCFunction)Vector3D_dot,      METH_VARARGS},
    {"cross",           (PyCFunction)Vector3D_cross,    METH_VARARGS},
    {"rotatex",         (PyCFunction)Vector3D_rotatex,  METH_VARARGS},
    {"rotatey",         (PyCFunction)Vector3D_rotatey,  METH_VARARGS},
    {"rotatez",         (PyCFunction)Vector3D_rotatez,  METH_VARARGS},
    {"angle",           (PyCFunction)Vector3D_angle,    METH_VARARGS},
    {"mag",             (PyCFunction)Vector3D_mag,      METH_VARARGS},
    {"unit_vector",     (PyCFunction)Vector3D_unit_vector,      METH_VARARGS},
    {"unit_vector_to_another_vector",   (PyCFunction)Vector3D_unit_vector_to,   METH_VARARGS},
    {"distance",        (PyCFunction)Vector3D_distance, METH_VARARGS},
    {NULL,              NULL}           /* sentinel */
};

static void Vector3D_dealloc(Vector3DObject *self)
{
    PyMem_DEL(self);
}

static PyObject * Vector3D_getattr(Vector3DObject *self, char *name)
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

static int Vector3D_setattr(Vector3DObject *self, char *name, PyObject *v)
{
    return 0;
}

static int Vector3D_compare(Vector3DObject * self, Vector3DObject * other)
{
    if (!PyVector3D_Check(other)) {
        return -1;
    }
    if (self->coords == other->coords) {
        return 0;
    }
    return 1;
}

static Vector3DObject*Vector3D_num_add(Vector3DObject*self,Vector3DObject*other)
{
    if (!PyVector3D_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Can only add Vector3D to Vector3D");
        return NULL;
    }
    Vector3DObject * ret = newVector3DObject(NULL);
    if (ret == NULL) {
        return NULL;
    }
    ret->coords = (self->coords + other->coords);
    return ret;
}

static Vector3DObject*Vector3D_num_sub(Vector3DObject*self,Vector3DObject*other)
{
    if (!PyVector3D_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Can only sub Vector3D from Vector3D");
        return NULL;
    }
    Vector3DObject * ret = newVector3DObject(NULL);
    if (ret == NULL) {
        return NULL;
    }
    ret->coords = (self->coords - other->coords);
    return ret;
}

static Vector3DObject * Vector3D_num_mul(Vector3DObject * self, PyObject * _other)
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
    Vector3DObject * ret = newVector3DObject(NULL);
    if (ret == NULL) {
        return NULL;
    }
    ret->coords = (self->coords * other);
    return ret;
}

static Vector3DObject * Vector3D_num_div(Vector3DObject * self, PyObject * _other)
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
    Vector3DObject * ret = newVector3DObject(NULL);
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

PyTypeObject Vector3D_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /*ob_size*/
        "Vector3D",                     /*tp_name*/
        sizeof(Vector3DObject),         /*tp_basicsize*/
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

Vector3DObject * newVector3DObject(PyObject *arg)
{
        Vector3DObject * self;
        self = PyObject_NEW(Vector3DObject, &Vector3D_Type);
        if (self == NULL) {
                return NULL;
        }
        new (&(self->coords)) Vector3D();
        return self;
}
