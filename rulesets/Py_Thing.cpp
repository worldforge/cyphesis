// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_Thing.h"
#include "Py_Object.h"
#include "Py_Vector3D.h"
#include "Py_Point3D.h"
#include "Py_Location.h"
#include "Py_World.h"
#include "Entity.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;

static PyObject * Entity_as_entity(PyEntity * self)
{
#ifndef NDEBUG
    if (self->m_entity == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in Entity.as_entity");
        return NULL;
    }
#endif // NDEBUG
    PyMessageElement * ret = newPyMessageElement();
    if (ret == NULL) {
        return NULL;
    }
    ret->m_obj = new Element(MapType());
    self->m_entity->addToMessage(ret->m_obj->asMap());
    return (PyObject *)ret;
}

static PyMethodDef Entity_methods[] = {
        {"as_entity",        (PyCFunction)Entity_as_entity,  METH_NOARGS},
        {NULL,          NULL}           /* sentinel */
};

static void Entity_dealloc(PyEntity *self)
{
    //if (self->m_entity != NULL) {
        //delete self->m_entity;
    //}
    Py_XDECREF(self->Entity_attr);
    PyMem_DEL(self);
}

static PyObject * Entity_getattr(PyEntity *self, char *name)
{
    // Fairly major re-write of this to use operator[] of Entity base class
#ifndef NDEBUG
    if (self->m_entity == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in Entity.getattr");
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
        PyObject * ent = PyString_FromString(self->m_entity->getType().c_str());
        PyList_Append(list, ent);
        Py_DECREF(ent);
        return list;
    }
    // if (strcmp(name, "map") == 0) {
        // MemMap * tMap = self->m_entity->getMap();
        // if (tMap == NULL) {
            // PyErr_SetString(PyExc_TypeError, "Body entity has no map");
            // return NULL;
        // }
        // MapObject * map = newMapObject(NULL);
        // map->m_map = tMap;
        // return (PyObject *)map;
    // }
    if (strcmp(name, "location") == 0) {
        PyLocation * loc = newPyLocation();
        loc->location = &self->m_entity->m_location;
        loc->own = 0;
        return (PyObject *)loc;
    }
    if (strcmp(name, "world") == 0) {
        PyWorld * world = newPyWorld();
        world->world = self->m_entity->m_world;
        return (PyObject *)world;
    }
    if (strcmp(name, "contains") == 0) {
        PyObject * list = PyList_New(0);
        if (list == NULL) {
            return NULL;
        }
        EntitySet::const_iterator I = self->m_entity->m_contains.begin();
        EntitySet::const_iterator Iend = self->m_entity->m_contains.end();
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
    if (self->Entity_attr != NULL) {
        PyObject *v = PyDict_GetItemString(self->Entity_attr, name);
        if (v != NULL) {
            Py_INCREF(v);
            return v;
        }
    }
    Entity * entity = self->m_entity;
    Element attr;
    if (!entity->get(name, attr)) {
        return Py_FindMethod(Entity_methods, (PyObject *)self, name);
    }
    PyObject * ret = MessageElement_asPyObject(attr);
    if (ret == NULL) {
        return Py_FindMethod(Entity_methods, (PyObject *)self, name);
    }
    return ret;
}

static int Entity_setattr(PyEntity *self, char *name, PyObject *v)
{
#ifndef NDEBUG
    if (self->m_entity == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in Entity.getattr");
        return -1;
    }
#endif // NDEBUG
    if (strcmp(name, "status") == 0) {
        // This needs to be here until we can sort the difference
        // between floats and ints in python.
        if (PyInt_Check(v)) {
            self->m_entity->setStatus((double)PyInt_AsLong(v));
        } else if (PyFloat_Check(v)) {
            self->m_entity->setStatus(PyFloat_AsDouble(v));
        } else {
            PyErr_SetString(PyExc_TypeError, "status must be numeric type");
            return -1;
        }
        return 0;
    }
    if (strcmp(name, "map") == 0) {
        PyErr_SetString(PyExc_AttributeError, "map attribute forbidden");
        return -1;
    }
    Entity * entity = self->m_entity;
    //std::string attr(name);
    //if (v == NULL) {
        //entity->attributes.erase(attr);
        //return 0;
    //}
    Element obj = PyObject_asMessageElement(v);
    if (!obj.isNone() && !obj.isMap() && !obj.isList()) {
        entity->set(name, obj);
        return 0;
    }
    // If we get here, then the attribute is not Atlas compatable, so we
    // need to store it in a python dictionary
    if (self->Entity_attr == NULL) {
        self->Entity_attr = PyDict_New();
        if (self->Entity_attr == NULL) {
            return -1;
        }
    }
    return PyDict_SetItemString(self->Entity_attr, name, v);
}

static int Entity_compare(PyEntity *self, PyEntity *other)
{
    if ((self->m_entity == NULL) || (other->m_entity == NULL)) {
        PyErr_SetString(PyExc_AssertionError, "NULL Entity in Entity.compare");
        return -1;
    }
    return (self->m_entity == other->m_entity) ? 0 : 1;
}

PyTypeObject PyEntity_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /*ob_size*/
        "Entity",                       /*tp_name*/
        sizeof(PyEntity),               /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        (destructor)Entity_dealloc,     /*tp_dealloc*/
        0,                              /*tp_print*/
        (getattrfunc)Entity_getattr,    /*tp_getattr*/
        (setattrfunc)Entity_setattr,    /*tp_setattr*/
        (cmpfunc)Entity_compare,        /*tp_compare*/
        0,                              /*tp_repr*/
        0,                              /*tp_as_number*/
        0,                              /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
};

PyEntity * newPyEntity()
{
    PyEntity * self;
    self = PyObject_NEW(PyEntity, &PyEntity_Type);
    if (self == NULL) {
        return NULL;
    }
    self->Entity_attr = NULL;
    return self;
}
