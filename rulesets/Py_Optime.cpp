// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include "Python_API.h"

PyMethodDef Optime_methods[] = {
    //{"update",		(PyCFunction)Optime_update,	METH_VARARGS},
    {NULL,		NULL}           /* sentinel */
};

static void Optime_dealloc(OptimeObject *self)
{
    PyMem_DEL(self);
}

static PyObject * Optime_getattr(OptimeObject *self, char *name)
{
    return Py_FindMethod(Optime_methods, (PyObject *)self, name);
}

static int Optime_setattr(OptimeObject *self, char *name, PyObject *v)
{
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid operation time");
        return -1;
    }
    if (strcmp(name, "sadd") == 0) {
        double fsecs;
        if (PyFloat_Check(v)) {
            fsecs = PyFloat_AsDouble(v);
        } else if (PyInt_Check(v)) {
            fsecs = double(PyInt_AsLong(v));
        } else {
            PyErr_SetString(PyExc_TypeError, "setting time to non number");
            return -1;
        }
        self->operation->SetFutureSeconds(fsecs);
        return 0;
    }
    return -1;
}

PyTypeObject Optime_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,				/*ob_size*/
	"Optime",			/*tp_name*/
	sizeof(OptimeObject),		/*tp_basicsize*/
	0,				/*tp_itemsize*/
	/* methods */
	(destructor)Optime_dealloc,	/*tp_dealloc*/
	0,				/*tp_print*/
	(getattrfunc)Optime_getattr,	/*tp_getattr*/
	(setattrfunc)Optime_setattr,	/*tp_setattr*/
	0,				/*tp_compare*/
	0,				/*tp_repr*/
	0,				/*tp_as_number*/
	0,				/*tp_as_sequence*/
	0,				/*tp_as_mapping*/
	0,				/*tp_hash*/
};

OptimeObject * newOptimeObject(PyObject *arg)
{
	OptimeObject * self;
	self = PyObject_NEW(OptimeObject, &Optime_Type);
	if (self == NULL) {
		return NULL;
	}
	return self;
}
