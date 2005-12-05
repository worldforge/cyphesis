// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_WorldTime.h"

#include "modules/WorldTime.h"

static PyObject *WorldTime_seconds(PyWorldTime *self)
{
#ifndef NDEBUG
    if (self->time == NULL) {
        PyErr_SetString(PyExc_AssertionError,"NULL WorldTime in WorldTime.seconds");
        return NULL;
    }
#endif // NDEBUG
    return PyFloat_FromDouble(self->time->seconds());
}

static PyObject * WorldTime_is_now(PyWorldTime *self, PyObject *args)
{
#ifndef NDEBUG
    if (self->time == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL WorldTime in WorldTime.is_now");
        return 0;
    }
#endif // NDEBUG
    char * other;
    if (!PyArg_ParseTuple(args, "s", &other)) {
        return NULL;
    }
    //printf("Python worldtime is string\n");
    bool eq = (*self->time == std::string(other));
    PyObject * ret = eq ? Py_True : Py_False;
    Py_INCREF(ret);
    return ret;
}

static PyMethodDef WorldTime_methods[] = {
    {"seconds",         (PyCFunction)WorldTime_seconds, METH_NOARGS},
    {"is_now",          (PyCFunction)WorldTime_is_now,  METH_VARARGS},
    {NULL,              NULL}           // sentinel
};

static void WorldTime_dealloc(PyWorldTime *self)
{
    if ((self->own) && (self->time != NULL)) {
        delete self->time;
    }
    PyMem_DEL(self);
}

static PyObject * WorldTime_getattr(PyWorldTime *self, char *name)
{
    std::string attr = (*self->time)[name];
    if (attr != "") {
        return PyString_FromString(attr.c_str());
    }
    return Py_FindMethod(WorldTime_methods, (PyObject *)self, name);
}

static int WorldTime_cmp(PyWorldTime *self, PyObject *other)
{
    if (PyString_Check(other)) {
        printf("Python compare of worldtime to string\n");
        bool eq = (*self->time == std::string(PyString_AsString(other)));
        return eq ? 0 : -1;
    } else {
        printf("Python compare of worldtime to ?\n");
        return -1;
    }
}

PyTypeObject PyWorldTime_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              // ob_size
        "WorldTime",                    // tp_name
        sizeof(PyWorldTime),            // tp_basicsize
        0,                              // tp_itemsize
        // methods
        (destructor)WorldTime_dealloc,  // tp_dealloc
        0,                              // tp_print
        (getattrfunc)WorldTime_getattr, // tp_getattr
        0,                              // tp_setattr
        (cmpfunc)WorldTime_cmp,         // tp_compare
        0,                              // tp_repr
        0,                              // tp_as_number
        0,                              // tp_as_sequence
        0,                              // tp_as_mapping
        0,                              // tp_hash
};

PyWorldTime * newPyWorldTime()
{
    PyWorldTime * self;
    self = PyObject_NEW(PyWorldTime, &PyWorldTime_Type);
    if (self == NULL) {
        return NULL;
    }
    self->own = false;
    return self;
}
