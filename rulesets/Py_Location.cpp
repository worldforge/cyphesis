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

#include "Py_Location.h"
#include "Py_Thing.h"
#include "Py_Vector3D.h"
#include "Py_Point3D.h"
#include "Py_Quaternion.h"
#include "Py_BBox.h"
#include "Py_World.h"

#include "BaseMind.h"

#include "common/BaseWorld.h"

#include <sstream>

static PyObject * Location_copy(PyLocation *self)
{
#ifndef NDEBUG
    if (self->location == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Location in Location.copy");
        return NULL;
    }
#endif // NDEBUG
    PyLocation * ret = newPyLocation();
    if (ret != NULL) {
        ret->location = new Location(self->location->m_loc,
                                     self->location->pos(),
                                     self->location->velocity());
    }
    return (PyObject *)ret;
}

static PyMethodDef Location_methods[] = {
    {"copy",            (PyCFunction)Location_copy,     METH_NOARGS},
    {NULL,              NULL}           /* sentinel */
};

static void Location_dealloc(PyLocation *self)
{
    if (self->owner == 0 && self->location != NULL) {
        delete self->location;
    }
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject * Location_getattro(PyLocation *self, PyObject *oname)
{
#ifndef NDEBUG
    if (self->location == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Location in Location.getattr");
        return NULL;
    }
#endif // NDEBUG
    char * name = PyString_AsString(oname);
    if (strcmp(name, "parent") == 0) {
        if (self->location->m_loc == NULL) {
            Py_INCREF(Py_None);
            return Py_None;
        }
        PyObject * wrapper = wrapEntity(self->location->m_loc);
        if (wrapper == NULL) {
            return NULL;
        }
        PyObject * wrapper_proxy = PyWeakref_NewProxy(wrapper, NULL);
        // FIXME Have wrapEntity return a borrowed reference
        Py_DECREF(wrapper);
        return wrapper_proxy;
    }
    if (strcmp(name, "coordinates") == 0) {
        PyPoint3D * v = newPyPoint3D();
        if (v != NULL) {
            v->coords = self->location->pos();
        }
        return (PyObject *)v;
    }
    if (strcmp(name, "velocity") == 0) {
        PyVector3D * v = newPyVector3D();
        if (v != NULL) {
            v->coords = self->location->velocity();
        }
        return (PyObject *)v;
    }
    if (strcmp(name, "orientation") == 0) {
        PyQuaternion * v = newPyQuaternion();
        if (v != NULL) {
            v->rotation = self->location->orientation();
        }
        return (PyObject *)v;
    }
    if (strcmp(name, "bbox") == 0) {
        PyBBox * b = newPyBBox();
        if (b != NULL) {
            b->box = self->location->bBox();
        }
        return (PyObject *)b;
    }
    return PyObject_GenericGetAttr((PyObject *)self, oname);
}

static int Location_setattro(PyLocation *self, PyObject *oname, PyObject *v)
{
#ifndef NDEBUG
    if (self->location == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Location in Location.setattr");
        return -1;
    }
#endif // NDEBUG
    char * name = PyString_AsString(oname);
    if (strcmp(name, "parent") == 0) {
        // FIXME Support for weakrefs
        if (!PyLocatedEntity_Check(v)) {
            PyErr_SetString(PyExc_TypeError, "parent must be an entity");
            return -1;
        }
        PyEntity * thing = (PyEntity *)v;
#ifndef NDEBUG
        if (thing->m_entity.l == NULL) {
            PyErr_SetString(PyExc_AssertionError, "invalid thing");
            return -1;
        }
#endif // NDEBUG
        self->location->m_loc = thing->m_entity.l;
        return 0;
    }
    if (strcmp(name, "bbox") == 0 && PyBBox_Check(v)) {
        PyBBox * box = (PyBBox *)v;
        self->location->setBBox(box->box);
        return 0;
    }
    if (strcmp(name, "orientation") == 0 && PyQuaternion_Check(v)) {
        PyQuaternion * quat = (PyQuaternion *)v;
        self->location->m_orientation = quat->rotation;
        return 0;
    }
    Vector3D vector;
    if (PyVector3D_Check(v)) {
        PyVector3D * vec = (PyVector3D *)v;
        vector = vec->coords;
    } else if (PyPoint3D_Check(v)) {
        PyPoint3D * p = (PyPoint3D *)v;
        vector = Vector3D(p->coords.x(), p->coords.y(), p->coords.z());
        vector.setValid(p->coords.isValid());
    } else if (PyTuple_Check(v)) {
        if (PyTuple_Size(v) != 3) {
            PyErr_SetString(PyExc_ValueError, "value must be sequence of 3");
            return -1;
        }
        for(int i = 0; i < 3; i++) {
            PyObject * item = PyTuple_GetItem(v, i);
            if (PyInt_Check(item)) {
                vector[i] = (double)PyInt_AsLong(item);
            } else if (PyFloat_Check(item)) {
                vector[i] = PyFloat_AsDouble(item);
            } else {
                PyErr_SetString(PyExc_TypeError,
                                "value must be tuple of floats, or ints");
                return -1;
            }
        }
        vector.setValid();
    } else if (PyList_Check(v)) {
        if (PyList_Size(v) != 3) {
            PyErr_SetString(PyExc_ValueError, "value must be sequence of 3");
            return -1;
        }
        for(int i = 0; i < 3; i++) {
            PyObject * item = PyList_GetItem(v, i);
            if (PyInt_Check(item)) {
                vector[i] = (double)PyInt_AsLong(item);
            } else if (PyFloat_Check(item)) {
                vector[i] = PyFloat_AsDouble(item);
            } else {
                PyErr_SetString(PyExc_TypeError,
                                "value must be list of floats, or ints");
                return -1;
            }
        }
        vector.setValid();
    } else {
        PyErr_SetString(PyExc_TypeError, "value must be a vector");
        return -1;
    }
    if (strcmp(name, "coordinates") == 0) {
        self->location->m_pos = Point3D(vector.x(), vector.y(), vector.z());
        return 0;
    }
    if (strcmp(name, "velocity") == 0) {
        self->location->m_velocity = vector;
        return 0;
    }
    if (strcmp(name, "bbox") == 0) {
        self->location->setBBox(BBox(WFMath::Point<3>(0.f,0.f,0.f),
                                     WFMath::Point<3>(vector.x(),
                                                      vector.y(),
                                                      vector.z())));
        return 0;
    }
    PyErr_SetString(PyExc_AttributeError, "unknown attribute");
    return -1;
}

static PyObject * Location_repr(PyLocation *self)
{
    std::stringstream r;
    r << *self->location;
    return PyString_FromString(r.str().c_str());
}

static int Location_init(PyLocation * self, PyObject * args, PyObject * kwds)
{
    // We need to deal with actual args here
    PyObject * refO = NULL;
    PyPoint3D * coords = NULL;
    LocatedEntity * ref_ent = NULL;
    if (!PyArg_ParseTuple(args, "|OO", &refO, &coords)) {
        return -1;
    }
    if (refO != NULL) {
        if (PyWeakref_CheckProxy(refO)) {
            refO = PyWeakref_GET_OBJECT(refO);
        }
        if (!PyLocatedEntity_Check(refO) && !PyWorld_Check(refO)) {
            // FIXME This is odd. Should be wrapped into the above check
            if (!PyObject_IsInstance(refO, (PyObject *)&PyEntity_Type)) {
                PyErr_SetString(PyExc_TypeError, "Arg ref required");
                return -1;
            }
        }
        if (coords != NULL && !PyPoint3D_Check(coords)) {
            PyErr_SetString(PyExc_TypeError, "Arg coords required");
            return -1;
        }
    
        if (PyWorld_Check(refO)) {
            ref_ent = &BaseWorld::instance().m_gameWorld;
        } else {
            PyEntity * ref = (PyEntity*)refO;
#ifndef NDEBUG
            if (ref->m_entity.l == NULL) {
                PyErr_SetString(PyExc_AssertionError, "Parent thing is invalid");
                return -1;
            }
#endif // NDEBUG
            ref_ent = ref->m_entity.l;
        }
    }
    if (coords == NULL) {
        self->location = new Location(ref_ent);
    } else {
        self->location = new Location(ref_ent, coords->coords);
    }
    return 0;
}

static PyObject * Location_new(PyTypeObject * type, PyObject *, PyObject *)
{
    // This looks allot like the default implementation, except we call the
    // in-place constructor.
    PyLocation * self = (PyLocation *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->location = NULL;
        self->owner = 0;
    }
    return (PyObject *)self;
}

static PyObject * Location_subtract(PyLocation * lhs, PyLocation * rhs)
{
    if (!PyLocation_CheckExact(rhs)) {
        PyErr_SetString(PyExc_TypeError, "Location must subtract Location");
        return 0;
    }
    PyVector3D * v = newPyVector3D();
    if (v != NULL) {
        v->coords = distanceTo(*rhs->location, *lhs->location);
    }
    return (PyObject*)v;
}

static PyNumberMethods Location_as_number = {
    0,                                        // nb_add;
    (binaryfunc)Location_subtract,            // nb_subtract;
    0,                                        // nb_multiply;
    0,                                        // nb_divide;
    0,                                        // nb_remainder;
    0,                                        // nb_divmod;
    0,                                        // nb_power;
    0,                                        // nb_negative;
    0,                                        // nb_positive;
    0,                                        // nb_absolute;
    0,                                        // nb_nonzero;
    0,                                        // nb_invert;
    0,                                        // nb_lshift;
    0,                                        // nb_rshift;
    0,                                        // nb_and;
    0,                                        // nb_xor;
    0,                                        // nb_or;
    0,                                        // nb_coerce;
    0,                                        // nb_int;
    0,                                        // nb_long;
    0,                                        // nb_float;
    0,                                        // nb_oct;
    0,                                        // nb_hex;
    /* Added in release 2.0 */
    0,                                        // nb_inplace_add;
    0,                                        // nb_inplace_subtract;
    0,                                        // nb_inplace_multiply;
    0,                                        // nb_inplace_divide;
    0,                                        // nb_inplace_remainder;
    0,                                        // nb_inplace_power;
    0,                                        // nb_inplace_lshift;
    0,                                        // nb_inplace_rshift;
    0,                                        // nb_inplace_and;
    0,                                        // nb_inplace_xor;
    0,                                        // nb_inplace_or;
};

PyTypeObject PyLocation_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /*ob_size*/
        "atlas.Location",               /*tp_name*/
        sizeof(PyLocation),             /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        (destructor)Location_dealloc,   /*tp_dealloc*/
        0,                              /*tp_print*/
        0,                              /*tp_getattr*/
        0,                              /*tp_setattr*/
        0,                              /*tp_compare*/
        (reprfunc)Location_repr,        /*tp_repr*/
        &Location_as_number,            /*tp_as_number*/
        0,                              /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
        0,                              // tp_call
        0,                              // tp_str
        (getattrofunc)Location_getattro,// tp_getattro
        (setattrofunc)Location_setattro,// tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT,             // tp_flags
        "Location objects",             // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        0,                              // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        Location_methods,               // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        0,                              // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)Location_init,        // tp_init
        0,                              // tp_alloc
        Location_new,                   // tp_new
};

PyLocation * newPyLocation()
{
    return (PyLocation *)PyLocation_Type.tp_new(&PyLocation_Type, 0, 0);
}
