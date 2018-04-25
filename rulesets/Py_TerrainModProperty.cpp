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


#include "Py_Property.h"

#include "Py_Message.h"
#include "Py_Point3D.h"
#include "Py_Shape.h"
#include "Py_Thing.h"

#include "rulesets/TerrainModProperty.h"

#include "common/log.h"

#include "physics/Shape.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;

static PyObject * TerrainModProperty_getattro(PyProperty *self,
                                              PyObject * oname)
{
#ifndef NDEBUG
    if (self->m_entity == nullptr || self->m_p.terrainmod == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr entity in TerrainModProperty.getattr");
        return nullptr;
    }
#endif // NDEBUG
    char * name = PyString_AsString(oname);
    Element val;
    if (self->m_p.terrainmod->getAttr(name, val) == 0) {
        if (strcmp(name, "shape") == 0 && val.isMap()) {
            Shape * shape = Shape::newFromAtlas(val.Map());
            if (shape != 0) {
                PyShape * wrapper = wrapShape(shape);
                return (PyObject*)wrapper;
            }
        }
        return MessageElement_asPyObject(val);
    }
    // It's probably not even necessary to do this as there is no method
    // table at time of writing, and no dictoffset for arbitrary attributes
    return PyObject_GenericGetAttr((PyObject *)self, oname);
}

static int TerrainModProperty_setattro(PyProperty * self,
                                       PyObject * oname,
                                       PyObject * v)
{
#ifndef NDEBUG
    if (self->m_entity == nullptr || self->m_p.terrainmod == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr entity in TerrainModProperty.setattro");
        return -1;
    }
#endif // NDEBUG
    char * name = PyString_AsString(oname);
    Element val;
    if (self->m_p.terrainmod->getAttr(name, val) == 0) {
        Element e;
        if (PyObject_asMessageElement(v, e, true) == 0) {
            self->m_p.terrainmod->setAttr(name, e);
            self->m_p.terrainmod->addFlags(flag_unsent);
            return 0;
        } else if (PyShape_Check(v)) {
            PyShape * ps = (PyShape*)v;
            MapType map;
            ps->shape.s->toAtlas(map);
            self->m_p.terrainmod->setAttr(name, map);
            self->m_p.terrainmod->addFlags(flag_unsent);
            return 0;
        } else {
            log(WARNING, "Cannot convert value.");
        }
    }
    PyErr_SetString(PyExc_AttributeError, "unknown attribute");
    return -1;
}

static int TerrainModProperty_init(PyProperty * self,
                                   PyObject * args,
                                   PyObject * kwd)
{
    if (!PyArg_ParseTuple(args, "")) {
        return -1;
    }

    return 0;
}

PyTypeObject PyTerrainModProperty_Type = {
        PyObject_HEAD_INIT(nullptr)
        0,                                                // ob_size
        "TerrainModProperty",                             // tp_name
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
        (getattrofunc)TerrainModProperty_getattro,        // tp_getattro
        (setattrofunc)TerrainModProperty_setattro,        // tp_setattro
        0,                                                // tp_as_buffer
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,         // tp_flags
        "TerrainModProperty objects",                     // tp_doc
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
        (initproc)TerrainModProperty_init,                // tp_init
        0,                                                // tp_alloc
        0,                                                // tp_new
};

PyProperty * newPyTerrainModProperty()
{
    return (PyProperty *)PyTerrainModProperty_Type.tp_new(&PyTerrainModProperty_Type, 0, 0);
}
