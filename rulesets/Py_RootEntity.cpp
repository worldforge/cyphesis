// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
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

#include "Py_RootEntity.h"
#include "Py_Message.h"
#include "Py_Location.h"

#include "common/log.h"
#include "common/compose.hpp"

#include "modules/Location.h"

#include <Atlas/Objects/Anonymous.h>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

/*
 * Beginning of RootEntity section.
 *
 * This is a python type that wraps up entity objects from
 * Atlas::Objects::Entity namespace.
 *
 */

static PyObject* RootEntity_get_name(PyRootEntity * self)
{
#ifndef NDEBUG
    if (!self->entity.isValid()) {
        PyErr_SetString(PyExc_AssertionError,"NULL RootEntity in RootEntity.get_name");
        return NULL;
    }
#endif // NDEBUG
    return PyString_FromString("obj");
}

/*
 * RootEntity methods structure.
 *
 * Generated from a macro in case we need one for each type of entity.
 *
 */

PyMethodDef RootEntity_methods[] = {
    {"get_name",        (PyCFunction)RootEntity_get_name,    METH_NOARGS},
    {NULL,          NULL}
};

PyMethodDef ConstRootEntity_methods[] = {
    {"get_name",        (PyCFunction)RootEntity_get_name,    METH_NOARGS},
    {NULL,          NULL}
};


/*
 * Beginning of RootEntity standard methods section.
 */

static void RootEntity_dealloc(PyRootEntity *self)
{
    self->entity.~RootEntity();
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject * RootEntity_getattr(PyRootEntity * self, char * name)
{
#ifndef NDEBUG
    if (!self->entity.isValid()) {
        PyErr_SetString(PyExc_AssertionError, "NULL RootEntity in RootEntity.getattr");
        return NULL;
    }
#endif // NDEBUG
    if (strcmp(name, "name") == 0) {
        return PyString_FromString(self->entity->getName().c_str());
    } else if (strcmp(name, "id") == 0) {
        return PyString_FromString(self->entity->getId().c_str());
    } else {
        Element attr;
        if (self->entity->copyAttr(name, attr) == 0) {
            assert(!attr.isNone());
            if (attr.isPtr()) {
                PyObject * ret = (PyObject*)attr.Ptr();
                Py_INCREF(ret);
                return ret;
            }
            return MessageElement_asPyObject(attr);
        }
    }
    return Py_FindMethod(RootEntity_methods, (PyObject *)self, name);
}

static int RootEntity_setattr(PyRootEntity *self, char *name, PyObject *v)
{
#ifndef NDEBUG
    if (!self->entity.isValid()) {
        PyErr_SetString(PyExc_AssertionError, "NULL RootEntity in RootEntity.setattr");
        return -1;
    }
#endif // NDEBUG
    if (strcmp(name, "name") == 0) {
        if (!PyString_Check(v)) {
            PyErr_SetString(PyExc_TypeError, "non string name");
            return -1;
        }
        self->entity->setName(PyString_AsString(v));
        return 0;
    }
    Element atlas_val;
    if (PyObject_asMessageElement(v, atlas_val) == 0) {
        if (atlas_val.isMap()) {
            log(NOTICE, String::compose("Setting \"%1\" as map attribute "
                                        "on RootEntity.", name));
        }
        if (atlas_val.isList()) {
            log(NOTICE, String::compose("Setting \"%1\" as list attribute "
                                        "on RootEntity.", name));
        }
        self->entity->setAttr(name, atlas_val);
        return 0;
    } else {
        // FIXME We do nothing to ensure that this reference is decremented currently
        // This causes a memory leak. In order to solve this, we are going
        // to need to find and remove all python attributes in the
        // underlying object when this wrapper is destructed.
        Py_INCREF(v);
        self->entity->setAttr(name, Element((void*)v));
        return 0;
    }
}

static int PySequence_asVector(PyObject * o, std::vector<double> & ret)
{
    int len;
    PyObject * item;
    if (PyList_Check(o)) {
        len = PyList_Size(o);
        ret.resize(len);
        for(int i = 0; i < len; i++) {
            item = PyList_GetItem(o, i);
            if (PyFloat_Check(item)) {
                ret[i] = PyFloat_AsDouble(item);
            } else if (PyInt_Check(item)) {
                ret[i] = PyInt_AsLong(item);
            } else {
                return -1;
            }
        }
    } else if (PyTuple_Check(o)) {
        len = PyTuple_Size(o);
        ret.resize(len);
        for(int i = 0; i < len; i++) {
            item = PyTuple_GetItem(o, i);
            if (PyFloat_Check(item)) {
                ret[i] = PyFloat_AsDouble(item);
            } else if (PyInt_Check(item)) {
                ret[i] = PyInt_AsLong(item);
            } else {
                return -1;
            }
        }
    } else {
        return -1;
    }
    return 0;
}

static int RootEntity_init(PyRootEntity * self, PyObject * args, PyObject * kwds)
{
    char * id = NULL;

    if (!PyArg_ParseTuple(args, "|s", &id)) {
        return -1;
    }
    if (id != NULL) {
        self->entity->setId(id);
    }
    if (kwds != NULL && PyDict_Check(kwds)) {
        PyObject * keys = PyDict_Keys(kwds);
        PyObject * vals = PyDict_Values(kwds);
        if (keys == NULL || vals == NULL) {
            PyErr_SetString(PyExc_RuntimeError, "Error in keywords");
            return -1;
        }
        int i, size = PyList_Size(keys);
        for(i = 0; i < size; i++) {
            char * key = PyString_AsString(PyList_GetItem(keys, i));
            PyObject * val = PyList_GetItem(vals, i);
            if (strcmp(key, "location") == 0) {
                if (!PyLocation_Check(val)) {
                    PyErr_SetString(PyExc_TypeError, "location must be a Location object");
                    return -1;
                }
                PyLocation * loc = (PyLocation*)val;
                loc->location->addToEntity(self->entity);
            } else if (strcmp(key, "pos") == 0) {
                std::vector<double> vector_val;
                if (PySequence_asVector(val, vector_val) != 0) {
                    PyErr_SetString(PyExc_TypeError, "pos must be a number sequence.");
                    return -1;
                }
                self->entity->setPos(vector_val);
            } else if (strcmp(key, "parent") == 0) {
                if (!PyString_Check(val)) {
                    PyErr_SetString(PyExc_TypeError, "parent must be a string.");
                    return -1;
                }
                self->entity->setLoc(PyString_AsString(val));
            } else if (strcmp(key, "type") == 0) {
                if (!PyString_Check(val)) {
                    PyErr_SetString(PyExc_TypeError, "type must be a string.");
                    return -1;
                }
                self->entity->setParents(std::list<std::string>(1, PyString_AsString(val)));
                self->entity->setObjtype("obj");
            } else {
                Element val_obj;
                if (PyObject_asMessageElement(val, val_obj) != 0) {
                    Py_DECREF(keys);
                    Py_DECREF(vals);
                    PyErr_SetString(PyExc_TypeError, "Arg has no type.");
                    return -1;
                }
                self->entity->setAttr(key, val_obj);
            }
        }
        Py_DECREF(keys);
        Py_DECREF(vals);
    }

    return 0;
}

static PyObject * RootEntity_new(PyTypeObject * type, PyObject *, PyObject *)
{
   PyRootEntity * self = (PyRootEntity *)type->tp_alloc(type, 0);
    if (self != NULL) {
        new (&(self->entity)) RootEntity(NULL);
        self->entity = Anonymous();
    }
    return (PyObject *)self;
}


PyTypeObject PyRootEntity_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      // ob_size
        "atlas.Entity",                         // tp_name
        sizeof(PyRootEntity),                   // tp_basicsize
        0,                                      // tp_itemsize
        //  methods 
        (destructor)RootEntity_dealloc,         // tp_dealloc
        0,                                      // tp_print
        (getattrfunc)RootEntity_getattr,        // tp_getattr
        (setattrfunc)RootEntity_setattr,        // tp_setattr
        0,                                      // tp_compare
        0,                                      // tp_repr
        0,                                      // tp_as_number
        0,                                      // tp_as_sequence
        0,                                      // tp_as_mapping
        0,                                      // tp_hash
        0,                                      // tp_call
        0,                                      // tp_str
        0,                                      // tp_getattro
        0,                                      // tp_setattro
        0,                                      // tp_as_buffer
        Py_TPFLAGS_DEFAULT,                     // tp_flags
        "RootEntity objects",                   // tp_doc
        0,                                      // tp_travers
        0,                                      // tp_clear
        0,                                      // tp_richcompare
        0,                                      // tp_weaklistoffset
        0,                                      // tp_iter
        0,                                      // tp_iternext
        0,                                      // tp_methods
        0,                                      // tp_members
        0,                                      // tp_getset
        0,                                      // tp_base
        0,                                      // tp_dict
        0,                                      // tp_descr_get
        0,                                      // tp_descr_set
        0,                                      // tp_dictoffset
        (initproc)RootEntity_init,              // tp_init
        0,                                      // tp_alloc
        RootEntity_new,                         // tp_new
};

/*
 * Beginning of RootEntity creation functions section.
 */

PyRootEntity * newPyRootEntity()
{
    return (PyRootEntity *)PyRootEntity_Type.tp_new(&PyRootEntity_Type, 0, 0);
}
