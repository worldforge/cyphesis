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


#include "Py_Operation.h"
#include "Py_RootEntity.h"
#include "Py_Oplist.h"
#include "Py_Message.h"
#include "Py_Thing.h"

#include "common/log.h"

#include <Atlas/Objects/Generic.h>

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Factories;
using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Operation::Generic;
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
    double seconds;
    if (PyFloat_CheckExact(py_secs)) {
        seconds = PyFloat_AsDouble(py_secs);
    } else if (PyInt_CheckExact(py_secs)) {
        seconds = PyInt_AsLong(py_secs);
    } else {
        PyErr_SetString(PyExc_TypeError, "seconds not a number");
        return NULL;
    }
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
    double futureseconds;
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
    std::vector<Root> argslist;
    for(int i = 0; i < PyList_Size(args); i++) {
        PyObject * item = PyList_GetItem(args, i);
        if (PyMessage_Check(item)) {
            Element & e = *((PyMessage*)item)->m_obj;
            if (!e.isMap()) {
                PyErr_SetString(PyExc_TypeError,"args contains non map");
                return NULL;
            }
            argslist.push_back(Factories::instance()->createObject(e.Map()));
        } else if (PyOperation_Check(item)) {
            argslist.push_back(((PyOperation*)item)->operation);
        } else if (PyRootEntity_Check(item)) {
            argslist.push_back(((PyRootEntity*)item)->entity);
        } else {
            std::cout << "o: " << i << item->ob_type->tp_name << std::endl << std::flush;
            PyErr_SetString(PyExc_TypeError,"args contains non Atlas Object");
            return NULL;
        }
    }
    self->operation->setArgs(argslist);

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
    if (args_pylist == NULL) {
        return NULL;
    }
    int j = 0;
    PyMessage * item;
    std::vector<Root>::const_iterator Iend = args_list.end();
    std::vector<Root>::const_iterator I = args_list.begin();
    for (; I != Iend; ++I, ++j) {
        item = newPyMessage();
        if (item == NULL) {
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

#if PY_VERSION_HEX < 0x02050000
typedef int Py_ssize_t;
#endif

static Py_ssize_t Operation_seq_length(PyOperation * self)
{
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError,"NULL Operation in Operation.seq_length");
        return -1;
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
    const std::vector<Root> & args= self->operation->getArgs();
    if (item < 0 || item >= (Py_ssize_t)args.size()) {
        PyErr_SetString(PyExc_IndexError,"Operation.[]: Not enough op arguments");
        return 0;
    }
    const Root & arg = args[item];
    RootOperation op = Atlas::Objects::smart_dynamic_cast<RootOperation>(arg);
    if (op.isValid()) {
        PyOperation * ret_op = newPyOperation();
        if (ret_op != NULL) {
            ret_op->operation = op;
        }
        return (PyObject *)ret_op;
    }
    RootEntity ent = Atlas::Objects::smart_dynamic_cast<RootEntity>(arg);
    if (ent.isValid()) {
        PyRootEntity * ret_ent = newPyRootEntity();
        if (ret_ent != NULL) {
            ret_ent->entity = ent;
        }
        return (PyObject *)ret_ent;
    }
    log(WARNING, "Non operation or entity being returned as arg of operation");
    PyMessage * ret = newPyMessage();
    if (ret != NULL) {
        ret->m_obj = new Element(arg->asMessage());
    }
    return (PyObject *)ret;
}

static PyObject * Operation_num_add(PyOperation *self, PyObject *other)
{
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError, "NULL Operation in Operation.num_add");
        return NULL;
    }
#endif // NDEBUG
    if (other == Py_None) {
#if 0
        PyOplist * res = newPyOplist();
        res->ops = new OpVector();
        res->ops->push_back(self->operation);
        return (PyObject*)res;
#else
        Py_INCREF(self);
        return (PyObject*)self;
#endif
    }
    if (PyOplist_Check(other)) {
        PyOplist * opl = (PyOplist*)other;
#ifndef NDEBUG
        if (opl->ops == NULL) {
            PyErr_SetString(PyExc_AssertionError, "invalid OpVector");
            return NULL;
        }
#endif // NDEBUG
        PyOplist * res = newPyOplist();
        if (res != NULL) {
            res->ops = new OpVector(*opl->ops);
            res->ops->push_back(self->operation);
        }
        return (PyObject*)res;
    }
    if (PyOperation_Check(other)) {
        PyOperation * op = (PyOperation*)other;
#ifndef NDEBUG
        if (!op->operation.isValid()) {
            PyErr_SetString(PyExc_AssertionError, "NULL Operation in other of Operation.num_add");
            return NULL;
        }
#endif // NDEBUG
        PyOplist * res = newPyOplist();
        if (res != NULL) {
            res->ops = new OpVector();
            res->ops->push_back(op->operation);
            res->ops->push_back(self->operation);
        }
        return (PyObject*)res;
    }
    PyErr_SetString(PyExc_TypeError, "Unknown other in Operation.num_add");
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

PyMethodDef Operation_methods[] = {
    {"setSerialno",     (PyCFunction)Operation_setSerialno,     METH_O},
    {"setRefno",        (PyCFunction)Operation_setRefno,        METH_O},
    {"setFrom",         (PyCFunction)Operation_setFrom,         METH_O},
    {"setTo",           (PyCFunction)Operation_setTo,           METH_O},
    {"setSeconds",      (PyCFunction)Operation_setSeconds,      METH_O},
    {"setFutureSeconds",(PyCFunction)Operation_setFutureSeconds,METH_O},
    {"setArgs",         (PyCFunction)Operation_setArgs,         METH_O},
    {NULL,          NULL}
};

PyMethodDef ConstOperation_methods[] = {
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
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject * Operation_getattro(PyOperation * self, PyObject * oname)
{
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError, "NULL Operation in Operation.getattr");
        return NULL;
    }
#endif // NDEBUG
    char * name = PyString_AsString(oname);
    if (strcmp(name, "from_") == 0) {
        return PyString_FromString(self->operation->getFrom().c_str());
    } else if (strcmp(name, "to") == 0) {
        return PyString_FromString(self->operation->getTo().c_str());
    } else if (strcmp(name, "id") == 0) {
        const std::list<std::string> & parents = self->operation->getParents();
        if (parents.empty()) {
            PyErr_SetString(PyExc_AttributeError, "Operation has no parents");
            return NULL;
        }
        return PyString_FromString(parents.front().c_str());
    }
    return PyObject_GenericGetAttr((PyObject *)self, oname);
}

static int Operation_setattro(PyOperation *self, PyObject * oname, PyObject *v)
{
#ifndef NDEBUG
    if (!self->operation.isValid()) {
        PyErr_SetString(PyExc_AssertionError, "NULL Operation in Operation.setattr");
        return -1;
    }
#endif // NDEBUG
    char * name = PyString_AsString(oname);
    if (strcmp(name, "from_") == 0) {
        PyObject * thing_id = PyObject_GetAttrString(v, "id");
        if (thing_id == NULL || !PyString_Check(thing_id)) {
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
        if (thing_id == NULL || !PyString_Check(thing_id)) {
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

static int addToArgs(std::vector<Root> & args, PyObject * arg)
{
    if (PyMessage_Check(arg)) {
        PyMessage * obj = (PyMessage*)arg;
#ifndef NDEBUG
        if (obj->m_obj == NULL) {
            PyErr_SetString(PyExc_AssertionError,"NULL MessageElement in Operation constructor argument");
            return -1;
        }
#endif // NDEBUG
        const Element & o = *obj->m_obj;
        if (o.isMap()) {
            args.push_back(Atlas::Objects::Factories::instance()->createObject(o.asMap()));
        } else {
            PyErr_SetString(PyExc_TypeError, "Operation arg is not a map");
            return -1;
        }
    } else if (PyOperation_Check(arg)) {
        PyOperation * op = (PyOperation*)arg;
#ifndef NDEBUG
        if (!op->operation.isValid()) {
            PyErr_SetString(PyExc_AssertionError,"Invalid operation in Operation constructor argument");
            return -1;
        }
#endif // NDEBUG
        args.push_back(op->operation);
    } else if (PyRootEntity_Check(arg)) {
        PyRootEntity * ent = (PyRootEntity*)arg;
#ifndef NDEBUG
        if (!ent->entity.isValid()) {
            PyErr_SetString(PyExc_AssertionError,"Invalid rootentity in Operation constructor argument");
            return -1;
        }
#endif // NDEBUG
        args.push_back(ent->entity);
    } else {
        PyErr_SetString(PyExc_TypeError, "Operation arg is of unknown type");
        return -1;
    }
    return 0;
}

static int Operation_init(PyOperation * self, PyObject * args, PyObject * kwds)
{
    char * type;
    PyObject * arg1 = NULL;
    PyObject * arg2 = NULL;
    PyObject * arg3 = NULL;

    if (!PyArg_ParseTuple(args, "s|OOO", &type, &arg1, &arg2, &arg3)) {
        return -1;
    }
    Root r = Atlas::Objects::Factories::instance()->createObject(type);
    self->operation = Atlas::Objects::smart_dynamic_cast<RootOperation>(r);
    if (!self->operation.isValid()) {
        // PyErr_SetString(PyExc_TypeError, "Operation() unknown operation type requested");
        // return -1;
        self->operation = Generic();
        self->operation->setParents(std::list<std::string>(1, type));
    }
    if (kwds != NULL) {
        PyObject * from = PyDict_GetItemString(kwds, "from_");
        if (from != NULL) {
            PyObject * from_id = 0;
            if (PyString_Check(from)) {
                from_id = from;
                Py_INCREF(from_id);
            } else if ((from_id = PyObject_GetAttrString(from, "id")) == NULL) {
                PyErr_SetString(PyExc_TypeError, "from is not a string and has no id");
                return -1;
            }
            if (!PyString_Check(from_id)) {
                Py_DECREF(from_id);
                PyErr_SetString(PyExc_TypeError, "id of from is not a string");
                return -1;
            }
            self->operation->setFrom(PyString_AsString(from_id));
            Py_DECREF(from_id);
        }
        PyObject * to = PyDict_GetItemString(kwds, "to");
        if (to != NULL) {
            PyObject * to_id = 0;
            if (PyString_Check(to)) {
                to_id = to;
                Py_INCREF(to_id);
            } else if ((to_id = PyObject_GetAttrString(to, "id")) == NULL) {
                PyErr_SetString(PyExc_TypeError, "to is not a string and has no id");
                return -1;
            }
            if (!PyString_Check(to_id)) {
                Py_DECREF(to_id);
                PyErr_SetString(PyExc_TypeError, "id of to is not a string");
                return -1;
            }
            self->operation->setTo(PyString_AsString(to_id));
            Py_DECREF(to_id);
        }
    }
    std::vector<Root> & args_list = self->operation->modifyArgs();
    assert(args_list.empty());
    if (arg1 != 0 && addToArgs(args_list, arg1) != 0) {
        return -1;
    }
    if (arg2 != 0 && addToArgs(args_list, arg2) != 0) {
        return -1;
    }
    if (arg3 != 0 && addToArgs(args_list, arg3) != 0) {
        return -1;
    }
    return 0;
}

static PyObject * Operation_new(PyTypeObject * type, PyObject *, PyObject *)
{
    // This looks allot like the default implementation, except we call the
    // in-place constructor.
    PyOperation * self = (PyOperation *)type->tp_alloc(type, 0);
    if (self != NULL) {
        new (&(self->operation)) RootOperation(nullptr);
    }
    return (PyObject *)self;
}

PyTypeObject PyConstOperation_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      // ob_size
        "atlas.Operation",                      // tp_name
        sizeof(PyOperation),                    // tp_basicsize
        0,                                      // tp_itemsize
        //  methods 
        (destructor)Operation_dealloc,          // tp_dealloc
        0,                                      // tp_print
        0,                                      // tp_getattr
        0,                                      // tp_setattr
        0,                                      // tp_compare
        0,                                      // tp_repr
        &Operation_num,                         // tp_as_number
        &Operation_seq,                         // tp_as_sequence
        0,                                      // tp_as_mapping
        0,                                      // tp_hash
        0,                                      // tp_call
        0,                                      // tp_str
        (getattrofunc)Operation_getattro,       // tp_getattro
        0,                                      // tp_setattro
        0,                                      // tp_as_buffer
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, // tp_flags
        "Operation objects",                    // tp_doc
        0,                                      // tp_travers
        0,                                      // tp_clear
        0,                                      // tp_richcompare
        0,                                      // tp_weaklistoffset
        0,                                      // tp_iter
        0,                                      // tp_iternext
        ConstOperation_methods,                 // tp_methods
        0,                                      // tp_members
        0,                                      // tp_getset
        0,                                      // tp_base
        0,                                      // tp_dict
        0,                                      // tp_descr_get
        0,                                      // tp_descr_set
        0,                                      // tp_dictoffset
        (initproc)Operation_init,               // tp_init
        0,                                      // tp_alloc
        Operation_new,                          // tp_new
};

PyTypeObject PyOperation_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      // ob_size
        "atlas.Operation",                      // tp_name
        0,                                      // tp_basicsize
        0,                                      // tp_itemsize
        //  methods 
        0,                                      // tp_dealloc
        0,                                      // tp_print
        0,                                      // tp_getattr
        0,                                      // tp_setattr
        0,                                      // tp_compare
        0,                                      // tp_repr
        &Operation_num,                         // tp_as_number
        &Operation_seq,                         // tp_as_sequence
        0,                                      // tp_as_mapping
        0,                                      // tp_hash
        0,                                      // tp_call
        0,                                      // tp_str
        0,                                      // tp_getattro
        (setattrofunc)Operation_setattro,       // tp_setattro
        0,                                      // tp_as_buffer
        Py_TPFLAGS_DEFAULT,                     // tp_flags
        "Operation objects",                    // tp_doc
        0,                                      // tp_travers
        0,                                      // tp_clear
        0,                                      // tp_richcompare
        0,                                      // tp_weaklistoffset
        0,                                      // tp_iter
        0,                                      // tp_iternext
        Operation_methods,                      // tp_methods
        0,                                      // tp_members
        0,                                      // tp_getset
        &PyConstOperation_Type,                 // tp_base
        0,                                      // tp_dict
        0,                                      // tp_descr_get
        0,                                      // tp_descr_set
        0,                                      // tp_dictoffset
        0,                                      // tp_init
        0,                                      // tp_alloc
        Operation_new,                          // tp_new
};

/*
 * Beginning of Operation creation functions section.
 */

PyOperation * newPyOperation()
{
    return (PyOperation *)PyOperation_Type.tp_new(&PyOperation_Type, 0, 0);
}

PyOperation * newPyConstOperation()
{
    return (PyOperation *)PyConstOperation_Type.tp_new(&PyConstOperation_Type, 0, 0);
}
