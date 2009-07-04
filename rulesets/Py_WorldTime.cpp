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

static PyObject * WorldTime_is_now(PyWorldTime * self, PyObject * py_other)
{
#ifndef NDEBUG
    if (self->time == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL WorldTime in WorldTime.is_now");
        return 0;
    }
#endif // NDEBUG
    if (!PyString_CheckExact(py_other)) {
        PyErr_SetString(PyExc_TypeError, "time must be a string");
        return NULL;
    }
    char * other = PyString_AsString(py_other);
    //printf("Python worldtime is string\n");
    bool eq = (*self->time == std::string(other));
    PyObject * ret = eq ? Py_True : Py_False;
    Py_INCREF(ret);
    return ret;
}

static PyMethodDef WorldTime_methods[] = {
    {"seconds",         (PyCFunction)WorldTime_seconds, METH_NOARGS},
    {"is_now",          (PyCFunction)WorldTime_is_now,  METH_O},
    {NULL,              NULL}           // sentinel
};

static void WorldTime_dealloc(PyWorldTime *self)
{
    if (self->own && self->time != NULL) {
        delete self->time;
    }
    self->ob_type->tp_free(self);
}

static PyObject * WorldTime_getattr(PyWorldTime *self, char *name)
{
    std::string attr = (*self->time)[name];
    if (attr != "") {
        return PyString_FromString(attr.c_str());
    }
    return Py_FindMethod(WorldTime_methods, (PyObject *)self, name);
}

static PyObject * WorldTime_new(PyTypeObject * type, PyObject *, PyObject *)
{
    // This looks allot like the default implementation, except we set some
    // stuff to null.
    PyWorldTime * self = (PyWorldTime *)type->tp_alloc(type, 0);
    self->time = 0;
    self->own = false;
    return (PyObject *)self;
}

static int WorldTime_init(PyWorldTime * self, PyObject * args, PyObject * kwds)
{
    int seconds;

    if (!PyArg_ParseTuple(args, "i", &seconds)) {
        return -1;
    }

    self->time = new WorldTime(seconds);
    self->own = true;

    return 0;
}

PyTypeObject PyWorldTime_Type = {
        PyObject_HEAD_INIT(NULL)
        0,                              // ob_size
        "server.WorldTime",             // tp_name
        sizeof(PyWorldTime),            // tp_basicsize
        0,                              // tp_itemsize
        // methods
        (destructor)WorldTime_dealloc,  // tp_dealloc
        0,                              // tp_print
        (getattrfunc)WorldTime_getattr, // tp_getattr
        0,                              // tp_setattr
        0,                              // tp_compare
        0,                              // tp_repr
        0,                              // tp_as_number
        0,                              // tp_as_sequence
        0,                              // tp_as_mapping
        0,                              // tp_hash
        0,                              // tp_call
        0,                              // tp_str
        0,                              // tp_getattro
        0,                              // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT,             // tp_flags
        "WorldTime objects",            // tp_doc
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
        (initproc)WorldTime_init,       // tp_init
        0,                              // tp_alloc
        WorldTime_new,                  // tp_new
};

PyWorldTime * newPyWorldTime()
{
    return (PyWorldTime *)PyWorldTime_Type.tp_new(&PyWorldTime_Type, 0, 0);
}
