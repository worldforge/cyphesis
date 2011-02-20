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

#include "Py_Message.h"
#include "Py_Point3D.h"
#include "Py_Shape.h"
#include "Py_Thing.h"
#include "Py_Vector3D.h"

#include "Entity.h"
#include "TerrainModProperty.h"

#include "common/log.h"

#include "physics/Shape.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;

static PyMethodDef TerrainModProperty_methods[] = {
    {NULL,           NULL}           /* sentinel */
};

static void TerrainModProperty_dealloc(PyTerrainModProperty *self)
{
    self->ob_type->tp_free(self);
}

static PyObject * TerrainModProperty_getattr(PyTerrainModProperty *self, char * name)
{
#ifndef NDEBUG
    if (self->m_entity == NULL || self->m_property == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in TerrainModProperty.getattr");
        return NULL;
    }
#endif // NDEBUG
    Element val;
    if (self->m_property->getAttr(name, val) == 0) {
        if (strcmp(name, "shape") == 0 && val.isMap()) {
            log(INFO, "Getting shape.");
            Shape * shape = Shape::newFromAtlas(val.Map());
            if (shape != 0) {
                PyShape * wrapper = newPyShape();
                wrapper->shape = shape;
                return (PyObject*)wrapper;
            }
        }
        return MessageElement_asPyObject(val);
    }
    return Py_FindMethod(TerrainModProperty_methods, (PyObject *)self, name);
}

static int TerrainModProperty_setattr(PyTerrainModProperty * self,
                                      char * name,
                                      PyObject *v)
{
#ifndef NDEBUG
    if (self->m_entity == NULL || self->m_property == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL entity in TerrainModProperty.setattro");
        return -1;
    }
#endif // NDEBUG
    Element val;
    if (self->m_property->getAttr(name, val) == 0) {
        Element e;
        if (PyObject_asMessageElement(v, e, true) == 0) {
            self->m_property->setAttr(name, e);
            self->m_property->setFlags(flag_unsent);
            return 0;
        } else if (PyShape_Check(v)) {
            log(INFO, "Setting shape.");
            PyShape * ps = (PyShape*)v;
            MapType map;
            ps->shape->toAtlas(map);
            self->m_property->setAttr(name, map);
            self->m_property->setFlags(flag_unsent);
            return 0;
        } else {
            log(WARNING, "Cannot convert value.");
        }
    }
    PyErr_SetString(PyExc_AttributeError, "unknown attribute");
    return -1;
}

static int TerrainModProperty_init(PyTerrainModProperty * self,
                                   PyObject * args,
                                   PyObject * kwd)
{
    if (!PyArg_ParseTuple(args, "")) {
        return -1;
    }

    return 0;
}

PyTypeObject PyTerrainModProperty_Type = {
        PyObject_HEAD_INIT(NULL)
        0,                                                // ob_size
        "TerrainModProperty",                             // tp_name
        sizeof(PyTerrainModProperty),                     // tp_basicsize
        0,                                                // tp_itemsize
        // methods 
        (destructor)TerrainModProperty_dealloc,           // tp_dealloc
        0,                                                // tp_print
        (getattrfunc)TerrainModProperty_getattr,          // tp_getattr
        (setattrfunc)TerrainModProperty_setattr,          // tp_setattr
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

PyTerrainModProperty * newPyTerrainModProperty()
{
    return (PyTerrainModProperty *)PyTerrainModProperty_Type.tp_new(&PyTerrainModProperty_Type, 0, 0);
}
