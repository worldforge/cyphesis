#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include "Python_API.h"

static PyObject * Operation_SetSerialno(RootOperationObject * self, PyObject * args)
{
	return NULL;
	// FIXME What the hell do we do with this?
}

static PyObject * Operation_SetRefno(RootOperationObject * self, PyObject * args)
{
	return NULL;
	// FIXME What the hell do we do with this?
}

static PyObject * Operation_SetFrom(RootOperationObject * self, PyObject * args)
{
	return NULL;
	// FIXME What the hell do we do with this?
}

static PyObject * Operation_SetTo(RootOperationObject * self, PyObject * args)
{
	return NULL;
	// FIXME What the hell do we do with this?
}

static PyObject * Operation_SetSeconds(RootOperationObject * self, PyObject * args)
{
	return NULL;
	// FIXME What the hell do we do with this?
}

static PyObject * Operation_SetFutureSeconds(RootOperationObject * self, PyObject * args)
{
	return NULL;
	// FIXME What the hell do we do with this?
}

static PyObject * Operation_SetTimeString(RootOperationObject * self, PyObject * args)
{
	return NULL;
	// FIXME What the hell do we do with this?
}

static PyObject * Operation_SetArgs(RootOperationObject * self, PyObject * args)
{
	return NULL;
	// FIXME What the hell do we do with this?
}

static PyObject * Operation_GetSerialno(RootOperationObject * self, PyObject * args)
{
	return NULL;
	// FIXME What the hell do we do with this?
}

static PyObject * Operation_GetRefno(RootOperationObject * self, PyObject * args)
{
	return NULL;
	// FIXME What the hell do we do with this?
}

static PyObject * Operation_GetFrom(RootOperationObject * self, PyObject * args)
{
	return NULL;
	// FIXME What the hell do we do with this?
}

static PyObject * Operation_GetTo(RootOperationObject * self, PyObject * args)
{
	return NULL;
	// FIXME What the hell do we do with this?
}

static PyObject * Operation_GetSeconds(RootOperationObject * self, PyObject * args)
{
	return NULL;
	// FIXME What the hell do we do with this?
}

static PyObject * Operation_GetFutureSeconds(RootOperationObject * self, PyObject * args)
{
	return NULL;
	// FIXME What the hell do we do with this?
}

static PyObject * Operation_GetTimeString(RootOperationObject * self, PyObject * args)
{
	return NULL;
	// FIXME What the hell do we do with this?
}

static PyObject * Operation_GetArgs(RootOperationObject * self, PyObject * args)
{
	return NULL;
	// FIXME What the hell do we do with this?
}

// At the moment it looks as though only one set of methods will be required
// as they can all use it together
ATLAS_OPERATION_METHODS(RootOperation)
//ATLAS_OPERATION_METHODS(Login)
//ATLAS_OPERATION_METHODS(Create)
//ATLAS_OPERATION_METHODS(Move)
//ATLAS_OPERATION_METHODS(Set)

static PyMethodDef Object_methods[] = {
	/* {"demo",        (PyCFunction)Xxo_demo,  1}, */
	{NULL,          NULL}           /* sentinel */
};

static void Operation_dealloc(RootOperationObject *self)
{
	Py_XDECREF(self->Operation_attr);
	PyMem_DEL(self);
}

static PyObject * Operation_getattr(RootOperationObject * self, char * name)
{
	return Py_FindMethod(RootOperation_methods, (PyObject *)self, name);
}

static int Operation_setattr(RootOperationObject *self, char *name, PyObject *v)
{
	return 0;
}

ATLAS_OPERATION_TYPE(RootOperation)

static void Object_dealloc(AtlasObject *self)
{
	Py_XDECREF(self->Object_attr);
	PyMem_DEL(self);
}

static PyObject * Object_getattr(AtlasObject *self, char *name)
{
	return Py_FindMethod(Object_methods, (PyObject *)self, name);
}

static int Object_setattr( AtlasObject *self, char *name, PyObject *v)
{
	return 0;
}

staticforward PyTypeObject Object_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,				/*ob_size*/
	"Object",			/*tp_name*/
	sizeof(AtlasObject),		/*tp_basicsize*/
	0,				/*tp_itemsize*/
	/* methods */
	(destructor)Object_dealloc,	/*tp_dealloc*/
	0,				/*tp_print*/
	(getattrfunc)Object_getattr,	/*tp_getattr*/
	(setattrfunc)Object_setattr,	/*tp_setattr*/
	0,				/*tp_compare*/
	0,				/*tp_repr*/
	0,				/*tp_as_number*/
	0,				/*tp_as_sequence*/
	0,				/*tp_as_mapping*/
	0,				/*tp_hash*/
};


static RootOperationObject * newAtlasRootOperation(PyObject *arg)
{
	RootOperationObject * self;
	self = PyObject_NEW(RootOperationObject, &RootOperation_Type);
	if (self == NULL) {
		return(NULL);
	}
	self->Operation_attr = NULL;
	return self;
}

static AtlasObject * newAtlasObject(PyObject *arg)
{
	AtlasObject * self;
	self = PyObject_NEW(AtlasObject, &Object_Type);
	if (self == NULL) {
		return NULL;
	}
	self->Object_attr = NULL;
	return self;
}


static PyObject * operation_new(PyObject * self, PyObject * args)
{
	RootOperationObject * op;

	if (!PyArg_ParseTuple(args, "")) {
		return NULL;
	}
	op = newAtlasRootOperation(args);
	if (op == NULL) {
		return NULL;
	}
	op->operation = new RootOperation;
	*op->operation = RootOperation::Instantiate();
	return (PyObject *)op;
}

static PyObject * cyphesis_new(PyObject * self, PyObject * args)
{
	AtlasObject *o;
	
	if (!PyArg_ParseTuple(args, "")) {
		return NULL;
	}
	o = newAtlasObject(args);
	if ( o == NULL ) {
		return NULL;
	}
	o->m_obj = new Object;
	return (PyObject *)o;
}

static PyMethodDef cyphesis_methods[] = {
	/* {"system",	spam_system, METH_VARARGS}, */
	{"Operation",	operation_new,	METH_VARARGS},
	{"Object",	cyphesis_new,	METH_VARARGS},
	{NULL,		NULL}				/* Sentinel */
};

void init_python_api()
{
	char * cwd;

	if ((cwd = getcwd(NULL, 0)) != NULL) {
		setenv("PYTHONPATH", cwd, 1);
	}

	Py_Initialize();

	if (Py_InitModule("cyphesis", cyphesis_methods) == NULL) {
		printf("Failed to Create cyphesis thing\n");
	} else {
		printf("Created cyphesis thing\n");
	}
}
