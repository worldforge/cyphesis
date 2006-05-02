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

#include "Py_Vector3D.h"

#include "Py_Quaternion.h"
#include "Py_Object.h"

static PyObject * Vector3D_dot(PyVector3D * self, PyVector3D * other)
{
    if (!PyVector3D_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Can only dot with Vector3D");
        return NULL;
    }
    return PyFloat_FromDouble(Dot(self->coords, other->coords));
}

static PyObject * Vector3D_cross(PyVector3D * self, PyVector3D * other)
{
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

static PyObject * Vector3D_rotatex(PyVector3D * self, PyObject * arg)
{
    if (!PyFloat_CheckExact(arg)) {
        PyErr_SetString(PyExc_TypeError, "Can only rotatex with a float");
    }
    double angle = PyFloat_AsDouble(arg);
    self->coords.rotateX(angle);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Vector3D_rotatey(PyVector3D * self, PyObject * arg)
{
    if (!PyFloat_CheckExact(arg)) {
        PyErr_SetString(PyExc_TypeError, "Can only rotatey with a float");
    }
    double angle = PyFloat_AsDouble(arg);
    self->coords.rotateY(angle);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Vector3D_rotatez(PyVector3D * self, PyObject * arg)
{
    if (!PyFloat_CheckExact(arg)) {
        PyErr_SetString(PyExc_TypeError, "Can only rotatez with a float");
    }
    double angle = PyFloat_AsDouble(arg);
    self->coords.rotateZ(angle);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Vector3D_rotate(PyVector3D * self, PyQuaternion * arg)
{
    if (!PyQuaternion_Check(arg)) {
        PyErr_SetString(PyExc_TypeError, "Can only rotate with a quaternion");
        return NULL;
    }
    self->coords.rotate(arg->rotation);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Vector3D_angle(PyVector3D * self, PyVector3D * other)
{
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
    WFMath::CoordType the_mag = ret->coords.mag();
    if (!the_mag > 0) {
        PyErr_SetString(PyExc_ZeroDivisionError, "Attempt to normalize a vector with zero magnitude");
        return NULL;
    }
    ret->coords /= the_mag;
    return (PyObject *)ret;
}

static PyObject *Vector3D_unit_vector_to(PyVector3D * self, PyVector3D * other)
{
    if (!PyVector3D_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Argument must be a Vector3D");
        return NULL;
    }
    PyVector3D * ret = newPyVector3D();
    if (ret == NULL) {
        return NULL;
    }
    ret->coords = (other->coords - self->coords);
    WFMath::CoordType the_mag = ret->coords.mag();
    if (!the_mag > 0) {
        PyErr_SetString(PyExc_ZeroDivisionError, "Attempt to normalize a vector with zero magnitude");
        return NULL;
    }
    ret->coords /= the_mag;
    return (PyObject *)ret;
}

static PyMethodDef Vector3D_methods[] = {
    {"dot",             (PyCFunction)Vector3D_dot,      METH_O},
    {"cross",           (PyCFunction)Vector3D_cross,    METH_O},
    {"rotatex",         (PyCFunction)Vector3D_rotatex,  METH_O},
    {"rotatey",         (PyCFunction)Vector3D_rotatey,  METH_O},
    {"rotatez",         (PyCFunction)Vector3D_rotatez,  METH_O},
    {"rotate",          (PyCFunction)Vector3D_rotate,   METH_O},
    {"angle",           (PyCFunction)Vector3D_angle,    METH_O},
    {"square_mag",      (PyCFunction)Vector3D_sqr_mag,  METH_NOARGS},
    {"mag",             (PyCFunction)Vector3D_mag,      METH_NOARGS},
    {"is_valid",        (PyCFunction)Vector3D_is_valid, METH_NOARGS},
    {"unit_vector",     (PyCFunction)Vector3D_unit_vector,      METH_NOARGS},
    {"unit_vector_to",  (PyCFunction)Vector3D_unit_vector_to,   METH_O},
    {NULL,              NULL}           /* sentinel */
};

static void Vector3D_dealloc(PyVector3D *self)
{
    self->coords.~Vector3D();
    self->ob_type->tp_free(self);
}

static int Vector3D_print(PyVector3D * self, FILE * fp, int)
{
    // if (flags & Py_PRINT_RAW) {
    // }
    fprintf(fp, "(%lf %lf %lf", self->coords.x(), self->coords.y(), self->coords.z());
    return 0;
}

static PyObject* Vector3D_repr(PyVector3D * self)
{
    char buf[64];
    ::snprintf(buf, 64, "(%f, %f, %f)", self->coords.x(), self->coords.y(), self->coords.z());
    return PyString_FromString(buf);
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
    float val;
    if (PyInt_Check(v)) {
        val = PyInt_AsLong(v);
    } else if (PyFloat_Check(v)) {
        val = PyFloat_AsDouble(v);
    } else {
        PyErr_SetString(PyExc_TypeError, "Vector3D attributes must be numeric");
        return -1;
    }
    if (strcmp(name, "x") == 0) {
        self->coords.x() = val;
    } else if (strcmp(name, "y") == 0) {
        self->coords.y() = val;
    } else if (strcmp(name, "z") == 0) {
        self->coords.z() = val;
    } else {
        PyErr_SetString(PyExc_AttributeError, "Vector3D attribute does not exist");
        return -1;
    }
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

static int Vector3D_init(PyVector3D * self, PyObject * args, PyObject * kwds)
{
    PyObject * clist;
    new (&(self->coords)) Vector3D();
    switch (PyTuple_Size(args)) {
        case 0:
            break;
        case 1:
            clist = PyTuple_GetItem(args, 0);
            if ((!PyList_Check(clist)) || (PyList_Size(clist) != 3)) {
                PyErr_SetString(PyExc_TypeError, "Vector3D() from single value must a list 3 long");
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
                        PyErr_SetString(PyExc_TypeError, "Vector3D() must take list of floats, or ints");
                        return -1;
                    }
                    self->coords[i] = mitem->m_obj->asNum();
                } else {
                    PyErr_SetString(PyExc_TypeError, "Vector3D() must take list of floats, or ints");
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
                    PyErr_SetString(PyExc_TypeError, "Vector3D() must take list of floats, or ints");
                    return -1;
                }
            }
            self->coords.setValid();
            break;
        default:
            PyErr_SetString(PyExc_TypeError, "Vector3D must take list of floats, or ints, 3 ints or 3 floats");
            return -1;
            break;
    }
        
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
        PyObject_HEAD_INIT(0)
        0,                              // ob_size
        "Vector3D.Vector3D",            // tp_name
        sizeof(PyVector3D),             // tp_basicsize
        0,                              // tp_itemsize
        // methods 
        (destructor)Vector3D_dealloc,   // tp_dealloc
        (printfunc)Vector3D_print,      // tp_print
        (getattrfunc)Vector3D_getattr,  // tp_getattr
        (setattrfunc)Vector3D_setattr,  // tp_setattr
        (cmpfunc)Vector3D_compare,      // tp_compare
        (reprfunc)Vector3D_repr,        // tp_repr
        &Vector3D_num,                  // tp_as_number
        0,                              // tp_as_sequence
        0,                              // tp_as_mapping
        0,                              // tp_hash
        0,                              // tp_call
        0,                              // tp_str
        0,                              // tp_getattro
        0,                              // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT,             // tp_flags
        "Vector3D objects",             // tp_doc
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
        (initproc)Vector3D_init,        // tp_init
        0,                              // tp_alloc
        0,                              // tp_new
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
