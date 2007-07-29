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

// $Id: Py_Object.cpp,v 1.46 2007-07-29 03:33:34 alriddoch Exp $

#include "Py_Object.h"
#include "Py_Operation.h"
#include "Py_Oplist.h"
#include "Py_Location.h"

#include "modules/Location.h"

#include "common/log.h"
#include "common/compose.hpp"
#include "common/debug.h"

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

static const bool debug_flag = false;

/*
 * Beginning of Object methods section.
 */

static PyObject* Object_get_name(PyMessageElement * self)
{
#ifndef NDEBUG
    if (self->m_obj == NULL) {
        PyErr_SetString(PyExc_AssertionError,"NULL MessageElement in MessageElement.get_name");
        return NULL;
    }
#endif // NDEBUG
    return PyString_FromString("obj");
}

/*
 * Object methods structure.
 */

static PyMethodDef Object_methods[] = {
        {"get_name",    (PyCFunction)Object_get_name,  METH_NOARGS},
        {NULL,          NULL}           /* sentinel */
};

/*
 * Beginning of Object standard methods section.
 */

static void Object_dealloc(PyMessageElement *self)
{
    if (self->m_obj != NULL) {
        delete self->m_obj;
    }
    PyObject_Free(self);
}

static PyObject * Object_getattr(PyMessageElement *self, char *name)
{
#ifndef NDEBUG
    if (self->m_obj == NULL) {
        PyErr_SetString(PyExc_AssertionError,"NULL MessageElement in MessageElement.getattr");
        return NULL;
    }
#endif // NDEBUG
    if (self->m_obj->isMap()) {
        const MapType & omap = self->m_obj->asMap();
        MapType::const_iterator I = omap.find(name);
        if (I != omap.end()) {
            return MessageElement_asPyObject(I->second);
        }
    }
    return Py_FindMethod(Object_methods, (PyObject *)self, name);
}

static int Object_setattr( PyMessageElement *self, char *name, PyObject *v)
{
#ifndef NDEBUG
    if (self->m_obj == NULL) {
        PyErr_SetString(PyExc_AssertionError,"NULL MessageElement in MessageElement.setattr");
        return -1;
    }
#endif // NDEBUG
    log(WARNING, String::compose("Setting \"%1\" attribute on an Atlas Message",
                                 name));
    if (self->m_obj->isMap()) {
        MapType & omap = self->m_obj->asMap();
        Element v_obj = PyObject_asMessageElement(v);
        if (v_obj.getType() != Element::TYPE_NONE) {
            omap[name] = v_obj;
            return 0;
        } else {
            PyErr_SetString(PyExc_TypeError, "object cannot be converted to Atlas data in MessageElement.setattr");
            return -1;
        }
    }
    PyErr_SetString(PyExc_TypeError, "Cannot set attribute on non-map in MessageElement.setattr");
    return -1;
}

PyTypeObject PyMessageElement_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /*ob_size*/
        "MessageElement",                     /*tp_name*/
        sizeof(PyMessageElement),               /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        (destructor)Object_dealloc,     /*tp_dealloc*/
        0,                              /*tp_print*/
        (getattrfunc)Object_getattr,    /*tp_getattr*/
        (setattrfunc)Object_setattr,    /*tp_setattr*/
        0,                              /*tp_compare*/
        0,                              /*tp_repr*/
        0,                              /*tp_as_number*/
        0,                              /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
};

/*
 * Beginning of Object creation functions section.
 */

PyMessageElement * newPyMessageElement()
{
    PyMessageElement * self;
    self = PyObject_NEW(PyMessageElement, &PyMessageElement_Type);
    if (self == NULL) {
        return NULL;
    }
    return self;
}

/*
 * Utility functions to munge between Object related types and python types
 */

static PyObject * MapType_asPyObject(const MapType & map)
{
    PyObject * args_pydict = PyDict_New();
    PyMessageElement * item;
    MapType::const_iterator Iend = map.end();
    for (MapType::const_iterator I = map.begin(); I != Iend; ++I) {
        const std::string & key = I->first;
        item = newPyMessageElement();
        if (item == NULL) {
            PyErr_SetString(PyExc_MemoryError,"error creating map");
            return NULL;
        }
        item->m_obj = new Element(I->second);
        // PyDict_SetItem() does not eat the reference passed to it
        PyDict_SetItemString(args_pydict,(char *)key.c_str(),(PyObject *)item);
        Py_DECREF(item);
    }
    return args_pydict;
}

static PyObject * ListType_asPyObject(const ListType & list)
{
    PyObject * args_pylist = PyList_New(list.size());
    int j = 0;
    PyMessageElement * item;
    ListType::const_iterator Iend = list.end();
    for (ListType::const_iterator I = list.begin(); I != Iend; ++I, ++j) {
        item = newPyMessageElement();
        if (item == NULL) {
            PyErr_SetString(PyExc_MemoryError,"error creating list");
            return NULL;
        }
        item->m_obj = new Element(*I);
        // PyList_SetItem() eats the reference passed to it
        PyList_SetItem(args_pylist, j, (PyObject *)item);
    }
    return args_pylist;
}

PyObject * MessageElement_asPyObject(const Element & obj)
{
    PyObject * ret = NULL;
    switch (obj.getType()) {
        case Element::TYPE_INT:
            ret = PyInt_FromLong(obj.Int());
            break;
        case Element::TYPE_FLOAT:
            ret = PyFloat_FromDouble(obj.Float());
            break;
        case Element::TYPE_STRING:
            ret = PyString_FromString(obj.String().c_str());
            break;
        case Element::TYPE_MAP:
            ret = MapType_asPyObject(obj.Map());
            break;
        case Element::TYPE_LIST:
            ret = ListType_asPyObject(obj.List());
            break;
        default:
            Py_INCREF(Py_None);
            ret = Py_None;
            break;
    }
    return ret;
}

static Element PyListObject_asElement(PyObject * list)
{
    ListType argslist;
    PyMessageElement * item;
    int len = PyList_Size(list);
    for(int i = 0; i < len; i++) {
        item = (PyMessageElement *)PyList_GetItem(list, i);
        if (PyMessageElement_Check(item)) {
            argslist.push_back(*(item->m_obj));
        } else {
            Element o = PyObject_asMessageElement((PyObject*)item);
            if (o.getType() != Element::TYPE_NONE) {
                argslist.push_back(o);
            } else {
                debug( std::cout << "Python to atlas conversion failed on element " << i << " of list" << std::endl << std::flush; );
                return Element();
            }
        }
    }
    return argslist;
}

static Element PyDictObject_asElement(PyObject * dict)
{
    MapType argsmap;
    PyMessageElement * item;
    PyObject * keys = PyDict_Keys(dict);
    PyObject * vals = PyDict_Values(dict);
    for(int i = 0; i < PyDict_Size(dict); i++) {
        PyObject * key = PyList_GetItem(keys, i);
        item = (PyMessageElement *)PyList_GetItem(vals, i);
        if (PyMessageElement_Check(item)) {
            argsmap[PyString_AsString(key)] = *(item->m_obj);
        } else {
            Element o = PyObject_asMessageElement((PyObject*)item);
            if (o.getType() != Element::TYPE_NONE) {
                argsmap[PyString_AsString(key)] = o;
            } else {
                debug( std::cout << "Python to atlas conversion failed on element " << PyString_AsString(key) << " of map" << std::endl << std::flush; );
                return Element();
            }
        }
    }
    Py_DECREF(keys);
    Py_DECREF(vals);
    return argsmap;
}

Element PyObject_asMessageElement(PyObject * o, bool simple)
{
    if (PyInt_Check(o)) {
        return Element((int)PyInt_AsLong(o));
    }
    if (PyFloat_Check(o)) {
        return Element(PyFloat_AsDouble(o));
    }
    if (PyString_Check(o)) {
        return Element(PyString_AsString(o));
    }
    // If the caller has specified that it is not interested in
    // map or list results, we should just return now.
    if (simple) {
        return Element();
    }
    if (PyList_Check(o)) {
        return PyListObject_asElement(o);
    }
    if (PyDict_Check(o)) {
        return PyDictObject_asElement(o);
    }
    if (PyTuple_Check(o)) {
        ListType list;
        int i, size = PyTuple_Size(o);
        for(i = 0; i < size; i++) {
            Element item = PyObject_asMessageElement(PyTuple_GetItem(o, i));
            if (item.getType() != Element::TYPE_NONE) {
                list.push_back(item);
            } else {
                debug( std::cout << "Python to atlas conversion failed on element " << i << " of tuple" << std::endl << std::flush; );
                return Element();
            }
        }
        return Element(list);
    }
    if (PyMessageElement_Check(o)) {
        PyMessageElement * obj = (PyMessageElement *)o;
        return *(obj->m_obj);
    }
    if (PyOperation_Check(o)) {
        PyOperation * op = (PyOperation *)o;
        return op->operation->asMessage();
    }
    if (PyOplist_Check(o)) {
        PyOplist * opl = (PyOplist *)o;
        Element msg = ListType();
        ListType & entlist = msg.asList();
        const OpVector & ops = *opl->ops;
        OpVector::const_iterator Iend = ops.end();
        for (OpVector::const_iterator I = ops.begin(); I != Iend; ++I) {
            entlist.push_back((*I)->asMessage());
        }
        return msg;
    }
    if (PyLocation_Check(o)) {
        PyLocation * loc = (PyLocation *)o;
        MapType _map;
        loc->location->addToMessage(_map);
        return Element(_map);
    }
    return Element();
}
