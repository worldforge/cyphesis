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

// $Id$

#include "Py_Property.h"

#include "Py_Point3D.h"
#include "Py_Thing.h"
#include "Py_Vector3D.h"

#include "Entity.h"
#include "TerrainProperty.h"

static PyObject * TerrainProperty_getHeight(PyProperty * self,
                                            PyObject * args)
{
#ifndef NDEBUG
    if (self->m_entity == NULL || self->m_p.terrain == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in TerrainProperty.getHeight");
        return NULL;
    }
#endif // NDEBUG
    float x, y;
    if (!PyArg_ParseTuple(args, "ff", &x, &y)) {
        return NULL;
    }
    // Return a sensible default.
    Vector3D normal(0,0,1);
    float h = 0;
    self->m_p.terrain->getHeightAndNormal(x, y, h, normal);
    return PyFloat_FromDouble(h);
}

static PyObject * TerrainProperty_getSurface(PyProperty * self,
                                             PyObject * args)
{
#ifndef NDEBUG
    if (self->m_entity == NULL || self->m_p.terrain == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in TerrainProperty.getSurface");
        return NULL;
    }
#endif // NDEBUG
    // FIXME Inconsistent interface. Should take the same args as others.
    // If it really should take a point, METH_O would work better.
    PyPoint3D * pos;
    if (!PyArg_ParseTuple(args, "O", &pos)) {
        return NULL;
    }
    if (!PyPoint3D_Check(pos)) {
        PyErr_SetString(PyExc_TypeError, "Position for surface must be Point3D");
        return NULL;
    }
    int surface;
    if (self->m_p.terrain->getSurface(pos->coords, surface) != 0) {
        PyErr_SetString(PyExc_TypeError, "How the hell should I know");
        return NULL;
    }
    return PyInt_FromLong(surface);
}

static PyObject * TerrainProperty_getNormal(PyProperty * self,
                                            PyObject * args)
{
#ifndef NDEBUG
    if (self->m_entity == NULL || self->m_p.terrain == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in TerrainProperty.getNormal");
        return NULL;
    }
#endif
    float x,y;
    if (!PyArg_ParseTuple(args, "ff", &x, &y)) {
        return NULL;
    }
    // Return a sensible default.
    Vector3D normal(0,0,1);
    float h = 0;
    self->m_p.terrain->getHeightAndNormal(x, y, h, normal);
    PyVector3D * ret = newPyVector3D();
    if (ret != NULL) {
        ret->coords = normal;
    }
    return (PyObject *)ret;
}

static PyObject * TerrainProperty_findMods(PyProperty * self,
                                          PyObject * other)
{
    if (!PyPoint3D_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Argument must be terrain pos");
        return NULL;
    }
    std::vector<LocatedEntity *> result;
    self->m_p.terrain->findMods(((PyPoint3D*)other)->coords, result);
    PyObject * ret = PyTuple_New(result.size());
    std::vector<LocatedEntity *>::const_iterator I = result.begin();
    std::vector<LocatedEntity *>::const_iterator Iend = result.end();
    for (int i = 0; I != Iend; ++I, ++i) {
        PyTuple_SetItem(ret, i, wrapEntity(*I));
    }
    return ret;
}

static PyMethodDef TerrainProperty_methods[] = {
    {"get_height",   (PyCFunction)TerrainProperty_getHeight,     METH_VARARGS},
    {"get_surface",  (PyCFunction)TerrainProperty_getSurface,    METH_VARARGS},
    {"get_normal",   (PyCFunction)TerrainProperty_getNormal,	 METH_VARARGS},
    {"find_mods",    (PyCFunction)TerrainProperty_findMods, METH_O},
    {NULL,           NULL}           /* sentinel */
};

static int TerrainProperty_init(PyProperty * self,
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
        sizeof(PyProperty),                               // tp_basicsize
        0,                                                // tp_itemsize
        // methods 
        0,                                                // tp_dealloc
        0,                                                // tp_print
        0,                                                // tp_getattr
        0,                                                // tp_setattr
        0,                                                // tp_compare
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
        TerrainProperty_methods,                          // tp_methods
        0,                                                // tp_members
        0,                                                // tp_getset
        0,                                                // tp_base
        0,                                                // tp_dict
        0,                                                // tp_descr_get
        0,                                                // tp_descr_set
        0,                                                // tp_dictoffset
        (initproc)TerrainProperty_init,                   // tp_init
        0,                                                // tp_alloc
        0,                                                // tp_new
};

PyProperty * newPyTerrainProperty()
{
    return (PyProperty *)PyTerrainProperty_Type.tp_new(&PyTerrainProperty_Type, 0, 0);
}
