#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include "Python_API.h"
#include "Thing.h"

PyMethodDef Thing_methods[] = {
	/* {"demo",        (PyCFunction)Xxo_demo,  1}, */
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
    cout << "Thing_getattr" << endl << flush;
    if (self->m_thing == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid thing");
        return NULL;
    }
    if (strcmp(name, "id") == 0) {
        cout << "Thing_getattr(id)" << endl << flush;
        return PyString_FromString(self->m_thing->fullid.c_str());
    }
    if (strcmp(name, "name") == 0) {
        cout << "Thing_getattr(name)" << endl << flush;
        return PyString_FromString(self->m_thing->name.c_str());
    }
    if (strcmp(name, "map") == 0) {
        cout << "Thing_getattr(map)" << endl << flush;
        MapObject * map = newMapObject(NULL);
        map->m_map = &self->m_thing->map;
        return (PyObject *)map;
    }
    if (strcmp(name, "location") == 0) {
        cout << "Thing_getattr(location)" << endl << flush;
        LocationObject * loc = newLocationObject(NULL);
        loc->location = &self->m_thing->location;
        return (PyObject *)loc;
    }
    if (strcmp(name, "world") == 0) {
        cout << "Thing_getattr(world)" << endl << flush;
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
        cout << name << " is in the attributes list" << endl << flush;
        return Object_asPyObject(thing->attributes[attr]);
    }
    cout << "Just doing the method lookup" << endl << flush;
    return Py_FindMethod(Thing_methods, (PyObject *)self, name);
}

int Thing_setattr(ThingObject *self, char *name, PyObject *v)
{
    cout << "Thing_setattr" << endl << flush;
    if (self->m_thing == NULL) {
        return -1;
    }
    if (self->Thing_attr == NULL) {
        self->Thing_attr = PyDict_New();
        if (self->Thing_attr == NULL) {
            return -1;
        }
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
    if (!obj.IsNone()) {
        thing->attributes[name] = obj;
        return(0);
    }
    // If we get here, then the attribute is not Atlas compatable, so we
    // need to store it in a python dictionary
    return PyDict_SetItemString(self->Thing_attr, name, v);
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
	0,				/*tp_compare*/
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
