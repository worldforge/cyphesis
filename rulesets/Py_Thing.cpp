// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_Thing.h"
#include "Py_Object.h"
#include "Py_Vector3D.h"
#include "Py_Location.h"
#include "Py_World.h"
#include "Entity.h"

static PyObject * Entity_as_entity(EntityObject * self, PyObject * args)
{
    if (self->m_entity == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid entity as_entity");
        return NULL;
    }
    if (!PyArg_ParseTuple(args, "")) {
        return NULL;
    }
    AtlasObject * ret = newAtlasObject(NULL);
    if (ret == NULL) {
        return NULL;
    }
    ret->m_obj = new Element(Element::MapType());
    self->m_entity->addToObject(ret->m_obj->AsMap());
    return (PyObject *)ret;
}

static PyObject * Entity_get_xyz(EntityObject * self, PyObject * args)
{
    if (self->m_entity == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid entity get_xyz");
        return NULL;
    }
    if (!PyArg_ParseTuple(args, "")) {
        return NULL;
    }
    Vector3DObject * ret = newVector3DObject(NULL);
    if (ret == NULL) {
        return NULL;
    }
    ret->coords = self->m_entity->getXyz();
    return (PyObject *)ret;
}

static PyMethodDef Entity_methods[] = {
	{"get_xyz",        (PyCFunction)Entity_get_xyz,  1},
	{"as_entity",        (PyCFunction)Entity_as_entity,  1},
	{NULL,          NULL}           /* sentinel */
};

static void Entity_dealloc(EntityObject *self)
{
    //if (self->m_entity != NULL) {
        //delete self->m_entity;
    //}
    Py_XDECREF(self->Entity_attr);
    PyMem_DEL(self);
}

static PyObject * Entity_getattr(EntityObject *self, char *name)
{
    // Fairly major re-write of this to use operator[] of Entity base class
    if (self->m_entity == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid entity getattr");
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
        PyObject * ent = PyString_FromString(self->m_entity->getType().c_str());
        PyList_Append(list, ent);
        Py_DECREF(ent);
        return list;
    }
    // if (strcmp(name, "map") == 0) {
        // MemMap * tMap = self->m_entity->getMap();
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
        loc->location = &self->m_entity->m_location;
        loc->own = 0;
        return (PyObject *)loc;
    }
    if (strcmp(name, "world") == 0) {
        WorldObject * world = newWorldObject(NULL);
        world->world = self->m_entity->m_world;
        return (PyObject *)world;
    }
    if (self->Entity_attr != NULL) {
        PyObject *v = PyDict_GetItemString(self->Entity_attr, name);
        if (v != NULL) {
            Py_INCREF(v);
            return v;
        }
    }
    Entity * entity = self->m_entity;
    Element attr;
    if (!entity->get(name, attr)) {
        return Py_FindMethod(Entity_methods, (PyObject *)self, name);
    }
    PyObject * ret = Object_asPyObject(attr);
    if (ret == NULL) {
        return Py_FindMethod(Entity_methods, (PyObject *)self, name);
    }
    return ret;
}

static int Entity_setattr(EntityObject *self, char *name, PyObject *v)
{
    if (self->m_entity == NULL) {
        return -1;
    }
    if (self->Entity_attr == NULL) {
        self->Entity_attr = PyDict_New();
        if (self->Entity_attr == NULL) {
            return -1;
        }
    }
    if (strcmp(name, "status") == 0) {
        // This needs to be here until we can sort the difference
        // between floats and ints in python.
        if (PyInt_Check(v)) {
            self->m_entity->setStatus((double)PyInt_AsLong(v));
        } else if (PyFloat_Check(v)) {
            self->m_entity->setStatus(PyFloat_AsDouble(v));
        } else {
            PyErr_SetString(PyExc_TypeError, "status must be numeric type");
            return -1;
        }
        return 0;
    }
    if (strcmp(name, "map") == 0) {
        return -1;
    }
    Entity * entity = self->m_entity;
    //std::string attr(name);
    //if (v == NULL) {
        //entity->attributes.erase(attr);
        //return 0;
    //}
    Element obj = PyObject_asObject(v);
    if (!obj.IsNone() && !obj.IsMap() && !obj.IsList()) {
        entity->set(name, obj);
        return 0;
    }
    // If we get here, then the attribute is not Atlas compatable, so we
    // need to store it in a python dictionary
    return PyDict_SetItemString(self->Entity_attr, name, v);
}

static int Entity_compare(EntityObject *self, EntityObject *other)
{
    if ((self->m_entity == NULL) || (other->m_entity == NULL)) {
        return -1;
    }
    return (self->m_entity == other->m_entity) ? 0 : 1;
}

PyTypeObject Entity_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,				/*ob_size*/
	"cppEntity",			/*tp_name*/
	sizeof(EntityObject),		/*tp_basicsize*/
	0,				/*tp_itemsize*/
	/* methods */
	(destructor)Entity_dealloc,	/*tp_dealloc*/
	0,				/*tp_print*/
	(getattrfunc)Entity_getattr,	/*tp_getattr*/
	(setattrfunc)Entity_setattr,	/*tp_setattr*/
	(cmpfunc)Entity_compare,		/*tp_compare*/
	0,				/*tp_repr*/
	0,				/*tp_as_number*/
	0,				/*tp_as_sequence*/
	0,				/*tp_as_mapping*/
	0,				/*tp_hash*/
};

EntityObject * newEntityObject(PyObject *arg)
{
	EntityObject * self;
	self = PyObject_NEW(EntityObject, &Entity_Type);
	if (self == NULL) {
		return NULL;
	}
	self->Entity_attr = NULL;
	return self;
}
