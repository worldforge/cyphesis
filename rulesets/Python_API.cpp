#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include "Python_API.h"
#include "Thing.h"

static PyObject * object_new(PyObject * self, PyObject * args)
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

static PyObject * cppthing_new(PyObject * self, PyObject * args)
{
	ThingObject *o;
	
	if (!PyArg_ParseTuple(args, "")) {
		return NULL;
	}
	o = newThingObject(args);
	if ( o == NULL ) {
		return NULL;
	}
	//o->m_thing = new Thing;
	return (PyObject *)o;
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

static PyMethodDef cyphesis_methods[] = {
	/* {"system",	spam_system, METH_VARARGS}, */
	{"Operation",	operation_new,	METH_VARARGS},
	{"Object",	object_new,	METH_VARARGS},
	{"cppThing",	cppthing_new,	METH_VARARGS},
	{NULL,		NULL}				/* Sentinel */
};

void init_python_api()
{
	char * cwd;

	if ((cwd = getcwd(NULL, 0)) != NULL) {
                size_t len = strlen(cwd) + 12;
                char * pypath = (char *)malloc(len);
                strcpy(pypath, cwd);
                strcat(pypath, "/rulesets/basic");
		setenv("PYTHONPATH", pypath, 1);
	}

	Py_Initialize();

	if (Py_InitModule("cyphesis", cyphesis_methods) == NULL) {
		printf("Failed to Create cyphesis thing\n");
	} else {
		printf("Created cyphesis thing\n");
	}
}
