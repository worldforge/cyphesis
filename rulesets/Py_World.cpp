#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include <server/WorldTime.h>

#include "Python_API.h"

static PyObject * World_get_time(WorldObject *self, PyObject *args, PyObject *kw)
{
    if (self->world == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid world object");
        return NULL;
    }
    if (!PyArg_ParseTuple(args, "")) {
        PyErr_SetString(PyExc_TypeError,"too many args");
        return NULL;
    }
    WorldTimeObject * wtime = newWorldTimeObject(NULL);
    if (wtime == NULL) {
        return NULL;
    }
    // wtime->time = self->world->get_time(); FIXME (IT)
    wtime->time = new WorldTime(0);
    return (PyObject *)wtime;
}

PyMethodDef World_methods[] = {
    {"get_time",	(PyCFunction)World_get_time,	METH_VARARGS},
    {NULL,		NULL}           /* sentinel */
};

static void World_dealloc(WorldObject *self)
{
    Py_XDECREF(self->World_attr);
    PyMem_DEL(self);
}

PyObject * World_getattr(WorldObject *self, char *name)
{
    return Py_FindMethod(World_methods, (PyObject *)self, name);
}

int World_setattr(WorldObject *self, char *name, PyObject *v)
{
    return(0);
}

PyTypeObject World_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,				/*ob_size*/
	"World",			/*tp_name*/
	sizeof(WorldObject),		/*tp_basicsize*/
	0,				/*tp_itemsize*/
	/* methods */
	(destructor)World_dealloc,	/*tp_dealloc*/
	0,				/*tp_print*/
	(getattrfunc)World_getattr,	/*tp_getattr*/
	(setattrfunc)World_setattr,	/*tp_setattr*/
	0,				/*tp_compare*/
	0,				/*tp_repr*/
	0,				/*tp_as_number*/
	0,				/*tp_as_sequence*/
	0,				/*tp_as_mapping*/
	0,				/*tp_hash*/
};

WorldObject * newWorldObject(PyObject *arg)
{
	WorldObject * self;
	self = PyObject_NEW(WorldObject, &World_Type);
	if (self == NULL) {
		return NULL;
	}
	self->World_attr = NULL;
	return self;
}
