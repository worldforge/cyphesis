#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include "Python_API.h"

PyMethodDef Location_methods[] = {
    /* {"update",		(PyCFunction)Location_update,	METH_VARARGS}, */
    {NULL,		NULL}           /* sentinel */
};

static void Location_dealloc(LocationObject *self)
{
    Py_XDECREF(self->Location_attr);
    PyMem_DEL(self);
}

PyObject * Location_getattr(LocationObject *self, char *name)
{
    return Py_FindMethod(Location_methods, (PyObject *)self, name);
}

int Location_setattr(LocationObject *self, char *name, PyObject *v)
{
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
	return self;
}
