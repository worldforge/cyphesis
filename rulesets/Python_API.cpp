#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include "Python_API.h"
#include "Thing.h"


void Create_PyThing(Thing * thing, const string & package, const string & type)
{
    PyObject * mod_dict;
    if ((mod_dict = PyImport_ImportModule((char *)package.c_str()))==NULL) {
        cout << "Cld no find python module " << package << endl << flush;
            PyErr_Print();
        return;
    } else {
        cout << "Got python module " << package << endl << flush;
    }
    PyObject * my_class = PyObject_GetAttrString(mod_dict, (char *)type.c_str());
    if (my_class == NULL) {
        cout << "Cld no find class in module " << package << endl << flush;
            PyErr_Print();
        return;
    } else {
        cout << "Got python class " << type << " in " << package << endl << flush;
    }
    if (PyCallable_Check(my_class) == 0) {
            cout << "It does not seem to be a class at all" << endl << flush;
        return;
    }
    ThingObject * pyThing = newThingObject(NULL);
    pyThing->m_thing = thing;
    if (thing->set_object(PyEval_CallFunction(my_class,"(O)", (PyObject *)pyThing)) == -1) {
        if (PyErr_Occurred() == NULL) {
            cout << "Could not get python obj" << endl << flush;
        } else {
            cout << "Reporting python error for " << type << endl << flush;
            PyErr_Print();
        }
    }
}

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

static PyMethodDef atlas_methods[] = {
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

	if (Py_InitModule("atlas", atlas_methods) == NULL) {
		printf("Failed to Create cyphesis thing\n");
	} else {
		printf("Created cyphesis thing\n");
	}
}
