// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include "Python_API.h"
#include "BaseMind.h"

static PyObject * Mind_as_entity(MindObject * self, PyObject * args)
{
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid thing");
        return NULL;
    }
    if (!PyArg_ParseTuple(args, "")) {
        return NULL;
    }
    AtlasObject * ret = newAtlasObject(NULL);
    if (ret == NULL) {
        return NULL;
    }
    ret->m_obj = new Object(self->m_mind->asObject());
    return (PyObject *)ret;
}

static PyObject * Mind_get_xyz(MindObject * self, PyObject * args)
{
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid thing");
        return NULL;
    }
    if (!PyArg_ParseTuple(args, "")) {
        return NULL;
    }
    Vector3DObject * ret = newVector3DObject(NULL);
    if (ret == NULL) {
        return NULL;
    }
    ret->coords = self->m_mind->getXyz();
    return (PyObject *)ret;
}

static PyMethodDef Mind_methods[] = {
	{"get_xyz",        (PyCFunction)Mind_get_xyz,  1},
	{"as_entity",        (PyCFunction)Mind_as_entity,  1},
	{NULL,          NULL}           /* sentinel */
};

static void Mind_dealloc(MindObject *self)
{
    //if (self->m_mind != NULL) {
        //delete self->m_mind;
    //}
    Py_XDECREF(self->Mind_attr);
    PyMem_DEL(self);
}

static PyObject * Mind_getattr(MindObject *self, char *name)
{
    // Fairly major re-write of this to use operator[] of Mind base class
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid thing");
        return NULL;
    }
    // If operation search gets to here, it goes no further
    if (strstr(name, "_operation") != NULL) {
        PyErr_SetString(PyExc_AttributeError, name);
        return NULL;
    }
    if (strcmp(name, "type") == 0) {
        PyObject * list = PyList_New(0);
        if (list == NULL) {
            return NULL;
        }
        PyList_Append(list, PyString_FromString(self->m_mind->type.c_str()));
        return list;
    }
    if (strcmp(name, "map") == 0) {
        MapObject * map = newMapObject(NULL);
        map->m_map = self->m_mind->getMap();
        return (PyObject *)map;
    }
    if (strcmp(name, "location") == 0) {
        LocationObject * loc = newLocationObject(NULL);
        loc->location = &self->m_mind->location;
        loc->own = 0;
        return (PyObject *)loc;
    }
    if (strcmp(name, "time") == 0) {
        WorldTimeObject * worldtime = newWorldTimeObject(NULL);
        worldtime->time = self->m_mind->getTime();
        return (PyObject *)worldtime;
    }
    if (self->Mind_attr != NULL) {
        PyObject *v = PyDict_GetItemString(self->Mind_attr, name);
        if (v != NULL) {
            Py_INCREF(v);
            return v;
        }
    }
    Entity * thing = self->m_mind;
    std::string attr(name);
    PyObject * ret = Object_asPyObject((*thing)[attr]);
    if (ret == NULL) {
        return Py_FindMethod(Mind_methods, (PyObject *)self, name);
    }
    return ret;
}

static int Mind_setattr(MindObject *self, char *name, PyObject *v)
{
    if (self->m_mind == NULL) {
        return -1;
    }
    if (self->Mind_attr == NULL) {
        self->Mind_attr = PyDict_New();
        if (self->Mind_attr == NULL) {
            return -1;
        }
    }
    if (strcmp(name, "status") == 0) {
        // This needs to be here until we can sort the difference
        // between floats and ints in python.
        if (PyInt_Check(v)) {
            self->m_mind->status = (double)PyInt_AsLong(v);
        } else if (PyFloat_Check(v)) {
            self->m_mind->status = PyFloat_AsDouble(v);
        } else {
            PyErr_SetString(PyExc_TypeError, "status must be numeric type");
            return -1;
        }
        return 0;
    }
    if (strcmp(name, "map") == 0) {
        return -1;
    }
    Entity * thing = self->m_mind;
    //string attr(name);
    //if (v == NULL) {
        //thing->attributes.erase(attr);
        //return 0;
    //}
    Object obj = PyObject_asObject(v);
    if (!obj.IsNone() && !obj.IsMap() && !obj.IsList()) {
        thing->set(name, obj);
        return 0;
    }
    // If we get here, then the attribute is not Atlas compatable, so we
    // need to store it in a python dictionary
    return PyDict_SetItemString(self->Mind_attr, name, v);
}

static int Mind_compare(MindObject *self, MindObject *other)
{
    if ((self->m_mind == NULL) || (other->m_mind == NULL)) {
        return -1;
    }
    return (self->m_mind == other->m_mind) ? 0 : 1;
}

PyTypeObject Mind_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,				/*ob_size*/
	"cppMind",			/*tp_name*/
	sizeof(MindObject),		/*tp_basicsize*/
	0,				/*tp_itemsize*/
	/* methods */
	(destructor)Mind_dealloc,	/*tp_dealloc*/
	0,				/*tp_print*/
	(getattrfunc)Mind_getattr,	/*tp_getattr*/
	(setattrfunc)Mind_setattr,	/*tp_setattr*/
	(cmpfunc)Mind_compare,		/*tp_compare*/
	0,				/*tp_repr*/
	0,				/*tp_as_number*/
	0,				/*tp_as_sequence*/
	0,				/*tp_as_mapping*/
	0,				/*tp_hash*/
};

MindObject * newMindObject(PyObject *arg)
{
	MindObject * self;
	self = PyObject_NEW(MindObject, &Mind_Type);
	if (self == NULL) {
		return NULL;
	}
	self->Mind_attr = NULL;
	return self;
}
