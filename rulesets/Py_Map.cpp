// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_Map.h"
#include "Py_Location.h"
#include "Py_Thing.h"
#include "Py_Operation.h"
#include "Py_Object.h"

#include "MemMap_methods.h"

static PyObject * Map_find_by_location(MapObject * self, PyObject * args)
{
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid memmap");
        return NULL;
    }
    double radius;
    PyObject * where_obj;
    if (!PyArg_ParseTuple(args, "Od", &where_obj, &radius)) {
        return NULL;
    }
    if (!PyLocation_Check(where_obj)) {
        PyErr_SetString(PyExc_TypeError, "Argument must be a location");
        return NULL;
    }
    LocationObject * where = (LocationObject *)where_obj;
    ThingObject * thing;
    evec_t res = self->m_map->findByLocation(*where->location,
                                                          radius);
    PyObject * list = PyList_New(res.size());
    if (list == NULL) {
        return NULL;
    } 
    evec_t::const_iterator I;
    int i = 0;
    for(I = res.begin(); I != res.end(); I++, i++) {
        thing = newThingObject(NULL);
        if (thing == NULL) {
            Py_DECREF(list);
            return NULL;
        }
        thing->m_thing = *I;
        PyList_SetItem(list, i, (PyObject*)thing);
    }
    return list;
}

static PyObject * Map_find_by_type(MapObject * self, PyObject * args)
{
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid memmap");
        return NULL;
    }
    char * what;
    if (!PyArg_ParseTuple(args, "s", &what)) {
        return NULL;
    }
    ThingObject * thing;
    evec_t res = self->m_map->findByType(std::string(what));
    PyObject * list = PyList_New(res.size());
    if (list == NULL) {
        return NULL;
    } 
    evec_t::const_iterator I;
    int i = 0;
    for(I = res.begin(); I != res.end(); I++, i++) {
        thing = newThingObject(NULL);
        if (thing == NULL) {
            Py_DECREF(list);
            return NULL;
        }
        thing->m_thing = *I;
        PyList_SetItem(list, i, (PyObject*)thing);
    }
    return list;
}

#if 0
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
    if (!PyThing_Check(thing)) {
        PyErr_SetString(PyExc_TypeError,"arg not a Thing");
        return NULL;
    }
    Entity * ret = self->m_map->addObject(thing->m_thing);
    thing = newThingObject(NULL);
    thing->m_thing = ret;
    return (PyObject *)thing;
}
#endif

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
    RootOperation * op = self->m_map->lookId();
    if (op == NULL) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    OperationObject * py_op = newAtlasRootOperation(NULL);
    py_op->operation = op;
    py_op->own = 1;
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
    if (!PyAtlasObject_Check(obj)) {
        PyErr_SetString(PyExc_TypeError,"arg is not an Object");
        return NULL;
    }
    Entity * ret = self->m_map->add(*(obj->m_obj));
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
    self->m_map->del(id);

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
    Entity * ret = self->m_map->get(id);
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
    Entity * ret = self->m_map->getAdd(id);
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
    if (!PyAtlasObject_Check(obj)) {
        PyErr_SetString(PyExc_TypeError,"arg is not an Object");
        return NULL;
    }
    Entity * ret = self->m_map->update(*(obj->m_obj));
    ThingObject * thing = newThingObject(NULL);
    thing->m_thing = ret;
    return (PyObject *)thing;
}

static PyObject * Map_add_hooks_append(MapObject * self, PyObject * args)
{
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid memmap");
        return NULL;
    }
    char * method;
    if (!PyArg_ParseTuple(args, "s", &method)) {
        PyErr_SetString(PyExc_TypeError,"arg is not an string");
        return NULL;
    }
    self->m_map->getAddHooks().push_back(std::string(method));

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Map_update_hooks_append(MapObject * self, PyObject * args)
{
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid memmap");
        return NULL;
    }
    char * method;
    if (!PyArg_ParseTuple(args, "s", &method)) {
        PyErr_SetString(PyExc_TypeError,"arg is not an string");
        return NULL;
    }
    self->m_map->getUpdateHooks().push_back(std::string(method));

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Map_delete_hooks_append(MapObject * self, PyObject * args)
{
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_TypeError, "invalid memmap");
        return NULL;
    }
    char * method;
    if (!PyArg_ParseTuple(args, "s", &method)) {
        PyErr_SetString(PyExc_TypeError,"arg is not an string");
        return NULL;
    }
    self->m_map->getDeleteHooks().push_back(std::string(method));

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef Map_methods[] = {
    {"find_by_location",(PyCFunction)Map_find_by_location,	METH_VARARGS},
    {"find_by_type",	(PyCFunction)Map_find_by_type,	METH_VARARGS},
    //{"add_object",	(PyCFunction)Map_add_object,	METH_VARARGS},
    {"look_id",		(PyCFunction)Map_look_id,	METH_VARARGS},
    {"add",		(PyCFunction)Map_add,		METH_VARARGS},
    {"delete",		(PyCFunction)Map_delete,	METH_VARARGS},
    {"get",		(PyCFunction)Map_get,		METH_VARARGS},
    {"get_add",		(PyCFunction)Map_get_add,	METH_VARARGS},
    {"update",		(PyCFunction)Map_update,	METH_VARARGS},
    {"add_hooks_append",(PyCFunction)Map_add_hooks_append,	METH_VARARGS},
    {"update_hooks_append",	(PyCFunction)Map_update_hooks_append,	METH_VARARGS},
    {"delete_hooks_append",	(PyCFunction)Map_delete_hooks_append,	METH_VARARGS},
    {NULL,		NULL}           /* sentinel */
};

static void Map_dealloc(MapObject *self)
{
    //if (self->m_thing != NULL) {
        //delete self->m_thing;
    //}
    PyMem_DEL(self);
}

static PyObject * Map_getattr(MapObject *self, char *name)
{
    return Py_FindMethod(Map_methods, (PyObject *)self, name);
}

static int Map_setattr(MapObject *self, char *name, PyObject *v)
{
    return 0;
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
	return self;
}
