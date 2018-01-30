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


#include "Py_BBox.h"

#include "Py_Vector3D.h"
#include "Py_Point3D.h"
#include "Py_Message.h"

#include <Atlas/Message/Element.h>

static PyObject * BBox_sqr_bounding_radius(PyBBox * self)
{
    float square_radius = 0;
    if (self->box.isValid()) {
        square_radius = boxSquareBoundingRadius(self->box);
    }
    return PyFloat_FromDouble(square_radius);
}

static PyObject * BBox_sqr_horizontal_bounding_radius(PyBBox * self)
{
    float square_radius = 0;
    if (self->box.isValid()) {
        square_radius = boxSquareHorizontalBoundingRadius(self->box);
    }
    return PyFloat_FromDouble(square_radius);
}


static PyObject * BBox_as_sequence(PyBBox * self)
{
    PyObject * res = PyList_New(6);
    const BBox & b = self->box;
    if (res != nullptr) {
        PyList_SetItem(res, 0, PyFloat_FromDouble(b.lowCorner().x()));
        PyList_SetItem(res, 1, PyFloat_FromDouble(b.lowCorner().y()));
        PyList_SetItem(res, 2, PyFloat_FromDouble(b.lowCorner().z()));
        PyList_SetItem(res, 3, PyFloat_FromDouble(b.highCorner().x()));
        PyList_SetItem(res, 4, PyFloat_FromDouble(b.highCorner().y()));
        PyList_SetItem(res, 5, PyFloat_FromDouble(b.highCorner().z()));
    }
    return res;
}

static PyMethodDef BBox_methods[] = {
    {"square_bounding_radius", (PyCFunction)BBox_sqr_bounding_radius, METH_NOARGS},
    {"square_horizontal_bounding_radius", (PyCFunction)BBox_sqr_horizontal_bounding_radius, METH_NOARGS},
    {"as_sequence",            (PyCFunction)BBox_as_sequence,     METH_NOARGS},
    {nullptr, nullptr}  // sentinel
};

static void BBox_dealloc(PyBBox * self)
{
    self->box.~BBox();
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject * BBox_getattro(PyBBox *self, PyObject *oname)
{
    char * name = PyString_AsString(oname);
    if (strcmp(name, "near_point") == 0) {
        PyPoint3D * v = newPyPoint3D();
        if (v != nullptr) {
            v->coords = self->box.lowCorner();
        }
        return (PyObject *)v;
    }
    if (strcmp(name, "far_point") == 0) {
        PyPoint3D * v = newPyPoint3D();
        if (v != nullptr) {
            v->coords = self->box.highCorner();
        }
        return (PyObject *)v;
    }

    return PyObject_GenericGetAttr((PyObject *)self, oname);
}

static int BBox_setattro(PyBBox *self, PyObject *oname, PyObject *v)
{
    char * name = PyString_AsString(oname);
    if (!PyPoint3D_Check(v)) {
        PyErr_SetString(PyExc_TypeError, "BBox setattr must take a Point");
        return -1;
    }
    PyPoint3D * pt = (PyPoint3D *)v;
    if (!pt->coords.isValid()) {
        PyErr_SetString(PyExc_ValueError, "BBox setattr must take a valid Point");
        return -1;
    }
    const Point3D & point = pt->coords;
    if (strcmp(name, "near_point") == 0) {
        self->box.lowCorner() = point;
        return 0;
    } else if (strcmp(name, "far_point") == 0) {
        self->box.highCorner()= point;
        return 0;
    }

    PyErr_SetString(PyExc_AttributeError, "unknown attribute");
    return -1;
}

static int BBox_compare(PyBBox * self, PyBBox * other)
{
    if (self->box == other->box) {
        return 0;
    }
    return 1;
}

static int BBox_init(PyBBox * self, PyObject * args, PyObject * kwds)
{
    std::vector<float> val;

    PyObject * clist;
    int tuple_size = PyTuple_Size(args);
    int clist_size;
    switch(tuple_size) {
        case 0:
            break;
        case 1:
            clist = PyTuple_GetItem(args, 0);
            if (!PyList_Check(clist)) {
                PyErr_SetString(PyExc_TypeError, "BBox() from single value must a list 3 or 6 long");
                return -1;
            }
            clist_size = PyList_Size(clist);
            if (clist_size != 3 && clist_size != 6) {
                PyErr_SetString(PyExc_ValueError, "BBox() from single value must a list 3 or 6 long");
                return -1;
            }
            val.resize(clist_size);
            for(int i = 0; i < clist_size; i++) {
                PyObject * item = PyList_GetItem(clist, i);
                if (PyInt_Check(item)) {
                    val[i] = (float)PyInt_AsLong(item);
                } else if (PyFloat_Check(item)) {
                    val[i] = PyFloat_AsDouble(item);
                } else if (PyMessage_Check(item)) {
                    PyMessage * mitem = (PyMessage*)item;
                    if (!mitem->m_obj->isNum()) {
                        PyErr_SetString(PyExc_TypeError, "BBox() must take list of floats, or ints");
                        return -1;
                    }
                    val[i] = mitem->m_obj->asNum();
                } else {
                    PyErr_SetString(PyExc_TypeError, "BBox() must take list of floats, or ints");
                    return -1;
                }
            }
            break;
        case 3:
        case 6:
            val.resize(tuple_size);
            for(int i = 0; i < tuple_size; i++) {
                PyObject * item = PyTuple_GetItem(args, i);
                if (PyInt_Check(item)) {
                    val[i] = (float)PyInt_AsLong(item);
                } else if (PyFloat_Check(item)) {
                    val[i] = PyFloat_AsDouble(item);
                } else {
                    PyErr_SetString(PyExc_TypeError, "BBox() must take list of floats, or ints");
                    return -1;
                }
            }
            break;
        default:
            PyErr_SetString(PyExc_TypeError, "Point3D must take list of floats, or ints, 3 ints or 3 floats");
            return -1;
            break;
    }
    if (val.size() == 3) {
        self->box = BBox(WFMath::Point<3>(0.f, 0.f, 0.f),
                         WFMath::Point<3>(val[0], val[1], val[2]));
    } else if (val.size() == 6) {
        self->box = BBox(WFMath::Point<3>(val[0], val[1], val[2]),
                         WFMath::Point<3>(val[3], val[4], val[5]));
    }
    return 0;
}

static PyObject * BBox_new(PyTypeObject * type, PyObject *, PyObject *)
{
    // This looks allot like the default implementation, except we call the
    // in-place constructor.
    PyBBox * self = (PyBBox *)type->tp_alloc(type, 0);
    if (self != nullptr) {
        new (&(self->box)) BBox();
    }
    return (PyObject *)self;
}

PyTypeObject PyBBox_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /*ob_size*/
        "BBox",                         /*tp_name*/
        sizeof(PyBBox),             /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        (destructor)BBox_dealloc,       /*tp_dealloc*/
        0,                              /*tp_print*/
        0,                              /*tp_getattr*/
        0,                              /*tp_setattr*/
        (cmpfunc)BBox_compare,          /*tp_compare*/
        0,                              /*tp_repr*/
        0,                              /*tp_as_number*/
        0,                              /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
        0,                              // tp_call
        0,                              // tp_str
        (getattrofunc)BBox_getattro,    // tp_getattro
        (setattrofunc)BBox_setattro,    // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT,             // tp_flags
        "BBox objects",                 // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        0,                              // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        BBox_methods,                   // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        0,                              // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)BBox_init,            // tp_init
        0,                              // tp_alloc
        BBox_new,                       // tp_new
};

PyBBox * newPyBBox()
{
    return (PyBBox *)PyBBox_Type.tp_new(&PyBBox_Type, 0, 0);
}
