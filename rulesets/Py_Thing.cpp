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

// $Id: Py_Thing.cpp,v 1.60 2007-07-30 18:12:51 alriddoch Exp $

#include "Py_Thing.h"
#include "Py_Object.h"
#include "Py_Vector3D.h"
#include "Py_Point3D.h"
#include "Py_Location.h"
#include "Py_World.h"
#include "Py_Property.h"
#include "Py_Operation.h"
#include "Py_Oplist.h"
#include "Py_Task.h"
#include "PythonWrapper.h"
#include "Character.h"

#include "common/log.h"
#include "common/Property.h"

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

static PyObject * Entity_send_world(PyEntity * self, PyOperation * op)
{
#ifndef NDEBUG
    if (self->m_entity == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in Entity.send_world");
        return NULL;
    }
#endif // NDEBUG
    if (PyOperation_Check(op)) {
        self->m_entity->sendWorld(op->operation);
    } else {
        PyErr_SetString(PyExc_TypeError, "Entity.send_world must be an op");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef Entity_methods[] = {
    {"as_entity",       (PyCFunction)Entity_as_entity,  METH_NOARGS},
    {"send_world",      (PyCFunction)Entity_send_world, METH_O},
    {NULL,              NULL}           /* sentinel */
};

static PyObject * Character_get_task(PyCharacter * self)
{
#ifndef NDEBUG
    if (self->m_entity == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in Entity.send_world");
        return NULL;
    }
#endif // NDEBUG
    if (self->m_entity->task() == 0) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    // FIXME Err, probably actually want to return the real Task.
    PyTask * ret = newPyTask();
    ret->m_task = self->m_entity->task();
    return (PyObject*)ret;
    
}

static PyObject * Character_set_task(PyCharacter * self, PyTask * task)
{
#ifndef NDEBUG
    if (self->m_entity == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in Entity.send_world");
        return NULL;
    }
#endif // NDEBUG
    if (!PyTask_Check(task)) {
        PyErr_SetString(PyExc_TypeError, "Entity.set_task must be a task");
        return NULL;
    }
    self->m_entity->setTask(task->m_task);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Character_clear_task(PyCharacter * self)
{
#ifndef NDEBUG
    if (self->m_entity == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in Entity.send_world");
        return NULL;
    }
#endif // NDEBUG
    self->m_entity->clearTask();
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Character_mind2body(PyCharacter * self, PyOperation * op)
{
#ifndef NDEBUG
    if (self->m_entity == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in Entity.send_world");
        return NULL;
    }
#endif // NDEBUG
    if (!PyOperation_Check(op)) {
         PyErr_SetString(PyExc_TypeError, "Entity.mind2body must be an operation");
         return NULL;
    }
    OpVector res;
    self->m_entity->mind2body(op->operation, res);
    if (res.empty()) {
        Py_INCREF(Py_None);
        return Py_None;
    } else if (res.size() == 1) {
        PyOperation * ret = newPyOperation();
        ret->operation = res[0];
        return (PyObject*)ret;
    } else {
        PyOplist * ret = newPyOplist();
        ret->ops = new OpVector(res);
        return (PyObject*)ret;
    }
}

static PyMethodDef Character_methods[] = {
    {"as_entity",       (PyCFunction)Entity_as_entity,     METH_NOARGS},
    {"send_world",      (PyCFunction)Entity_send_world,    METH_O},
    {"get_task",        (PyCFunction)Character_get_task,   METH_NOARGS},
    {"set_task",        (PyCFunction)Character_set_task,   METH_O},
    {"clear_task",      (PyCFunction)Character_clear_task, METH_NOARGS},
    {"mind2body",       (PyCFunction)Character_mind2body,  METH_O},
    {NULL,              NULL}           /* sentinel */
};

static void Entity_dealloc(PyEntity *self)
{
    Py_XDECREF(self->Entity_attr);
    PyObject_Free(self);
}

static PyObject * Entity_getattr(PyEntity *self, char *name)
{
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
    if (strcmp(name, "location") == 0) {
        PyLocation * loc = newPyLocation();
        loc->location = &self->m_entity->m_location;
        loc->owner = self->m_entity;
        return (PyObject *)loc;
    }
    if (strcmp(name, "world") == 0) {
        PyWorld * world = newPyWorld();
        world->world = &BaseWorld::instance();
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
            Entity * child = *I;
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
    if (self->Entity_attr != NULL) {
        PyObject *v = PyDict_GetItemString(self->Entity_attr, name);
        if (v != NULL) {
            Py_INCREF(v);
            return v;
        }
    }
    Entity * entity = self->m_entity;
    PropertyBase * prop = entity->getProperty(name);
    if (prop != 0) {
        PyObject * ret = Property_asPyObject(prop, entity);
        if (ret != 0) {
            return ret;
        }
        Element attr;
        // If this property is not set with a value, return none.
        if (prop->get(attr)) {
            return MessageElement_asPyObject(attr);
        } else {
            Py_INCREF(Py_None);
            return Py_None;
        }
    }
    const MapType & attrs = entity->getAttributes();
    MapType::const_iterator I = attrs.find(name);
    if (I != attrs.end()) {
        return MessageElement_asPyObject(I->second);
    }
    return Py_FindMethod(self->m_methods, (PyObject *)self, name);
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
    if (!obj.isNone()) {
        if (obj.isMap()) {
            log(NOTICE, "Setting a map attribute on an entity from a script");
        }
        if (obj.isList()) {
            log(NOTICE, "Setting a list attribute on an entity from a script");
        }
        entity->setAttr(name, obj);
        return 0;
    }
    // FIXME In fact it seems that nothing currently hits this bit, so
    // all this code is redundant for entity scripts.
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
    if (self->m_entity == NULL || other->m_entity == NULL) {
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

PyObject * wrapEntity(Entity * entity)
{
    PyObject * wrapper;
    PythonWrapper * pw = dynamic_cast<PythonWrapper *>(entity->script());
    if (pw == 0) {
        Character * ch_entity = dynamic_cast<Character *>(entity);
        if (ch_entity != 0) {
            PyCharacter * pc = newPyCharacter();
            if (pc == NULL) {
                return NULL;
            }
            pc->m_entity = ch_entity;
            wrapper = (PyObject *)pc;
        } else {
            PyEntity * pe = newPyEntity();
            if (pe == NULL) {
                return NULL;
            }
            pe->m_entity = entity;
            wrapper = (PyObject *)pe;
        }
        if (entity->script() == &noScript) {
            pw = new PythonWrapper(wrapper);
            entity->setScript(pw);
        } else {
            log(WARNING, "Entity has script of unknown type");
        }
    } else {
        wrapper = pw->wrapper();
        assert(wrapper != NULL);
        Py_INCREF(wrapper);
    }
    return wrapper;
}

PyEntity * newPyEntity()
{
    PyEntity * self;
    self = PyObject_NEW(PyEntity, &PyEntity_Type);
    if (self == NULL) {
        return NULL;
    }
    self->Entity_attr = NULL;
    self->m_methods = Entity_methods;
    return self;
}

PyCharacter * newPyCharacter()
{
    PyCharacter * self;
    self = PyObject_NEW(PyCharacter, &PyEntity_Type);
    if (self == NULL) {
        return NULL;
    }
    self->Entity_attr = NULL;
    self->m_methods = Character_methods;
    return self;
}
