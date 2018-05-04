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


#include "Py_Task.h"

#include "Py_Message.h"
#include "Py_Operation.h"
#include "Py_Thing.h"
#include "PythonWrapper.h"

#include "Task.h"
#include "Character.h"

static PyObject * Task_irrelevant(PyTask * self)
{
#ifndef NDEBUG
    if (self->m_task == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr task in Task.irrelevant");
        return nullptr;
    }
#endif // NDEBUG
    self->m_task->irrelevant();
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Task_obsolete(PyTask * self)
{
#ifndef NDEBUG
    if (self->m_task == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr task in Task.obsolete");
        return nullptr;
    }
#endif // NDEBUG
    PyObject * ret = self->m_task->obsolete() ? Py_True : Py_False;
    Py_INCREF(ret);
    return ret;
}

static PyObject * Task_count(PyTask * self)
{
#ifndef NDEBUG
    if (self->m_task == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr task in Task.count");
        return nullptr;
    }
#endif // NDEBUG
    return PyLong_FromLong(self->m_task->count());
}

static PyObject * Task_newtick(PyTask * self)
{
#ifndef NDEBUG
    if (self->m_task == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr task in Task.newtick");
        return nullptr;
    }
#endif // NDEBUG
    return PyLong_FromLong(self->m_task->newTick());
}

static PyObject * Task_nexttick(PyTask * self, PyObject * arg)
{
#ifndef NDEBUG
    if (self->m_task == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr task in Task.nexttick");
        return nullptr;
    }
#endif // NDEBUG
    double interval;
    if (PyFloat_Check(arg)) {
        interval = PyFloat_AsDouble(arg);
    } else if (PyLong_Check(arg)) {
        interval = PyLong_AsLong(arg);
    } else {
        PyErr_SetString(PyExc_TypeError, "Interval must be a number");
        return nullptr;
    }
    PyOperation * tick_op = newPyOperation();
    if (tick_op != 0) {
        tick_op->operation = self->m_task->nextTick(interval);
        Py_INCREF(tick_op);
    }
    return (PyObject*)tick_op;
}

static PyMethodDef Task_methods[] = {
        {"irrelevant",     (PyCFunction)Task_irrelevant, METH_NOARGS},
        {"obsolete",       (PyCFunction)Task_obsolete, METH_NOARGS},
        {"count",          (PyCFunction)Task_count, METH_NOARGS},
        {"new_tick",       (PyCFunction)Task_newtick, METH_NOARGS},
        {"next_tick",      (PyCFunction)Task_nexttick, METH_O},
        {nullptr,          nullptr}           /* sentinel */
};

static PyObject * Task_getattro(PyTask *self, PyObject *oname)
{
    // Fairly major re-write of this to use operator[] of Task base class
#ifndef NDEBUG
    if (self->m_task == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr task Task.getattr");
        return nullptr;
    }
#endif // NDEBUG
    char * name = PyUnicode_AsUTF8(oname);
    if (strcmp(name, "character") == 0) {
        return wrapEntity(&self->m_task->owner());
    }
    if (strcmp(name, "progress") == 0) {
        return PyFloat_FromDouble(self->m_task->progress());
    }
    if (strcmp(name, "rate") == 0) {
        return PyFloat_FromDouble(self->m_task->rate());
    }
    Atlas::Message::Element val;
    if (self->m_task->getAttr(name, val) == 0) {
        return MessageElement_asPyObject(val);
    }
    return PyObject_GenericGetAttr((PyObject *)self, oname);
}

static int Task_setattro(PyTask *self, PyObject * oname, PyObject *v)
{
#ifndef NDEBUG
    if (self->m_task == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr task Task.setattr");
        return -1;
    }
#endif // NDEBUG
    char * name = PyUnicode_AsUTF8(oname);
    if (strcmp(name, "progress") == 0) {
        if (PyFloat_Check(v)) {
            self->m_task->progress() = PyFloat_AsDouble(v);
        } else if (PyLong_Check(v)) {
            self->m_task->progress() = PyLong_AsLong(v);
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
        } else if (PyLong_Check(v)) {
            rate = PyLong_AsLong(v);
        } else {
            PyErr_SetString(PyExc_TypeError, "rate must be a number");
            return -1;
        }
        self->m_task->rate() = rate;
        return 0;
    }
    if (PyWeakref_CheckProxy(v)) {
        PyErr_SetString(PyExc_TypeError, "don't store proxy objects as attributes");
        return -1;
    }
    if (PyLocatedEntity_Check(v)) {
        PyErr_SetString(PyExc_TypeError, "don't store server objects as attributes");
        return -1;
    }
    // FIXME Something may be required here long term, for task attributes.
    return PyObject_GenericSetAttr((PyObject*)self, oname, v);
}

static PyObject* Task_compare(PyObject *a, PyObject *b, int op)
{
    PyObject *result = Py_NotImplemented;

    auto self = (PyTask*)a;
    if (PyTask_Check(b)) {
        auto other = (PyTask*)b;
#ifndef NDEBUG
        if (self->m_task == nullptr || other->m_task == nullptr) {
            Py_INCREF(Py_False);
            return Py_False;
        }
#endif // NDEBUG
        if (op == Py_EQ) {
            result = (self->m_task == other->m_task) ? Py_True : Py_False;
        } else if (op == Py_NE) {
            result = (self->m_task != other->m_task) ? Py_True : Py_False;
        }
    }

    Py_IncRef(result);
    return result;
}

static int Task_init(PyTask * self, PyObject * args, PyObject * kwds)
{
    PyObject * arg;
    if (!PyArg_ParseTuple(args, "O", &arg)) {
        return -1;
    }
    if (PyTask_Check(arg)) {
        PyTask * wrap_task = (PyTask *)arg;
#ifndef NDEBUG
        if (wrap_task->m_task == nullptr) {
            PyErr_SetString(PyExc_AssertionError, "nullptr task Task.__init__");
            return -1;
        }
#endif // NDEBUG
        self->m_task = wrap_task->m_task;
        return 0;
    }
    if (PyLocatedEntity_Check(arg)) {
        PyEntity * owner = (PyEntity *)arg;
#ifndef NDEBUG
        if (owner->m_entity.l == nullptr) {
            PyErr_SetString(PyExc_AssertionError, "nullptr owner Task.__init__");
            return -1;
        }
#endif // NDEBUG
        self->m_task = new Task(*owner->m_entity.l);
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "Task requires a Task, or Entity");
    return -1;
}

PyTypeObject PyTask_Type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
        "Task",                         /*tp_name*/
        sizeof(PyTask),                 /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        0,                              /*tp_dealloc*/
        0,                              /*tp_print*/
        0,                              /*tp_getattr*/
        0,                              /*tp_setattr*/
        0,                              /*tp_compare*/
        0,                              /*tp_repr*/
        0,                              /*tp_as_number*/
        0,                              /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
        0,                              // tp_call
        0,                              // tp_str
        (getattrofunc)Task_getattro,    // tp_getattro
        (setattrofunc)Task_setattro,    // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,             // tp_flags
        "Task objects",                 // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        (richcmpfunc)Task_compare,      // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        Task_methods,                   // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        0,                              // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)Task_init,            // tp_init
        0,                              // tp_alloc
        0,                              // tp_new
};

#if 0
PyObject * wrapTask(Task * task)
{
    PyObject * wrapper;
    Task * ts = dynamic_cast<Task *>(task);
    PythonWrapper * pw;
    if (ts == 0 || ((pw = dynamic_cast<PythonWrapper *>(ts->script())) == 0)) {
        PyTask * pt = newPyTask();
        if (pt != nullptr) {
            pt->m_task = task;
        }
        wrapper = (PyObject *)pt;
        // This wrapper cannot be stashed back int the task yet so
        // we don't have to do this next time.
    } else {
        wrapper = pw->wrapper();
        assert(wrapper != nullptr);
        Py_INCREF(wrapper);
    }
    return wrapper;
}
#endif

template<>
PyObject * wrapPython<Task>(Task * t)
{
    PyTask * pt = newPyTask();
    if (pt == nullptr) {
        return nullptr;
    }
    pt->m_task = t;
    return (PyObject*)pt;
}


PyTask * newPyTask()
{
    return (PyTask *)PyTask_Type.tp_new(&PyTask_Type, 0, 0);
}
