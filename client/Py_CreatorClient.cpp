// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <rulesets/Py_Operation.h>
#include <rulesets/Py_WorldTime.h>
#include <rulesets/Py_Vector3D.h>
#include <rulesets/Py_Location.h>
#include <rulesets/Py_Object.h>
#include <rulesets/Py_Thing.h>
#include <rulesets/Py_Map.h>

#include "CreatorClient.h"
#include "Py_CreatorClient.h"

static PyObject * CreatorClient_as_entity(CreatorClientObject * self, PyObject * args)
{
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid creator as_entity");
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

static PyObject * CreatorClient_get_xyz(CreatorClientObject * self, PyObject * args)
{
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid creator get_xyz");
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

static PyObject * CreatorClient_make(CreatorClientObject * self, PyObject * args)
{
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid creator make");
        return NULL;
    }
    AtlasObject * entity = NULL;
    if (!PyArg_ParseTuple(args, "O", &entity)) {
        return NULL;
    }
    if (!PyAtlasObject_Check(entity)) {
        PyErr_SetString(PyExc_TypeError, "Can only make Atlas entity");
        return NULL;
    }
    Entity * retval = self->m_mind->make(*entity->m_obj);
    if (retval == NULL) {
        PyErr_SetString(PyExc_TypeError, "Entity creation failed");
        return NULL;
    }
    ThingObject * ret = newThingObject(NULL);
    ret->m_thing = retval;
    return (PyObject *)ret;
}

static PyObject * CreatorClient_send(CreatorClientObject * self, PyObject * args)
{
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid creator send");
        return NULL;
    }
    OperationObject * op;
    if (!PyArg_ParseTuple(args, "O", &op)) {
        return NULL;
    }
    if (!PyOperation_Check(op)) {
        PyErr_SetString(PyExc_TypeError, "Can only send Atlas operation");
        return NULL;
    }
    self->m_mind->send(*op->operation);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef CreatorClient_methods[] = {
	{"get_xyz",        (PyCFunction)CreatorClient_get_xyz, 1},
	{"as_entity",      (PyCFunction)CreatorClient_as_entity, 1},
	{"make",           (PyCFunction)CreatorClient_make, 1},
	{"send",           (PyCFunction)CreatorClient_send, 1},
	{NULL,          NULL}           /* sentinel */
};

static void CreatorClient_dealloc(CreatorClientObject *self)
{
    //if (self->m_mind != NULL) {
        //delete self->m_mind;
    //}
    Py_XDECREF(self->CreatorClient_attr);
    PyMem_DEL(self);
}

static PyObject * CreatorClient_getattr(CreatorClientObject *self, char *name)
{
    // Fairly major re-write of this to use operator[] of CreatorClient base class
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid creator getattr");
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
        PyList_Append(list, PyString_FromString(self->m_mind->getType().c_str()));
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
    if (self->CreatorClient_attr != NULL) {
        PyObject *v = PyDict_GetItemString(self->CreatorClient_attr, name);
        if (v != NULL) {
            Py_INCREF(v);
            return v;
        }
    }
    Entity * thing = self->m_mind;
    std::string attr(name);
    PyObject * ret = Object_asPyObject(thing->get(attr));
    if (ret == NULL) {
        return Py_FindMethod(CreatorClient_methods, (PyObject *)self, name);
    }
    return ret;
}

static int CreatorClient_setattr(CreatorClientObject *self, char *name, PyObject *v)
{
    if (self->m_mind == NULL) {
        return -1;
    }
    if (self->CreatorClient_attr == NULL) {
        self->CreatorClient_attr = PyDict_New();
        if (self->CreatorClient_attr == NULL) {
            return -1;
        }
    }
    if (strcmp(name, "status") == 0) {
        // This needs to be here until we can sort the difference
        // between floats and ints in python.
        if (PyInt_Check(v)) {
            self->m_mind->setStatus((double)PyInt_AsLong(v));
        } else if (PyFloat_Check(v)) {
            self->m_mind->setStatus(PyFloat_AsDouble(v));
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
    //std::string attr(name);
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
    return PyDict_SetItemString(self->CreatorClient_attr, name, v);
}

static int CreatorClient_compare(CreatorClientObject *self, CreatorClientObject *other)
{
    if ((self->m_mind == NULL) || (other->m_mind == NULL)) {
        return -1;
    }
    return (self->m_mind == other->m_mind) ? 0 : 1;
}

PyTypeObject CreatorClient_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,					/*ob_size*/
	"cppCreatorClient",			/*tp_name*/
	sizeof(CreatorClientObject),		/*tp_basicsize*/
	0,					/*tp_itemsize*/
	/* methods */
	(destructor)CreatorClient_dealloc,	/*tp_dealloc*/
	0,					/*tp_print*/
	(getattrfunc)CreatorClient_getattr,	/*tp_getattr*/
	(setattrfunc)CreatorClient_setattr,	/*tp_setattr*/
	(cmpfunc)CreatorClient_compare,		/*tp_compare*/
	0,					/*tp_repr*/
	0,					/*tp_as_number*/
	0,					/*tp_as_sequence*/
	0,					/*tp_as_mapping*/
	0,					/*tp_hash*/
};

CreatorClientObject * newCreatorClientObject(PyObject *arg)
{
	CreatorClientObject * self;
	self = PyObject_NEW(CreatorClientObject, &CreatorClient_Type);
	if (self == NULL) {
		return NULL;
	}
	self->CreatorClient_attr = NULL;
	return self;
}
