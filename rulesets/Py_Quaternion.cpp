// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_Quaternion.h"

static PyObject * Quaternion_as_list(QuaternionObject * self, PyObject * args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return NULL;
    }
    PyObject * r = PyList_New(0);
    PyObject * i = PyFloat_FromDouble(self->rotation.X());
    i = PyFloat_FromDouble(self->rotation.X());
    PyList_Append(r, i);
    Py_DECREF(i);
    i = PyFloat_FromDouble(self->rotation.Y());
    PyList_Append(r, i);
    Py_DECREF(i);
    i = PyFloat_FromDouble(self->rotation.Z());
    PyList_Append(r, i);
    Py_DECREF(i);
    i = PyFloat_FromDouble(self->rotation.W());
    PyList_Append(r, i);
    Py_DECREF(i);
    return r;
}

static PyMethodDef Quaternion_methods[] = {
    {"as_list",         (PyCFunction)Quaternion_as_list, METH_VARARGS},
    {NULL,		NULL}           /* sentinel */
};

static void Quaternion_dealloc(QuaternionObject *self)
{
    PyMem_DEL(self);
}

static PyObject * Quaternion_getattr(QuaternionObject *self, char *name)
{
    if (strcmp(name, "x") == 0) { return PyFloat_FromDouble(self->rotation.X()); }
    if (strcmp(name, "y") == 0) { return PyFloat_FromDouble(self->rotation.Y()); }
    if (strcmp(name, "z") == 0) { return PyFloat_FromDouble(self->rotation.Z()); }
    if (strcmp(name, "w") == 0) { return PyFloat_FromDouble(self->rotation.W()); }

    return Py_FindMethod(Quaternion_methods, (PyObject *)self, name);
}

static int Quaternion_setattr(QuaternionObject *self, char *name, PyObject *v)
{
    return 0;
}

static int Quaternion_compare(QuaternionObject * self, QuaternionObject * other)
{
    if (!PyQuaternion_Check(other)) {
        return -1;
    }
    if (self->rotation == other->rotation) {
        return 0;
    }
    return 1;
}

PyTypeObject Quaternion_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,				/*ob_size*/
	"Quaternion",			/*tp_name*/
	sizeof(QuaternionObject),		/*tp_basicsize*/
	0,				/*tp_itemsize*/
	/* methods */
	(destructor)Quaternion_dealloc,	/*tp_dealloc*/
	0,				/*tp_print*/
	(getattrfunc)Quaternion_getattr,	/*tp_getattr*/
	(setattrfunc)Quaternion_setattr,	/*tp_setattr*/
	(cmpfunc)Quaternion_compare,	/*tp_compare*/
	0,				/*tp_repr*/
	0,				/*tp_as_number*/
	0,				/*tp_as_sequence*/
	0,				/*tp_as_mapping*/
	0,				/*tp_hash*/
};

QuaternionObject * newQuaternionObject(PyObject *arg)
{
	QuaternionObject * self;
	self = PyObject_NEW(QuaternionObject, &Quaternion_Type);
	if (self == NULL) {
		return NULL;
	}
	return self;
}
