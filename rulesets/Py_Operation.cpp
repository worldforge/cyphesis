#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include "Python_API.h"

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Entity/RootEntity.h>
#include <Atlas/Objects/Operation/RootOperation.h>

#include <common/utility.h>

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

static PyObject* Operation_get_name(RootOperationObject * self, PyObject * args)
{
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid operation");
        return NULL;
    }
    if (!PyArg_ParseTuple(args, "")) {
        PyErr_SetString(PyExc_TypeError,"too many args");
        return NULL;
    }
    return PyString_FromString("op");
}

/*
 * Operation sequence methods.
 */

static int Operation_seq_length(RootOperationObject * self)
{
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid operation");
        return 0;
    }
    return self->operation->GetArgs().size();
} 

static PyObject * Operation_seq_item(RootOperationObject * self, int item)
{
    printf("Operation_seq_item\n");
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid operation");
        return 0;
    }
    Object::ListType & args_list = self->operation->GetArgs();
    Object::ListType::iterator I = args_list.begin();
    int i;
    for(i = 0; i < item && I != args_list.end(); i++, I++);
    if (I != args_list.end()) {
        Object * obj = new Object(*I);
        Root * op = utility::Object_asRoot(*obj);
        if ((op != NULL) && (op->GetObjtype() == "op")) {
            RootOperationObject * ret_op = newAtlasRootOperation(NULL);
            ret_op->operation = (RootOperation *)op;
            return (PyObject *)ret_op;
        }
        AtlasObject * ret = newAtlasObject(NULL);
        ret->m_obj = obj;
        return (PyObject *)ret;
    }
    return NULL;
}

PyObject * Operation_num_add(RootOperationObject *self, PyObject *other)
{
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid operation");
    }
    if ((PyTypeObject*)PyObject_Type(other) == & Oplist_Type) {
        OplistObject * opl = (OplistObject*)other;
        if (opl->ops == NULL) {
            PyErr_SetString(PyExc_TypeError, "invalid oplist");
            return NULL;
        }
        OplistObject * res = newOplistObject(NULL);
        res->ops = new oplist();
        if (res == NULL) {
            return NULL;
        }
        *res->ops = *opl->ops;
        res->ops->push_back(self->operation);
        return (PyObject*)res;
    }
    if ((PyTypeObject*)PyObject_Type(other) == & RootOperation_Type) {
        RootOperationObject * op = (RootOperationObject*)other;
        if (op->operation == NULL) {
            PyErr_SetString(PyExc_TypeError, "invalid operation");
        }
        OplistObject * res = newOplistObject(NULL);
        res->ops = new oplist();
        if (res == NULL) {
            return NULL;
        }
        res->ops->push_back(op->operation);
        res->ops->push_back(self->operation);
        return (PyObject*)res;
    }
    return NULL;
}

/*
 * Operation numerical methods structure.
 */

static PyNumberMethods Operation_num = {
	(binaryfunc)Operation_num_add,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};

/*
 * Operation sequence methods structure.
 */

static PySequenceMethods Operation_seq = {
    (inquiry)Operation_seq_length,	/* sq_length */
    NULL,				/*  sq_concat */
    NULL,				/* sq_repeat */
    (intargfunc)Operation_seq_item,	/* sq_item */
    NULL,				/* sq_slice */
    NULL,				/* sq_ass_item */
    NULL				/* sq_ass_slice */
};

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
    cout << "Operation_getattr" << endl << flush;
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid operation");
        return NULL;
    }
    if (strcmp(name, "from_") == 0) {
        cout << "Operation_getattr(from)" << endl << flush;
        if (self->from != NULL) {
            ThingObject * thing_obj = newThingObject(NULL);
            if (thing_obj == NULL) {
                return NULL;
            }
            thing_obj->m_thing = self->from;
            return (PyObject *)thing_obj;
        } else {
            AtlasObject * obj = newAtlasObject(NULL);
            Object::MapType omap;
            omap["id"] = Object(self->operation->GetFrom());
            obj->m_obj = new Object(omap);
            return (PyObject *)obj;
        }
    } else if (strcmp(name, "to") == 0) {
        if (self->to != NULL) {
            ThingObject * thing_obj = newThingObject(NULL);
            if (thing_obj == NULL) {
                return NULL;
            }
            thing_obj->m_thing = self->to;
            return (PyObject *)thing_obj;
        } else {
            AtlasObject * obj = newAtlasObject(NULL);
            Object::MapType omap;
            omap["id"] = Object(self->operation->GetTo());
            obj->m_obj = new Object(omap);
            return (PyObject *)obj;
        }
    } else if (strcmp(name, "time") == 0) {
        OptimeObject * time_obj = newOptimeObject(NULL);
        if (time_obj == NULL) {
            return NULL;
        }
        time_obj->operation = self->operation;
        return (PyObject *)time_obj;
    } else if (strcmp(name, "id") == 0) {
        Object::ListType & parents = self->operation->GetParents();
        if ((parents.size() < 1) || (!parents.front().IsString())) {
            PyErr_SetString(PyExc_TypeError, "Operation has no parents");
            return NULL;
        }
        return PyString_FromString(parents.front().AsString().c_str());
    }
    return Py_FindMethod(RootOperation_methods, (PyObject *)self, name);
}

static int Operation_setattr(RootOperationObject *self, char *name, PyObject *v)
{
    cout << "Operation_setattr" << endl << flush;
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid operation");
        return -1;
    }
    if (strcmp(name, "from_") == 0) {
        cout << "Operation_setattr(from)" << endl << flush;
        PyObject * thing_id = PyObject_GetAttrString(v, "id");
        if ((thing_id == NULL) || (!PyString_Check(thing_id))) {
            PyErr_SetString(PyExc_TypeError, "invalid from");
            return -1;
        }
        if (((PyTypeObject*)PyObject_Type(v) == &Thing_Type) &&
            (((ThingObject *)v)->m_thing != NULL)) {
            self->from = ((ThingObject *)v)->m_thing;
        }
        self->operation->SetFrom(PyString_AsString(thing_id));
        return 0;
    }
    if (strcmp(name, "to") == 0) {
        cout << "Operation_setattr(to)" << endl << flush;
        PyObject * thing_id = PyObject_GetAttrString(v, "id");
        if ((thing_id == NULL) || (!PyString_Check(thing_id))) {
            PyErr_SetString(PyExc_TypeError, "invalid to");
            return -1;
        }
        if (((PyTypeObject*)PyObject_Type(v) == &Thing_Type) &&
            (((ThingObject *)v)->m_thing != NULL)) {
            self->to = ((ThingObject *)v)->m_thing;
        }
        self->operation->SetTo(PyString_AsString(thing_id));
        return 0;
    }
    return 0;
}

ATLAS_OPERATION_TYPE(RootOperation)

/*
 * Beginning of Operation creation functions section.
 */

RootOperationObject * newAtlasRootOperation(PyObject *arg)
{
	RootOperationObject * self;
	self = PyObject_NEW(RootOperationObject, &RootOperation_Type);
	if (self == NULL) {
		return(NULL);
	}
	self->Operation_attr = NULL;
	self->from = NULL;
	self->to = NULL;
	return self;
}
