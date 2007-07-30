// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2007 Alistair Riddoch
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

// $Id: Py_TerrainProperty.cpp,v 1.3 2007-07-30 18:12:51 alriddoch Exp $

#include "Py_Property.h"

#include "Py_Point3D.h"

#include "TerrainProperty.h"

static PyObject * TerrainProperty_getHeight(PyTerrainProperty * self,
                                            PyObject * args)
{
#ifndef NDEBUG
    if (self->m_entity == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in TerrainProperty.getattr");
        return NULL;
    }
#endif // NDEBUG
    double x, y;
    if (!PyArg_ParseTuple(args, "ff", &x, &y)) {
        return NULL;
    }
    float height = self->m_property->getHeight(x, y);
    return PyFloat_FromDouble(height);
}

static PyObject * TerrainProperty_getSurface(PyTerrainProperty * self,
                                             PyObject * args)
{
#ifndef NDEBUG
    if (self->m_entity == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in TerrainProperty.getattr");
        return NULL;
    }
#endif // NDEBUG
    PyPoint3D * pos;
    if (!PyArg_ParseTuple(args, "O", &pos)) {
        return NULL;
    }
    if (!PyPoint3D_Check(pos)) {
        PyErr_SetString(PyExc_TypeError, "Position for surface must be Point3D");
        return NULL;
    }
    int surface;
    if (self->m_property->getSurface(pos->coords, surface) != 0) {
        PyErr_SetString(PyExc_TypeError, "How the hell should I know");
        return NULL;
    }
    return PyInt_FromLong(surface);
}

static PyMethodDef TerrainProperty_methods[] = {
    {"get_height",   (PyCFunction)TerrainProperty_getHeight,     METH_VARARGS},
    {"get_surface",  (PyCFunction)TerrainProperty_getSurface,    METH_VARARGS},
    {NULL,           NULL}           /* sentinel */
};

static void TerrainProperty_dealloc(PyTerrainProperty *self)
{
    self->ob_type->tp_free(self);
}

static PyObject * TerrainProperty_getattr(PyTerrainProperty *self, char * name)
{
#ifndef NDEBUG
    if (self->m_entity == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in TerrainProperty.getattr");
        return NULL;
    }
#endif // NDEBUG
    return Py_FindMethod(TerrainProperty_methods, (PyObject *)self, name);
}

static int TerrainProperty_setattr(PyTerrainProperty * self,
                                   char * name,
                                   PyObject *v)
{
#ifndef NDEBUG
    if (self->m_entity == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in TerrainProperty.setattro");
        return -1;
    }
#endif // NDEBUG
    PyErr_SetString(PyExc_AttributeError, "unknown attribute");
    return -1;
}

static int TerrainProperty_compare(PyTerrainProperty *self, PyTerrainProperty *other)
{
    if (self->m_entity == NULL || other->m_entity == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in TerrainProperty.compare");
        return -1;
    }
    return (self->m_entity == other->m_entity) ? 0 : 1;
}

static PyObject * TerrainProperty_new(PyTypeObject * type,
                                      PyObject *,
                                      PyObject *)
{
    // This looks allot like the default implementation, except we set some
    // stuff to null.
    PyTerrainProperty * self = (PyTerrainProperty *)type->tp_alloc(type, 0);
    return (PyObject *)self;
}

static int TerrainProperty_init(PyTerrainProperty * self,
                                PyObject * args,
                                PyObject * kwd)
{
    if (!PyArg_ParseTuple(args, "")) {
        return -1;
    }

    return 0;
}

PyTypeObject PyTerrainProperty_Type = {
        PyObject_HEAD_INIT(NULL)
        0,                                                // ob_size
        "TerrainProperty",                                // tp_name
        sizeof(PyTerrainProperty),                        // tp_basicsize
        0,                                                // tp_itemsize
        // methods 
        (destructor)TerrainProperty_dealloc,              // tp_dealloc
        0,                                                // tp_print
        (getattrfunc)TerrainProperty_getattr,             // tp_getattr
        (setattrfunc)TerrainProperty_setattr,             // tp_setattr
        (cmpfunc)TerrainProperty_compare,                 // tp_compare
        0,                                                // tp_repr
        0,                                                // tp_as_number
        0,                                                // tp_as_sequence
        0,                                                // tp_as_mapping
        0,                                                // tp_hash
        0,                                                // tp_call
        0,                                                // tp_str
        0,                                                // tp_getattro
        0,                                                // tp_setattro
        0,                                                // tp_as_buffer
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,         // tp_flags
        "TerrainProperty objects",                        // tp_doc
        0,                                                // tp_travers
        0,                                                // tp_clear
        0,                                                // tp_richcompare
        0,                                                // tp_weaklistoffset
        0,                                                // tp_iter
        0,                                                // tp_iternext
        0,                                                // tp_methods
        0,                                                // tp_members
        0,                                                // tp_getset
        0,                                                // tp_base
        0,                                                // tp_dict
        0,                                                // tp_descr_get
        0,                                                // tp_descr_set
        0,                                                // tp_dictoffset
        (initproc)TerrainProperty_init,                   // tp_init
        0,                                                // tp_alloc
        TerrainProperty_new,                              // tp_new
};

PyTerrainProperty * newPyTerrainProperty()
{
    PyTerrainProperty * self;
    self = PyObject_NEW(PyTerrainProperty, &PyTerrainProperty_Type);
    if (self == NULL) {
        return NULL;
    }
    return self;
}
