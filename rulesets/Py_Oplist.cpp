// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include "Python_API.h"

static PyObject* Oplist_append(OplistObject * self, PyObject * args)
{
    if (self->ops == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid oplist");
        return NULL;
    }
    RootOperationObject * op;
    if (!PyArg_ParseTuple(args, "O", &op)) {
        return NULL;
    }
    if (PyOperation_Check(op)) {
        self->ops->push_back(op->operation);
        op->own = 0;
    } else if ((PyObject*)op != Py_None) {
        PyErr_SetString(PyExc_TypeError, "Append must be an op");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}


static PyMethodDef Oplist_methods[] = {
    {"append",		(PyCFunction)Oplist_append,	METH_VARARGS},
    {NULL,		NULL}           /* sentinel */
};

static void Oplist_dealloc(OplistObject *self)
{
    if (self->ops != NULL) {
        delete self->ops;
    }
    PyMem_DEL(self);
}

static PyObject * Oplist_getattr(OplistObject *self, char *name)
{
    return Py_FindMethod(Oplist_methods, (PyObject *)self, name);
}


static PyObject * Oplist_num_add(OplistObject *self, PyObject *other)
{
    if (self->ops == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid oplist");
        return NULL;
    }
    if (other == Py_None) {
        Py_INCREF(self);
        return (PyObject*)self;
    }
    if (PyOplist_Check(other)) {
        OplistObject * opl = (OplistObject*)other;
        OplistObject * res = newOplistObject(NULL);
        res->ops = new oplist(*self->ops);
        if (res == NULL) {
            return NULL;
        }
        // res->ops->merge(*self->ops);
        for(oplist::const_iterator I=opl->ops->begin();I!=opl->ops->end();I++) {
            res->ops->push_back(*I);
        }
        //res->ops->merge(*opl->ops);
        return (PyObject*)res;
    }
    if (PyOperation_Check(other)) {
        RootOperationObject * op = (RootOperationObject*)other;
        if (op->operation == NULL) {
            PyErr_SetString(PyExc_TypeError, "invalid operation");
        }
        OplistObject * res = newOplistObject(NULL);
        res->ops = new oplist(*self->ops);
        if (res == NULL) {
            return NULL;
        }
        // res->ops->merge(*self->ops);
        res->ops->push_back(op->operation);
        op->own = 0;
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

static int Oplist_seq_length(OplistObject * self)
{
    if (self->ops == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid operation");
        return 0;
    }
    return self->ops->size();
} 

static PyMappingMethods Oplist_as_mapping = {
    (inquiry)Oplist_seq_length,      /* mp_length */
    NULL,
    NULL
};

static PySequenceMethods Oplist_as_sequence = {
    (inquiry)Oplist_seq_length,      /* sq_length */
    NULL,                            /* sq_concat */
    NULL,                            /* sq_repeat */
    NULL,			     /* sq_item */
    NULL,                            /* sq_slice */
    NULL,                            /* sq_ass_item */
    NULL                             /* sq_ass_slice */
};




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

PyTypeObject Oplist_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,				/*ob_size*/
	"Oplist",			/*tp_name*/
	sizeof(OplistObject),		/*tp_basicsize*/
	0,				/*tp_itemsize*/
	/* methods */
	(destructor)Oplist_dealloc,	/*tp_dealloc*/
	0,				/*tp_print*/
	(getattrfunc)Oplist_getattr,	/*tp_getattr*/
	0,				/*tp_setattr*/
	0,				/*tp_compare*/
	0,				/*tp_repr*/
	&Oplist_as_number,		/*tp_as_number*/
	&Oplist_as_sequence,		/*tp_as_sequence*/
	&Oplist_as_mapping,		/*tp_as_mapping*/
	0,				/*tp_hash*/
};

OplistObject * newOplistObject(PyObject *arg)
{
	OplistObject * self;
	self = PyObject_NEW(OplistObject, &Oplist_Type);
	if (self == NULL) {
		return NULL;
	}
	return self;
}
