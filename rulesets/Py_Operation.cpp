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

// $Id: Py_Operation.cpp,v 1.56 2007-01-09 14:00:25 alriddoch Exp $

#include "Py_Operation.h"
#include "Py_RootEntity.h"
#include "Py_Oplist.h"
#include "Py_Object.h"
#include "Py_Thing.h"

#include "common/log.h"

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Entity::RootEntity;

/*
 * Beginning of Operation section.
 *
 * This is a python type that wraps up operation objects from
 * Atlas::Objects::Operation namespace.
 *
 */

/*
 * Beginning of Operation methods section.
 */

static PyObject * Operation_setSerialno(PyOperation * self, PyObject * py_sno)
{
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError,"NULL Operation in Operation.setSerialno");
        return NULL;
    }
#endif // NDEBUG
    // Takes integer, returns none
    if (!PyInt_CheckExact(py_sno)) {
        PyErr_SetString(PyExc_TypeError, "serialno not an integer");
        return NULL;
    }
    int serialno = PyInt_AsLong(py_sno);
    self->operation->setSerialno(serialno);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Operation_setRefno(PyOperation * self, PyObject * py_rno)
{
    // Takes integer, returns none
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError,"NULL Operation in Operation.setRefno");
        return NULL;
    }
#endif // NDEBUG
    if (!PyInt_CheckExact(py_rno)) {
        PyErr_SetString(PyExc_TypeError, "refno not an integer");
        return NULL;
    }
    int refno = PyInt_AsLong(py_rno);
    self->operation->setRefno(refno);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Operation_setFrom(PyOperation * self, PyObject * py_from)
{
    // Takes string, returns none
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError,"NULL Operation in Operation.setFrom");
        return NULL;
    }
#endif // NDEBUG
    if (!PyString_CheckExact(py_from)) {
        PyErr_SetString(PyExc_TypeError, "from not a string");
        return NULL;
    }
    char * from = PyString_AsString(py_from);
    self->operation->setFrom(from);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Operation_setTo(PyOperation * self, PyObject * py_to)
{
    // Takes string, returns none
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError,"NULL Operation in Operation.setTo");
        return NULL;
    }
#endif // NDEBUG
    if (!PyString_CheckExact(py_to)) {
        PyErr_SetString(PyExc_TypeError, "to not a string");
        return NULL;
    }
    char * to = PyString_AsString(py_to);
    self->operation->setTo(to);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Operation_setSeconds(PyOperation * self, PyObject * py_secs)
{
    // Takes float, returns none
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError,"NULL Operation in Operation.setSeconds");
        return NULL;
    }
#endif // NDEBUG
    if (!PyFloat_CheckExact(py_secs)) {
        PyErr_SetString(PyExc_TypeError, "seconds not a float");
        return NULL;
    }
    double seconds = PyFloat_AsDouble(py_secs);
    self->operation->setSeconds(seconds);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Operation_setFutureSeconds(PyOperation * self,
                                             PyObject * py_fsecs)
{
    // Takes float, returns none
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError,"NULL Operation in Operation.setFutureSeconds");
        return NULL;
    }
#endif // NDEBUG
    double futureseconds = PyFloat_AsDouble(py_fsecs);
    if (PyFloat_CheckExact(py_fsecs)) {
        futureseconds = PyFloat_AsDouble(py_fsecs);
    } else if (PyInt_CheckExact(py_fsecs)) {
        futureseconds = PyInt_AsLong(py_fsecs);
    } else {
        PyErr_SetString(PyExc_TypeError, "future_seconds not a number");
        return NULL;
    }
    self->operation->setFutureSeconds(futureseconds);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Operation_setArgs(PyOperation * self, PyObject * args)
{
    // Takes List, returns none
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError,"NULL Operation in Operation.setArgs");
        return NULL;
    }
#endif // NDEBUG
    // FIXME This is a big mess - either get rid of it in entirity, or
    // make something better.

    if (!PyList_Check(args)) {
        PyErr_SetString(PyExc_TypeError, "args not a list");
        return NULL;
    }
    ListType argslist;
    for(int i = 0; i < PyList_Size(args); i++) {
        PyObject * item = PyList_GetItem(args, i);
        if (PyMessageElement_Check(item)) {
            argslist.push_back(*((PyMessageElement*)item)->m_obj);
        } else if (PyOperation_Check(item)) {
            argslist.push_back(((PyOperation*)item)->operation->asMessage());
        } else {
            PyErr_SetString(PyExc_TypeError,"args contains non Atlas Object");
            return NULL;
        }
    }
    self->operation->setArgsAsList(argslist);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Operation_getSerialno(PyOperation * self)
{
    // Returns int
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError,"NULL Operation in Operation.getSerialno");
        return NULL;
    }
#endif // NDEBUG
    return PyInt_FromLong(self->operation->getSerialno());
}

static PyObject * Operation_getRefno(PyOperation * self)
{
    // Returns int
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError,"NULL Operation in Operation.getRefno");
        return NULL;
    }
#endif // NDEBUG
    return PyInt_FromLong(self->operation->getRefno());
}

static PyObject * Operation_getFrom(PyOperation * self)
{
    // Returns string
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError,"NULL Operation in Operation.getFrom");
        return NULL;
    }
#endif // NDEBUG
    return PyString_FromString(self->operation->getFrom().c_str());
}

static PyObject * Operation_getTo(PyOperation * self)
{
    // Returns string
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError,"NULL Operation in Operation.getTo");
        return NULL;
    }
#endif // NDEBUG
    return PyString_FromString(self->operation->getTo().c_str());
}

static PyObject * Operation_getSeconds(PyOperation * self)
{
    // Returns float
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError,"NULL Operation in Operation.getSeconds");
        return NULL;
    }
#endif // NDEBUG
    return PyFloat_FromDouble(self->operation->getSeconds());
}

static PyObject * Operation_getFutureSeconds(PyOperation * self)
{
    // Returns float
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError,"NULL Operation in Operation.getFutureSeconds");
        return NULL;
    }
#endif // NDEBUG
    return PyFloat_FromDouble(self->operation->getFutureSeconds());
}

static PyObject * Operation_getArgs(PyOperation * self)
{
    // Returns list
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError,"NULL Operation in Operation.getArgs");
        return NULL;
    }
#endif // NDEBUG
    // Here we go:- 

    // FIXME
    // Here we need a generic way to make a Root object into a python object,
    // either by wrapping as a Operation, or as an Entity. New Entity binding
    // thus required. Once written this code should also be used in
    // Operation_seq_item(), and the code there is a better starting point.

    const std::vector<Root> & args_list = self->operation->getArgs();
    PyObject * args_pylist = PyList_New(args_list.size());
    int j = 0;
    PyMessageElement * item;
    std::vector<Root>::const_iterator Iend = args_list.end();
    std::vector<Root>::const_iterator I = args_list.begin();
    for (; I != Iend; ++I, ++j) {
        item = newPyMessageElement();
        if (item == NULL) {
            PyErr_SetString(PyExc_TypeError,"error creating list");
            Py_DECREF(args_pylist);
            return NULL;
        }
        item->m_obj = new Element((*I)->asMessage());
        PyList_SetItem(args_pylist, j, (PyObject *)item);
    }
    return args_pylist;
}

static PyObject* Operation_get_name(PyOperation * self)
{
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError,"NULL Operation in Operation.get_name");
        return NULL;
    }
#endif // NDEBUG
    return PyString_FromString("op");
}

/*
 * Operation sequence methods.
 */

#if PY_MINOR_VERSION < 5
#define Py_ssize_t int
#endif

static Py_ssize_t Operation_seq_length(PyOperation * self)
{
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError,"NULL Operation in Operation.seq_length");
        return 0;
    }
#endif // NDEBUG
    return self->operation->getArgs().size();
} 

static PyObject * Operation_seq_item(PyOperation * self, Py_ssize_t item)
{
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError,"NULL Operation in Operation.seq_item");
        return 0;
    }
#endif // NDEBUG
    const std::vector<Root> & args_list = self->operation->getArgs();
    std::vector<Root>::const_iterator I = args_list.begin();
    std::vector<Root>::const_iterator Iend = args_list.end();
    for(int i = 0; i < item && I != Iend; ++i, ++I);
    if (I == args_list.end()) {
        PyErr_SetString(PyExc_TypeError,"Operation.[]: Not enought op arguments");
        return 0;
    }
    const Root & arg = *I;
    RootOperation op = Atlas::Objects::smart_dynamic_cast<RootOperation>(arg);
    if (op.isValid()) {
        PyOperation * ret_op = newPyOperation();
        ret_op->operation = op;
        return (PyObject *)ret_op;
    }
    RootEntity ent = Atlas::Objects::smart_dynamic_cast<RootEntity>(arg);
    if (ent.isValid()) {
        PyRootEntity * ret_ent = newPyRootEntity();
        ret_ent->entity = ent;
        return (PyObject *)ret_ent;
    }
    log(WARNING, "Non operation or entity being returned as arg of operation");
    PyMessageElement * ret = newPyMessageElement();
    ret->m_obj = new Element(arg->asMessage());
    return (PyObject *)ret;
}

static PyObject * Operation_num_add(PyOperation *self, PyObject *other)
{
    fflush(stdout);
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError, "NULL Operation in Operation.num_add");
        fflush(stdout);
        return NULL;
    }
#endif // NDEBUG
    if (other == Py_None) {
        PyOplist * res = newPyOplist();
        res->ops = new OpVector();
        res->ops->push_back(self->operation);
        fflush(stdout);
        return (PyObject*)res;
    }
    if (PyOplist_Check(other)) {
        PyOplist * opl = (PyOplist*)other;
        if (opl->ops == NULL) {
            PyErr_SetString(PyExc_AssertionError, "invalid OpVector");
            return NULL;
        }
        PyOplist * res = newPyOplist();
        if (res == NULL) {
            return NULL;
        }
        res->ops = new OpVector(*opl->ops);
        res->ops->push_back(self->operation);
        fflush(stdout);
        return (PyObject*)res;
    }
    if (PyOperation_Check(other)) {
        PyOperation * op = (PyOperation*)other;
#ifndef NDEBUG
        if (!op->operation.isValid()) {
            PyErr_SetString(PyExc_AssertionError, "NULL Operation in other of Operation.num_add");
        }
#endif // NDEBUG
        PyOplist * res = newPyOplist();
        if (res == NULL) {
            return NULL;
        }
        res->ops = new OpVector();
        res->ops->push_back(op->operation);
        res->ops->push_back(self->operation);
        fflush(stdout);
        return (PyObject*)res;
    }
    fflush(stdout);
    return NULL;
}

/*
 * Operation numerical methods.
 */

static int Operation_num_coerce(PyObject ** self, PyObject ** other)
{
    //if (*other == Py_None) {
        Py_INCREF(*self);
        Py_INCREF(*other);
        return 0;
    //}
    //return -1;
}


/*
 * Operation numerical methods structure.
 */

static PyNumberMethods Operation_num = {
        (binaryfunc)Operation_num_add,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        Operation_num_coerce,
        0,
        0,
        0,
        0,
        0
};

#if PY_MINOR_VERSION < 5
#define lenfunc inquiry
#define ssizeargfunc intargfunc
#endif

/*
 * Operation sequence methods structure.
 */

static PySequenceMethods Operation_seq = {
    (lenfunc)Operation_seq_length,      /* sq_length */
    NULL,                               /* sq_concat */
    NULL,                               /* sq_repeat */
    (ssizeargfunc)Operation_seq_item,   /* sq_item */
    NULL,                               /* sq_slice */
    NULL,                               /* sq_ass_item */
    NULL                                /* sq_ass_slice */
};

/*
 * Operation methods structure.
 *
 * Generated from a macro in case we need one for each type of operation.
 *
 */

PyMethodDef RootOperation_methods[] = {
    {"setSerialno",     (PyCFunction)Operation_setSerialno,     METH_O},
    {"setRefno",        (PyCFunction)Operation_setRefno,        METH_O},
    {"setFrom",         (PyCFunction)Operation_setFrom,         METH_O},
    {"setTo",           (PyCFunction)Operation_setTo,           METH_O},
    {"setSeconds",      (PyCFunction)Operation_setSeconds,      METH_O},
    {"setFutureSeconds",(PyCFunction)Operation_setFutureSeconds,METH_O},
    {"setArgs",         (PyCFunction)Operation_setArgs,         METH_O},
    {"getSerialno",     (PyCFunction)Operation_getSerialno,     METH_NOARGS},
    {"getRefno",        (PyCFunction)Operation_getRefno,        METH_NOARGS},
    {"getFrom",         (PyCFunction)Operation_getFrom,         METH_NOARGS},
    {"getTo",           (PyCFunction)Operation_getTo,           METH_NOARGS},
    {"getSeconds",      (PyCFunction)Operation_getSeconds,      METH_NOARGS},
    {"getFutureSeconds",(PyCFunction)Operation_getFutureSeconds,METH_NOARGS},
    {"getArgs",         (PyCFunction)Operation_getArgs,         METH_NOARGS},
    {"get_name",        (PyCFunction)Operation_get_name,        METH_NOARGS},
    {NULL,          NULL}
};

PyMethodDef ConstRootOperation_methods[] = {
    {"getSerialno",     (PyCFunction)Operation_getSerialno,     METH_NOARGS},
    {"getRefno",        (PyCFunction)Operation_getRefno,        METH_NOARGS},
    {"getFrom",         (PyCFunction)Operation_getFrom,         METH_NOARGS},
    {"getTo",           (PyCFunction)Operation_getTo,           METH_NOARGS},
    {"getSeconds",      (PyCFunction)Operation_getSeconds,      METH_NOARGS},
    {"getFutureSeconds",(PyCFunction)Operation_getFutureSeconds,METH_NOARGS},
    {"getArgs",         (PyCFunction)Operation_getArgs,         METH_NOARGS},
    {"get_name",        (PyCFunction)Operation_get_name,        METH_NOARGS},
    {NULL,          NULL}
};


/*
 * Beginning of Operation standard methods section.
 */

static void Operation_dealloc(PyOperation *self)
{
    self->operation.~RootOperation();
    PyMem_DEL(self);
}

static inline PyObject * findMethod(PyOperation * self, char * name)
{
    return Py_FindMethod(RootOperation_methods, (PyObject *)self, name);
}

static inline PyObject * findMethod(PyConstOperation * self, char * name)
{
    return Py_FindMethod(ConstRootOperation_methods, (PyObject *)self, name);
}

template <typename T>
static PyObject * getattr(T * self, char * name)
{
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError, "NULL Operation in Operation.getattr");
        return NULL;
    }
#endif // NDEBUG
    if (strcmp(name, "from_") == 0) {
        return PyString_FromString(self->operation->getFrom().c_str());
    } else if (strcmp(name, "to") == 0) {
        return PyString_FromString(self->operation->getTo().c_str());
    } else if (strcmp(name, "id") == 0) {
        const std::list<std::string> & parents = self->operation->getParents();
        if (parents.empty()) {
            PyErr_SetString(PyExc_TypeError, "Operation has no parents");
            return NULL;
        }
        return PyString_FromString(parents.front().c_str());
    }
    return findMethod(self, name);
}

static PyObject * Operation_getattr(PyOperation * self, char * name)
{
    return getattr(self, name);
}

static PyObject * ConstOperation_getattr(PyConstOperation * self, char * name)
{
    return getattr(self, name);
}


static int Operation_setattr(PyOperation *self, char *name, PyObject *v)
{
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError, "NULL Operation in Operation.setattr");
        return -1;
    }
#endif // NDEBUG
    if (strcmp(name, "from_") == 0) {
        PyObject * thing_id = PyObject_GetAttrString(v, "id");
        if ((thing_id == NULL) || (!PyString_Check(thing_id))) {
            PyErr_SetString(PyExc_TypeError, "invalid from");
            if (thing_id != NULL) {
                Py_DECREF(thing_id);
            }
            return -1;
        }
        self->operation->setFrom(PyString_AsString(thing_id));
        Py_DECREF(thing_id);
        return 0;
    }
    if (strcmp(name, "to") == 0) {
        PyObject * thing_id = PyObject_GetAttrString(v, "id");
        if ((thing_id == NULL) || (!PyString_Check(thing_id))) {
            PyErr_SetString(PyExc_TypeError, "invalid to");
            if (thing_id != NULL) {
                Py_DECREF(thing_id);
            }
            return -1;
        }
        self->operation->setTo(PyString_AsString(thing_id));
        Py_DECREF(thing_id);
        return 0;
    }
    PyErr_SetString(PyExc_AttributeError, "unknown attribute");
    return -1;
}

PyTypeObject PyOperation_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      // ob_size
        "Operation",                            // tp_name
        sizeof(PyOperation),                    // tp_basicsize
        0,                                      // tp_itemsize
        //  methods 
        (destructor)Operation_dealloc,          // tp_dealloc
        0,                                      // tp_print
        (getattrfunc)Operation_getattr,         // tp_getattr
        (setattrfunc)Operation_setattr,         // tp_setattr
        0,                                      // tp_compare
        0,                                      // tp_repr
        &Operation_num,                         // tp_as_number
        &Operation_seq,                         // tp_as_sequence
        0,                                      // tp_as_mapping
        0,                                      // tp_hash
};

PyTypeObject PyConstOperation_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      // ob_size
        "Operation",                            // tp_name
        sizeof(PyConstOperation),               // tp_basicsize
        0,                                      // tp_itemsize
        //  methods 
        (destructor)Operation_dealloc,          // tp_dealloc
        0,                                      // tp_print
        (getattrfunc)ConstOperation_getattr,    // tp_getattr
        0,                                      // tp_setattr
        0,                                      // tp_compare
        0,                                      // tp_repr
        &Operation_num,                         // tp_as_number
        &Operation_seq,                         // tp_as_sequence
        0,                                      // tp_as_mapping
        0,                                      // tp_hash
};

/*
 * Beginning of Operation creation functions section.
 */

PyOperation * newPyOperation()
{
    PyOperation * self;
    self = PyObject_NEW(PyOperation, &PyOperation_Type);
    if (self == NULL) {
        return NULL;
    }
    new (&(self->operation)) RootOperation(NULL);
    return self;
}

PyConstOperation * newPyConstOperation()
{
    PyConstOperation * self;
    self = PyObject_NEW(PyConstOperation, &PyConstOperation_Type);
    if (self == NULL) {
        return NULL;
    }
    new (&(self->operation)) RootOperation(NULL);
    return self;
}
