// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_Thing.h"
#include "Py_Object.h"
#include "Py_Vector3D.h"
#include "Py_Location.h"
#include "Py_World.h"
#include "Entity.h"

static PyObject * Thing_as_entity(ThingObject * self, PyObject * args)
{
    if (self->m_thing == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid thing as_entity");
        return NULL;
    }
    if (!PyArg_ParseTuple(args, "")) {
        return NULL;
    }
    AtlasObject * ret = newAtlasObject(NULL);
    if (ret == NULL) {
        return NULL;
    }
    ret->m_obj = new Fragment(Fragment::MapType());
    self->m_thing->addToObject(ret->m_obj->AsMap());
    return (PyObject *)ret;
}

static PyObject * Thing_get_xyz(ThingObject * self, PyObject * args)
{
    if (self->m_thing == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid thing get_xyz");
        return NULL;
    }
    if (!PyArg_ParseTuple(args, "")) {
        return NULL;
    }
    Vector3DObject * ret = newVector3DObject(NULL);
    if (ret == NULL) {
        return NULL;
    }
    ret->coords = self->m_thing->getXyz();
    return (PyObject *)ret;
}

static PyMethodDef Thing_methods[] = {
	{"get_xyz",        (PyCFunction)Thing_get_xyz,  1},
	{"as_entity",        (PyCFunction)Thing_as_entity,  1},
	{NULL,          NULL}           /* sentinel */
};

static void Thing_dealloc(ThingObject *self)
{
    //if (self->m_thing != NULL) {
        //delete self->m_thing;
    //}
    Py_XDECREF(self->Thing_attr);
    PyMem_DEL(self);
}

static PyObject * Thing_getattr(ThingObject *self, char *name)
{
    // Fairly major re-write of this to use operator[] of Thing base class
    if (self->m_thing == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid thing getattr");
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
        PyObject * ent = PyString_FromString(self->m_thing->getType().c_str());
        PyList_Append(list, ent);
        Py_DECREF(ent);
        return list;
    }
    // if (strcmp(name, "map") == 0) {
        // MemMap * tMap = self->m_thing->getMap();
        // if (tMap == NULL) {
            // PyErr_SetString(PyExc_TypeError, "Body entity has no map");
            // return NULL;
        // }
        // MapObject * map = newMapObject(NULL);
        // map->m_map = tMap;
        // return (PyObject *)map;
    // }
    if (strcmp(name, "location") == 0) {
        LocationObject * loc = newLocationObject(NULL);
        loc->location = &self->m_thing->location;
        loc->own = 0;
        return (PyObject *)loc;
    }
    if (strcmp(name, "world") == 0) {
        WorldObject * world = newWorldObject(NULL);
        world->world = self->m_thing->world;
        return (PyObject *)world;
    }
    if (self->Thing_attr != NULL) {
        PyObject *v = PyDict_GetItemString(self->Thing_attr, name);
        if (v != NULL) {
            Py_INCREF(v);
            return v;
        }
    }
    Entity * thing = self->m_thing;
    std::string attr(name);
    PyObject * ret = Object_asPyObject(thing->get(attr));
    if (ret == NULL) {
        return Py_FindMethod(Thing_methods, (PyObject *)self, name);
    }
    return ret;
}

static int Thing_setattr(ThingObject *self, char *name, PyObject *v)
{
    if (self->m_thing == NULL) {
        return -1;
    }
    if (self->Thing_attr == NULL) {
        self->Thing_attr = PyDict_New();
        if (self->Thing_attr == NULL) {
            return -1;
        }
    }
    if (strcmp(name, "status") == 0) {
        // This needs to be here until we can sort the difference
        // between floats and ints in python.
        if (PyInt_Check(v)) {
            self->m_thing->setStatus((double)PyInt_AsLong(v));
        } else if (PyFloat_Check(v)) {
            self->m_thing->setStatus(PyFloat_AsDouble(v));
        } else {
            PyErr_SetString(PyExc_TypeError, "status must be numeric type");
            return -1;
        }
        return 0;
    }
    if (strcmp(name, "map") == 0) {
        return -1;
    }
    Entity * thing = self->m_thing;
    //std::string attr(name);
    //if (v == NULL) {
        //thing->attributes.erase(attr);
        //return 0;
    //}
    Fragment obj = PyObject_asObject(v);
    if (!obj.IsNone() && !obj.IsMap() && !obj.IsList()) {
        thing->set(name, obj);
        return 0;
    }
    // If we get here, then the attribute is not Atlas compatable, so we
    // need to store it in a python dictionary
    return PyDict_SetItemString(self->Thing_attr, name, v);
}

static int Thing_compare(ThingObject *self, ThingObject *other)
{
    if ((self->m_thing == NULL) || (other->m_thing == NULL)) {
        return -1;
    }
    return (self->m_thing == other->m_thing) ? 0 : 1;
}

PyTypeObject Thing_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,				/*ob_size*/
	"cppThing",			/*tp_name*/
	sizeof(ThingObject),		/*tp_basicsize*/
	0,				/*tp_itemsize*/
	/* methods */
	(destructor)Thing_dealloc,	/*tp_dealloc*/
	0,				/*tp_print*/
	(getattrfunc)Thing_getattr,	/*tp_getattr*/
	(setattrfunc)Thing_setattr,	/*tp_setattr*/
	(cmpfunc)Thing_compare,		/*tp_compare*/
	0,				/*tp_repr*/
	0,				/*tp_as_number*/
	0,				/*tp_as_sequence*/
	0,				/*tp_as_mapping*/
	0,				/*tp_hash*/
};

ThingObject * newThingObject(PyObject *arg)
{
	ThingObject * self;
	self = PyObject_NEW(ThingObject, &Thing_Type);
	if (self == NULL) {
		return NULL;
	}
	self->Thing_attr = NULL;
	return self;
}
