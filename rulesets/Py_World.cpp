// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_World.h"
#include "Py_WorldTime.h"
#include "Py_Thing.h"

#include "Entity.h"

#include "modules/WorldTime.h"

static PyObject * World_get_time(PyWorld *self)
{
#ifndef NDEBUG
    if (self->world == NULL) {
        PyErr_SetString(PyExc_AssertionError,"invalid world object");
        return NULL;
    }
#endif // NDEBUG
    PyWorldTime * wtime = newPyWorldTime();
    if (wtime == NULL) {
        return NULL;
    }
    wtime->time = new WorldTime((int)self->world->getTime());
    wtime->own = true;
    return (PyObject *)wtime;
}

static PyObject * World_get_object(PyWorld *self, PyObject *args)
{
#ifndef NDEBUG
    if (self->world == NULL) {
        PyErr_SetString(PyExc_AssertionError,"invalid world object");
        return NULL;
    }
#endif // NDEBUG
    char * id = NULL;
    if (!PyArg_ParseTuple(args, "s", &id)) {
        return NULL;
    }
    Entity * ent = self->world->getEntity(id);
    if (ent == NULL) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    PyEntity * o = newPyEntity();
    o->m_entity = ent;
    return (PyObject *)o;
}

static PyMethodDef World_methods[] = {
    {"get_time",        (PyCFunction)World_get_time,    METH_NOARGS},
    {"get_object",      (PyCFunction)World_get_object,  METH_VARARGS},
    {NULL,              NULL}           /* sentinel */
};

static void World_dealloc(PyWorld *self)
{
    PyMem_DEL(self);
}

static PyObject * World_getattr(PyWorld *self, char *name)
{
    return Py_FindMethod(World_methods, (PyObject *)self, name);
}

static int World_setattr(PyWorld *self, char *name, PyObject *v)
{
    return 0;
}

PyTypeObject PyWorld_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /*ob_size*/
        "World",                        /*tp_name*/
        sizeof(PyWorld),                /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        (destructor)World_dealloc,      /*tp_dealloc*/
        0,                              /*tp_print*/
        (getattrfunc)World_getattr,     /*tp_getattr*/
        (setattrfunc)World_setattr,     /*tp_setattr*/
        0,                              /*tp_compare*/
        0,                              /*tp_repr*/
        0,                              /*tp_as_number*/
        0,                              /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
};

PyWorld * newPyWorld()
{
    PyWorld * self;
    self = PyObject_NEW(PyWorld, &PyWorld_Type);
    if (self == NULL) {
        return NULL;
    }
    return self;
}
