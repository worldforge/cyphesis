#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include "Python_API.h"

PyMethodDef Map_methods[] = {
	/* {"demo",        (PyCFunction)Xxo_demo,  1}, */
	{NULL,          NULL}           /* sentinel */
};

static void Map_dealloc(MapObject *self)
{
    //if (self->m_thing != NULL) {
        //delete self->m_thing;
    //}
    Py_XDECREF(self->Map_attr);
    PyMem_DEL(self);
}

PyObject * Map_getattr(MapObject *self, char *name)
{
    return Py_FindMethod(Map_methods, (PyObject *)self, name);
}

int Map_setattr(MapObject *self, char *name, PyObject *v)
{
    return(0);
}

PyTypeObject Map_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,				/*ob_size*/
	"cppMap",			/*tp_name*/
	sizeof(MapObject),		/*tp_basicsize*/
	0,				/*tp_itemsize*/
	/* methods */
	(destructor)Map_dealloc,	/*tp_dealloc*/
	0,				/*tp_print*/
	(getattrfunc)Map_getattr,	/*tp_getattr*/
	(setattrfunc)Map_setattr,	/*tp_setattr*/
	0,				/*tp_compare*/
	0,				/*tp_repr*/
	0,				/*tp_as_number*/
	0,				/*tp_as_sequence*/
	0,				/*tp_as_mapping*/
	0,				/*tp_hash*/
};

MapObject * newMapObject(PyObject *arg)
{
	MapObject * self;
	self = PyObject_NEW(MapObject, &Map_Type);
	if (self == NULL) {
		return NULL;
	}
	self->Map_attr = NULL;
	return self;
}
