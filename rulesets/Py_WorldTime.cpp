// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_WorldTime.h"
#include <modules/WorldTime.h>

static PyObject *WorldTime_seconds(WorldTimeObject *self, PyObject *args, PyObject *kwds)
{
    if (self->time == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid world object");
        return NULL;
    }
    if (!PyArg_ParseTuple(args, "")) {
        PyErr_SetString(PyExc_TypeError,"too many args");
        return NULL;
    }
    return PyFloat_FromDouble(self->time->seconds());
}

static PyObject * WorldTime_is_now(WorldTimeObject *self, PyObject *args)
{
    char * other;
    if (!PyArg_ParseTuple(args, "s", &other)) {
        PyErr_SetString(PyExc_TypeError,"too many args");
        return NULL;
    }
    //printf("Python worldtime is string\n");
    bool eq = (*self->time == std::string(other));
    PyObject * ret = eq ? Py_True : Py_False;
    Py_INCREF(ret);
    return ret;
}

static PyMethodDef WorldTime_methods[] = {
    {"seconds",		(PyCFunction)WorldTime_seconds,	METH_VARARGS},
    {"is_now",		(PyCFunction)WorldTime_is_now,	METH_VARARGS},
    {NULL,		NULL}           /* sentinel */
};

static void WorldTime_dealloc(WorldTimeObject *self)
{
    if ((self->own) && (self->time != NULL)) {
        delete self->time;
    }
    PyMem_DEL(self);
}

static PyObject * WorldTime_getattr(WorldTimeObject *self, char *name)
{
    return Py_FindMethod(WorldTime_methods, (PyObject *)self, name);
}

static int WorldTime_setattr(WorldTimeObject *self, char *name, PyObject *v)
{
    if (self->time == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid worldtime");
        return -1;
    }
    if (strcmp(name, "foo") == 0) {
    }
    return -1;
}

static int WorldTime_cmp(WorldTimeObject *self, PyObject *other)
{
    if (PyString_Check(other)) {
        printf("Python compare of worldtime to string\n");
        bool eq = (*self->time == std::string(PyString_AsString(other)));
        return eq ? 0 : -1;
    } else {
        printf("Python compare of worldtime to ?\n");
        return -1;
    }
}

PyTypeObject WorldTime_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,				/*ob_size*/
	"WorldTime",			/*tp_name*/
	sizeof(WorldTimeObject),		/*tp_basicsize*/
	0,				/*tp_itemsize*/
	/* methods */
	(destructor)WorldTime_dealloc,	/*tp_dealloc*/
	0,				/*tp_print*/
	(getattrfunc)WorldTime_getattr,	/*tp_getattr*/
	(setattrfunc)WorldTime_setattr,	/*tp_setattr*/
	(cmpfunc)WorldTime_cmp,		/*tp_compare*/
	0,				/*tp_repr*/
	0,				/*tp_as_number*/
	0,				/*tp_as_sequence*/
	0,				/*tp_as_mapping*/
	0,				/*tp_hash*/
};

WorldTimeObject * newWorldTimeObject(PyObject *arg)
{
	WorldTimeObject * self;
	self = PyObject_NEW(WorldTimeObject, &WorldTime_Type);
	if (self == NULL) {
		return NULL;
	}
        self->own = false;
	return self;
}
