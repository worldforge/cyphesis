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
        (cmpfunc)WorldTime_cmp,         // tp_compare
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
    PyWorldTime * self;
    self = PyObject_NEW(PyWorldTime, &PyWorldTime_Type);
    if (self == NULL) {
        return NULL;
    }
    self->own = false;
    return self;
}
