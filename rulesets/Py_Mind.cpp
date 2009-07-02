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

#include "Py_Mind.h"
#include "Py_Thing.h"
#include "Py_Map.h"
#include "Py_Message.h"
#include "Py_Vector3D.h"
#include "Py_Point3D.h"
#include "Py_Location.h"
#include "Py_World.h"
#include "Py_WorldTime.h"
#include "PythonWrapper.h"
#include "BaseMind.h"

#include "common/id.h"
#include "common/TypeNode.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;

// This method is almost profoundly pointless. Remove it soon, once we
// sort out something to do with the one placed it is used in the python
// mind code.
static PyObject * Mind_as_entity(PyMind * self)
{
#ifndef NDEBUG
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_AssertionError, "invalid mind as_entity");
        return NULL;
    }
#endif // NDEBUG
    PyMessage * ret = newPyMessage();
    if (ret == NULL) {
        PyErr_SetString(PyExc_MemoryError, "error creating MessageElement");
        return NULL;
    }
    ret->m_obj = new Element(MapType());
    self->m_mind->addToMessage(ret->m_obj->asMap());
    return (PyObject *)ret;
}

static PyMethodDef Mind_methods[] = {
    {"as_entity",        (PyCFunction)Mind_as_entity,  METH_NOARGS},
    {NULL,               NULL}           // sentinel
};

static void Mind_dealloc(PyMind *self)
{
    Py_XDECREF(self->Mind_attr);
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject * Mind_getattro(PyMind *self, PyObject *oname)
{
#ifndef NDEBUG
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL mind in Mind.getattr");
        return NULL;
    }
#endif // NDEBUG
    char * name = PyString_AsString(oname);
    if (strcmp(name, "id") == 0) {
        return (PyObject *)PyString_FromString(self->m_mind->getId().c_str());
    }
    if (strcmp(name, "type") == 0) {
        if (self->m_mind->getType() == NULL) {
            PyErr_SetString(PyExc_AttributeError, name);
            return NULL;
        }
        PyObject * list = PyList_New(0);
        if (list == NULL) {
            return NULL;
        }
        PyObject * ent = PyString_FromString(self->m_mind->getType()->name().c_str());
        PyList_Append(list, ent);
        Py_DECREF(ent);
        return list;
    }
    if (strcmp(name, "map") == 0) {
        PyMap * map = newPyMap();
        map->m_map = self->m_mind->getMap();
        return (PyObject *)map;
    }
    if (strcmp(name, "location") == 0) {
        PyLocation * loc = newPyLocation();
        loc->location = &self->m_mind->m_location;
        loc->owner = self->m_mind;
        return (PyObject *)loc;
    }
    if (strcmp(name, "time") == 0) {
        PyWorldTime * worldtime = newPyWorldTime();
        worldtime->time = self->m_mind->getTime();
        return (PyObject *)worldtime;
    }
    if (strcmp(name, "contains") == 0) {
        if (self->m_mind->m_contains == 0) {
            Py_INCREF(Py_None);
            return Py_None;
        }
        PyObject * list = PyList_New(0);
        if (list == NULL) {
            return NULL;
        }
        LocatedEntitySet::const_iterator I = self->m_mind->m_contains->begin();
        LocatedEntitySet::const_iterator Iend = self->m_mind->m_contains->end();
        for (; I != Iend; ++I) {
            LocatedEntity * child = *I;
            PyObject * wrapper = wrapEntity(child);
            if (wrapper == NULL) {
                Py_DECREF(list);
                return NULL;
            }
            PyList_Append(list, wrapper);
            Py_DECREF(wrapper);
        }
        return list;
    }
    if (self->Mind_attr != NULL) {
        PyObject *v = PyDict_GetItemString(self->Mind_attr, name);
        if (v != NULL) {
            Py_INCREF(v);
            return v;
        }
    }
    LocatedEntity * mind = self->m_mind;
    Element attr;
    if (mind->getAttr(name, attr)) {
        return MessageElement_asPyObject(attr);
    }
    PyObject * ret = PyObject_GenericGetAttr((PyObject *)self, oname);
    if (ret != 0) {
        return ret;
    }
    // PyObject_GenericGetAttr sets and error if nothing was found
    PyErr_Clear();
    return Py_FindMethod(Mind_methods, (PyObject *)self, name);
}

static int Mind_setattro(PyMind *self, PyObject *oname, PyObject *v)
{
    char * name = PyString_AsString(oname);
    if (strcmp(name, "mind") == 0) {
        if (!PyMind_Check(v)) {
            PyErr_SetString(PyExc_TypeError, "Mind.mind requires Mind");
            return -1;
        }
        self->m_mind = ((PyMind*)v)->m_mind;
        return 0;
    }
#ifndef NDEBUG
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL mind in Mind.setattr");
        return -1;
    }
#endif // NDEBUG
    if (strcmp(name, "map") == 0) {
        PyErr_SetString(PyExc_RuntimeError, "Setting map on mind is forbidden");
        return -1;
    }
    LocatedEntity * entity = self->m_mind;
    // Should we support removal of attributes?
    //std::string attr(name);
    //if (v == NULL) {
        //entity->attributes.erase(attr);
        //return 0;
    //}
    Element obj;
    if (PyObject_asMessageElement(v, obj, true) == 0) {
        assert(!obj.isMap() && !obj.isList());
        // In the Python wrapper for Entity in Py_Thing.cpp notices are issued
        // for some types.
        entity->setAttr(name, obj);
        return 0;
    }
    // Minds set a number of native Python members on themselves to store
    // important state information, which seems to be lost if we munge them
    // into Atlas data, probably because of the weird stuff that happens
    // in the wrappers when scripts manipulate complex attributes. They
    // are copied on getattr, rather than referenced.
    // If we get here, then the attribute is not Atlas compatable, so we
    // need to store it in a python dictionary
    if (self->Mind_attr == NULL) {
        self->Mind_attr = PyDict_New();
        if (self->Mind_attr == NULL) {
            return -1;
        }
    }
    return PyDict_SetItemString(self->Mind_attr, name, v);
}

static int Mind_compare(PyMind *self, PyMind *other)
{
    if (self->m_mind == NULL || other->m_mind == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL mind in Mind.compare");
        return -1;
    }
    return (self->m_mind == other->m_mind) ? 0 : 1;
}

static int Mind_init(PyMind * self, PyObject * args, PyObject * kwds)
{
    char * id = NULL;

    if (!PyArg_ParseTuple(args, "s", &id)) {
        return -1;
    }
    long intId = integerId(id);
    if (intId == -1L) {
        PyErr_SetString(PyExc_TypeError, "Mind() requires string/int ID");
        return -1;
    }
    self->m_mind = new BaseMind(id, intId);
    return 0;
}

PyTypeObject PyMind_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              // ob_size
        "Mind",                         // tp_name
        sizeof(PyMind),                 // tp_basicsize
        0,                              // tp_itemsize
        // methods 
        (destructor)Mind_dealloc,       // tp_dealloc
        0,                              // tp_print
        0,                              // tp_getattr
        0,                              // tp_setattr
        (cmpfunc)Mind_compare,          // tp_compare
        0,                              // tp_repr
        0,                              // tp_as_number
        0,                              // tp_as_sequence
        0,                              // tp_as_mapping
        0,                              // tp_hash
        0,                              // tp_call
        0,                              // tp_str
        (getattrofunc)Mind_getattro,    // tp_getattro
        (setattrofunc)Mind_setattro,    // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,             // tp_flags
        "Mind objects",                 // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        0,                              // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        0,                              // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        0,                              // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)Mind_init,            // tp_init
        0,                              // tp_alloc
        0,                              // tp_new
};

PyMind * newPyMind()
{
#if 0
    PyMind * self;
    self = PyObject_NEW(PyMind, &PyMind_Type);
    if (self == NULL) {
        return NULL;
    }
    self->Mind_attr = NULL;
    return self;
#else
    return (PyMind *)PyMind_Type.tp_new(&PyMind_Type, 0, 0);
#endif
}
