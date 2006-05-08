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

#include "Py_BBox.h"
#include "Py_Vector3D.h"

#include "common/log.h"

static PyObject * BBox_sqr_bounding_radius(PyBBox * self)
{
    float square_radius = 0;
    if (self->box.isValid()) {
        square_radius = boxSquareBoundingRadius(self->box);
    }
    return PyFloat_FromDouble(square_radius);
}

static PyMethodDef BBox_methods[] = {
    {"square_bounding_radius", (PyCFunction)BBox_sqr_bounding_radius, METH_NOARGS},
    {NULL, NULL}  // sentinel
};

static void BBox_dealloc(PyBBox * self)
{
    self->box.~BBox();
    PyMem_DEL(self);
}

static PyObject * BBox_getattr(PyBBox *self, char *name)
{
    if (strcmp(name, "near_point") == 0) {
        PyVector3D * v = newPyVector3D();
        const WFMath::Point<3> & lc = self->box.lowCorner();
        v->coords = Vector3D(lc.x(), lc.y(), lc.z());
        return (PyObject *)v;
    }
    if (strcmp(name, "far_point") == 0) {
        PyVector3D * v = newPyVector3D();
        const WFMath::Point<3> & hc = self->box.highCorner();
        v->coords = Vector3D(hc.x(), hc.y(), hc.z());
        return (PyObject *)v;
    }

    return Py_FindMethod(BBox_methods, (PyObject *)self, name);
}

static int BBox_setattr(PyBBox *self, char *name, PyObject *v)
{
    if (!PyVector3D_Check(v)) {
        PyErr_SetString(PyExc_TypeError, "BBox setattr must take tuple of floats, or ints");
    }
    PyVector3D * vec = (PyVector3D *)v;
    if (!vec->coords.isValid()) {
        log(ERROR, "BBox.setattr() vector is not set.");
    }
    const Vector3D & vector = vec->coords;
    if (strcmp(name, "near_point") == 0) {
        (WFMath::Point<3>&)self->box.lowCorner() = WFMath::Point<3>(vector.x(),
                                                                    vector.y(),
                                                                    vector.z());
        return 0;
    } else if (strcmp(name, "far_point") == 0) {
        (WFMath::Point<3>&)self->box.highCorner()= WFMath::Point<3>(vector.x(),
                                                                    vector.y(),
                                                                    vector.z());
        return 0;
    }

    PyErr_SetString(PyExc_AttributeError, "unknown attribute");
    return -1;
}

static int BBox_compare(PyBBox * self, PyBBox * other)
{
    if (!PyBBox_Check(other)) {
        return -1;
    }
    if (self->box == other->box) {
        return 0;
    }
    return 1;
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
        (getattrfunc)BBox_getattr,      /*tp_getattr*/
        (setattrfunc)BBox_setattr,      /*tp_setattr*/
        (cmpfunc)BBox_compare,          /*tp_compare*/
        0,                              /*tp_repr*/
        0,                              /*tp_as_number*/
        0,                              /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
};

PyBBox * newPyBBox()
{
        PyBBox * self;
        self = PyObject_NEW(PyBBox, &PyBBox_Type);
        if (self == NULL) {
                return NULL;
        }
        new (&(self->box)) BBox();
        return self;
}
