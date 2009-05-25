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

#include "Py_Map.h"
#include "Py_Location.h"
#include "Py_Thing.h"
#include "Py_Operation.h"
#include "Py_RootEntity.h"
#include "Py_Object.h"

#include "MemEntity.h"
#include "MemMap.h"
#include "Script.h"

#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/objectFactory.h>

using Atlas::Objects::Entity::RootEntity;

static PyObject * Map_find_by_location(PyMap * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Map in Map.find_by_location");
        return NULL;
    }
#endif // NDEBUG
    PyObject * where_obj;
    double radius;
    char * type;
    if (!PyArg_ParseTuple(args, "Ods", &where_obj, &radius, &type)) {
        return NULL;
    }
    if (!PyLocation_Check(where_obj)) {
        PyErr_SetString(PyExc_TypeError, "Argument must be a location");
        return NULL;
    }
    PyLocation * where = (PyLocation *)where_obj;
    if (!where->location->isValid()) {
        PyErr_SetString(PyExc_RuntimeError, "Location is incomplete");
        return NULL;
    }
    MemEntityVector res = self->m_map->findByLocation(*where->location,
                                                      radius, type);
    PyObject * list = PyList_New(res.size());
    if (list == NULL) {
        return NULL;
    } 
    MemEntityVector::const_iterator Iend = res.end();
    int i = 0;
    for (MemEntityVector::const_iterator I = res.begin(); I != Iend; ++I, ++i) {
        PyObject * thing = wrapEntity(*I);
        if (thing == NULL) {
            Py_DECREF(list);
            return NULL;
        }
        PyList_SetItem(list, i, thing);
    }
    return list;
}

static PyObject * Map_find_by_type(PyMap * self, PyObject * py_what)
{
#ifndef NDEBUG
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Map in Map.find_by_type");
        return NULL;
    }
#endif // NDEBUG
    if (!PyString_CheckExact(py_what)) {
        PyErr_SetString(PyExc_TypeError, "Map.find_by_type must be string");
        return NULL;
    }
    char * what = PyString_AsString(py_what);
    MemEntityVector res = self->m_map->findByType(std::string(what));
    PyObject * list = PyList_New(res.size());
    if (list == NULL) {
        return NULL;
    } 
    MemEntityVector::const_iterator Iend = res.end();
    int i = 0;
    for (MemEntityVector::const_iterator I = res.begin(); I != Iend; ++I, ++i) {
        PyObject * thing = wrapEntity(*I);
        if (thing == NULL) {
            Py_DECREF(list);
            return NULL;
        }
        PyList_SetItem(list, i, thing);
    }
    return list;
}

static PyObject * Map_updateAdd(PyMap * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Map in Map.updateAdd");
        return NULL;
    }
#endif // NDEBUG
    PyObject * arg;
    double time;
    if (!PyArg_ParseTuple(args, "Od", &arg, &time)) {
        return NULL;
    }
    if (PyMessageElement_Check(arg)) {
        PyMessageElement * me = (PyMessageElement*)arg;
        if (!me->m_obj->isMap()) {
            PyErr_SetString(PyExc_TypeError, "arg is a Message that is not a map");
            return NULL;
        }
        Atlas::Objects::Root obj = Atlas::Objects::Factories::instance()->createObject(me->m_obj->asMap());
        RootEntity ent = Atlas::Objects::smart_dynamic_cast<RootEntity>(obj);
        if (!ent.isValid()) {
            PyErr_SetString(PyExc_TypeError, "arg is a Message that does not represent an entity");
            return NULL;
        }
        MemEntity * ret = self->m_map->updateAdd(ent, time);
        PyObject * thing = wrapEntity(ret);
        if (thing == NULL) {
            return NULL;
        }
        return thing;
    } else if (PyRootEntity_Check(arg)) {
        PyRootEntity * ent = (PyRootEntity*)arg;
        MemEntity * ret = self->m_map->updateAdd(ent->entity, time);
        PyObject * thing = wrapEntity(ret);
        if (thing == NULL) {
            return NULL;
        }
        return thing;
    } else {
        PyErr_SetString(PyExc_TypeError, "arg is not an Atlas Entity or Message");
        return NULL;
    }
}

static PyObject * Map_delete(PyMap * self, PyObject * py_id)
{
#ifndef NDEBUG
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Map in Map.delete");
        return NULL;
    }
#endif // NDEBUG
    if (!PyString_CheckExact(py_id)) {
        PyErr_SetString(PyExc_TypeError, "Map.delete must be string");
        return NULL;
    }
    char * id = PyString_AsString(py_id);
    self->m_map->del(id);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Map_get(PyMap * self, PyObject * py_id)
{
#ifndef NDEBUG
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Map in Map.get");
        return NULL;
    }
#endif // NDEBUG
    if (!PyString_CheckExact(py_id)) {
        PyErr_SetString(PyExc_TypeError, "Map.get must be string");
        return NULL;
    }
    char * id = PyString_AsString(py_id);
    MemEntity * ret = self->m_map->get(id);
    if (ret == NULL) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    PyObject * thing = wrapEntity(ret);
    if (thing == NULL) {
        return NULL;
    }
    return thing;
}

static PyObject * Map_get_add(PyMap * self, PyObject * py_id)
{
#ifndef NDEBUG
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Map in Map.get_add");
        return NULL;
    }
#endif // NDEBUG
    if (!PyString_CheckExact(py_id)) {
        PyErr_SetString(PyExc_TypeError, "Map.get_add must be string");
        return NULL;
    }
    char * id = PyString_AsString(py_id);
    MemEntity * ret = self->m_map->getAdd(id);
    assert(ret != 0);
    PyObject * thing = wrapEntity(ret);
    if (thing == NULL) {
        return NULL;
    }
    return thing;
}

static PyObject * Map_add_hooks_append(PyMap * self, PyObject * py_method)
{
#ifndef NDEBUG
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Map in Map.add_hooks_append");
        return NULL;
    }
#endif // NDEBUG
    if (!PyString_CheckExact(py_method)) {
        PyErr_SetString(PyExc_TypeError, "Map.add_hooks_append must be string");
        return NULL;
    }
    char * method = PyString_AsString(py_method);
    self->m_map->getAddHooks().push_back(std::string(method));

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Map_update_hooks_append(PyMap * self, PyObject * py_method)
{
#ifndef NDEBUG
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Map in Map.update_hooks_append");
        return NULL;
    }
#endif // NDEBUG
    if (!PyString_CheckExact(py_method)) {
        PyErr_SetString(PyExc_TypeError, "Map.update_hooks_append must be string");
        return NULL;
    }
    char * method = PyString_AsString(py_method);
    self->m_map->getUpdateHooks().push_back(std::string(method));

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Map_delete_hooks_append(PyMap * self, PyObject * py_method)
{
#ifndef NDEBUG
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Map in Map.delete_hooks_append");
        return NULL;
    }
#endif // NDEBUG
    if (!PyString_CheckExact(py_method)) {
        PyErr_SetString(PyExc_TypeError, "Map.delete_hooks_append must be string");
        return NULL;
    }
    char * method = PyString_AsString(py_method);
    self->m_map->getDeleteHooks().push_back(std::string(method));

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef Map_methods[] = {
    {"find_by_location",    (PyCFunction)Map_find_by_location,    METH_VARARGS},
    {"find_by_type",        (PyCFunction)Map_find_by_type,        METH_O},
    {"add",                 (PyCFunction)Map_updateAdd,           METH_VARARGS},
    {"delete",              (PyCFunction)Map_delete,              METH_O},
    {"get",                 (PyCFunction)Map_get,                 METH_O},
    {"get_add",             (PyCFunction)Map_get_add,             METH_O},
    {"update",              (PyCFunction)Map_updateAdd,           METH_VARARGS},
    {"add_hooks_append",    (PyCFunction)Map_add_hooks_append,    METH_O},
    {"update_hooks_append", (PyCFunction)Map_update_hooks_append, METH_O},
    {"delete_hooks_append", (PyCFunction)Map_delete_hooks_append, METH_O},
    {NULL,                  NULL}           // sentinel
};

static void Map_dealloc(PyMap *self)
{
    PyObject_Free(self);
}

static int Map_init(PyMap * self, PyObject * args, PyObject * kwds)
{
    Script ** s = new Script*;
    *s = &noScript;
    self->m_map = new MemMap(*s);
    return 0;
}

static PyObject * Map_new(PyTypeObject * type, PyObject *, PyObject *)
{
    PyMap * self = (PyMap *)type->tp_alloc(type, 0);
    return (PyObject *)self;
}

PyTypeObject PyMap_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              // ob_size
        "Map",                          // tp_name
        sizeof(PyMap),                  // tp_basicsize
        0,                              // tp_itemsize
        // methods
        (destructor)Map_dealloc,        // tp_dealloc
        0,                              // tp_print
        0,                              // tp_getattr
        0,                              // tp_setattr
        0,                              // tp_compare
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
        "Map objects",                  // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        0,                              // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        Map_methods,                    // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        0,                              // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)Map_init,             // tp_init
        0,                              // tp_alloc
        Map_new,                        // tp_new
};

PyMap * newPyMap()
{
#if 0
    PyMap * self;
    self = PyObject_NEW(PyMap, &PyMap_Type);
    if (self == NULL) {
        return NULL;
    }
    return self;
#else
    return (PyMap *)PyMap_Type.tp_new(&PyMap_Type, 0, 0);
#endif
}
