// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "Py_CreatorClient.h"

#include "CreatorClient.h"

#include "rulesets/Py_Operation.h"
#include "rulesets/Py_WorldTime.h"
#include "rulesets/Py_Vector3D.h"
#include "rulesets/Py_Location.h"
#include "rulesets/Py_Object.h"
#include "rulesets/Py_Thing.h"
#include "rulesets/Py_Map.h"

#include "common/debug.h"

static const bool debug_flag = false;

static PyObject * CreatorClient_as_entity(PyCreatorClient * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.as_entity");
        return NULL;
    }
#endif // NDEBUG
    if (!PyArg_ParseTuple(args, "")) {
        return NULL;
    }
    PyMessageElement * ret = newPyMessageElement();
    if (ret == NULL) {
        return NULL;
    }
    ret->m_obj = new Element(MapType());
    self->m_mind->addToMessage(ret->m_obj->asMap());
    return (PyObject *)ret;
}

static PyObject * CreatorClient_get_xyz(PyCreatorClient * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.get_xyz");
        return NULL;
    }
#endif // NDEBUG
    if (!PyArg_ParseTuple(args, "")) {
        return NULL;
    }
    PyVector3D * ret = newPyVector3D();
    if (ret == NULL) {
        return NULL;
    }
    ret->coords = self->m_mind->getXyz();
    return (PyObject *)ret;
}

static PyObject * CreatorClient_make(PyCreatorClient * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.make");
        return NULL;
    }
#endif // NDEBUG
    PyMessageElement * entity = NULL;
    if (!PyArg_ParseTuple(args, "O", &entity)) {
        return NULL;
    }
    if (!PyMessageElement_Check(entity)) {
        PyErr_SetString(PyExc_TypeError, "Can only make Atlas entity");
        return NULL;
    }
    Entity * retval = self->m_mind->make(*entity->m_obj);
    if (retval == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Entity creation failed");
        return NULL;
    }
    PyEntity * ret = newPyEntity();
    ret->m_entity = retval;
    return (PyObject *)ret;
}

static PyObject * CreatorClient_set(PyCreatorClient * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.set");
        return NULL;
    }
#endif // NDEBUG
    PyMessageElement * entity = NULL;
    char * id = NULL;
    if (!PyArg_ParseTuple(args, "sO", &id, &entity)) {
        return NULL;
    }
    if (!PyMessageElement_Check(entity)) {
        PyErr_SetString(PyExc_TypeError, "Can only set Atlas entity");
        return NULL;
    }
    self->m_mind->sendSet(id, *entity->m_obj);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * CreatorClient_look(PyCreatorClient * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.look");
        return NULL;
    }
#endif // NDEBUG
    char * id = NULL;
    if (!PyArg_ParseTuple(args, "s", &id)) {
        return NULL;
    }
    Entity * retval = self->m_mind->look(id);
    if (retval == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Entity look failed");
        return NULL;
    }
    PyEntity * ret = newPyEntity();
    ret->m_entity = retval;
    return (PyObject *)ret;
}
static PyObject * CreatorClient_look_for(PyCreatorClient * self,
                                         PyObject * args)
{
#ifndef NDEBUG
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.look_for");
        return NULL;
    }
#endif // NDEBUG
    PyMessageElement * ent;
    if (!PyArg_ParseTuple(args, "O", &ent)) {
        return NULL;
    }
    if (!PyMessageElement_Check(ent)) {
        PyErr_SetString(PyExc_TypeError, "Can only look for Atlas description");
        return NULL;
    }
    Entity * retval = self->m_mind->lookFor(*ent->m_obj);
    if (retval == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Entity look_for failed");
        return NULL;
    }
    PyEntity * ret = newPyEntity();
    ret->m_entity = retval;
    return (PyObject *)ret;
}

static PyObject * CreatorClient_send(PyCreatorClient * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.send");
        return NULL;
    }
#endif // NDEBUG
    PyOperation * op;
    if (!PyArg_ParseTuple(args, "O", &op)) {
        return NULL;
    }
    if (!PyOperation_Check(op)) {
        PyErr_SetString(PyExc_TypeError, "Can only send Atlas operation");
        return NULL;
    }
    self->m_mind->send(*op->operation);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef CreatorClient_methods[] = {
	{"get_xyz",        (PyCFunction)CreatorClient_get_xyz, 1},
	{"as_entity",      (PyCFunction)CreatorClient_as_entity, 1},
	{"make",           (PyCFunction)CreatorClient_make, 1},
	{"set",            (PyCFunction)CreatorClient_set, 1},
	{"look",           (PyCFunction)CreatorClient_look, 1},
	{"look_for",       (PyCFunction)CreatorClient_look_for, 1},
	{"send",           (PyCFunction)CreatorClient_send, 1},
	{NULL,          NULL}           /* sentinel */
};

static void CreatorClient_dealloc(PyCreatorClient *self)
{
    //if (self->m_mind != NULL) {
        //delete self->m_mind;
    //}
    Py_XDECREF(self->CreatorClient_attr);
    PyMem_DEL(self);
}

static PyObject * CreatorClient_getattr(PyCreatorClient *self, char *name)
{
    // Fairly major re-write of this to use operator[] of CreatorClient base class
#ifndef NDEBUG
    if (self->m_mind == NULL) {
        PyErr_SetString(PyExc_AssertionError, "NULL CreatorClient in CreatorClient.getattr");
        return NULL;
    }
#endif // NDEBUG
    // If operation search gets to here, it goes no further
    if (strstr(name, "_operation") != NULL) {
        PyErr_SetString(PyExc_AttributeError, name);
        return NULL;
    }
    if (strcmp(name, "type") == 0) {
        PyObject * list = PyList_New(0);
        if (list == NULL) {
            return NULL;
        }
        PyList_Append(list, PyString_FromString(self->m_mind->getType().c_str()));
        return list;
    }
    if (strcmp(name, "map") == 0) {
        PyMap * map = newPyMap();
        map->m_map = self->m_mind->getMap();
        return (PyObject *)map;
    }
    if (strcmp(name, "location") == 0) {
        PyLocation * loc = newPyLocation();
        loc->location = &self->m_mind->m_location;
        loc->own = 0;
        return (PyObject *)loc;
    }
    if (strcmp(name, "time") == 0) {
        PyWorldTime * worldtime = newPyWorldTime();
        worldtime->time = self->m_mind->getTime();
        return (PyObject *)worldtime;
    }
    if (self->CreatorClient_attr != NULL) {
        PyObject *v = PyDict_GetItemString(self->CreatorClient_attr, name);
        if (v != NULL) {
            Py_INCREF(v);
            return v;
        }
    }
    Entity * thing = self->m_mind;
    Element attr;
    if (!thing->get(name, attr)) {
        return Py_FindMethod(CreatorClient_methods, (PyObject *)self, name);
    }
    PyObject * ret = MessageElement_asPyObject(attr);
    if (ret == NULL) {
        return Py_FindMethod(CreatorClient_methods, (PyObject *)self, name);
    }
    return ret;
}

static int CreatorClient_setattr(PyCreatorClient *self, char *name, PyObject *v)
{
    if (self->m_mind == NULL) {
        return -1;
    }
    if (self->CreatorClient_attr == NULL) {
        self->CreatorClient_attr = PyDict_New();
        if (self->CreatorClient_attr == NULL) {
            return -1;
        }
    }
    if (strcmp(name, "status") == 0) {
        // This needs to be here until we can sort the difference
        // between floats and ints in python.
        if (PyInt_Check(v)) {
            self->m_mind->setStatus((double)PyInt_AsLong(v));
        } else if (PyFloat_Check(v)) {
            self->m_mind->setStatus(PyFloat_AsDouble(v));
        } else {
            PyErr_SetString(PyExc_TypeError, "status must be numeric type");
            return -1;
        }
        return 0;
    }
    if (strcmp(name, "map") == 0) {
        return -1;
    }
    Entity * thing = self->m_mind;
    //std::string attr(name);
    //if (v == NULL) {
        //thing->attributes.erase(attr);
        //return 0;
    //}
    Element obj = PyObject_asMessageElement(v);
    if (!obj.isNone() && !obj.isMap() && !obj.isList()) {
        thing->set(name, obj);
        return 0;
    }
    // If we get here, then the attribute is not Atlas compatable, so we
    // need to store it in a python dictionary
    return PyDict_SetItemString(self->CreatorClient_attr, name, v);
}

static int CreatorClient_compare(PyCreatorClient *self, PyCreatorClient *other)
{
    if ((self->m_mind == NULL) || (other->m_mind == NULL)) {
        return -1;
    }
    return (self->m_mind == other->m_mind) ? 0 : 1;
}

PyTypeObject PyCreatorClient_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,					/*ob_size*/
	"cppCreatorClient",			/*tp_name*/
	sizeof(PyCreatorClient),		/*tp_basicsize*/
	0,					/*tp_itemsize*/
	/* methods */
	(destructor)CreatorClient_dealloc,	/*tp_dealloc*/
	0,					/*tp_print*/
	(getattrfunc)CreatorClient_getattr,	/*tp_getattr*/
	(setattrfunc)CreatorClient_setattr,	/*tp_setattr*/
	(cmpfunc)CreatorClient_compare,		/*tp_compare*/
	0,					/*tp_repr*/
	0,					/*tp_as_number*/
	0,					/*tp_as_sequence*/
	0,					/*tp_as_mapping*/
	0,					/*tp_hash*/
};

PyCreatorClient * newPyCreatorClient()
{
    PyCreatorClient * self;
    self = PyObject_NEW(PyCreatorClient, &PyCreatorClient_Type);
    if (self == NULL) {
        return NULL;
    }
    self->CreatorClient_attr = NULL;
    return self;
}

bool runClientScript(CreatorClient * c, const std::string & package, const std::string & func)
{
    PyObject * package_name = PyString_FromString(package.c_str());
    PyObject * mod_dict = PyImport_Import(package_name);
    Py_DECREF(package_name);
    if (mod_dict == NULL) {
        std::cerr << "Cld not find python module " << package
                  << std::endl << std::flush;
        PyErr_Print();
        return false;
    }
    PyObject * function = PyObject_GetAttrString(mod_dict,
                                                 (char *)func.c_str());
    Py_DECREF(mod_dict);
    if (function == NULL) {
        std::cerr << "Could not find " << func << " function" << std::endl
                  << std::flush;
        PyErr_Print();
        return false;
    }
    if (PyCallable_Check(function) == 0) {
        std::cerr << "It does not seem to be a function at all" << std::endl
                  << std::flush;
        Py_DECREF(function);
        return false;
    }
    PyCreatorClient * editor = newPyCreatorClient();
    editor->m_mind = c;
    PyObject * pyob = PyEval_CallFunction(function, "(O)", editor);

    if (pyob == NULL) {
        if (PyErr_Occurred() == NULL) {
            std::cerr << "Could not call function" << std::endl << std::flush;
        } else {
            std::cerr << "Reporting python error" << std::endl << std::flush;
            PyErr_Print();
        }
    }
    Py_DECREF(function);
    return true;

}
