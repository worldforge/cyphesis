#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include "Thing.h"

#include "Python_API.h"

PyObject * Location_copy(LocationObject *self, PyObject *args)
{
    cout << "Location_copy" << endl << flush;
    if (self->location == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid location");
        return NULL;
    }
    if (!PyArg_ParseTuple(args, "")) {
        PyErr_SetString(PyExc_TypeError, "location.copy() has no args");
        return NULL;
    }
    LocationObject * ret = newLocationObject(NULL);
    ret->location = new Location(*self->location);
    ret->own = 1;
    return (PyObject *)ret;
}

PyMethodDef Location_methods[] = {
    {"copy",		(PyCFunction)Location_copy,	METH_VARARGS},
    {NULL,		NULL}           /* sentinel */
};

static void Location_dealloc(LocationObject *self)
{
    if ((self->own != 0) && (self->location != NULL)) {
        delete self->location;
    }
    Py_XDECREF(self->Location_attr);
    PyMem_DEL(self);
}

PyObject * Location_getattr(LocationObject *self, char *name)
{
    cout << "Location_getattr" << endl << flush;
    if (self->location == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid location");
        return NULL;
    }
    if (strcmp(name, "parent") == 0) {
        cout << "Location_getattr(parent)" << endl << flush;
        if (self->location->parent == NULL) {
            Py_INCREF(Py_None);
            return Py_None;
        }
        ThingObject * thing = newThingObject(NULL);
        thing->m_thing = (Thing *)self->location->parent;
        return (PyObject *)thing;
    }
    if (strcmp(name, "coordinates") == 0) {
        cout << "Location_getattr(coordinates)" << endl << flush;
        Vector3DObject * v = newVector3DObject(NULL);
        v->coords = self->location->coords;
        return (PyObject *)v;
    }
    if (strcmp(name, "velocity") == 0) {
        cout << "Location_getattr(velocity)" << endl << flush;
        Vector3DObject * v = newVector3DObject(NULL);
        v->coords = self->location->velocity;
        return (PyObject *)v;
    }
    if (strcmp(name, "rotation") == 0) {
        cout << "Location_getattr(rotation)" << endl << flush;
        Vector3DObject * v = newVector3DObject(NULL);
        v->coords = self->location->face;
        return (PyObject *)v;
    }
    return Py_FindMethod(Location_methods, (PyObject *)self, name);
}

int Location_setattr(LocationObject *self, char *name, PyObject *v)
{
    cout << "Location_setattr" << endl << flush;
    if (self->location == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid location");
        return -1;
    }
    if (strcmp(name, "parent") == 0) {
        if ((PyTypeObject*)PyObject_Type(v) != &Thing_Type) {
            PyErr_SetString(PyExc_TypeError, "parent must be a thing");
            return -1;
        }
        ThingObject * thing = (ThingObject *)v;
        if (thing->m_thing == NULL) {
            PyErr_SetString(PyExc_TypeError, "invalid thing");
            return -1;
        }
        self->location->parent = thing->m_thing;
        return(0);
    }
    if ((PyTypeObject*)PyObject_Type(v) != &Vector3D_Type) {
        PyErr_SetString(PyExc_TypeError, "arg must be a vector");
        return -1;
    }
    Vector3DObject * vec = (Vector3DObject *)v;
    if (!vec->coords) {
        printf("This vector is not set\n");
    }
    if (strcmp(name, "coordinates") == 0) {
        self->location->coords = vec->coords;
    }
    if (strcmp(name, "velocity") == 0) {
        self->location->velocity = vec->coords;
    }
    if (strcmp(name, "rotation") == 0) {
        self->location->face = vec->coords;
    }
    return(0);
}

PyTypeObject Location_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,				/*ob_size*/
	"Location",			/*tp_name*/
	sizeof(LocationObject),		/*tp_basicsize*/
	0,				/*tp_itemsize*/
	/* methods */
	(destructor)Location_dealloc,	/*tp_dealloc*/
	0,				/*tp_print*/
	(getattrfunc)Location_getattr,	/*tp_getattr*/
	(setattrfunc)Location_setattr,	/*tp_setattr*/
	0,				/*tp_compare*/
	0,				/*tp_repr*/
	0,				/*tp_as_number*/
	0,				/*tp_as_sequence*/
	0,				/*tp_as_mapping*/
	0,				/*tp_hash*/
};

LocationObject * newLocationObject(PyObject *arg)
{
	LocationObject * self;
	self = PyObject_NEW(LocationObject, &Location_Type);
	if (self == NULL) {
		return NULL;
	}
	self->Location_attr = NULL;
        self->location = NULL;
        self->own = 0;
	return self;
}
