#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include "Python_API.h"
#include "Thing.h"

static PyObject * Thing_as_entity(ThingObject * self, PyObject * args)
{
    if (self->m_thing == NULL) {
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
    ret->m_obj = new Object(self->m_thing->asObject());
    return (PyObject *)ret;
}

static PyObject * Thing_get_xyz(ThingObject * self, PyObject * args)
{
    if (self->m_thing == NULL) {
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
    ret->coords = self->m_thing->get_xyz();
    return (PyObject *)ret;
}

PyMethodDef Thing_methods[] = {
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

PyObject * Thing_getattr(ThingObject *self, char *name)
{
    if (self->m_thing == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid thing");
        return NULL;
    }
    if (strcmp(name, "id") == 0) {
        return PyString_FromString(self->m_thing->fullid.c_str());
    }
    if (strcmp(name, "name") == 0) {
        return PyString_FromString(self->m_thing->name.c_str());
    }
    if (strcmp(name, "type") == 0) {
        PyObject * list = PyList_New(0);
        if (list == NULL) {
            return NULL;
        }
        PyList_Append(list, PyString_FromString(self->m_thing->type.c_str()));
        return list;
    }
    if (strcmp(name, "status") == 0) {
        return PyFloat_FromDouble(self->m_thing->status);
    }
    if (strcmp(name, "map") == 0) {
        MapObject * map = newMapObject(NULL);
        map->m_map = &self->m_thing->map;
        return (PyObject *)map;
    }
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
    Thing * thing = self->m_thing;
    string attr(name);
    if (thing->attributes.find(attr) != thing->attributes.end()) {
        return Object_asPyObject(thing->attributes[attr]);
    }
    return Py_FindMethod(Thing_methods, (PyObject *)self, name);
}

int Thing_setattr(ThingObject *self, char *name, PyObject *v)
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
        if (PyInt_Check(v)) {
            self->m_thing->status = (double)PyInt_AsLong(v);
        } else if (PyFloat_Check(v)) {
            self->m_thing->status = PyFloat_AsDouble(v);
        } else {
            PyErr_SetString(PyExc_TypeError, "status must be numeric type");
            return -1;
        }
        return 0;
    }
    if (strcmp(name, "map") == 0) {
        return -1;
    }
    Thing * thing = self->m_thing;
    string attr(name);
    if (v == NULL) {
        thing->attributes.erase(attr);
        return(0);
    }
    Object obj = PyObject_asObject(v);
    if (!obj.IsNone() && !obj.IsMap() && !obj.IsList()) {
        thing->attributes[name] = obj;
        return(0);
    }
    // If we get here, then the attribute is not Atlas compatable, so we
    // need to store it in a python dictionary
    return PyDict_SetItemString(self->Thing_attr, name, v);
}

int Thing_compare(ThingObject *self, ThingObject *other)
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
