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
#include "Py_Oplist.h"

static PyObject* Oplist_append(PyOplist * self, PyOperation * op)
{
#ifndef NDEBUG
    if (self->ops == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr Oplist in Oplist.append");
        return nullptr;
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
        return nullptr;
    }
    Py_INCREF(Py_None);
    return Py_None;
}


static PyMethodDef Oplist_methods[] = {
    {"append",          (PyCFunction)Oplist_append,     METH_O},
    {nullptr,              nullptr}           /* sentinel */
};

static void Oplist_dealloc(PyOplist *self)
{
    delete self->ops;
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject * Oplist_num_add(PyOplist *self, PyObject *other)
{
#ifndef NDEBUG
    if (self->ops == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr Oplist in Oplist.num_add");
        return nullptr;
    }
#endif // NDEBUG
    if (other == Py_None) {
        Py_INCREF(self);
        return (PyObject*)self;
    }
    if (PyOplist_Check(other)) {
        PyOplist * opl = (PyOplist*)other;
        PyOplist * res = newPyOplist();
        if (res != nullptr) {
            res->ops = new OpVector(*self->ops);
            OpVector::const_iterator Iend = opl->ops->end();
            for (OpVector::const_iterator I = opl->ops->begin(); I != Iend; ++I) {
                res->ops->push_back(*I);
            }
        }
        return (PyObject*)res;
    }
    if (PyOperation_Check(other)) {
        PyOperation * op = (PyOperation*)other;
#ifndef NDEBUG
        if (!op->operation.isValid()) {
            PyErr_SetString(PyExc_ValueError, "Invalid Operation in other of Oplist.num_add");
            return nullptr;
        }
#endif // NDEBUG
        PyOplist * res = newPyOplist();
        if (res != nullptr) {
            res->ops = new OpVector(*self->ops);
            res->ops->push_back(op->operation);
        }
        return (PyObject*)res;
    }
    PyErr_SetString(PyExc_TypeError, "Unknown other in Oplist.num_add");
    return nullptr;
}

static PyObject * Oplist_num_inplace_add(PyOplist * self, PyObject * other)
{
#ifndef NDEBUG
    if (self->ops == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr Oplist in Oplist.num_inplace_add");
        return nullptr;
    }
#endif // NDEBUG
    if (other == Py_None) {
        Py_INCREF(self);
        return (PyObject*)self;
    }
    if (PyOplist_Check(other)) {
        PyOplist * opl = (PyOplist*)other;
        OpVector::const_iterator Iend = opl->ops->end();
        for (OpVector::const_iterator I = opl->ops->begin(); I != Iend; ++I) {
            self->ops->push_back(*I);
        }
        Py_INCREF(self);
        return (PyObject*)self;
    }
    if (PyOperation_Check(other)) {
        PyOperation * op = (PyOperation*)other;
#ifndef NDEBUG
        if (!op->operation.isValid()) {
            PyErr_SetString(PyExc_ValueError, "Invalid Operation in other of Oplist.num_inplace_add");
            return nullptr;
        }
#endif // NDEBUG
        self->ops->push_back(op->operation);
        Py_INCREF(self);
        return (PyObject*)self;
    }
    PyErr_SetString(PyExc_TypeError, "Unknown other in Oplist.num_inplace_add");
    return nullptr;
}

static Py_ssize_t Oplist_seq_length(PyOplist * self)
{
#ifndef NDEBUG
    if (self->ops == nullptr) {
        PyErr_SetString(PyExc_AssertionError,"Invalid Oplist in Oplist.seq_length");
        return -1;
    }
#endif // NDEBUG
    return self->ops->size();
} 


static PyMappingMethods Oplist_as_mapping = {
    (lenfunc)Oplist_seq_length,      /* mp_length */
    nullptr,
    nullptr
};

static PySequenceMethods Oplist_as_sequence = {
    (lenfunc)Oplist_seq_length,      /* sq_length */
    nullptr,                            /* sq_concat */
    nullptr,                            /* sq_repeat */
    nullptr,                            /* sq_item */
    nullptr,                            /* sq_slice */
    nullptr,                            /* sq_ass_item */
    nullptr                             /* sq_ass_slice */
};

static inline int addToOplist(PyOperation * op, PyOplist * o)
{
    if (op != nullptr) {
       if (PyOperation_Check(op)) {
           o->ops->push_back(op->operation);
       } else if ((PyObject*)op != Py_None) {
           PyErr_SetString(PyExc_TypeError, "Argument must be an op");
           return -1;
       }
    }
    return 0;
}

// FIXME Lots of silent failure here, and oddly unexpected results.
static int Oplist_init(PyOplist * self, PyObject * args, PyObject * kwds)
{
    PyOperation *op1 = nullptr, *op2 = nullptr, *op3 = nullptr, *op4 = nullptr;
    if (!PyArg_ParseTuple(args, "|OOOO", &op1, &op2, &op3, &op4)) {
        return -1;
    }
    self->ops = new OpVector();
    if (addToOplist(op1, self) != 0) {
        return -1;
    }
    if (addToOplist(op2, self) != 0) {
        return -1;
    }
    if (addToOplist(op3, self) != 0) {
        return -1;
    }
    if (addToOplist(op4, self) != 0) {
        return -1;
    }
    return 0;
}

static PyNumberMethods Oplist_as_number = {
    (binaryfunc)Oplist_num_add,               // nb_add;
    0,                                        // nb_subtract;
    0,                                        // nb_multiply;
    0,                                        // nb_remainder;
    0,                                        // nb_divmod;
    0,                                        // nb_power;
    0,                                        // nb_negative;
    0,                                        // nb_positive;
    0,                                        // nb_absolute;
    0,                                        // nb_nonzero;
    0,                                        // nb_invert;
    0,                                        // nb_lshift;
    0,                                        // nb_rshift;
    0,                                        // nb_and;
    0,                                        // nb_xor;
    0,                                        // nb_or;
    0,                                        // nb_int;
    0,                                        // nb_long;
    0,                                        // nb_float;
    /* Added in release 2.0 */
    (binaryfunc)Oplist_num_inplace_add,       // nb_inplace_add;
    0,                                        // nb_inplace_subtract;
    0,                                        // nb_inplace_multiply;
    0,                                        // nb_inplace_remainder;
    0,                                        // nb_inplace_power;
    0,                                        // nb_inplace_lshift;
    0,                                        // nb_inplace_rshift;
    0,                                        // nb_inplace_and;
    0,                                        // nb_inplace_xor;
    0,                                        // nb_inplace_or;
};



PyTypeObject PyOplist_Type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
        "atlas.Oplist",                       /*tp_name*/
        sizeof(PyOplist),               /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        (destructor)Oplist_dealloc,     /*tp_dealloc*/
        0,                              /*tp_print*/
        0,                              /*tp_getattr*/
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
        Oplist_methods,                 // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        0,                              // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)Oplist_init,          // tp_init
        0,                              // tp_alloc
        0,                              // tp_new
};

PyOplist * newPyOplist()
{
    return (PyOplist *)PyOplist_Type.tp_new(&PyOplist_Type, 0, 0);
}
