#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include <server/WorldTime.h>

#include "Python_API.h"

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


PyMethodDef WorldTime_methods[] = {
    {"seconds",		(PyCFunction)WorldTime_seconds,	METH_VARARGS},
    {NULL,		NULL}           /* sentinel */
};

static void WorldTime_dealloc(WorldTimeObject *self)
{
    Py_XDECREF(self->WorldTime_attr);
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
	0,				/*tp_compare*/
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
	self->WorldTime_attr = NULL;
	return self;
}
