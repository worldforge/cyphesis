// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_Task.h"

#include "Py_Thing.h"

#include "Task.h"
#include "Character.h"

static PyObject * Task_irrelevant(PyTask * self)
{
#ifndef NDEBUG
    if (self->m_task == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL task in Task.irrelevant");
        return NULL;
    }
#endif // NDEBUG
    self->m_task->irrelevant();
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Task_obsolete(PyTask * self)
{
#ifndef NDEBUG
    if (self->m_task == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL task in Task.irrelevant");
        return NULL;
    }
#endif // NDEBUG
    PyObject * ret = self->m_task->obsolete() ? Py_True : Py_False;
    Py_INCREF(ret);
    return ret;
}

static PyMethodDef Task_methods[] = {
        {"irrelevant",     (PyCFunction)Task_irrelevant, METH_NOARGS},
        {"obsolete",       (PyCFunction)Task_obsolete, METH_NOARGS},
        {NULL,          NULL}           /* sentinel */
};

static void Task_dealloc(PyTask *self)
{
    Py_XDECREF(self->Task_attr);
    PyMem_DEL(self);
}

static PyObject * Task_getattr(PyTask *self, char *name)
{
    // Fairly major re-write of this to use operator[] of Task base class
#ifndef NDEBUG
    if (self->m_task == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL task Task.getattr");
        return NULL;
    }
#endif // NDEBUG
    if (strcmp(name, "character") == 0) {
        return wrapEntity(&self->m_task->character());
    }
    if (self->Task_attr != NULL) {
        PyObject *v = PyDict_GetItemString(self->Task_attr, name);
        if (v != NULL) {
            Py_INCREF(v);
            return v;
        }
    }
    return Py_FindMethod(Task_methods, (PyObject *)self, name);
}

static int Task_setattr(PyTask *self, char *name, PyObject *v)
{
#ifndef NDEBUG
    if (self->m_task == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL task Task.setattr");
        return -1;
    }
#endif // NDEBUG
    // FIXME Something may be required here long term, for task attributes.
    if (self->Task_attr == NULL) {
        self->Task_attr = PyDict_New();
        if (self->Task_attr == NULL) {
            return -1;
        }
    }
    return PyDict_SetItemString(self->Task_attr, name, v);
}

static int Task_compare(PyTask *self, PyTask *other)
{
    if ((self->m_task == NULL) || (other->m_task == NULL)) {
        PyErr_SetString(PyExc_AssertionError, "NULL Task in Task.compare");
        return -1;
    }
    return (self->m_task == other->m_task) ? 0 : 1;
}

PyTypeObject PyTask_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /*ob_size*/
        "Task",                         /*tp_name*/
        sizeof(PyTask),                 /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        (destructor)Task_dealloc,       /*tp_dealloc*/
        0,                              /*tp_print*/
        (getattrfunc)Task_getattr,      /*tp_getattr*/
        (setattrfunc)Task_setattr,      /*tp_setattr*/
        (cmpfunc)Task_compare,          /*tp_compare*/
        0,                              /*tp_repr*/
        0,                              /*tp_as_number*/
        0,                              /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
};

PyTask * newPyTask()
{
    PyTask * self;
    self = PyObject_NEW(PyTask, &PyTask_Type);
    if (self == NULL) {
        return NULL;
    }
    self->Task_attr = NULL;
    return self;
}
