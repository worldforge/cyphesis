#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include "Python_API.h"
#include "MemMap.h"

static PyObject * Map_add_object(MapObject * self, PyObject * args)
{
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid memmap");
        return NULL;
    }
    ThingObject * thing;
    if (!PyArg_ParseTuple(args, "O", &thing)) {
        PyErr_SetString(PyExc_TypeError,"arg not an object");
        return NULL;
    }
    if ((PyTypeObject*)PyObject_Type((PyObject *)thing) != &Thing_Type) {
        PyErr_SetString(PyExc_TypeError,"arg not a Thing");
        return NULL;
    }
    Thing * ret = self->m_map->add_object(thing->m_thing);
    thing = newThingObject(NULL);
    thing->m_thing = ret;
    return (PyObject *)thing;
}

static PyObject * Map_look_id(MapObject * self, PyObject * args)
{
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid memmap");
        return NULL;
    }
    if (!PyArg_ParseTuple(args, "")) {
        PyErr_SetString(PyExc_TypeError,"expected 1, got multiple");
        return NULL;
    }
    RootOperation * op = self->m_map->look_id();
    RootOperationObject * py_op = newAtlasRootOperation(NULL);
    py_op->operation = op;
    return (PyObject *)py_op;
}

static PyObject * Map_add(MapObject * self, PyObject * args)
{
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid memmap");
        return NULL;
    }
    AtlasObject * obj;
    if (!PyArg_ParseTuple(args, "O", &obj)) {
        PyErr_SetString(PyExc_TypeError,"arg is not an object");
        return NULL;
    }
    if ((PyTypeObject*)PyObject_Type((PyObject *)obj) != &Object_Type) {
        PyErr_SetString(PyExc_TypeError,"arg is not an Object");
        return NULL;
    }
    Thing * ret = self->m_map->add(*(obj->m_obj));
    ThingObject * thing = newThingObject(NULL);
    thing->m_thing = ret;
    return (PyObject *)thing;
}

static PyObject * Map_delete(MapObject * self, PyObject * args)
{
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid memmap");
        return NULL;
    }
    char * id;
    if (!PyArg_ParseTuple(args, "s", &id)) {
        PyErr_SetString(PyExc_TypeError,"id not a string");
        return NULL;
    }
    self->m_map->_delete(id);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Map_get(MapObject * self, PyObject * args)
{
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid memmap");
        return NULL;
    }
    char * id;
    if (!PyArg_ParseTuple(args, "s", &id)) {
        PyErr_SetString(PyExc_TypeError,"id not a string");
        return NULL;
    }
    Thing * ret = self->m_map->get(id);
    ThingObject * thing = newThingObject(NULL);
    thing->m_thing = ret;
    return (PyObject *)thing;
}

static PyObject * Map_get_add(MapObject * self, PyObject * args)
{
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid memmap");
        return NULL;
    }
    char * id;
    if (!PyArg_ParseTuple(args, "s", &id)) {
        PyErr_SetString(PyExc_TypeError,"id not a string");
        return NULL;
    }
    Thing * ret = self->m_map->get_add(id);
    ThingObject * thing = newThingObject(NULL);
    thing->m_thing = ret;
    return (PyObject *)thing;
}

static PyObject * Map_update(MapObject * self, PyObject * args)
{
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid memmap");
        return NULL;
    }
    AtlasObject * obj;
    if (!PyArg_ParseTuple(args, "O", &obj)) {
        PyErr_SetString(PyExc_TypeError,"arg is not an object");
        return NULL;
    }
    if ((PyTypeObject*)PyObject_Type((PyObject *)obj) != &Object_Type) {
        PyErr_SetString(PyExc_TypeError,"arg is not an Object");
        return NULL;
    }
    Thing * ret = self->m_map->update(*(obj->m_obj));
    ThingObject * thing = newThingObject(NULL);
    thing->m_thing = ret;
    return (PyObject *)thing;
}


PyMethodDef Map_methods[] = {
    {"add_object",	(PyCFunction)Map_add_object,	METH_VARARGS},
    {"look_id",		(PyCFunction)Map_look_id,	METH_VARARGS},
    {"add",		(PyCFunction)Map_add,		METH_VARARGS},
    {"delete",		(PyCFunction)Map_delete,	METH_VARARGS},
    {"get",		(PyCFunction)Map_get,		METH_VARARGS},
    {"get_add",		(PyCFunction)Map_get_add,	METH_VARARGS},
    {"update",		(PyCFunction)Map_update,	METH_VARARGS},
    {NULL,		NULL}           /* sentinel */
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
	"Map",				/*tp_name*/
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
