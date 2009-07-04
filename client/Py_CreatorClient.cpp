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

#include "rulesets/Python_Script_Utils.h"
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
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.as_entity");
        return NULL;
    }
#endif // NDEBUG
    PyMessage * ret = newPyMessage();
    if (ret != NULL) {
        ret->m_obj = new Element(MapType());
        self->m_mind->addToMessage(ret->m_obj->asMap());
    }
    return (PyObject *)ret;
}

static PyObject * CreatorClient_make(PyCreatorClient * self,
                                     PyRootEntity * entity)
{
#ifndef NDEBUG
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.make");
        return NULL;
    }
#endif // NDEBUG
    if (!PyRootEntity_Check(entity)) {
        PyErr_SetString(PyExc_TypeError, "Can only make Atlas entity");
        return NULL;
    }
    LocatedEntity * retval = self->m_mind->make(entity->entity);
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
    if (self->m_mind == NULL) {
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
    self->m_mind->sendSet(id, entity->entity);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * CreatorClient_look(PyCreatorClient * self, PyObject * py_id)
{
#ifndef NDEBUG
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.look");
        return NULL;
    }
#endif // NDEBUG
    if (!PyString_CheckExact(py_id)) {
        PyErr_SetString(PyExc_TypeError, "CreatorClient.look must be a string");
        return NULL;
    }
    char * id = PyString_AsString(py_id);
    LocatedEntity * retval = self->m_mind->look(id);
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
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.look_for");
        return NULL;
    }
#endif // NDEBUG
    if (!PyRootEntity_Check(ent)) {
        PyErr_SetString(PyExc_TypeError, "Can only look for Atlas description");
        return NULL;
    }
    LocatedEntity * retval = self->m_mind->lookFor(ent->entity);
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
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.send");
        return NULL;
    }
#endif // NDEBUG
    if (!PyOperation_Check(op)) {
        PyErr_SetString(PyExc_TypeError, "Can only send Atlas operation");
        return NULL;
    }
    self->m_mind->send(op->operation);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * CreatorClient_delete(PyCreatorClient * self, PyObject * py_id)
{
#ifndef NDEBUG
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.send");
        return NULL;
    }
#endif // NDEBUG
    if (!PyString_CheckExact(py_id)) {
        PyErr_SetString(PyExc_TypeError, "CreatorClient.delete must be a string");
        return NULL;
    }
    char * id = PyString_AsString(py_id);
    self->m_mind->del(id);
    Py_INCREF(Py_None);
    return Py_None;
}

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

static void CreatorClient_dealloc(PyCreatorClient *self)
{
    //if (self->m_mind != NULL) {
        //delete self->m_mind;
    //}
    Py_XDECREF(self->CreatorClient_attr);
    PyMem_DEL(self);
}

static PyObject * CreatorClient_getattr(PyCreatorClient *self, char *name)
{
    // Fairly major re-write of this to use operator[] of CreatorClient base class
#ifndef NDEBUG
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.getattr");
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
        PyList_Append(list, PyString_FromString(self->m_mind->getType()->name().c_str()));
        return list;
    }
    if (strcmp(name, "map") == 0) {
        PyMap * map = newPyMap();
        if (map != NULL) {
            map->m_map = self->m_mind->getMap();
        }
        return (PyObject *)map;
    }
    if (strcmp(name, "location") == 0) {
        PyLocation * loc = newPyLocation();
        if (loc != NULL) {
            loc->location = &self->m_mind->m_location;
            loc->owner = self->m_mind;
        }
        return (PyObject *)loc;
    }
    if (strcmp(name, "time") == 0) {
        PyWorldTime * worldtime = newPyWorldTime();
        if (worldtime != NULL) {
            worldtime->time = self->m_mind->getTime();
        }
        return (PyObject *)worldtime;
    }
    if (self->CreatorClient_attr != NULL) {
        PyObject *v = PyDict_GetItemString(self->CreatorClient_attr, name);
        if (v != NULL) {
            Py_INCREF(v);
            return v;
        }
    }
    LocatedEntity * thing = self->m_mind;
    Element attr;
    if (!thing->getAttr(name, attr)) {
        return Py_FindMethod(CreatorClient_methods, (PyObject *)self, name);
    }
    PyObject * ret = MessageElement_asPyObject(attr);
    if (ret == NULL) {
        return Py_FindMethod(CreatorClient_methods, (PyObject *)self, name);
    }
    return ret;
}

static int CreatorClient_setattr(PyCreatorClient *self, char *name, PyObject *v)
{
    if (self->m_mind == NULL) {
        return -1;
    }
    if (self->CreatorClient_attr == NULL) {
        self->CreatorClient_attr = PyDict_New();
        if (self->CreatorClient_attr == NULL) {
            return -1;
        }
    }
    if (strcmp(name, "map") == 0) {
        return -1;
    }
    LocatedEntity * thing = self->m_mind;
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
    // If we get here, then the attribute is not Atlas compatable, so we
    // need to store it in a python dictionary
    return PyDict_SetItemString(self->CreatorClient_attr, name, v);
}

static int CreatorClient_compare(PyCreatorClient *self, PyCreatorClient *other)
{
    if (self->m_mind == NULL || other->m_mind == NULL) {
        return -1;
    }
    return (self->m_mind == other->m_mind) ? 0 : 1;
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
    // self->m_mind = new CreatorClient(id, intId);
    return 0;
}

PyTypeObject PyCreatorClient_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /*ob_size*/
        "CreatorClient",                     /*tp_name*/
        sizeof(PyCreatorClient),                /*tp_basicsize*/
        0,                                      /*tp_itemsize*/
        /* methods */
        (destructor)CreatorClient_dealloc,      /*tp_dealloc*/
        0,                                      /*tp_print*/
        (getattrfunc)CreatorClient_getattr,     /*tp_getattr*/
        (setattrfunc)CreatorClient_setattr,     /*tp_setattr*/
        (cmpfunc)CreatorClient_compare,         /*tp_compare*/
        0,                                      /*tp_repr*/
        0,                                      /*tp_as_number*/
        0,                                      /*tp_as_sequence*/
        0,                                      /*tp_as_mapping*/
        0,                                      /*tp_hash*/
        0,                                      // tp_call
        0,                                      // tp_str
        0,                                      // tp_getattro
        0,                                      // tp_setattro
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

int runClientScript(CreatorClient * c, const std::string & package,
                                       const std::string & func)
{
    PyObject * module = Get_PyModule(package);
    if (module == NULL) {
        return -1;
    }
    PyObject * function = PyObject_GetAttrString(module,
                                                 (char *)func.c_str());
    Py_DECREF(module);
    if (function == NULL) {
        std::cerr << "Could not find " << func << " function" << std::endl
                  << std::flush;
        PyErr_Print();
        return -1;
    }
    if (PyCallable_Check(function) == 0) {
        std::cerr << "It does not seem to be a function at all" << std::endl
                  << std::flush;
        Py_DECREF(function);
        return -1;
    }
    PyCreatorClient * editor = newPyCreatorClient();
    if (editor == NULL) {
        Py_DECREF(function);
        return -1;
    }
    editor->m_mind = c;
    PyObject * pyob = PyEval_CallFunction(function, "(O)", editor);

    if (pyob == NULL) {
        if (PyErr_Occurred() == NULL) {
            std::cerr << "Could not call function" << std::endl << std::flush;
        } else {
            std::cerr << "Reporting python error" << std::endl << std::flush;
            PyErr_Print();
        }
    }
    Py_DECREF(function);
    return 0;

}

void extend_client_python_api()
{
    PyObject * server = Get_PyModule("server");
    if (server == 0) {
        return;
    }

    PyCreatorClient_Type.tp_new = PyType_GenericNew;
    if (PyType_Ready(&PyCreatorClient_Type) < 0) {
        log(CRITICAL, "Python init failed to ready CreatorClient wrapper type");
        return;
    }
    PyModule_AddObject(server, "CreatorClient", (PyObject *)&PyMap_Type);
}
