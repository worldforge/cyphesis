// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_Object.h"
#include "Py_Operation.h"
#include "Py_Oplist.h"
#include "Py_Location.h"

#include <modules/Location.h>
/*
 * Beginning of Object methods section.
 */

static PyObject* Object_get_name(AtlasObject * self, PyObject * args)
{
    if (self->m_obj == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid atlas object");
        return NULL;
    }
    if (!PyArg_ParseTuple(args, "")) {
        PyErr_SetString(PyExc_TypeError,"too many args");
        return NULL;
    }
    return PyString_FromString("obj");
}

/*
 * Object methods structure.
 */

static PyMethodDef Object_methods[] = {
	{"get_name",    (PyCFunction)Object_get_name,  1},
	{NULL,          NULL}           /* sentinel */
};

/*
 * Beginning of Object standard methods section.
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
    if (self->m_obj == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid object");
        return NULL;
    }
    if (self->m_obj->IsMap()) {
        const Fragment::MapType & omap = self->m_obj->AsMap();
        Fragment::MapType::const_iterator I = omap.find(name);
        if (I != omap.end()) {
            return Object_asPyObject(I->second);
        }
    }
    if (self->Object_attr != NULL) {
        PyObject *v = PyDict_GetItemString(self->Object_attr, name);
        if (v != NULL) {
            Py_INCREF(v);
            return v;
        }
    }
    return Py_FindMethod(Object_methods, (PyObject *)self, name);
}

static int Object_setattr( AtlasObject *self, char *name, PyObject *v)
{
    if (self->m_obj == NULL) {
        PyErr_SetString(PyExc_TypeError,"invalid object");
        return -1;
    }
    if (self->m_obj->IsMap()) {
        Fragment::MapType & omap = self->m_obj->AsMap();
        Fragment v_obj = PyObject_asObject(v);
        if ((v_obj.GetType() != Fragment::TYPE_NONE) &&
            (v_obj.GetType() != Fragment::TYPE_MAP) &&
            (v_obj.GetType() != Fragment::TYPE_LIST)) {
            omap[name] = v_obj;
            return 0;
        }
    }
    if (self->Object_attr == NULL) {
        self->Object_attr = PyDict_New();
        if (self->Object_attr == NULL) {
            return -1;
        }
    }
    return PyDict_SetItemString(self->Object_attr, name, v);
}

PyTypeObject Object_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,				/*ob_size*/
	"AtlasObject",			/*tp_name*/
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

AtlasObject * newAtlasObject(PyObject *arg)
{
	AtlasObject * self;
	self = PyObject_NEW(AtlasObject, &Object_Type);
	if (self == NULL) {
		return NULL;
	}
	self->Object_attr = NULL;
	return self;
}

/*
 * Utility functions to munge between Object related types and python types
 */

static PyObject * MapType_asPyObject(const Fragment::MapType & map)
{
    PyObject * args_pydict = PyDict_New();
    Fragment::MapType::const_iterator I;
    AtlasObject * item;
    for(I=map.begin();I!=map.end();I++) {
        const std::string & key = I->first;
        item = newAtlasObject(NULL);
        if (item == NULL) {
            PyErr_SetString(PyExc_TypeError,"error creating map");
            return NULL;
        }
        item->m_obj = new Fragment(I->second);
        // PyDict_SetItem() does not eat the reference passed to it
        PyDict_SetItemString(args_pydict,(char *)key.c_str(),(PyObject *)item);
        Py_DECREF(item);
    }
    return args_pydict;
}

static PyObject * ListType_asPyObject(const Fragment::ListType & list)
{
    PyObject * args_pylist = PyList_New(list.size());
    Fragment::ListType::const_iterator I;
    int j=0;
    AtlasObject * item;
    for(I=list.begin();I!=list.end();I++,j++) {
        item = newAtlasObject(NULL);
        if (item == NULL) {
            PyErr_SetString(PyExc_TypeError,"error creating list");
            return NULL;
        }
        item->m_obj = new Fragment(*I);
        // PyList_SetItem() eats the reference passed to it
        PyList_SetItem(args_pylist, j, (PyObject *)item);
    }
    return args_pylist;
}

PyObject * Object_asPyObject(const Fragment & obj)
{
    PyObject * ret = NULL;
    switch (obj.GetType()) {
        case Fragment::TYPE_INT:
            ret = PyInt_FromLong(obj.AsInt());
            break;
        case Fragment::TYPE_FLOAT:
            ret = PyFloat_FromDouble(obj.AsFloat());
            break;
        case Fragment::TYPE_STRING:
            ret = PyString_FromString(obj.AsString().c_str());
            break;
        case Fragment::TYPE_MAP:
            ret = MapType_asPyObject(obj.AsMap());
            break;
        case Fragment::TYPE_LIST:
            ret = ListType_asPyObject(obj.AsList());
            break;
        default:
            break;
    }
    return ret;
}

Fragment::ListType PyListObject_asListType(PyObject * list)
{
    Fragment::ListType argslist;
    AtlasObject * item;
    for(int i = 0; i < PyList_Size(list); i++) {
        item = (AtlasObject *)PyList_GetItem(list, i);
        if (PyAtlasObject_Check(item)) {
            argslist.push_back(*(item->m_obj));
        } else {
            Fragment o = PyObject_asObject((PyObject*)item);
            if (o.GetType() != Fragment::TYPE_NONE) {
                argslist.push_back(o);
            }
        }
    }
    return argslist;
}

Fragment::MapType PyDictObject_asMapType(PyObject * dict)
{
    Fragment::MapType argsmap;
    AtlasObject * item;
    PyObject * keys = PyDict_Keys(dict);
    PyObject * vals = PyDict_Values(dict);
    for(int i = 0; i < PyDict_Size(dict); i++) {
        PyObject * key = PyList_GetItem(keys, i);
        item = (AtlasObject *)PyList_GetItem(vals, i);
        if (PyAtlasObject_Check(item)) {
            argsmap[PyString_AsString(key)] = *(item->m_obj);
        } else {
            Fragment o = PyObject_asObject((PyObject*)item);
            if (o.GetType() != Fragment::TYPE_NONE) {
                argsmap[PyString_AsString(key)] = o;
            }
        }
    }
    Py_DECREF(keys);
    Py_DECREF(vals);
    return argsmap;
}

Fragment PyObject_asObject(PyObject * o)
{
    if (PyInt_Check(o)) {
        return Fragment((int)PyInt_AsLong(o));
    }
    if (PyFloat_Check(o)) {
        return Fragment(PyFloat_AsDouble(o));
    }
    if (PyString_Check(o)) {
        return Fragment(PyString_AsString(o));
    }
    if (PyList_Check(o)) {
        return Fragment(PyListObject_asListType(o));
    }
    if (PyDict_Check(o)) {
        return Fragment(PyDictObject_asMapType(o));
    }
    if (PyTuple_Check(o)) {
        Fragment::ListType list;
        int i, size = PyTuple_Size(o);
        for(i = 0; i < size; i++) {
            Fragment item = PyObject_asObject(PyTuple_GetItem(o, i));
            if (item.GetType() != Fragment::TYPE_NONE) {
                list.push_back(item);
            }
        }
        return Fragment(list);
    }
    if (PyAtlasObject_Check(o)) {
        AtlasObject * obj = (AtlasObject *)o;
        return *(obj->m_obj);
    }
    if (PyOperation_Check(o)) {
        OperationObject * op = (OperationObject *)o;
        return op->operation->AsObject();
    }
    if (PyOplist_Check(o)) {
        OplistObject * opl = (OplistObject *)o;
        Fragment::ListType _list;
        Fragment msg(_list);
        Fragment::ListType & entlist = msg.AsList();
        const OpVector & ops = *opl->ops;
        OpVector::const_iterator I;
        for(I = ops.begin(); I != ops.end(); I++) {
            entlist.push_back((*I)->AsObject());
        }
        return msg;
    }
    if (PyLocation_Check(o)) {
        LocationObject * loc = (LocationObject *)o;
        Fragment::MapType _map;
        loc->location->addToObject(_map);
        return Fragment(_map);
    }
    return Fragment();
}
