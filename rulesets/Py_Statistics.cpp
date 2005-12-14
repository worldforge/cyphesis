// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_Statistics.h"

#include "Py_Thing.h"

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
#ifndef NDEBUG
    if (self->m_entity == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in Statistics.getattr");
        return NULL;
    }
#endif // NDEBUG
    if (strcmp(name, "character") == 0) {
        return wrapEntity(self->m_entity);
    }
    return Py_FindMethod(Statistics_methods, (PyObject *)self, name);
}

static int Statistics_setattr(PyStatistics *self, char *name, PyObject *v)
{
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

PyTypeObject PyStatistics_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /*ob_size*/
        "Statistics",                   /*tp_name*/
        sizeof(PyStatistics),           /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        (destructor)Statistics_dealloc, /*tp_dealloc*/
        0,                              /*tp_print*/
        (getattrfunc)Statistics_getattr,/*tp_getattr*/
        (setattrfunc)Statistics_setattr,/*tp_setattr*/
        (cmpfunc)Statistics_compare,    /*tp_compare*/
        0,                              /*tp_repr*/
        0,                              /*tp_as_number*/
        0,                              /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
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
