// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_Map.h"
#include "Py_Location.h"
#include "Py_Thing.h"
#include "Py_Operation.h"
#include "Py_Object.h"

#include "MemMap.h"

static PyObject * Map_find_by_location(PyMap * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Map in Map.find_by_location");
        return NULL;
    }
#endif // NDEBUG
    double radius;
    PyObject * where_obj;
    if (!PyArg_ParseTuple(args, "Od", &where_obj, &radius)) {
        return NULL;
    }
    if (!PyLocation_Check(where_obj)) {
        PyErr_SetString(PyExc_TypeError, "Argument must be a location");
        return NULL;
    }
    PyLocation * where = (PyLocation *)where_obj;
    PyEntity * thing;
    EntityVector res = self->m_map->findByLocation(*where->location,
                                                          radius);
    PyObject * list = PyList_New(res.size());
    if (list == NULL) {
        return NULL;
    } 
    EntityVector::const_iterator I;
    int i = 0;
    for(I = res.begin(); I != res.end(); I++, i++) {
        thing = newPyEntity();
        if (thing == NULL) {
            Py_DECREF(list);
            return NULL;
        }
        thing->m_entity = *I;
        PyList_SetItem(list, i, (PyObject*)thing);
    }
    return list;
}

static PyObject * Map_find_by_type(PyMap * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Map in Map.find_by_type");
        return NULL;
    }
#endif // NDEBUG
    char * what;
    if (!PyArg_ParseTuple(args, "s", &what)) {
        return NULL;
    }
    PyEntity * thing;
    EntityVector res = self->m_map->findByType(std::string(what));
    PyObject * list = PyList_New(res.size());
    if (list == NULL) {
        return NULL;
    } 
    EntityVector::const_iterator I;
    int i = 0;
    for(I = res.begin(); I != res.end(); I++, i++) {
        thing = newPyEntity();
        if (thing == NULL) {
            Py_DECREF(list);
            return NULL;
        }
        thing->m_entity = *I;
        PyList_SetItem(list, i, (PyObject*)thing);
    }
    return list;
}

#if 0
static PyObject * Map_add_object(PyMap * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Map in Map.add_object");
        return NULL;
    }
#endif // NDEBUG
    PyEntity * thing;
    if (!PyArg_ParseTuple(args, "O", &thing)) {
        return NULL;
    }
    if (!PyEntity_Check(thing)) {
        PyErr_SetString(PyExc_TypeError,"arg not a Entity");
        return NULL;
    }
    Entity * ret = self->m_map->addObject(thing->m_entity);
    thing = newPyEntity();
    thing->m_entity = ret;
    return (PyObject *)thing;
}
#endif

static PyObject * Map_look_id(PyMap * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Map in Map.look_id");
        return NULL;
    }
#endif // NDEBUG
    if (!PyArg_ParseTuple(args, "")) {
        return NULL;
    }
    RootOperation * op = self->m_map->lookId();
    if (op == NULL) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    PyOperation * py_op = newPyOperation();
    py_op->operation = op;
    py_op->own = 1;
    return (PyObject *)py_op;
}

static PyObject * Map_add(PyMap * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Map in Map.add");
        return NULL;
    }
#endif // NDEBUG
    PyMessageElement * obj;
    if (!PyArg_ParseTuple(args, "O", &obj)) {
        return NULL;
    }
    if (!PyMessageElement_Check(obj)) {
        PyErr_SetString(PyExc_TypeError,"arg is not an Object");
        return NULL;
    }
    Entity * ret = self->m_map->add(obj->m_obj->asMap());
    PyEntity * thing = newPyEntity();
    thing->m_entity = ret;
    return (PyObject *)thing;
}

static PyObject * Map_delete(PyMap * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Map in Map.delete");
        return NULL;
    }
#endif // NDEBUG
    char * id;
    if (!PyArg_ParseTuple(args, "s", &id)) {
        return NULL;
    }
    self->m_map->del(id);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Map_get(PyMap * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Map in Map.get");
        return NULL;
    }
#endif // NDEBUG
    char * id;
    if (!PyArg_ParseTuple(args, "s", &id)) {
        return NULL;
    }
    Entity * ret = self->m_map->get(id);
    if (ret == NULL) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    PyEntity * thing = newPyEntity();
    thing->m_entity = ret;
    return (PyObject *)thing;
}

static PyObject * Map_get_add(PyMap * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Map in Map.get_add");
        return NULL;
    }
#endif // NDEBUG
    char * id;
    if (!PyArg_ParseTuple(args, "s", &id)) {
        return NULL;
    }
    Entity * ret = self->m_map->getAdd(id);
    PyEntity * thing = newPyEntity();
    thing->m_entity = ret;
    return (PyObject *)thing;
}

static PyObject * Map_update(PyMap * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Map in Map.update");
        return NULL;
    }
#endif // NDEBUG
    PyMessageElement * obj;
    if (!PyArg_ParseTuple(args, "O", &obj)) {
        return NULL;
    }
    if (!PyMessageElement_Check(obj)) {
        PyErr_SetString(PyExc_TypeError,"arg is not an Object");
        return NULL;
    }
    Entity * ret = self->m_map->update(obj->m_obj->asMap());
    PyEntity * thing = newPyEntity();
    thing->m_entity = ret;
    return (PyObject *)thing;
}

static PyObject * Map_add_hooks_append(PyMap * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Map in Map.add_hooks_append");
        return NULL;
    }
#endif // NDEBUG
    char * method;
    if (!PyArg_ParseTuple(args, "s", &method)) {
        return NULL;
    }
    self->m_map->getAddHooks().push_back(std::string(method));

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Map_update_hooks_append(PyMap * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Map in Map.update_hooks_append");
        return NULL;
    }
#endif // NDEBUG
    char * method;
    if (!PyArg_ParseTuple(args, "s", &method)) {
        return NULL;
    }
    self->m_map->getUpdateHooks().push_back(std::string(method));

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Map_delete_hooks_append(PyMap * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_map == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL Map in Map.delete_hooks_append");
        return NULL;
    }
#endif // NDEBUG
    char * method;
    if (!PyArg_ParseTuple(args, "s", &method)) {
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

static void Map_dealloc(PyMap *self)
{
    //if (self->m_entity != NULL) {
        //delete self->m_entity;
    //}
    PyMem_DEL(self);
}

static PyObject * Map_getattr(PyMap *self, char *name)
{
    return Py_FindMethod(Map_methods, (PyObject *)self, name);
}

static int Map_setattr(PyMap *self, char *name, PyObject *v)
{
    return 0;
}

PyTypeObject Map_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,				/*ob_size*/
	"Map",				/*tp_name*/
	sizeof(PyMap),		/*tp_basicsize*/
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

PyMap * newPyMap()
{
	PyMap * self;
	self = PyObject_NEW(PyMap, &Map_Type);
	if (self == NULL) {
		return NULL;
	}
	return self;
}
