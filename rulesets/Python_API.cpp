#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include "Python_API.h"

static PyMethodDef Thing_methods[] = {
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

static PyObject * Thing_getattr(ThingObject *self, char *name)
{
    return Py_FindMethod(Thing_methods, (PyObject *)self, name);
}

static int Thing_setattr(ThingObject *self, char *name, PyObject *v)
{
    return 0;
}

staticforward PyTypeObject Thing_Type = {
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

static ThingObject * newThingObject(PyObject *arg)
{
	ThingObject * self;
	self = PyObject_NEW(ThingObject, &Thing_Type);
	if (self == NULL) {
		return NULL;
	}
	self->Thing_attr = NULL;
	return self;
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

/*
 * Beginning of Object methods section.
 */

/*
 * Object methods structure.
 */

static PyMethodDef Object_methods[] = {
	/* {"demo",        (PyCFunction)Xxo_demo,  1}, */
	{NULL,          NULL}           /* sentinel */
};

/*
 * Beginning of Operation standard methods section.
 */

static void Object_dealloc(AtlasObject *self)
{
    if (self->m_obj != NULL) {
        delete self->m_obj;
    }
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

/*
 * Beginning of Object creation functions section.
 */

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

/*
 * Beginning of Operation section.
 *
 * This is a python type that wraps up operation objects from
 * Atlas::Objects::Operation namespace.
 *
 */

/*
 * Beginning of Operation methods section.
 */

static PyObject * Operation_SetSerialno(RootOperationObject * self, PyObject * args)
{
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid operation");
        return NULL;
    }
    // Takes integer, returns none
    int serialno;
    if (!PyArg_ParseTuple(args, "i", &serialno)) {
        PyErr_SetString(PyExc_TypeError,"serialno not an integer");
        return NULL;
    }
    self->operation->SetSerialno(serialno);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Operation_SetRefno(RootOperationObject * self, PyObject * args)
{
    // Takes integer, returns none
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid operation");
        return NULL;
    }
    int refno;
    if (!PyArg_ParseTuple(args, "i", &refno)) {
        PyErr_SetString(PyExc_TypeError,"refno not an integer");
        return NULL;
    }
    self->operation->SetRefno(refno);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Operation_SetFrom(RootOperationObject * self, PyObject * args)
{
    // Takes string, returns none
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid operation");
        return NULL;
    }
    char * from;
    if (!PyArg_ParseTuple(args, "s", &from)) {
        PyErr_SetString(PyExc_TypeError,"from not a string");
        return NULL;
    }
    self->operation->SetFrom(from);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Operation_SetTo(RootOperationObject * self, PyObject * args)
{
    // Takes string, returns none
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid operation");
        return NULL;
    }
    char * to;
    if (!PyArg_ParseTuple(args, "s", &to)) {
        PyErr_SetString(PyExc_TypeError,"to not a string");
        return NULL;
    }
    self->operation->SetTo(to);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Operation_SetSeconds(RootOperationObject * self, PyObject * args)
{
    // Takes float, returns none
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid operation");
        return NULL;
    }
    double seconds;
    if (!PyArg_ParseTuple(args, "d", &seconds)) {
        PyErr_SetString(PyExc_TypeError,"seconds not a float");
        return NULL;
    }
    self->operation->SetSeconds(seconds);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Operation_SetFutureSeconds(RootOperationObject * self, PyObject * args)
{
    // Takes float, returns none
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid operation");
        return NULL;
    }
    double futureseconds;
    if (!PyArg_ParseTuple(args, "d", &futureseconds)) {
        PyErr_SetString(PyExc_TypeError,"futureseconds not a float");
        return NULL;
    }
    self->operation->SetFutureSeconds(futureseconds);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Operation_SetTimeString(RootOperationObject * self, PyObject * args)
{
    // Takes string, returns none
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid operation");
        return NULL;
    }
    char * timestring;
    if (!PyArg_ParseTuple(args, "s", &timestring)) {
        PyErr_SetString(PyExc_TypeError,"timestring not a string");
        return NULL;
    }
    self->operation->SetTimeString(timestring);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Operation_SetArgs(RootOperationObject * self, PyObject * args)
{
    // Takes List, returns none
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid operation");
        return NULL;
    }
    PyObject * args_object;
    if (!PyArg_ParseTuple(args, "O", &args_object)) {
        PyErr_SetString(PyExc_TypeError,"args not an object");
        return NULL;
    }
    if (!PyList_Check(args_object)) {
        PyErr_SetString(PyExc_TypeError,"args not a list");
        return NULL;
    }
    Object::ListType argslist;
    for(int i = 0; i < PyList_Size(args_object); i++) {
        AtlasObject * item = (AtlasObject *)PyList_GetItem(args_object, i);
        if ((PyTypeObject*)PyObject_Type((PyObject *)item) != &Object_Type) {
            PyErr_SetString(PyExc_TypeError,"args contains non Atlas Object");
            return NULL;
        }
        
        argslist.push_back(*(item->m_obj));
    }
    self->operation->SetArgs(argslist);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Operation_GetSerialno(RootOperationObject * self, PyObject * args)
{
    // Returns int
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid operation");
        return NULL;
    }
    if (!PyArg_ParseTuple(args, "")) {
        PyErr_SetString(PyExc_TypeError,"too many args");
        return NULL;
    }
    return PyInt_FromLong(self->operation->GetSerialno());
}

static PyObject * Operation_GetRefno(RootOperationObject * self, PyObject * args)
{
    // Returns int
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid operation");
        return NULL;
    }
    if (!PyArg_ParseTuple(args, "")) {
        PyErr_SetString(PyExc_TypeError,"too many args");
        return NULL;
    }
    return PyInt_FromLong(self->operation->GetRefno());
}

static PyObject * Operation_GetFrom(RootOperationObject * self, PyObject * args)
{
    // Returns string
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid operation");
        return NULL;
    }
    if (!PyArg_ParseTuple(args, "")) {
        PyErr_SetString(PyExc_TypeError,"too many args");
        return NULL;
    }
    return PyString_FromString(self->operation->GetFrom().c_str());
}

static PyObject * Operation_GetTo(RootOperationObject * self, PyObject * args)
{
    // Returns string
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid operation");
        return NULL;
    }
    if (!PyArg_ParseTuple(args, "")) {
        PyErr_SetString(PyExc_TypeError,"too many args");
        return NULL;
    }
    return PyString_FromString(self->operation->GetTo().c_str());
}

static PyObject * Operation_GetSeconds(RootOperationObject * self, PyObject * args)
{
    // Returns float
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid operation");
        return NULL;
    }
    if (!PyArg_ParseTuple(args, "")) {
        PyErr_SetString(PyExc_TypeError,"too many args");
        return NULL;
    }
    return PyFloat_FromDouble(self->operation->GetSeconds());
}

static PyObject * Operation_GetFutureSeconds(RootOperationObject * self, PyObject * args)
{
    // Returns float
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid operation");
        return NULL;
    }
    if (!PyArg_ParseTuple(args, "")) {
        PyErr_SetString(PyExc_TypeError,"too many args");
        return NULL;
    }
    return PyFloat_FromDouble(self->operation->GetFutureSeconds());
}

static PyObject * Operation_GetTimeString(RootOperationObject * self, PyObject * args)
{
    // Returns string
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid operation");
        return NULL;
    }
    if (!PyArg_ParseTuple(args, "")) {
        PyErr_SetString(PyExc_TypeError,"too many args");
        return NULL;
    }
    return PyString_FromString(self->operation->GetTimeString().c_str());
}

static PyObject * Operation_GetArgs(RootOperationObject * self, PyObject * args)
{
    // Returns list
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid operation");
        return NULL;
    }
    if (!PyArg_ParseTuple(args, "")) {
        PyErr_SetString(PyExc_TypeError,"too many args");
        return NULL;
    }
    // Here we go:- 
    Object::ListType & args_list = self->operation->GetArgs();
    PyObject * args_pylist = PyList_New(args_list.size());
    Object::ListType::iterator I;
    int j=0;
    AtlasObject * item;
    for(I=args_list.begin();I!=args_list.end();I++,j++) {
        item = newAtlasObject(NULL);
        if (item == NULL) {
            PyErr_SetString(PyExc_TypeError,"error creating list");
            return NULL;
        }
        item->m_obj = new Object(*I);
        PyList_SetItem(args_pylist, j, (PyObject *)item);
    }
    return args_pylist;
}

/*
 * Operation methods structure.
 *
 * Generated from a macro in case we need one for each type of operation.
 *
 */

// At the moment it looks as though only one set of methods will be required
// as they can all use it together
ATLAS_OPERATION_METHODS(RootOperation)
//ATLAS_OPERATION_METHODS(Login)
//ATLAS_OPERATION_METHODS(Create)
//ATLAS_OPERATION_METHODS(Move)
//ATLAS_OPERATION_METHODS(Set)

/*
 * Beginning of Operation standard methods section.
 */

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

/*
 * Beginning of Operation creation functions section.
 */

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
		setenv("PYTHONPATH", cwd, 1);
	}

	Py_Initialize();

	if (Py_InitModule("cyphesis", cyphesis_methods) == NULL) {
		printf("Failed to Create cyphesis thing\n");
	} else {
		printf("Created cyphesis thing\n");
	}
}
