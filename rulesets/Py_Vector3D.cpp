#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include "Python_API.h"

PyMethodDef Vector3D_methods[] = {
    //{"update",		(PyCFunction)Vector3D_update,	METH_VARARGS},
    {NULL,		NULL}           /* sentinel */
};

static void Vector3D_dealloc(Vector3DObject *self)
{
    Py_XDECREF(self->Vector3D_attr);
    PyMem_DEL(self);
}

PyObject * Vector3D_getattr(Vector3DObject *self, char *name)
{
    return Py_FindMethod(Vector3D_methods, (PyObject *)self, name);
}

int Vector3D_setattr(Vector3DObject *self, char *name, PyObject *v)
{
    return(0);
}

PyTypeObject Vector3D_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,				/*ob_size*/
	"Vector3D",			/*tp_name*/
	sizeof(Vector3DObject),		/*tp_basicsize*/
	0,				/*tp_itemsize*/
	/* methods */
	(destructor)Vector3D_dealloc,	/*tp_dealloc*/
	0,				/*tp_print*/
	(getattrfunc)Vector3D_getattr,	/*tp_getattr*/
	(setattrfunc)Vector3D_setattr,	/*tp_setattr*/
	0,				/*tp_compare*/
	0,				/*tp_repr*/
	0,				/*tp_as_number*/
	0,				/*tp_as_sequence*/
	0,				/*tp_as_mapping*/
	0,				/*tp_hash*/
};

Vector3DObject * newVector3DObject(PyObject *arg)
{
	Vector3DObject * self;
	self = PyObject_NEW(Vector3DObject, &Vector3D_Type);
	if (self == NULL) {
		return NULL;
	}
	self->Vector3D_attr = NULL;
	return self;
}
