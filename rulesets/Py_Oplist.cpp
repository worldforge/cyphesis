#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include "Python_API.h"

static PyObject* Oplist_append(OplistObject * self, PyObject * args)
{
    if (self->ops == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid oplist");
        return NULL;
    }
    PyObject * op;
    if (!PyArg_ParseTuple(args, "O", &op)) {
        return NULL;
    }
    if ((PyTypeObject*)PyObject_Type(op) == &RootOperation_Type) {
        self->ops->push_back(((RootOperationObject*)op)->operation);
    } else if (op != Py_None) {
        PyErr_SetString(PyExc_TypeError, "Append must be an op");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}


PyMethodDef Oplist_methods[] = {
    {"append",		(PyCFunction)Oplist_append,	METH_VARARGS},
    {NULL,		NULL}           /* sentinel */
};

static void Oplist_dealloc(OplistObject *self)
{
    Py_XDECREF(self->Oplist_attr);
    PyMem_DEL(self);
}

PyObject * Oplist_getattr(OplistObject *self, char *name)
{
    return Py_FindMethod(Oplist_methods, (PyObject *)self, name);
}


PyObject * Oplist_num_add(OplistObject *self, PyObject *other)
{
    printf("Adding to an oplist\n");
    if (self->ops == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid oplist");
        return NULL;
    }
    if (other == Py_None) {
        printf("Adding None to an oplist\n");
        //OplistObject * res = newOplistObject(NULL);
        //res->ops = new oplist();
        //res->ops->merge(*self->ops);
        //return (PyObject*)res;
        Py_INCREF(self);
        return (PyObject*)self;
    }
    if ((PyTypeObject*)PyObject_Type(other) == & Oplist_Type) {
        printf("Adding oplist to an oplist\n");
        OplistObject * opl = (OplistObject*)other;
        OplistObject * res = newOplistObject(NULL);
        res->ops = new oplist();
        if (res == NULL) {
            return NULL;
        }
        res->ops->merge(*self->ops);
        res->ops->merge(*opl->ops);
        return (PyObject*)res;
    }
    if ((PyTypeObject*)PyObject_Type(other) == & RootOperation_Type) {
        printf("Adding operation to an oplist\n");
        RootOperationObject * op = (RootOperationObject*)other;
        if (op->operation == NULL) {
            PyErr_SetString(PyExc_TypeError, "invalid operation");
        }
        OplistObject * res = newOplistObject(NULL);
        res->ops = new oplist();
        if (res == NULL) {
            return NULL;
        }
        res->ops->merge(*self->ops);
        res->ops->push_back(op->operation);
        op->own = 0;
        return (PyObject*)res;
    }
    printf("Failed to find out how to add it\n");
    return NULL;
}

static int Oplist_num_coerce(PyObject ** self, PyObject ** other)
{
    //if (*other == Py_None) {
        Py_INCREF(*self);
        Py_INCREF(*other);
        return(0);
    //}
    //return -1;
}

static PyNumberMethods Oplist_as_number = {
	(binaryfunc)Oplist_num_add,
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
	Oplist_num_coerce,
	0,
	0,
	0,
	0,
	0
};

PyTypeObject Oplist_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,				/*ob_size*/
	"Oplist",			/*tp_name*/
	sizeof(OplistObject),		/*tp_basicsize*/
	0,				/*tp_itemsize*/
	/* methods */
	(destructor)Oplist_dealloc,	/*tp_dealloc*/
	0,				/*tp_print*/
	(getattrfunc)Oplist_getattr,	/*tp_getattr*/
	0,				/*tp_setattr*/
	0,				/*tp_compare*/
	0,				/*tp_repr*/
	&Oplist_as_number,		/*tp_as_number*/
	0,				/*tp_as_sequence*/
	0,				/*tp_as_mapping*/
	0,				/*tp_hash*/
};

OplistObject * newOplistObject(PyObject *arg)
{
	OplistObject * self;
	self = PyObject_NEW(OplistObject, &Oplist_Type);
	if (self == NULL) {
		return NULL;
	}
	self->Oplist_attr = NULL;
	return self;
}
