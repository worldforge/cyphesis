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

#include "Py_CreatorClient.h"

#include "CreatorClient.h"

#include "rulesets/Py_Operation.h"
#include "rulesets/Py_RootEntity.h"
#include "rulesets/Py_WorldTime.h"
#include "rulesets/Py_Point3D.h"
#include "rulesets/Py_Location.h"
#include "rulesets/Py_Message.h"
#include "rulesets/Py_Thing.h"
#include "rulesets/Py_Map.h"

#include "common/debug.h"
#include "common/id.h"
#include "common/log.h"
#include "common/TypeNode.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;

static const bool debug_flag = false;

static PyObject * CreatorClient_as_entity(PyCreatorClient * self, PyObject *)
{
#ifndef NDEBUG
    if (self->m_mind.c == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.as_entity");
        return NULL;
    }
#endif // NDEBUG
    PyMessage * ret = newPyMessage();
    if (ret != NULL) {
        ret->m_obj = new Element(MapType());
        self->m_mind.c->addToMessage(ret->m_obj->asMap());
    }
    return (PyObject *)ret;
}

static PyObject * CreatorClient_make(PyCreatorClient * self,
                                     PyRootEntity * entity)
{
#ifndef NDEBUG
    if (self->m_mind.c == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.make");
        return NULL;
    }
#endif // NDEBUG
    if (!PyRootEntity_Check(entity)) {
        PyErr_SetString(PyExc_TypeError, "Can only make Atlas entity");
        return NULL;
    }
    LocatedEntity * retval = self->m_mind.a->make(entity->entity);
    if (retval == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Entity creation failed");
        return NULL;
    }
    PyEntity * ret = newPyLocatedEntity();
    if (ret != NULL) {
        ret->m_entity.l = retval;
    }
    return (PyObject *)ret;
}

static PyObject * CreatorClient_set(PyCreatorClient * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_mind.c == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.set");
        return NULL;
    }
#endif // NDEBUG
    PyRootEntity * entity = NULL;
    char * id = NULL;
    if (!PyArg_ParseTuple(args, "sO", &id, &entity)) {
        return NULL;
    }
    if (!PyRootEntity_Check(entity)) {
        PyErr_SetString(PyExc_TypeError, "Can only set Atlas entity");
        return NULL;
    }
    self->m_mind.a->sendSet(id, entity->entity);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * CreatorClient_look(PyCreatorClient * self, PyObject * py_id)
{
#ifndef NDEBUG
    if (self->m_mind.c == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.look");
        return NULL;
    }
#endif // NDEBUG
    if (!PyString_CheckExact(py_id)) {
        PyErr_SetString(PyExc_TypeError, "CreatorClient.look must be a string");
        return NULL;
    }
    char * id = PyString_AsString(py_id);
    LocatedEntity * retval = self->m_mind.c->look(id);
    if (retval == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Entity look failed");
        return NULL;
    }
    PyEntity * ret = newPyLocatedEntity();
    if (ret != NULL) {
        ret->m_entity.l = retval;
    }
    return (PyObject *)ret;
}
static PyObject * CreatorClient_look_for(PyCreatorClient * self,
                                         PyRootEntity * ent)
{
#ifndef NDEBUG
    if (self->m_mind.c == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.look_for");
        return NULL;
    }
#endif // NDEBUG
    if (!PyRootEntity_Check(ent)) {
        PyErr_SetString(PyExc_TypeError, "Can only look for Atlas description");
        return NULL;
    }
    LocatedEntity * retval = self->m_mind.c->lookFor(ent->entity);
    if (retval == NULL) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    PyEntity * ret = newPyLocatedEntity();
    if (ret != NULL) {
        ret->m_entity.l = retval;
    }
    return (PyObject *)ret;
}

static PyObject * CreatorClient_send(PyCreatorClient * self, PyOperation * op)
{
#ifndef NDEBUG
    if (self->m_mind.c == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.send");
        return NULL;
    }
#endif // NDEBUG
    if (!PyOperation_Check(op)) {
        PyErr_SetString(PyExc_TypeError, "Can only send Atlas operation");
        return NULL;
    }
    self->m_mind.c->send(op->operation);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * CreatorClient_delete(PyCreatorClient * self, PyObject * py_id)
{
#ifndef NDEBUG
    if (self->m_mind.c == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.send");
        return NULL;
    }
#endif // NDEBUG
    if (!PyString_CheckExact(py_id)) {
        PyErr_SetString(PyExc_TypeError, "CreatorClient.delete must be a string");
        return NULL;
    }
    char * id = PyString_AsString(py_id);
    self->m_mind.a->del(id);
    Py_INCREF(Py_None);
    return Py_None;
}

#if 0
static PyMethodDef CharacterClient_methods[] = {
        {"as_entity",      (PyCFunction)CreatorClient_as_entity, METH_NOARGS},
        {"make",           (PyCFunction)CreatorClient_make,      METH_O},
        {"look",           (PyCFunction)CreatorClient_look,      METH_O},
        {"look_for",       (PyCFunction)CreatorClient_look_for,  METH_O},
        {"send",           (PyCFunction)CreatorClient_send,      METH_O},
        {NULL,          NULL}           /* sentinel */
};
#endif

static PyMethodDef CreatorClient_methods[] = {
        {"as_entity",      (PyCFunction)CreatorClient_as_entity, METH_NOARGS},
        {"make",           (PyCFunction)CreatorClient_make,      METH_O},
        {"set",            (PyCFunction)CreatorClient_set,       METH_VARARGS},
        {"look",           (PyCFunction)CreatorClient_look,      METH_O},
        {"look_for",       (PyCFunction)CreatorClient_look_for,  METH_O},
        {"send",           (PyCFunction)CreatorClient_send,      METH_O},
        {"delete",         (PyCFunction)CreatorClient_delete,    METH_O},
        {NULL,          NULL}           /* sentinel */
};

static PyObject * CreatorClient_getattro(PyCreatorClient *self,
                                         PyObject *oname)
{
    // Fairly major re-write of this to use operator[] of CreatorClient base class
#ifndef NDEBUG
    if (self->m_mind.c == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.getattr");
        return NULL;
    }
#endif // NDEBUG
    char * name = PyString_AsString(oname);
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
        PyList_Append(list, PyString_FromString(self->m_mind.c->getType()->name().c_str()));
        return list;
    }
    if (strcmp(name, "map") == 0) {
        PyMap * map = newPyMap();
        if (map != NULL) {
            map->m_map = self->m_mind.c->getMap();
        }
        return (PyObject *)map;
    }
    if (strcmp(name, "location") == 0) {
        PyLocation * loc = newPyLocation();
        if (loc != NULL) {
            loc->location = &self->m_mind.c->m_location;
            loc->owner = self->m_mind.c;
        }
        return (PyObject *)loc;
    }
    if (strcmp(name, "time") == 0) {
        PyWorldTime * worldtime = newPyWorldTime();
        if (worldtime != NULL) {
            worldtime->time = self->m_mind.c->getTime();
        }
        return (PyObject *)worldtime;
    }
    LocatedEntity * thing = self->m_mind.c;
    Element attr;
    if (thing->getAttr(name, attr) != 0) {
        return Py_FindMethod(CreatorClient_methods, (PyObject *)self, name);
    }
    PyObject * ret = MessageElement_asPyObject(attr);
    if (ret == NULL) {
        return Py_FindMethod(CreatorClient_methods, (PyObject *)self, name);
    }
    return ret;
}

static int CreatorClient_setattro(PyCreatorClient *self,
                                  PyObject * oname,
                                  PyObject *v)
{
    char * name = PyString_AsString(oname);
    if (self->m_mind.c == NULL) {
        return -1;
    }
    if (strcmp(name, "map") == 0) {
        return -1;
    }
    LocatedEntity * thing = self->m_mind.c;
    //std::string attr(name);
    //if (v == NULL) {
        //thing->attributes.erase(attr);
        //return 0;
    //}
    Element obj;
    if (PyObject_asMessageElement(v, obj) == 0 && !obj.isMap() && !obj.isList()) {
        thing->setAttr(name, obj);
        return 0;
    }
    return -1;
}

static int CreatorClient_compare(PyCreatorClient *self, PyCreatorClient *other)
{
    if (self->m_mind.c == NULL || other->m_mind.c == NULL) {
        return -1;
    }
    return (self->m_mind.c == other->m_mind.c) ? 0 : 1;
}

static int CreatorClient_init(PyCreatorClient * self,
                              PyObject * args, PyObject * kwds)
{
    char * id = NULL;

    if (!PyArg_ParseTuple(args, "s", &id)) {
        return -1;
    }
    long intId = integerId(id);
    if (intId == -1L) {
        PyErr_SetString(PyExc_TypeError, "CreatorClient() requires string/int ID");
        return -1;
    }
    self->m_mind.c = 0;
    return 0;
}

PyTypeObject PyCharacterClient_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /*ob_size*/
        "CreatorClient",                        /*tp_name*/
        sizeof(PyCreatorClient),                /*tp_basicsize*/
        0,                                      /*tp_itemsize*/
        /* methods */
        0,                                      /*tp_dealloc*/
        0,                                      /*tp_print*/
        0,                                      /*tp_getattr*/
        0,                                      /*tp_setattr*/
        (cmpfunc)CreatorClient_compare,         /*tp_compare*/
        0,                                      /*tp_repr*/
        0,                                      /*tp_as_number*/
        0,                                      /*tp_as_sequence*/
        0,                                      /*tp_as_mapping*/
        0,                                      /*tp_hash*/
        0,                                      // tp_call
        0,                                      // tp_str
        (getattrofunc)CreatorClient_getattro,   // tp_getattro
        (setattrofunc)CreatorClient_setattro,   // tp_setattro
        0,                                      // tp_as_buffer
        Py_TPFLAGS_DEFAULT,                     // tp_flags
        "CreatorClient objects",                // tp_doc
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
        (initproc)CreatorClient_init,           // tp_init
        0,                                      // tp_alloc
        0,                                      // tp_new
};

PyTypeObject PyCreatorClient_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /*ob_size*/
        "CreatorClient",                     /*tp_name*/
        sizeof(PyCreatorClient),                /*tp_basicsize*/
        0,                                      /*tp_itemsize*/
        /* methods */
        0,                                      /*tp_dealloc*/
        0,                                      /*tp_print*/
        0,                                      /*tp_getattr*/
        0,                                      /*tp_setattr*/
        (cmpfunc)CreatorClient_compare,         /*tp_compare*/
        0,                                      /*tp_repr*/
        0,                                      /*tp_as_number*/
        0,                                      /*tp_as_sequence*/
        0,                                      /*tp_as_mapping*/
        0,                                      /*tp_hash*/
        0,                                      // tp_call
        0,                                      // tp_str
        (getattrofunc)CreatorClient_getattro,   // tp_getattro
        (setattrofunc)CreatorClient_setattro,   // tp_setattro
        0,                                      // tp_as_buffer
        Py_TPFLAGS_DEFAULT,                     // tp_flags
        "CreatorClient objects",                // tp_doc
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
        (initproc)CreatorClient_init,           // tp_init
        0,                                      // tp_alloc
        0,                                      // tp_new
};

PyCreatorClient * newPyCreatorClient()
{
    return (PyCreatorClient *)PyCreatorClient_Type.tp_new(&PyCreatorClient_Type, 0, 0);
}

