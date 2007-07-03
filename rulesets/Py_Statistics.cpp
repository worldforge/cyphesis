// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
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

// $Id: Py_Statistics.cpp,v 1.17 2007-07-03 13:42:50 alriddoch Exp $

#include "Py_Statistics.h"

#include "Py_Thing.h"
#include "Character.h"

#include "Task.h"

static PyMethodDef Statistics_methods[] = {
    {NULL,          NULL}           /* sentinel */
};

static void Statistics_dealloc(PyStatistics *self)
{
    self->ob_type->tp_free(self);
}

static PyObject * Statistics_getattro(PyStatistics *self, PyObject *pn)
{
    // We use getattro rather than the simpler getattr because otherwise
    // overriding of scripts does not work correctly. Statistics objects
    // in game inherit directly from this class.
    char * name = PyString_AS_STRING(pn);
#ifndef NDEBUG
    if (self->m_entity == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in Statistics.getattro");
        return NULL;
    }
#endif // NDEBUG
    // We check for methods first rather the last as is more usual. This is
    // so that a value is returned for any statistic requested, so we can
    // drop back to a default for anything not expicitly supported.
    // First check for methods in the script instance.
    PyObject * method = PyObject_GenericGetAttr((PyObject*)self, pn);
    if (method != 0) {
        return method;
    }
    // Second check for methods in this base class method table.
    method = Py_FindMethod(Statistics_methods, (PyObject *)self, name);
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

static int Statistics_setattro(PyStatistics *self, PyObject *pn, PyObject *v)
{
#ifndef NDEBUG
    if (self->m_entity == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in Statistics.setattro");
        return -1;
    }
#endif // NDEBUG
    // FIXME Allow setting of statistics values.
    // char * name = PyString_AS_STRING(pn);
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
    return (PyObject *)self;
}

static int Statistics_init(PyStatistics * self, PyObject * args, PyObject * kwd)
{
    PyObject * entity;

    if (!PyArg_ParseTuple(args, "O", &entity)) {
        return -1;
    }

    if (!PyEntity_Check(entity)) {
        PyErr_SetString(PyExc_TypeError, "Arg to Statistics must be an entity.");
        return -1;
    }
    PyCharacter * character = (PyCharacter *)entity;
    self->m_entity = character->m_entity;

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
        0,                                                // tp_getattr
        0,                                                // tp_setattr
        (cmpfunc)Statistics_compare,                      // tp_compare
        0,                                                // tp_repr
        0,                                                // tp_as_number
        0,                                                // tp_as_sequence
        0,                                                // tp_as_mapping
        0,                                                // tp_hash
        0,                                                // tp_call
        0,                                                // tp_str
        (getattrofunc)Statistics_getattro,                // tp_getattro
        (setattrofunc)Statistics_setattro,                // tp_setattro
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
