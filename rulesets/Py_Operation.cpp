// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_Operation.h"
#include "Py_Oplist.h"
#include "Py_Optime.h"
#include "Py_Object.h"
#include "Py_Thing.h"

#include <common/utility.h>

using Atlas::Message::Object;
using Atlas::Objects::Operation::RootOperation;

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

static PyObject * Operation_SetSerialno(OperationObject * self, PyObject * args)
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

static PyObject * Operation_SetRefno(OperationObject * self, PyObject * args)
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

static PyObject * Operation_SetFrom(OperationObject * self, PyObject * args)
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

static PyObject * Operation_SetTo(OperationObject * self, PyObject * args)
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

static PyObject * Operation_SetSeconds(OperationObject * self, PyObject * args)
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

static PyObject * Operation_SetFutureSeconds(OperationObject * self, PyObject * args)
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

static PyObject * Operation_SetTimeString(OperationObject * self, PyObject * args)
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

static PyObject * Operation_SetArgs(OperationObject * self, PyObject * args)
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
        if (!PyAtlasObject_Check(item)) {
            PyErr_SetString(PyExc_TypeError,"args contains non Atlas Object");
            return NULL;
        }
        
        argslist.push_back(*(item->m_obj));
    }
    self->operation->SetArgs(argslist);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Operation_GetSerialno(OperationObject * self, PyObject * args)
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

static PyObject * Operation_GetRefno(OperationObject * self, PyObject * args)
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

static PyObject * Operation_GetFrom(OperationObject * self, PyObject * args)
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

static PyObject * Operation_GetTo(OperationObject * self, PyObject * args)
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

static PyObject * Operation_GetSeconds(OperationObject * self, PyObject * args)
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

static PyObject * Operation_GetFutureSeconds(OperationObject * self, PyObject * args)
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

static PyObject * Operation_GetTimeString(OperationObject * self, PyObject * args)
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

static PyObject * Operation_GetArgs(OperationObject * self, PyObject * args)
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
    Object::ListType::const_iterator I;
    int j=0;
    AtlasObject * item;
    for(I = args_list.begin();I != args_list.end(); I++, j++) {
        item = newAtlasObject(NULL);
        if (item == NULL) {
            PyErr_SetString(PyExc_TypeError,"error creating list");
            Py_DECREF(args_pylist);
            return NULL;
        }
        item->m_obj = new Object(*I);
        PyList_SetItem(args_pylist, j, (PyObject *)item);
    }
    return args_pylist;
}

static PyObject* Operation_get_name(OperationObject * self, PyObject * args)
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

static int Operation_seq_length(OperationObject * self)
{
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid operation");
        return 0;
    }
    return self->operation->GetArgs().size();
} 

static PyObject * Operation_seq_item(OperationObject * self, int item)
{
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid operation");
        return 0;
    }
    Object::ListType & args_list = self->operation->GetArgs();
    Object::ListType::const_iterator I = args_list.begin();
    int i;
    for(i = 0; i < item && I != args_list.end(); i++, I++);
    if ((I != args_list.end()) && (I->IsMap())) {
        const Object::MapType & obj = I->AsMap();
        RootOperation op;
        bool isOp = utility::Object_asOperation(obj, op);
        if (isOp) {
            OperationObject * ret_op = newAtlasRootOperation(NULL);
            ret_op->operation = new RootOperation(op);
            ret_op->own = 1;
            return (PyObject *)ret_op;
        }
        AtlasObject * ret = newAtlasObject(NULL);
        ret->m_obj = new Object(obj);
        return (PyObject *)ret;
    }
    return NULL;
}

static PyObject * Operation_num_add(OperationObject *self, PyObject *other)
{
    fflush(stdout);
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid operation");
        fflush(stdout);
        return NULL;
    }
    if (other == Py_None) {
        OplistObject * res = newOplistObject(NULL);
        res->ops = new OpVector();
        res->ops->push_back(self->operation);
        self->own = 0;
        fflush(stdout);
        return (PyObject*)res;
    }
    if (PyOplist_Check(other)) {
        OplistObject * opl = (OplistObject*)other;
        if (opl->ops == NULL) {
            PyErr_SetString(PyExc_TypeError, "invalid OpVector");
            return NULL;
        }
        OplistObject * res = newOplistObject(NULL);
        if (res == NULL) {
            return NULL;
        }
        res->ops = new OpVector(*opl->ops);
        res->ops->push_back(self->operation);
        self->own = 0;
        fflush(stdout);
        return (PyObject*)res;
    }
    if (PyOperation_Check(other)) {
        OperationObject * op = (OperationObject*)other;
        if (op->operation == NULL) {
            PyErr_SetString(PyExc_TypeError, "invalid operation");
        }
        OplistObject * res = newOplistObject(NULL);
        if (res == NULL) {
            return NULL;
        }
        res->ops = new OpVector();
        res->ops->push_back(op->operation);
        op->own = 0;
        res->ops->push_back(self->operation);
        self->own = 0;
        fflush(stdout);
        return (PyObject*)res;
    }
    fflush(stdout);
    return NULL;
}

/*
 * Operation numerical methods.
 */

static int Operation_num_coerce(PyObject ** self, PyObject ** other)
{
    //if (*other == Py_None) {
        Py_INCREF(*self);
        Py_INCREF(*other);
        return 0;
    //}
    //return -1;
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
	Operation_num_coerce,
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

PyMethodDef RootOperation_methods[] = {
    {"SetSerialno",     (PyCFunction)Operation_SetSerialno,     METH_VARARGS},
    {"SetRefno",        (PyCFunction)Operation_SetRefno,        METH_VARARGS},
    {"SetFrom",         (PyCFunction)Operation_SetFrom,         METH_VARARGS},
    {"SetTo",           (PyCFunction)Operation_SetTo,           METH_VARARGS},
    {"SetSeconds",      (PyCFunction)Operation_SetSeconds,      METH_VARARGS},
    {"SetFutureSeconds",(PyCFunction)Operation_SetFutureSeconds,METH_VARARGS},
    {"SetTimeString",   (PyCFunction)Operation_SetTimeString,   METH_VARARGS},
    {"SetArgs",         (PyCFunction)Operation_SetArgs,         METH_VARARGS},
    {"GetSerialno",     (PyCFunction)Operation_GetSerialno,     METH_VARARGS},
    {"GetRefno",        (PyCFunction)Operation_GetRefno,        METH_VARARGS},
    {"GetFrom",         (PyCFunction)Operation_GetFrom,         METH_VARARGS},
    {"GetTo",           (PyCFunction)Operation_GetTo,           METH_VARARGS},
    {"GetSeconds",      (PyCFunction)Operation_GetSeconds,      METH_VARARGS},
    {"GetFutureSeconds",(PyCFunction)Operation_GetFutureSeconds,METH_VARARGS},
    {"GetTimeString",   (PyCFunction)Operation_GetTimeString,   METH_VARARGS},
    {"GetArgs",         (PyCFunction)Operation_GetArgs,         METH_VARARGS},
    {"get_name",        (PyCFunction)Operation_get_name,        METH_VARARGS},
    {NULL,          NULL}
};

PyMethodDef ConstRootOperation_methods[] = {
    {"GetSerialno",     (PyCFunction)Operation_GetSerialno,     METH_VARARGS},
    {"GetRefno",        (PyCFunction)Operation_GetRefno,        METH_VARARGS},
    {"GetFrom",         (PyCFunction)Operation_GetFrom,         METH_VARARGS},
    {"GetTo",           (PyCFunction)Operation_GetTo,           METH_VARARGS},
    {"GetSeconds",      (PyCFunction)Operation_GetSeconds,      METH_VARARGS},
    {"GetFutureSeconds",(PyCFunction)Operation_GetFutureSeconds,METH_VARARGS},
    {"GetTimeString",   (PyCFunction)Operation_GetTimeString,   METH_VARARGS},
    {"GetArgs",         (PyCFunction)Operation_GetArgs,         METH_VARARGS},
    {"get_name",        (PyCFunction)Operation_get_name,        METH_VARARGS},
    {NULL,          NULL}
};


/*
 * Beginning of Operation standard methods section.
 */

static void Operation_dealloc(OperationObject *self)
{
        if ((self->own != 0) && (self->operation != NULL)) {
            // Can't delete until I have sorted out bugs with own flag
            delete self->operation;
        }
	PyMem_DEL(self);
}

static inline PyObject * findMethod(OperationObject * self, char * name)
{
    return Py_FindMethod(RootOperation_methods, (PyObject *)self, name);
}

static inline PyObject * findMethod(ConstOperationObject * self, char * name)
{
    return Py_FindMethod(ConstRootOperation_methods, (PyObject *)self, name);
}

static inline PyObject * handleTime(OperationObject * self)
{
    OptimeObject * time_obj = newOptimeObject(NULL);
    if (time_obj == NULL) {
        return NULL;
    }
    time_obj->operation = self->operation;
    return (PyObject *)time_obj;
}

static inline PyObject * handleTime(ConstOperationObject * self)
{
    // If it becomes necessary for python scripts to query the time
    // of ops, this can be adapted to provide them with that information
    PyErr_SetString(PyExc_TypeError, "cannot get time on const ops");
    return NULL;
}

template <typename T>
static PyObject * getattr(T * self, char * name)
{
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid operation");
        return NULL;
    }
    if (strcmp(name, "from_") == 0) {
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
        return handleTime(self);
    } else if (strcmp(name, "id") == 0) {
        const Object::ListType & parents = self->operation->GetParents();
        if ((parents.empty()) || (!parents.front().IsString())) {
            PyErr_SetString(PyExc_TypeError, "Operation has no parents");
            return NULL;
        }
        return PyString_FromString(parents.front().AsString().c_str());
    }
    return findMethod(self, name);
}

static PyObject * Operation_getattr(OperationObject * self, char * name)
{
    return getattr(self, name);
}

static PyObject * ConstOperation_getattr(ConstOperationObject * self, char * name)
{
    return getattr(self, name);
}


static int Operation_setattr(OperationObject *self, char *name, PyObject *v)
{
    if (self->operation == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid operation");
        return -1;
    }
    if (strcmp(name, "from_") == 0) {
        PyObject * thing_id = PyObject_GetAttrString(v, "id");
        if ((thing_id == NULL) || (!PyString_Check(thing_id))) {
            PyErr_SetString(PyExc_TypeError, "invalid from");
            if (thing_id != NULL) {
                Py_DECREF(thing_id);
            }
            return -1;
        }
        if (((PyTypeObject*)PyObject_Type(v) == &Thing_Type) &&
            (((ThingObject *)v)->m_thing != NULL)) {
            self->from = ((ThingObject *)v)->m_thing;
        }
        self->operation->SetFrom(PyString_AsString(thing_id));
        Py_DECREF(thing_id);
        return 0;
    }
    if (strcmp(name, "to") == 0) {
        PyObject * thing_id = PyObject_GetAttrString(v, "id");
        if ((thing_id == NULL) || (!PyString_Check(thing_id))) {
            PyErr_SetString(PyExc_TypeError, "invalid to");
            if (thing_id != NULL) {
                Py_DECREF(thing_id);
            }
            return -1;
        }
        if (((PyTypeObject*)PyObject_Type(v) == &Thing_Type) &&
            (((ThingObject *)v)->m_thing != NULL)) {
            self->to = ((ThingObject *)v)->m_thing;
        }
        self->operation->SetTo(PyString_AsString(thing_id));
        Py_DECREF(thing_id);
        return 0;
    }
    return 0;
}

PyTypeObject Operation_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      // ob_size
        "Operation",                            // tp_name
        sizeof(OperationObject),            // tp_basicsize
        0,                                      // tp_itemsize
        //  methods 
        (destructor)Operation_dealloc,          // tp_dealloc
        0,                                      // tp_print
        (getattrfunc)Operation_getattr,  // tp_getattr
        (setattrfunc)Operation_setattr,         // tp_setattr
        0,                                      // tp_compare
        0,                                      // tp_repr
        &Operation_num,                         // tp_as_number
        &Operation_seq,                         // tp_as_sequence
        0,                                      // tp_as_mapping
        0,                                      // tp_hash
};

PyTypeObject ConstOperation_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      // ob_size
        "Operation",                            // tp_name
        sizeof(ConstOperationObject),           // tp_basicsize
        0,                                      // tp_itemsize
        //  methods 
        (destructor)Operation_dealloc,          // tp_dealloc
        0,                                      // tp_print
        (getattrfunc)ConstOperation_getattr,    // tp_getattr
        0,                                      // tp_setattr
        0,                                      // tp_compare
        0,                                      // tp_repr
        &Operation_num,                         // tp_as_number
        &Operation_seq,                         // tp_as_sequence
        0,                                      // tp_as_mapping
        0,                                      // tp_hash
};

/*
 * Beginning of Operation creation functions section.
 */

OperationObject * newAtlasRootOperation(PyObject *arg)
{
	OperationObject * self;
	self = PyObject_NEW(OperationObject, &Operation_Type);
	if (self == NULL) {
		return NULL;
	}
	self->operation = NULL;
	self->from = NULL;
	self->to = NULL;
	self->own = 0;
	return self;
}

ConstOperationObject * newAtlasConstRootOperation(PyObject *arg)
{
	ConstOperationObject * self;
	self = PyObject_NEW(ConstOperationObject, &ConstOperation_Type);
	if (self == NULL) {
		return NULL;
	}
	self->operation = NULL;
	self->from = NULL;
	self->to = NULL;
	self->own = 0;
	return self;
}
