// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "Py_RootEntity.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;

/*
 * Beginning of Entity section.
 *
 * This is a python type that wraps up entity objects from
 * Atlas::Objects::Entity namespace.
 *
 */

static PyObject* Entity_get_name(PyRootEntity * self)
{
#ifndef NDEBUG
    if (!self->entity.isValid()) {
        PyErr_SetString(PyExc_AssertionError,"NULL Entity in Entity.get_name");
        return NULL;
    }
#endif // NDEBUG
    return PyString_FromString("op");
}

/*
 * Entity methods structure.
 *
 * Generated from a macro in case we need one for each type of entity.
 *
 */

PyMethodDef RootEntity_methods[] = {
    {"get_name",        (PyCFunction)Entity_get_name,        METH_NOARGS},
    {NULL,          NULL}
};

PyMethodDef ConstRootEntity_methods[] = {
    {"get_name",        (PyCFunction)Entity_get_name,        METH_NOARGS},
    {NULL,          NULL}
};


/*
 * Beginning of Entity standard methods section.
 */

static void Entity_dealloc(PyRootEntity *self)
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
        PyErr_SetString(PyExc_AssertionError, "NULL Entity in Entity.getattr");
        return NULL;
    }
#endif // NDEBUG
    if (strcmp(name, "name") == 0) {
        return PyString_FromString(self->entity->getName().c_str());
    } else if (strcmp(name, "id") == 0) {
        return PyString_FromString(self->entity->getId().c_str());
    }
    return findMethod(self, name);
}

static PyObject * Entity_getattr(PyRootEntity * self, char * name)
{
    return getattr(self, name);
}

static PyObject * ConstEntity_getattr(PyConstRootEntity * self, char * name)
{
    return getattr(self, name);
}


static int Entity_setattr(PyRootEntity *self, char *name, PyObject *v)
{
#ifndef NDEBUG
    if (!self->entity.isValid()) {
        PyErr_SetString(PyExc_AssertionError, "NULL Entity in Entity.setattr");
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
    return 0;
}

PyTypeObject PyRootEntity_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      // ob_size
        "Entity",                               // tp_name
        sizeof(PyRootEntity),                   // tp_basicsize
        0,                                      // tp_itemsize
        //  methods 
        (destructor)Entity_dealloc,             // tp_dealloc
        0,                                      // tp_print
        (getattrfunc)Entity_getattr,            // tp_getattr
        (setattrfunc)Entity_setattr,            // tp_setattr
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
        (destructor)Entity_dealloc,             // tp_dealloc
        0,                                      // tp_print
        (getattrfunc)ConstEntity_getattr,       // tp_getattr
        0,                                      // tp_setattr
        0,                                      // tp_compare
        0,                                      // tp_repr
        0,                                      // tp_as_number
        0,                                      // tp_as_sequence
        0,                                      // tp_as_mapping
        0,                                      // tp_hash
};

/*
 * Beginning of Entity creation functions section.
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
