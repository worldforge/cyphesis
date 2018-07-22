// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2011 Alistair Riddoch
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


#include "Py_Message.h"
#include "Py_Operation.h"
#include "Py_Oplist.h"
#include "Py_Location.h"

#include "Location.h"

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

static PyObject* Message_get_name(PyMessage * self)
{
#ifndef NDEBUG
    if (self->m_obj == nullptr) {
        PyErr_SetString(PyExc_AssertionError,"nullptr MessageElement in MessageElement.get_name");
        return nullptr;
    }
#endif // NDEBUG
    return PyUnicode_FromString("obj");
}


/**
 * Method for forcing an instance of atlas.Message into a native Python struct.
 *
 * This should perhaps be done in a better way though, as it makes the Python code be littered
 * with calls to "pythonize()".
 * @param self A Python message.
 * @return A native python struct.
 */
static PyObject* Message_pythonize(PyMessage * self)
{
#ifndef NDEBUG
    if (self->m_obj == nullptr) {
        PyErr_SetString(PyExc_AssertionError,"nullptr MessageElement in MessageElement.pythonize");
        return nullptr;
    }
#endif // NDEBUG
    return MessageElement_asPyObject(*self->m_obj);
}

/*
 * Object methods structure.
 */

static PyMethodDef Message_methods[] = {
        {"get_name",    (PyCFunction)Message_get_name,  METH_NOARGS},
        {"pythonize",   (PyCFunction)Message_pythonize,  METH_NOARGS},
        {nullptr,          nullptr}           /* sentinel */
};

/*
 * Beginning of Object standard methods section.
 */

static void Message_dealloc(PyMessage *self)
{
    delete self->m_obj;
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject * Message_repr(PyMessage *self)
{
#ifndef NDEBUG
    if (self->m_obj == nullptr) {
        PyErr_SetString(PyExc_AssertionError,"nullptr MessageElement in MessageElement.repr");
        return nullptr;
    }
#endif // NDEBUG
    if (self->m_obj->isString()) {
        return PyUnicode_FromString(self->m_obj->asString().c_str());
    }
    return PyUnicode_FromFormat("<%s object at %p>(%s)",
                               Py_TYPE(self)->tp_name,
                               self,
                               debug_tostring(*self->m_obj).c_str());
}

static PyObject * Message_getattro(PyMessage *self, PyObject *oname)
{
#ifndef NDEBUG
    if (self->m_obj == nullptr) {
        PyErr_SetString(PyExc_AssertionError,"nullptr MessageElement in MessageElement.getattr");
        return nullptr;
    }
#endif // NDEBUG
    char * name = PyUnicode_AsUTF8(oname);
    if (self->m_obj->isMap()) {
        const MapType & omap = self->m_obj->asMap();
        MapType::const_iterator I = omap.find(name);
        if (I != omap.end()) {
            return MessageElement_asPyObject(I->second);
        }
    }
    return PyObject_GenericGetAttr((PyObject *)self, oname);
}

static int Message_setattro(PyMessage *self, PyObject *oname, PyObject *v)
{
#ifndef NDEBUG
    if (self->m_obj == nullptr) {
        PyErr_SetString(PyExc_AssertionError,"nullptr MessageElement in MessageElement.setattr");
        return -1;
    }
#endif // NDEBUG
    char * name = PyUnicode_AsUTF8(oname);
    log(WARNING, String::compose("Setting \"%1\" attribute on an Atlas Message",
                                 name));
    if (self->m_obj->isMap()) {
        MapType & omap = self->m_obj->asMap();
        Element v_obj;
        if (PyObject_asMessageElement(v, v_obj) == 0) {
            omap[name] = v_obj;
            return 0;
        }
        PyErr_SetString(PyExc_TypeError, "object cannot be converted to Atlas data in MessageElement.setattr");
        return -1;
    }
    PyErr_SetString(PyExc_AttributeError, "Cannot set attribute on non-map in MessageElement.setattr");
    return -1;
}

PyObject * Message_richcompare(PyMessage * self, PyObject * other, int op)
{
#ifndef NDEBUG
    if (self->m_obj == nullptr) {
        PyErr_SetString(PyExc_AssertionError,
                        "nullptr MessageElement in MessageElement.richcompare");
        return 0;
    }
#endif // NDEBUG
    bool equal = false;
    if ((op != Py_EQ) && (op != Py_NE)) {
        PyErr_SetString(PyExc_TypeError,
                        "MessageElement object can only be check for == or !=");
        return 0;
    }
    if (self->m_obj->isString()) {
        if (PyUnicode_Check(other) &&
            self->m_obj->asString() == PyUnicode_AsUTF8(other)) {
            equal = true;
        }
    } else if (self->m_obj->isInt()) {
        if (PyLong_Check(other) &&
            self->m_obj->asInt() == PyLong_AsLong(other)) {
            equal = true;
        }
    } else if (self->m_obj->isFloat()) {
        if (PyFloat_Check(other)
            && self->m_obj->asFloat() == PyFloat_AsDouble(other)) {
            equal = true;
        }
    }

    if ((equal && op == Py_EQ) || (!equal && op == Py_NE)) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    Py_INCREF(Py_False);
    return Py_False;
}

static int Message_init(PyMessage * self, PyObject * args, PyObject * kwds)
{
    PyObject * arg = 0;
    if (!PyArg_ParseTuple(args, "|O", &arg)) {
        return -1;
    }
    self->m_obj = new Element();
    if (arg == 0) {
        return 0;
    }
    if (PyObject_asMessageElement(arg, *self->m_obj) != 0) {
        PyErr_SetString(PyExc_TypeError, "Message must take Atlas data");
        return -1;
    }
    return 0;
}

PyTypeObject PyMessage_Type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
        "atlas.Message",                /*tp_name*/
        sizeof(PyMessage),       /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        (destructor)Message_dealloc,    /*tp_dealloc*/
        0,                              /*tp_print*/
        0,                              /*tp_getattr*/
        0,                              /*tp_setattr*/
        0,                              /*tp_compare*/
        (reprfunc)Message_repr,         /*tp_repr*/
        0,                              /*tp_as_number*/
        0,                              /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
        0,                              // tp_call
        0,                              // tp_str
        (getattrofunc)Message_getattro, // tp_getattro
        (setattrofunc)Message_setattro, // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT,             // tp_flags
        "Message objects",              // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        (richcmpfunc)Message_richcompare,// tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        Message_methods,                // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        0,                              // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)Message_init,         // tp_init
        0,                              // tp_alloc
        0,                              // tp_new
};

/*
 * Beginning of Object creation functions section.
 */

PyMessage * newPyMessage()
{
    return (PyMessage *)PyMessage_Type.tp_new(&PyMessage_Type, 0, 0);
}

/*
 * Utility functions to munge between Object related types and python types
 */

static PyObject * MapType_asPyObject(const MapType & map)
{
    PyObject * args_pydict = PyDict_New();
    PyMessage * item;
    MapType::const_iterator Iend = map.end();
    for (MapType::const_iterator I = map.begin(); I != Iend; ++I) {
        const std::string & key = I->first;
        item = newPyMessage();
        if (item == nullptr) {
            PyErr_SetString(PyExc_MemoryError,"error creating map");
            return nullptr;
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
    PyMessage * item;
    ListType::const_iterator Iend = list.end();
    for (ListType::const_iterator I = list.begin(); I != Iend; ++I, ++j) {
        item = newPyMessage();
        if (item == nullptr) {
            PyErr_SetString(PyExc_MemoryError,"error creating list");
            return nullptr;
        }
        item->m_obj = new Element(*I);
        // PyList_SetItem() eats the reference passed to it
        PyList_SetItem(args_pylist, j, (PyObject *)item);
    }
    return args_pylist;
}

PyObject * MessageElement_asPyObject(const Element & obj)
{
    PyObject * ret = nullptr;
    switch (obj.getType()) {
        case Element::TYPE_INT:
            ret = PyLong_FromLong(obj.Int());
            break;
        case Element::TYPE_FLOAT:
            ret = PyFloat_FromDouble(obj.Float());
            break;
        case Element::TYPE_STRING:
            ret = PyUnicode_FromString(obj.String().c_str());
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

int PyListObject_asElement(PyObject * list, ListType & res)
{
    PyMessage * item;
    int len = PyList_Size(list);
    for(int i = 0; i < len; i++) {
        item = (PyMessage *)PyList_GetItem(list, i);
        if (PyMessage_Check(item)) {
            res.push_back(*(item->m_obj));
        } else {
            Element o;
            if (PyObject_asMessageElement((PyObject*)item, o) == 0) {
                res.push_back(o);
            } else {
                debug( std::cout << "Python to atlas conversion failed on element " << i << " of list" << std::endl << std::flush; );
                return -1;
            }
        }
    }
    return 0;
}

int PyDictObject_asElement(PyObject * dict, MapType & res)
{
    PyMessage * item;
    PyObject * keys = PyDict_Keys(dict);
    PyObject * vals = PyDict_Values(dict);
    for(int i = 0; i < PyDict_Size(dict); i++) {
        PyObject * key = PyList_GetItem(keys, i);
        item = (PyMessage *)PyList_GetItem(vals, i);
        if (PyMessage_Check(item)) {
            res[PyUnicode_AsUTF8(key)] = *(item->m_obj);
        } else {
            if (PyObject_asMessageElement((PyObject*)item, res[PyUnicode_AsUTF8(key)]) != 0) {
                debug( std::cout << "Python to atlas conversion failed on element " << PyUnicode_AsUTF8(key) << " of map" << std::endl << std::flush; );
                return -1;
            }
        }
    }
    Py_DECREF(keys);
    Py_DECREF(vals);
    return 0;
}

int PyObject_asMessageElement(PyObject * o, Element & res, bool simple)
{
    if (PyLong_Check(o)) {
        res = (int)PyLong_AsLong(o);
        return 0;
    }
    if (PyFloat_Check(o)) {
        res = PyFloat_AsDouble(o);
        return 0;
    }
    if (PyUnicode_Check(o)) {
        res = PyUnicode_AsUTF8(o);
        return 0;
    }
    // If the caller has specified that it is not interested in
    // map or list results, we should just return now.
    if (simple) {
        return -1;
    }
    if (PyList_Check(o)) {
        res = ListType();
        return PyListObject_asElement(o, res.List());
    }
    if (PyDict_Check(o)) {
        res = MapType();
        return PyDictObject_asElement(o, res.Map());
    }
    if (PyTuple_Check(o)) {
        ListType list;
        int i, size = PyTuple_Size(o);
        for(i = 0; i < size; i++) {
            Element item;
            if (PyObject_asMessageElement(PyTuple_GetItem(o, i), item) == 0) {
                list.push_back(item);
            } else {
                debug( std::cout << "Python to atlas conversion failed on element " << i << " of tuple" << std::endl << std::flush; );
                return -1;
            }
        }
        res = list;
        return 0;
    }
    if (PyMessage_Check(o)) {
        PyMessage * obj = (PyMessage *)o;
        res = *(obj->m_obj);
        return 0;
    }
    if (PyOperation_Check(o)) {
        PyOperation * op = (PyOperation *)o;
        res = op->operation->asMessage();
        return 0;
    }
    if (PyOplist_Check(o)) {
        PyOplist * opl = (PyOplist *)o;
        res = ListType();
        ListType & entlist = res.asList();
        const OpVector & ops = *opl->ops;
        OpVector::const_iterator Iend = ops.end();
        for (OpVector::const_iterator I = ops.begin(); I != Iend; ++I) {
            entlist.push_back((*I)->asMessage());
        }
        return 0;
    }
    if (PyLocation_Check(o)) {
        PyLocation * loc = (PyLocation *)o;
        MapType _map;
        loc->location->addToMessage(_map);
        res = _map;
        return 0;
    }
    return -1;
}
