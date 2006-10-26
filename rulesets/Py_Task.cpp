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

// $Id: Py_Task.cpp,v 1.14 2006-10-26 00:48:11 alriddoch Exp $

#include "Py_Task.h"

#include "Py_Thing.h"
#include "Py_Operation.h"
#include "PythonWrapper.h"

#include "TaskScript.h"
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

static PyObject * Task_count(PyTask * self)
{
#ifndef NDEBUG
    if (self->m_task == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL task in Task.irrelevant");
        return NULL;
    }
#endif // NDEBUG
    return PyInt_FromLong(self->m_task->count());
}

static PyObject * Task_newtick(PyTask * self)
{
#ifndef NDEBUG
    if (self->m_task == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL task in Task.irrelevant");
        return NULL;
    }
#endif // NDEBUG
    return PyInt_FromLong(self->m_task->newTick());
}

static PyObject * Task_nexttick(PyTask * self, PyObject * arg)
{
#ifndef NDEBUG
    if (self->m_task == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL task in Task.irrelevant");
        return NULL;
    }
#endif // NDEBUG
    double interval;
    if (PyFloat_Check(arg)) {
        interval = PyFloat_AsDouble(arg);
    } else if (PyInt_Check(arg)) {
        interval = PyInt_AsLong(arg);
    } else {
        PyErr_SetString(PyExc_TypeError, "Interval must be a number");
        return NULL;
    }
    PyOperation * tick_op = newPyOperation();
    if (tick_op != 0) {
        tick_op->operation = self->m_task->nextTick(interval);
    }
    return (PyObject*)tick_op;
}

static PyMethodDef Task_methods[] = {
        {"irrelevant",     (PyCFunction)Task_irrelevant, METH_NOARGS},
        {"obsolete",       (PyCFunction)Task_obsolete, METH_NOARGS},
        {"count",          (PyCFunction)Task_count, METH_NOARGS},
        {"new_tick",       (PyCFunction)Task_newtick, METH_NOARGS},
        {"next_tick",      (PyCFunction)Task_nexttick, METH_O},
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
    if (strcmp(name, "progress") == 0) {
        return PyFloat_FromDouble(self->m_task->progress());
    }
    if (strcmp(name, "rate") == 0) {
        return PyFloat_FromDouble(self->m_task->rate());
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
    if (strcmp(name, "progress") == 0) {
        if (PyFloat_Check(v)) {
            self->m_task->progress() = PyFloat_AsDouble(v);
        } else if (PyInt_Check(v)) {
            self->m_task->progress() = PyInt_AsLong(v);
        } else {
            PyErr_SetString(PyExc_TypeError, "progress must be a number");
            return -1;
        }
        return 0;
    }
    if (strcmp(name, "rate") == 0) {
        double rate;
        if (PyFloat_Check(v)) {
            rate = PyFloat_AsDouble(v);
        } else if (PyInt_Check(v)) {
            rate = PyInt_AsLong(v);
        } else {
            PyErr_SetString(PyExc_TypeError, "rate must be a number");
            return -1;
        }
        self->m_task->rate() = rate;
        return 0;
    }
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

PyObject * wrapTask(Task * task)
{
    PyObject * wrapper;
    TaskScript * ts = dynamic_cast<TaskScript *>(task);
    PythonWrapper * pw;
    if (ts == 0 || ((pw = dynamic_cast<PythonWrapper *>(ts->script())) == 0)) {
        PyTask * pt = newPyTask();
        pt->m_task = task;
        wrapper = (PyObject *)pt;
        // This wrapper cannot be stashed back int the task yet so
        // we don't have to do this next time.
    } else {
        wrapper = pw->wrapper();
        assert(wrapper != NULL);
        Py_INCREF(wrapper);
    }
    return wrapper;
}

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
