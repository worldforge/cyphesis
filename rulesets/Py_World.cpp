// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include <server/WorldRouter.h>
#include <server/WorldTime.h>
#include <rulesets/Thing.h>

#include "Python_API.h"

static PyObject * World_get_time(WorldObject *self, PyObject *args, PyObject *kw)
{
    if (self->world == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid world object");
        return NULL;
    }
    if (!PyArg_ParseTuple(args, "")) {
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

static PyObject * World_is_object_deleted(WorldObject *self, PyObject *args, PyObject *kw)
{
    if (self->world == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid world object");
        return NULL;
    }
    PyObject * obj;
    if (!PyArg_ParseTuple(args, "O", &obj)) {
        return NULL;
    }
    if (!PyThing_Check(obj)) {
        PyErr_SetString(PyExc_TypeError,"Arg is not a thing");
        return NULL;
    }
    ThingObject * o = (ThingObject*)obj;
    if (o->m_thing == NULL) {
        PyErr_SetString(PyExc_TypeError,"Invalid thing");
        return NULL;
    }
    return PyInt_FromLong(self->world->is_object_deleted(o->m_thing));
}

static PyMethodDef World_methods[] = {
    {"get_time",	(PyCFunction)World_get_time,	METH_VARARGS},
    {"is_object_deleted",	(PyCFunction)World_is_object_deleted,	METH_VARARGS},
    {NULL,		NULL}           /* sentinel */
};

static void World_dealloc(WorldObject *self)
{
    PyMem_DEL(self);
}

static PyObject * World_getattr(WorldObject *self, char *name)
{
    return Py_FindMethod(World_methods, (PyObject *)self, name);
}

static int World_setattr(WorldObject *self, char *name, PyObject *v)
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
	return self;
}
