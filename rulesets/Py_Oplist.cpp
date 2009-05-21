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

#include "Py_Operation.h"
#include "Py_Oplist.h"

static PyObject* Oplist_append(PyOplist * self, PyOperation * op)
{
#ifndef NDEBUG
    if (self->ops == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Oplist in Oplist.append");
        return NULL;
    }
#endif // NDEBUG
    if (PyOperation_Check(op)) {
        self->ops->push_back(op->operation);
    } else if (PyOplist_Check(op)) {
        PyOplist * opl = (PyOplist*)op;
        OpVector::const_iterator Iend = opl->ops->end();
        for (OpVector::const_iterator I = opl->ops->begin(); I != Iend; ++I) {
            self->ops->push_back(*I);
        }
    } else if ((PyObject*)op != Py_None) {
        PyErr_SetString(PyExc_TypeError, "Append must be an op or message");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}


static PyMethodDef Oplist_methods[] = {
    {"append",          (PyCFunction)Oplist_append,     METH_O},
    {NULL,              NULL}           /* sentinel */
};

static void Oplist_dealloc(PyOplist *self)
{
    if (self->ops != NULL) {
        delete self->ops;
    }
    PyObject_Free(self);
}

static PyObject * Oplist_getattr(PyOplist *self, char *name)
{
    return Py_FindMethod(Oplist_methods, (PyObject *)self, name);
}


static PyObject * Oplist_num_add(PyOplist *self, PyObject *other)
{
#ifndef NDEBUG
    if (self->ops == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Oplist in Oplist.num_add");
        return NULL;
    }
#endif // NDEBUG
    if (other == Py_None) {
        Py_INCREF(self);
        return (PyObject*)self;
    }
    if (PyOplist_Check(other)) {
        PyOplist * opl = (PyOplist*)other;
        PyOplist * res = newPyOplist();
        if (res == NULL) {
            return NULL;
        }
        res->ops = new OpVector(*self->ops);
        OpVector::const_iterator Iend = opl->ops->end();
        for (OpVector::const_iterator I = opl->ops->begin(); I != Iend; ++I) {
            res->ops->push_back(*I);
        }
        return (PyObject*)res;
    }
    if (PyOperation_Check(other)) {
        PyOperation * op = (PyOperation*)other;
#ifndef NDEBUG
        if (!op->operation.isValid()) {
            PyErr_SetString(PyExc_AssertionError, "NULL Operation in other of Oplist.num_add");
        }
#endif // NDEBUG
        PyOplist * res = newPyOplist();
        if (res == NULL) {
            return NULL;
        }
        res->ops = new OpVector(*self->ops);
        // res->ops->merge(*self->ops);
        res->ops->push_back(op->operation);
        return (PyObject*)res;
    }
    return NULL;
}

static int Oplist_num_coerce(PyObject ** self, PyObject ** other)
{
    //if (*other == Py_None) {
        Py_INCREF(*self);
        Py_INCREF(*other);
        return 0;
    //}
    //return -1;
}

#if PY_MINOR_VERSION < 5
#define lenfunc inquiry
#endif

#if PY_VERSION_HEX < 0x02050000
typedef int Py_ssize_t;
#endif

static Py_ssize_t Oplist_seq_length(PyOplist * self)
{
#ifndef NDEBUG
    if (self->ops == NULL) {
        PyErr_SetString(PyExc_AssertionError,"Invalid Oplist in Oplist.seq_length");
        return 0;
    }
#endif // NDEBUG
    return self->ops->size();
} 

#if PY_MINOR_VERSION < 5
#define lenfunc inquiry
#endif

static PyMappingMethods Oplist_as_mapping = {
    (lenfunc)Oplist_seq_length,      /* mp_length */
    NULL,
    NULL
};

static PySequenceMethods Oplist_as_sequence = {
    (lenfunc)Oplist_seq_length,      /* sq_length */
    NULL,                            /* sq_concat */
    NULL,                            /* sq_repeat */
    NULL,                            /* sq_item */
    NULL,                            /* sq_slice */
    NULL,                            /* sq_ass_item */
    NULL                             /* sq_ass_slice */
};

static inline void addToOplist(PyOperation * op, PyOplist * o)
{
    if (op != NULL) {
       if (PyOperation_Check(op)) {
           o->ops->push_back(op->operation);
       } else if ((PyObject*)op != Py_None) {
           PyErr_SetString(PyExc_TypeError, "Argument must be an op");
           return;
       }
    }
}

static int Oplist_init(PyOplist * self, PyObject * args, PyObject * kwds)
{
    PyOperation *op1 = NULL, *op2 = NULL, *op3 = NULL, *op4 = NULL;
    if (!PyArg_ParseTuple(args, "|OOOO", &op1, &op2, &op3, &op4)) {
        return -1;
    }
    self->ops = new OpVector();
    addToOplist(op1, self);
    addToOplist(op2, self);
    addToOplist(op3, self);
    addToOplist(op4, self);
    return 0;
}

static PyObject * Oplist_new(PyTypeObject * type, PyObject *, PyObject *)
{
    // This looks allot like the default implementation, except we call the
    // in-place constructor.
    PyOplist * self = (PyOplist *)type->tp_alloc(type, 0);
    return (PyObject *)self;
}

static PyNumberMethods Oplist_as_number = {
        (binaryfunc)Oplist_num_add,
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
        Oplist_num_coerce,
        0,
        0,
        0,
        0,
        0
};

PyTypeObject PyOplist_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /*ob_size*/
        "Oplist",                       /*tp_name*/
        sizeof(PyOplist),               /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        (destructor)Oplist_dealloc,     /*tp_dealloc*/
        0,                              /*tp_print*/
        (getattrfunc)Oplist_getattr,    /*tp_getattr*/
        0,                              /*tp_setattr*/
        0,                              /*tp_compare*/
        0,                              /*tp_repr*/
        &Oplist_as_number,              /*tp_as_number*/
        &Oplist_as_sequence,            /*tp_as_sequence*/
        &Oplist_as_mapping,             /*tp_as_mapping*/
        0,                              /*tp_hash*/
        0,                              // tp_call
        0,                              // tp_str
        0,                              // tp_getattro
        0,                              // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT,             // tp_flags
        "Oplist objects",               // tp_doc
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
        (initproc)Oplist_init,          // tp_init
        0,                              // tp_alloc
        Oplist_new,                     // tp_new
};

PyOplist * newPyOplist()
{
#if 0
        PyOplist * self;
        self = PyObject_NEW(PyOplist, &PyOplist_Type);
        if (self == NULL) {
                return NULL;
        }
        return self;
#else
    return (PyOplist *)PyOplist_Type.tp_new(&PyOplist_Type, 0, 0);
#endif
}
