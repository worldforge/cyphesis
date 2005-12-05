// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_Mind.h"
#include "Py_Thing.h"
#include "Py_Map.h"
#include "Py_Object.h"
#include "Py_Vector3D.h"
#include "Py_Point3D.h"
#include "Py_Location.h"
#include "Py_World.h"
#include "Py_WorldTime.h"
#include "BaseMind.h"

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
    PyMessageElement * ret = newPyMessageElement();
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
    //if (self->m_mind != NULL) {
        //delete self->m_mind;
    //}
    Py_XDECREF(self->Mind_attr);
    PyMem_DEL(self);
}

static PyObject * Mind_getattr(PyMind *self, char *name)
{
    // Fairly major re-write of this to use operator[] of Mind base class
#ifndef NDEBUG
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_AssertionError, "invalid mind getattr");
        return NULL;
    }
#endif // NDEBUG
    // If operation search gets to here, it goes no further
    if (strstr(name, "_operation") != NULL) {
        PyErr_SetString(PyExc_AttributeError, name);
        return NULL;
    }
    if (strcmp(name, "type") == 0) {
        PyObject * list = PyList_New(0);
        if (list == NULL) {
            return NULL;
        }
        PyObject * ent = PyString_FromString(self->m_mind->getType().c_str());
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
        loc->own = 0;
        return (PyObject *)loc;
    }
    if (strcmp(name, "time") == 0) {
        PyWorldTime * worldtime = newPyWorldTime();
        worldtime->time = self->m_mind->getTime();
        return (PyObject *)worldtime;
    }
    if (strcmp(name, "contains") == 0) {
        PyObject * list = PyList_New(0);
        if (list == NULL) {
            return NULL;
        }
        EntitySet::const_iterator I = self->m_mind->m_contains.begin();
        EntitySet::const_iterator Iend = self->m_mind->m_contains.end();
        for (; I != Iend; ++I) {
            PyEntity * child = newPyEntity();
            if (child == NULL) {
                Py_DECREF(list);
                return NULL;
            }
            // FIXME Do we need to increment the reference count on this?
            child->m_entity = *I;
            PyList_Append(list, (PyObject*)child);
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
    Entity * thing = self->m_mind;
    Element attr;
    if (!thing->get(name, attr)) {
        return Py_FindMethod(Mind_methods, (PyObject *)self, name);
    }
    PyObject * ret = MessageElement_asPyObject(attr);
    if (ret == NULL) {
        return Py_FindMethod(Mind_methods, (PyObject *)self, name);
    }
    return ret;
}

static int Mind_setattr(PyMind *self, char *name, PyObject *v)
{
    if (self->m_mind == NULL) {
        return -1;
    }
    if (self->Mind_attr == NULL) {
        self->Mind_attr = PyDict_New();
        if (self->Mind_attr == NULL) {
            return -1;
        }
    }
    if (strcmp(name, "status") == 0) {
        // This needs to be here until we can sort the difference
        // between floats and ints in python.
        if (PyInt_Check(v)) {
            self->m_mind->setStatus((double)PyInt_AsLong(v));
        } else if (PyFloat_Check(v)) {
            self->m_mind->setStatus(PyFloat_AsDouble(v));
        } else {
            PyErr_SetString(PyExc_TypeError, "status must be numeric type");
            return -1;
        }
        return 0;
    }
    if (strcmp(name, "map") == 0) {
        return -1;
    }
    Entity * thing = self->m_mind;
    //std::string attr(name);
    //if (v == NULL) {
        //thing->attributes.erase(attr);
        //return 0;
    //}
    Element obj = PyObject_asMessageElement(v);
    if (!obj.isNone() && !obj.isMap() && !obj.isList()) {
        thing->set(name, obj);
        return 0;
    }
    // If we get here, then the attribute is not Atlas compatable, so we
    // need to store it in a python dictionary
    return PyDict_SetItemString(self->Mind_attr, name, v);
}

static int Mind_compare(PyMind *self, PyMind *other)
{
    if ((self->m_mind == NULL) || (other->m_mind == NULL)) {
        PyErr_SetString(PyExc_AssertionError, "NULL mind in Mind.compare");
        return -1;
    }
    return (self->m_mind == other->m_mind) ? 0 : 1;
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
    (getattrfunc)Mind_getattr,      // tp_getattr
    (setattrfunc)Mind_setattr,      // tp_setattr
    (cmpfunc)Mind_compare,          // tp_compare
    0,                              // tp_repr
    0,                              // tp_as_number
    0,                              // tp_as_sequence
    0,                              // tp_as_mapping
    0,                              // tp_hash
};

PyMind * newPyMind()
{
    PyMind * self;
    self = PyObject_NEW(PyMind, &PyMind_Type);
    if (self == NULL) {
        return NULL;
    }
    self->Mind_attr = NULL;
    return self;
}
