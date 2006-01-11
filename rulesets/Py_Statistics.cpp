// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_Statistics.h"

#include "Py_Thing.h"
#include "Character.h"

#include "Task.h"

static PyMethodDef Statistics_methods[] = {
    {NULL,          NULL}           /* sentinel */
};

static void Statistics_dealloc(PyStatistics *self)
{
    PyMem_DEL(self);
}

static PyObject * Statistics_getattr(PyStatistics *self, char *name)
{
    std::cout << "Statistics_getattr " << name << std::endl << std::flush;
#ifndef NDEBUG
    if (self->m_entity == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in Statistics.getattr");
        return NULL;
    }
#endif // NDEBUG
    // We check for methods first rather the last as is more usual. This is
    // so that a value is returned for any statistic requested, so we can
    // drop back to a default for anything not expicitly supported.
    PyObject * method = Py_FindMethod(Statistics_methods, (PyObject *)self, name);
    if (method != 0) {
        return method;
    }
    // If no method was found, Py_FindMethod sets an exception, which we don't
    // really want.
    PyErr_Clear();

    if (strcmp(name, "character") == 0) {
        return wrapEntity(self->m_entity);
    }
    return PyFloat_FromDouble(dynamic_cast<Character*>(self->m_entity)->statistics().get(name));
}

static int Statistics_setattr(PyStatistics *self, char *name, PyObject *v)
{
    std::cout << "Statistics_setattr " << name << std::endl << std::flush;
#ifndef NDEBUG
    if (self->m_entity == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in Statistics.setattr");
        return -1;
    }
#endif // NDEBUG
    // FIXME Allow setting of statistics values.
    PyErr_SetString(PyExc_AttributeError, "unknown attribute");
    return -1;
}

static int Statistics_compare(PyStatistics *self, PyStatistics *other)
{
    if ((self->m_entity == NULL) || (other->m_entity == NULL)) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in Statistics.compare");
        return -1;
    }
    return (self->m_entity == other->m_entity) ? 0 : 1;
}

static PyObject * Statistics_new(PyTypeObject * type, PyObject *, PyObject *)
{
    // This looks allot like the default implementation, except we set some
    // stuff to null.
    PyStatistics * self = (PyStatistics *)type->tp_alloc(type, 0);
    // self->time = 0;
    // self->own = false;
    return (PyObject *)self;
}

static int Statistics_init(PyStatistics * self, PyObject * args, PyObject * kwd)
{
    // int seconds;

    // if (!PyArg_ParseTuple(args, "i", &seconds)) {
        // return -1;
    // }

    // self->time = new WorldTime(seconds);
    // self->own = true;

    std::cout << "Statistics_init" << std::endl << std::flush;

    return 0;
}

PyTypeObject PyStatistics_Type = {
        PyObject_HEAD_INIT(NULL)
        0,                                                // ob_size
        "rules.Statistics",                               // tp_name
        sizeof(PyStatistics),                             // tp_basicsize
        0,                                                // tp_itemsize
        // methods 
        (destructor)Statistics_dealloc,                   // tp_dealloc
        0,                                                // tp_print
        (getattrfunc)Statistics_getattr,                  // tp_getattr
        (setattrfunc)Statistics_setattr,                  // tp_setattr
        (cmpfunc)Statistics_compare,                      // tp_compare
        0,                                                // tp_repr
        0,                                                // tp_as_number
        0,                                                // tp_as_sequence
        0,                                                // tp_as_mapping
        0,                                                // tp_hash
        0,                                                // tp_call
        0,                                                // tp_str
        0,                                                // tp_getattro
        0,                                                // tp_setattro
        0,                                                // tp_as_buffer
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,         // tp_flags
        "Statistics objects",                             // tp_doc
        0,                                                // tp_travers
        0,                                                // tp_clear
        0,                                                // tp_richcompare
        0,                                                // tp_weaklistoffset
        0,                                                // tp_iter
        0,                                                // tp_iternext
        0,                                                // tp_methods
        0,                                                // tp_members
        0,                                                // tp_getset
        0,                                                // tp_base
        0,                                                // tp_dict
        0,                                                // tp_descr_get
        0,                                                // tp_descr_set
        0,                                                // tp_dictoffset
        (initproc)Statistics_init,                        // tp_init
        0,                                                // tp_alloc
        Statistics_new,                                   // tp_new
};

PyStatistics * newPyStatistics()
{
    PyStatistics * self;
    self = PyObject_NEW(PyStatistics, &PyStatistics_Type);
    if (self == NULL) {
        return NULL;
    }
    return self;
}
