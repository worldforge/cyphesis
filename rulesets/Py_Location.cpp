// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_Location.h"
#include "Py_Thing.h"
#include "Py_Vector3D.h"
#include "Py_Point3D.h"
#include "Py_Quaternion.h"
#include "Py_BBox.h"
#include "PythonThingScript.h"

#include "Entity.h"

#include "common/log.h"

static PyObject * Location_copy(PyLocation *self)
{
#ifndef NDEBUG
    if (self->location == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Location in Location.copy");
        return NULL;
    }
#endif // NDEBUG
    PyLocation * ret = newPyLocation();
    ret->location = new Location(self->location->m_loc, self->location->pos(), self->location->velocity());
    ret->own = 1;
    return (PyObject *)ret;
}

static PyMethodDef Location_methods[] = {
    {"copy",            (PyCFunction)Location_copy,     METH_NOARGS},
    {NULL,              NULL}           /* sentinel */
};

static void Location_dealloc(PyLocation *self)
{
    if ((self->own != 0) && (self->location != NULL)) {
        delete self->location;
    }
    PyMem_DEL(self);
}

static PyObject * Location_getattr(PyLocation *self, char *name)
{
#ifndef NDEBUG
    if (self->location == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Location in Location.getattr");
        return NULL;
    }
#endif // NDEBUG
    if (strcmp(name, "parent") == 0) {
        if (self->location->m_loc == NULL) {
            Py_INCREF(Py_None);
            return Py_None;
        }
        PythonWrapper * pw = dynamic_cast<PythonWrapper *>(self->location->m_loc->script());
        if (pw == 0) {
            PyEntity * o = newPyEntity();
            o->m_entity = self->location->m_loc;
            if (self->location->m_loc->script() == &noScript) {
                pw = new PythonWrapper((PyObject *)o);
                self->location->m_loc->setScript(pw);
            } else {
                std::cerr << "Bizare script!" << std::endl << std::flush;
            }
            return (PyObject *)o;
        } else {
            PyObject * o = pw->wrapper();
            assert(o != NULL);
            Py_INCREF(o);
            return o;
        }
    }
    if (strcmp(name, "coordinates") == 0) {
        PyPoint3D * v = newPyPoint3D();
        v->coords = self->location->pos();
        return (PyObject *)v;
    }
    if (strcmp(name, "velocity") == 0) {
        PyVector3D * v = newPyVector3D();
        v->coords = self->location->velocity();
        return (PyObject *)v;
    }
    if (strcmp(name, "orientation") == 0) {
        PyQuaternion * v = newPyQuaternion();
        v->rotation = self->location->orientation();
        return (PyObject *)v;
    }
    if (strcmp(name, "bbox") == 0) {
        PyBBox * b = newPyBBox();
        b->box = self->location->bBox();
        return (PyObject *)b;
    }
    return Py_FindMethod(Location_methods, (PyObject *)self, name);
}

static int Location_setattr(PyLocation *self, char *name, PyObject *v)
{
#ifndef NDEBUG
    if (self->location == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Location in Location.setattr");
        return -1;
    }
#endif // NDEBUG
    if (strcmp(name, "parent") == 0) {
        if (!PyEntity_Check(v)) {
            PyErr_SetString(PyExc_TypeError, "parent must be a thing");
            return -1;
        }
        PyEntity * thing = (PyEntity *)v;
#ifndef NDEBUG
        if (thing->m_entity == NULL) {
            PyErr_SetString(PyExc_AssertionError, "invalid thing");
            return -1;
        }
#endif // NDEBUG
        self->location->m_loc = thing->m_entity;
        return 0;
    }
    if ((strcmp(name, "bbox") == 0) && PyBBox_Check(v)) {
        PyBBox * box = (PyBBox *)v;
        self->location->setBBox(box->box);
        return 0;
    }
    if ((strcmp(name, "orientation") == 0) && PyQuaternion_Check(v)) {
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
    } else if (PyTuple_Check(v) && (PyTuple_Size(v) == 3)) {
        for(int i = 0; i < 3; i++) {
            PyObject * item = PyTuple_GetItem(v, i);
            if (PyInt_Check(item)) {
                vector[i] = (double)PyInt_AsLong(item);
            } else if (PyFloat_Check(item)) {
                vector[i] = PyFloat_AsDouble(item);
            } else {
                PyErr_SetString(PyExc_TypeError, "Vector3D() must take tuple of floats, or ints");
                return -1;
            }
        }
        vector.setValid();
    } else if (PyList_Check(v) && (PyList_Size(v) == 3)) {
        for(int i = 0; i < 3; i++) {
            PyObject * item = PyList_GetItem(v, i);
            if (PyInt_Check(item)) {
                vector[i] = (double)PyInt_AsLong(item);
            } else if (PyFloat_Check(item)) {
                vector[i] = PyFloat_AsDouble(item);
            } else {
                PyErr_SetString(PyExc_TypeError, "Vector3D() must take list of floats, or ints");
                return -1;
            }
        }
        vector.setValid();
    } else {
        PyErr_SetString(PyExc_TypeError, "arg must be a vector");
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

PyTypeObject PyLocation_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /*ob_size*/
        "Location",                     /*tp_name*/
        sizeof(PyLocation),             /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        (destructor)Location_dealloc,   /*tp_dealloc*/
        0,                              /*tp_print*/
        (getattrfunc)Location_getattr,  /*tp_getattr*/
        (setattrfunc)Location_setattr,  /*tp_setattr*/
        0,                              /*tp_compare*/
        0,                              /*tp_repr*/
        0,                              /*tp_as_number*/
        0,                              /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
};

PyLocation * newPyLocation()
{
        PyLocation * self;
        self = PyObject_NEW(PyLocation, &PyLocation_Type);
        if (self == NULL) {
                return NULL;
        }
        self->location = NULL;
        self->own = 0;
        return self;
}
