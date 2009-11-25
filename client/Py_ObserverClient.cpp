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

#include "Py_ObserverClient.h"
#include "Py_CreatorClient.h"

#include "ObserverClient.h"

#include "common/debug.h"
#include "common/id.h"
#include "common/log.h"
#include "common/TypeNode.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;

static const bool debug_flag = false;

static PyObject * ObserverClient_setup(PyObserverClient * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_client == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL ObserverClient in ObserverClient.set");
        return NULL;
    }
#endif // NDEBUG
    char * username = NULL;
    char * password = NULL;
    if (!PyArg_ParseTuple(args, "ss", &username, &password)) {
        return NULL;
    }
    self->m_client->setup(username, password);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef ObserverClient_methods[] = {
        {"setup",          (PyCFunction)ObserverClient_setup,     METH_VARARGS},
        {NULL,          NULL}           /* sentinel */
};

static void ObserverClient_dealloc(PyObserverClient *self)
{
    if (self->m_client != NULL) {
        delete self->m_client;
    }
    PyMem_DEL(self);
}

static PyObject * ObserverClient_getattr(PyObserverClient *self, char *name)
{
    // Fairly major re-write of this to use operator[] of ObserverClient base class
#ifndef NDEBUG
    if (self->m_client == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL ObserverClient in ObserverClient.getattr");
        return NULL;
    }
#endif // NDEBUG
    if (strcmp(name, "character") == 0) {
        if (self->m_client->character() == 0) {
            Py_INCREF(Py_None);
            return Py_None;
        }
        PyCreatorClient * pcc = newPyCreatorClient();
        pcc->m_mind = self->m_client->character();
        return (PyObject*)pcc;
    }
    return Py_FindMethod(ObserverClient_methods, (PyObject *)self, name);
}

static int ObserverClient_setattr(PyObserverClient *self, char *name, PyObject *v)
{
    if (strcmp(name, "server") == 0 && PyString_Check(v)) {
        self->m_client->setServer(PyString_AsString(v));
        return 0;
    }
    return -1;
}

static int ObserverClient_compare(PyObserverClient *self, PyObserverClient *other)
{
    if (self->m_client == NULL || other->m_client == NULL) {
        return -1;
    }
    return (self->m_client == other->m_client) ? 0 : 1;
}

static int ObserverClient_init(PyObserverClient * self,
                              PyObject * args, PyObject * kwds)
{
    if (!PyArg_ParseTuple(args, "")) {
        return -1;
    }
    self->m_client = new ObserverClient();
    return 0;
}

PyTypeObject PyObserverClient_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /*ob_size*/
        "ObserverClient",                       /*tp_name*/
        sizeof(PyObserverClient),               /*tp_basicsize*/
        0,                                      /*tp_itemsize*/
        /* methods */
        (destructor)ObserverClient_dealloc,     /*tp_dealloc*/
        0,                                      /*tp_print*/
        (getattrfunc)ObserverClient_getattr,    /*tp_getattr*/
        (setattrfunc)ObserverClient_setattr,    /*tp_setattr*/
        (cmpfunc)ObserverClient_compare,        /*tp_compare*/
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
        "ObserverClient objects",               // tp_doc
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
        (initproc)ObserverClient_init,          // tp_init
        0,                                      // tp_alloc
        0,                                      // tp_new
};

PyObserverClient * newPyObserverClient()
{
    return (PyObserverClient *)PyObserverClient_Type.tp_new(&PyObserverClient_Type, 0, 0);
}
