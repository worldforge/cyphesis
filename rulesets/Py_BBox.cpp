// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_BBox.h"
#include "Py_Vector3D.h"

static PyMethodDef BBox_methods[] = {
    {NULL, NULL}  // sentinel
};

static void BBox_dealloc(BBoxObject * self)
{
    PyMem_DEL(self);
}

static PyObject * BBox_getattr(BBoxObject *self, char *name)
{
    if (strcmp(name, "near_point") == 0) {
        Vector3DObject * v = newVector3DObject(NULL);
        v->coords = self->box.nearPoint();
        return (PyObject *)v;
    }
    if (strcmp(name, "far_point") == 0) {
        Vector3DObject * v = newVector3DObject(NULL);
        v->coords = self->box.farPoint();
        return (PyObject *)v;
    }

    return Py_FindMethod(BBox_methods, (PyObject *)self, name);
}

static int BBox_setattr(BBoxObject *self, char *name, PyObject *v)
{
    if (!PyVector3D_Check(v)) {
        PyErr_SetString(PyExc_TypeError, "BBox setattr must take tuple of floats, or ints");
    }
    Vector3DObject * vec = (Vector3DObject *)v;
    if (!vec->coords.isValid()) {
        fprintf(stderr, "This vector is not valid\n");
    }
    Vector3D vector = vec->coords;
    if (strcmp(name, "near_point") == 0) {
        self->box.nearPoint() = vector;
    } else if (strcmp(name, "far_point") == 0) {
        self->box.farPoint() = vector;
    }

    return 0;
}

static int BBox_compare(BBoxObject * self, BBoxObject * other)
{
    if (!PyBBox_Check(other)) {
        return -1;
    }
    if (self->box == other->box) {
        return 0;
    }
    return 1;
}

PyTypeObject BBox_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /*ob_size*/
        "BBox",                         /*tp_name*/
        sizeof(BBoxObject),             /*tp_basicsize*/
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

BBoxObject * newBBoxObject(PyObject *arg)
{
        BBoxObject * self;
        self = PyObject_NEW(BBoxObject, &BBox_Type);
        if (self == NULL) {
                return NULL;
        }
        return self;
}
