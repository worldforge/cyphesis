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

#include "Py_Thing.h"
#include "Py_Message.h"
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

#include "common/id.h"
#include "common/log.h"
#include "common/Property.h"
#include "common/TypeNode.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;

static PyObject * Entity_as_entity(PyLocatedEntity * self)
{
#ifndef NDEBUG
    if (self->m_entity == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in Entity.as_entity");
        return NULL;
    }
#endif // NDEBUG
    PyMessage * ret = newPyMessage();
    if (ret == NULL) {
        return NULL;
    }
    ret->m_obj = new Element(MapType());
    self->m_entity->addToMessage(ret->m_obj->asMap());
    return (PyObject *)ret;
}

static PyMethodDef LocatedEntity_methods[] = {
    {"as_entity",       (PyCFunction)Entity_as_entity,  METH_NOARGS},
    {NULL,              NULL}           /* sentinel */
};

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
        if (self->m_entity->getType() == NULL) {
            PyErr_SetString(PyExc_AttributeError, name);
            return NULL;
        }
        PyObject * list = PyList_New(0);
        if (list == NULL) {
            return NULL;
        }
        PyObject * ent = PyString_FromString(self->m_entity->getType()->name().c_str());
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
    if (strcmp(name, "contains") == 0) {
        if (self->m_entity->m_contains == 0) {
            Py_INCREF(Py_None);
            return Py_None;
        }
        PyObject * list = PyList_New(0);
        if (list == NULL) {
            return NULL;
        }
        LocatedEntitySet::const_iterator I = self->m_entity->m_contains->begin();
        LocatedEntitySet::const_iterator Iend = self->m_entity->m_contains->end();
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
    if (self->Entity_attr != NULL) {
        PyObject *v = PyDict_GetItemString(self->Entity_attr, name);
        if (v != NULL) {
            Py_INCREF(v);
            return v;
        }
    }
    Entity * entity = self->m_entity;
    PropertyBase * prop = entity->modProperty(name);
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
    return Py_FindMethod(self->ob_type->tp_methods, (PyObject *)self, name);
}

static int Entity_setattr(PyEntity *self, char *name, PyObject *v)
{
#ifndef NDEBUG
    if (self->m_entity == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in Entity.getattr");
        return -1;
    }
#endif // NDEBUG
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
    Element obj;
    if (PyObject_asMessageElement(v, obj) == 0) {
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

static int LocatedEntity_init(PyEntity * self, PyObject * args, PyObject * kwds)
{
    char * id = NULL;

    if (!PyArg_ParseTuple(args, "s", &id)) {
        return -1;
    }
    long intId = integerId(id);
    if (intId == -1L) {
        PyErr_SetString(PyExc_TypeError, "Entity() requires string/int ID");
        return -1;
    }
    self->m_entity = new Entity(id, intId);
    return 0;
}

static int Entity_init(PyEntity * self, PyObject * args, PyObject * kwds)
{
    char * id = NULL;

    if (!PyArg_ParseTuple(args, "s", &id)) {
        return -1;
    }
    long intId = integerId(id);
    if (intId == -1L) {
        PyErr_SetString(PyExc_TypeError, "Entity() requires string/int ID");
        return -1;
    }
    self->m_entity = new Entity(id, intId);
    return 0;
}

static int Character_init(PyEntity * self, PyObject * args, PyObject * kwds)
{
    char * id = NULL;

    if (!PyArg_ParseTuple(args, "s", &id)) {
        return -1;
    }
    long intId = integerId(id);
    if (intId == -1L) {
        PyErr_SetString(PyExc_TypeError, "Entity() requires string/int ID");
        return -1;
    }
    self->m_entity = new Character(id, intId);
    return 0;
}

static PyObject * Entity_new(PyTypeObject * type, PyObject *, PyObject *)
{
    PyEntity * self = (PyEntity *)type->tp_alloc(type, 0);
    return (PyObject *)self;
}

PyTypeObject PyLocatedEntity_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /*ob_size*/
        "server.Entity",                /*tp_name*/
        sizeof(PyLocatedEntity),        /*tp_basicsize*/
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
        0,                              // tp_call
        0,                              // tp_str
        0,                              // tp_getattro
        0,                              // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT,             // tp_flags
        "Entity objects",               // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        0,                              // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        LocatedEntity_methods,          // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        0,                              // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)LocatedEntity_init,   // tp_init
        0,                              // tp_alloc
        Entity_new,                     // tp_new
};

PyTypeObject PyEntity_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /*ob_size*/
        "server.Entity",                /*tp_name*/
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
        0,                              // tp_call
        0,                              // tp_str
        0,                              // tp_getattro
        0,                              // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT,             // tp_flags
        "Entity objects",               // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        0,                              // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        Entity_methods,                 // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        0,                              // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)Entity_init,          // tp_init
        0,                              // tp_alloc
        Entity_new,                     // tp_new
};

PyTypeObject PyCharacter_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /*ob_size*/
        "server.Entity",                /*tp_name*/
        sizeof(PyCharacter),            /*tp_basicsize*/
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
        0,                              // tp_call
        0,                              // tp_str
        0,                              // tp_getattro
        0,                              // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT,             // tp_flags
        "Entity objects",               // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        0,                              // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        Character_methods,              // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        0,                              // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)Character_init,       // tp_init
        0,                              // tp_alloc
        Entity_new,                     // tp_new
};

PyObject * wrapEntity(LocatedEntity * le)
{
    PyObject * wrapper;
    PythonWrapper * pw = dynamic_cast<PythonWrapper *>(le->script());
    if (pw == 0) {
        Entity * entity = dynamic_cast<Entity *>(le);
        if (entity != 0) {
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
        } else {
          PyLocatedEntity * pe = newPyLocatedEntity();
          if (pe == NULL) {
              return NULL;
          }
          pe->m_entity = le;
          wrapper = (PyObject *)pe;
        }
        if (le->script() == &noScript) {
            pw = new PythonWrapper(wrapper);
            le->setScript(pw);
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

// FIXME if this is cleaner to use the methods field of the type for methods,
// one PyTypeObject for each, and just share the members. That way we lose
// the need for a members entry on the object.

PyLocatedEntity * newPyLocatedEntity()
{
#if 0
    PyLocatedEntity * self;
    self = PyObject_NEW(PyLocatedEntity, &PyEntity_Type);
    if (self == NULL) {
        return NULL;
    }
    self->Entity_attr = NULL;
    self->m_methods = LocatedEntity_methods;
    return self;
#else
    PyLocatedEntity * self = (PyLocatedEntity *)PyLocatedEntity_Type.tp_new(&PyLocatedEntity_Type, 0, 0);
    return self;
#endif
}

PyEntity * newPyEntity()
{
#if 0
    PyEntity * self;
    self = PyObject_NEW(PyEntity, &PyEntity_Type);
    if (self == NULL) {
        return NULL;
    }
    self->Entity_attr = NULL;
    self->m_methods = Entity_methods;
    return self;
#else
    PyEntity * self = (PyEntity *)PyEntity_Type.tp_new(&PyEntity_Type, 0, 0);
    return self;
#endif
}

PyCharacter * newPyCharacter()
{
#if 0
    PyCharacter * self;
    self = PyObject_NEW(PyCharacter, &PyEntity_Type);
    if (self == NULL) {
        return NULL;
    }
    self->Entity_attr = NULL;
    self->m_methods = Character_methods;
    return self;
#else
    PyCharacter * self = (PyCharacter *)PyCharacter_Type.tp_new(&PyCharacter_Type, 0, 0);
    return self;
#endif
}
