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

#include "Py_RootEntity.h"
#include "Py_Object.h"

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;

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
    PyMem_DEL(self);
}

static inline PyObject * findMethod(PyRootEntity * self, char * name)
{
    return Py_FindMethod(RootEntity_methods, (PyObject *)self, name);
}

static inline PyObject * findMethod(PyConstRootEntity * self, char * name)
{
    return Py_FindMethod(ConstRootEntity_methods, (PyObject *)self, name);
}

template <typename T>
static PyObject * getattr(T * self, char * name)
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
    PyObject * ret = findMethod(self, name);
    return ret;
}

static PyObject * RootEntity_getattr(PyRootEntity * self, char * name)
{
    return getattr(self, name);
}

static PyObject * ConstRootEntity_getattr(PyConstRootEntity * self, char * name)
{
    return getattr(self, name);
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
    Element atlas_val = PyObject_asMessageElement(v);
    if (!atlas_val.isNone()) {
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

PyTypeObject PyRootEntity_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      // ob_size
        "Entity",                               // tp_name
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
};

PyTypeObject PyConstRootEntity_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      // ob_size
        "Entity",                               // tp_name
        sizeof(PyConstRootEntity),              // tp_basicsize
        0,                                      // tp_itemsize
        //  methods 
        (destructor)RootEntity_dealloc,         // tp_dealloc
        0,                                      // tp_print
        (getattrfunc)ConstRootEntity_getattr,   // tp_getattr
        0,                                      // tp_setattr
        0,                                      // tp_compare
        0,                                      // tp_repr
        0,                                      // tp_as_number
        0,                                      // tp_as_sequence
        0,                                      // tp_as_mapping
        0,                                      // tp_hash
};

/*
 * Beginning of RootEntity creation functions section.
 */

PyRootEntity * newPyRootEntity()
{
    PyRootEntity * self;
    self = PyObject_NEW(PyRootEntity, &PyRootEntity_Type);
    if (self == NULL) {
        return NULL;
    }
    new (&(self->entity)) RootEntity(NULL);
    return self;
}

PyConstRootEntity * newPyConstRootEntity()
{
    PyConstRootEntity * self;
    self = PyObject_NEW(PyConstRootEntity, &PyConstRootEntity_Type);
    if (self == NULL) {
        return NULL;
    }
    new (&(self->entity)) RootEntity(NULL);
    return self;
}
