#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include "Python_API.h"

PyMethodDef Oplist_methods[] = {
    //{"update",		(PyCFunction)Oplist_update,	METH_VARARGS},
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
    if (self->ops == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid oplist");
        return NULL;
    }
    if ((PyTypeObject*)PyObject_Type(other) == & Oplist_Type) {
        OplistObject * opl = (OplistObject*)other;
        OplistObject * res = newOplistObject(NULL);
        res->ops = new oplist();
        if (res == NULL) {
            return NULL;
        }
        *res->ops = *self->ops;
        res->ops->merge(*opl->ops);
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
        *res->ops = *self->ops;
        res->ops->push_back(op->operation);
        return (PyObject*)res;
    }
    return NULL;
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
	0,
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
