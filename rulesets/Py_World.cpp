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

// $Id$

#include "Py_World.h"
#include "Py_WorldTime.h"
#include "Py_Thing.h"

#include "Entity.h"

#include "modules/WorldTime.h"

static PyObject * World_get_time(PyWorld *self)
{
    PyWorldTime * wtime = newPyWorldTime();
    if (wtime == NULL) {
        return NULL;
    }
    wtime->time = new WorldTime((int)BaseWorld::instance().getTime());
    wtime->own = true;
    return (PyObject *)wtime;
}

static PyObject * World_get_object(PyWorld *self, PyObject * id)
{
    if (!PyString_CheckExact(id)) {
        PyErr_SetString(PyExc_TypeError, "World.get_object must be string");
        return NULL;
    }
    Entity * ent = BaseWorld::instance().getEntity(PyString_AsString(id));
    if (ent == NULL) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    PyObject * wrapper = wrapEntity(ent);
    return wrapper;
}

static PyMethodDef World_methods[] = {
    {"get_time",        (PyCFunction)World_get_time,    METH_NOARGS},
    {"get_object",      (PyCFunction)World_get_object,  METH_O},
    {NULL,              NULL}           // sentinel
};

static void World_dealloc(PyWorld *self)
{
    PyObject_Free(self);
}

static int World_compare(PyWorld * self, PyObject * other)
{
    if (PyWorld_Check(other)) {
        return 0;
    } else if (PyEntity_Check(other)) {
        PyEntity * other_entity = (PyEntity *)other;
        return (&BaseWorld::instance().m_gameWorld == other_entity->m_entity) ? 0 : 1;
    }
    return -1;
}

static int World_init(PyWorld * self, PyObject * args, PyObject * kwds)
{
    return 0;
}

static PyObject * World_new(PyTypeObject * type, PyObject *, PyObject *)
{
    PyWorld * self = (PyWorld *)type->tp_alloc(type, 0);
    return (PyObject *)self;
}

PyTypeObject PyWorld_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              // ob_size
        "World",                        // tp_name
        sizeof(PyWorld),                // tp_basicsize
        0,                              // tp_itemsize
        // methods
        (destructor)World_dealloc,      // tp_dealloc
        0,                              // tp_print
        0,                              // tp_getattr
        0,                              // tp_setattr
        (cmpfunc)World_compare,         // tp_compare
        0,                              // tp_repr
        0,                              // tp_as_number
        0,                              // tp_as_sequence
        0,                              // tp_as_mapping
        0,                              // tp_hash
        0,                              // tp_call
        0,                              // tp_str
        0,                              // tp_getattro
        0,                              // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT,             // tp_flags
        "World objects",                // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        0,                              // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        World_methods,                  // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        0,                              // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)World_init,           // tp_init
        0,                              // tp_alloc
        World_new,                      // tp_new
};

PyWorld * newPyWorld()
{
#if 0
    PyWorld * self;
    self = PyObject_NEW(PyWorld, &PyWorld_Type);
    if (self == NULL) {
        return NULL;
    }
    return self;
#else
    return (PyWorld *)PyWorld_Type.tp_new(&PyWorld_Type, 0, 0);
#endif
}
