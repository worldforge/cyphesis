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
    if (self->m_thing != NULL) {
        cout << "got thing" << endl << flush;
        Thing * thing = self->m_thing;
        string attr(name);
        if (thing->attributes.find(attr) != thing->attributes.end()) {
            cout << name << " is in the attributes list" << endl << flush;
            return Object_asPyObject(thing->attributes[attr]);
        }
        cout << "falling through" << endl << flush;
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
    Thing * thing = self->m_thing;
    string attr(name);
    if (v == NULL) {
        thing->attributes.erase(attr);
        return(0);
    } else {
        Object obj = PyObject_asObject(v);
        thing->attributes[name] = obj;
        return(0);
    }
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
