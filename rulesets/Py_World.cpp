// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_World.h"
#include "Py_WorldTime.h"
#include "Py_Thing.h"

#include "Entity.h"

#include "modules/WorldTime.h"

#include "common/BaseWorld.h"

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
    wtime->time = new WorldTime(self->world->getTime());
    wtime->own = true;
    return (PyObject *)wtime;
}

static PyObject * World_get_object(WorldObject *self, PyObject *args, PyObject *kw)
{
    if (self->world == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid world object");
        return NULL;
    }
    char * id = NULL;
    if (!PyArg_ParseTuple(args, "s", &id)) {
        return NULL;
    }
    Entity * ent = self->world->getObject(id);
    if (ent == NULL) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    EntityObject * o = newEntityObject(NULL);
    o->m_entity = ent;
    return (PyObject *)o;
}

static PyMethodDef World_methods[] = {
    {"get_time",	(PyCFunction)World_get_time,	METH_VARARGS},
    {"get_object",	(PyCFunction)World_get_object,	METH_VARARGS},
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
    return 0;
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
