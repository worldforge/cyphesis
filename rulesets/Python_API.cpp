// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2005 Alistair Riddoch

#include "Python.h"

#include "Python_API.h"
#include "Python_Script_Utils.h"

#include "Py_BBox.h"
#include "Py_Object.h"
#include "Py_Thing.h"
#include "Py_Mind.h"
#include "Py_Map.h"
#include "Py_Location.h"
#include "Py_Vector3D.h"
#include "Py_Point3D.h"
#include "Py_Quaternion.h"
#include "Py_WorldTime.h"
#include "Py_World.h"
#include "Py_Operation.h"
#include "Py_Oplist.h"

#include "PythonThingScript.h"
#include "PythonMindScript.h"
#include "World.h"
#include "Entity.h"
#include "BaseMind.h"

#include "common/globals.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/log.h"

#include "common/Tick.h"
#include "common/Burn.h"
#include "common/Chop.h"
#include "common/Cut.h"
#include "common/Setup.h"
#include "common/Eat.h"
#include "common/Nourish.h"

#include "common/Generic.h"

#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Talk.h>
#include <Atlas/Objects/Operation/Touch.h>
#include <Atlas/Objects/Operation/Info.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Burn;
using Atlas::Objects::Operation::Action;
using Atlas::Objects::Operation::Chop;
using Atlas::Objects::Operation::Cut;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Setup;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Talk;
using Atlas::Objects::Operation::Touch;
using Atlas::Objects::Operation::Eat;
using Atlas::Objects::Operation::Nourish;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Tick;
using Atlas::Objects::Operation::Generic;

static const bool debug_flag = false;

/// \brief Python wrapper for C++ functions to be exposed to Python
typedef struct {
    PyObject_HEAD
} FunctionObject;

static void Function_dealloc(FunctionObject * self)
{
    PyMem_DEL(self);
}

static PyObject * log_debug(PyObject * self, PyObject * args, PyObject * kwds)
{
    Py_INCREF(Py_None);
    return Py_None;
}

PyTypeObject log_debug_type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,
        "Function",
        sizeof(FunctionObject),
        0,
        /* methods */
        (destructor)Function_dealloc,
        0,              /* tp_print */
        0,              /* tp_getattr */
        0,              /* tp_setattr */
        0,              /* tp_compare */
        0,              /* tp_repr */
        0,              /* tp_as_number */
        0,              /* tp_as_sequence */
        0,              /* tp_as_mapping */
        0,              /* tp_hash */
        log_debug,      /* tp_call */
};

//////////////////////////////////////////////////////////////////////////
// Logger replaces sys.stdout and sys.stderr so the nothing goes to output
//////////////////////////////////////////////////////////////////////////

/// \brief Python struct to handle output from python scripts
///
/// In instance of this struct is used to replace sys.stdout and sys.stderr
/// in the Python interpreter so that all script output goes to the cyphesis
/// log subsystem
typedef struct {
    PyObject_HEAD
} PyLogger;

static void python_log(LogLevel lvl, const char * msg)
{
    std::string message(msg);
    std::string::size_type n = 0;
    std::string::size_type p;
    for (p = message.find_first_of('\n');
         p != std::string::npos;
         p = message.find_first_of('\n', n)) {
        log(lvl, message.substr(n, p - n).c_str());
        n = p + 1;
    }
    if (message.size() > n) {
        log(lvl, message.substr(n, message.size() - n).c_str());
    }
}

static PyObject * PyOutLogger_write(PyObject * self, PyObject * args)
{
    char * mesg;
    if (!PyArg_ParseTuple(args, "s", &mesg)) {
        return 0;
    }

    python_log(SCRIPT, mesg);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * PyErrLogger_write(PyObject * self, PyObject * args)
{
    char * mesg;
    if (!PyArg_ParseTuple(args, "s", &mesg)) {
        return 0;
    }

    python_log(SCRIPT_ERROR, mesg);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef PyOutLogger_methods[] = {
    {"write",       PyOutLogger_write,          METH_VARARGS},
    {NULL,          NULL}                       /* Sentinel */
};

static PyMethodDef PyErrLogger_methods[] = {
    {"write",       PyErrLogger_write,          METH_VARARGS},
    {NULL,          NULL}                       /* Sentinel */
};

static void PyLogger_dealloc(PyObject * self)
{
    PyMem_DEL(self);
}

static PyObject * PyOutLogger_getattr(PyObject * self, char *name)
{
    return Py_FindMethod(PyOutLogger_methods, self, name);
}

static PyObject * PyErrLogger_getattr(PyObject * self, char *name)
{
    return Py_FindMethod(PyErrLogger_methods, self, name);
}

PyTypeObject PyOutLogger_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                   // ob_size
        "OutLogger",         // tp_name
        sizeof(PyLogger),    // tp_basicsize
        0,                   // tp_itemsize
        //  methods 
        PyLogger_dealloc,    // tp_dealloc
        0,                   // tp_print
        PyOutLogger_getattr, // tp_getattr
        0,                   // tp_setattr
        0,                   // tp_compare
        0,                   // tp_repr
        0,                   // tp_as_number
        0,                   // tp_as_sequence
        0,                   // tp_as_mapping
        0,                   // tp_hash
};

PyTypeObject PyErrLogger_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                   // ob_size
        "ErrLogger",         // tp_name
        sizeof(PyLogger),    // tp_basicsize
        0,                   // tp_itemsize
        //  methods 
        PyLogger_dealloc,    // tp_dealloc
        0,                   // tp_print
        PyErrLogger_getattr, // tp_getattr
        0,                   // tp_setattr
        0,                   // tp_compare
        0,                   // tp_repr
        0,                   // tp_as_number
        0,                   // tp_as_sequence
        0,                   // tp_as_mapping
        0,                   // tp_hash
};

//////////////////////////////////////////////////////////////////////////
// dictlist
//////////////////////////////////////////////////////////////////////////

static PyObject * dictlist_remove_value(PyObject * self, PyObject * args, PyObject * kwds)
{
    PyObject * dict;
    PyEntity * item;
    long remove_empty_key = 1;
    if (!PyArg_ParseTuple(args, "OO|i", &dict, &item, &remove_empty_key)) {
        return NULL;
    }
    int flag = 0;
    if (!PyDict_Check(dict)) {
        PyErr_SetString(PyExc_TypeError, "Trying to set item in not dictlist");
        return NULL;
    }
    PyObject * keys = PyDict_Keys(dict);
    PyObject * values = PyDict_Values(dict);

    if ((keys == NULL) || (values == NULL)) {
        PyErr_SetString(PyExc_RuntimeError, "Error getting keys from dictlist");
        return NULL;
    }
    int i, size = PyList_Size(keys);
    for(i = 0; i < size; i++) {
        PyObject * value = PyList_GetItem(values, i);
        PyObject * key = PyList_GetItem(keys, i);
        int j, lsize = PyList_Size(value);
        for(j = 0; j < lsize; j++) {
            PyEntity * entry = (PyEntity*)PyList_GetItem(value, j);
            if (entry->m_entity == item->m_entity) {
                flag = 1;
                PyList_SetSlice(value, j, j+1, NULL);
                j--; lsize--;
                if ((remove_empty_key !=0) && (PyList_Size(value) == 0)) {
                    PyDict_DelItem(dict, key);
                }
            }
        }
    }
    Py_DECREF(keys);
    Py_DECREF(values);
    return PyInt_FromLong(flag);
}

PyTypeObject dictlist_remove_value_type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,
        "Function",
        sizeof(FunctionObject),
        0,
        /* methods */
        (destructor)Function_dealloc,
        0,              /* tp_print */
        0,              /* tp_getattr */
        0,              /* tp_setattr */
        0,              /* tp_compare */
        0,              /* tp_repr */
        0,              /* tp_as_number */
        0,              /* tp_as_sequence */
        0,              /* tp_as_mapping */
        0,              /* tp_hash */
        dictlist_remove_value,  /* tp_call */
};
static PyObject * dictlist_add_value(PyObject * self, PyObject * args, PyObject * kwds)
{
    PyObject * dict;
    PyEntity * item;
    char * key;
    if (!PyArg_ParseTuple(args, "OsO", &dict, &key, &item)) {
        return NULL;
    }
    if (!PyDict_Check(dict)) {
        PyErr_SetString(PyExc_TypeError, "Dict is not a dictionary");
        return NULL;
    }
    PyObject * list = PyDict_GetItemString(dict, key);
    if (list != NULL) {
        if (!PyList_Check(list)) {
            PyErr_SetString(PyExc_TypeError, "Dict does not contain a list");
            return NULL;
        }
        int i, size = PyList_Size(list);
        for(i = 0; i < size; i++) {
            PyEntity * entry = (PyEntity*)PyList_GetItem(list,i);
            if (entry->m_entity == item->m_entity) {
                goto present;
            }
        }
        PyList_Append(list, (PyObject*)item);
    } else {
        list = PyList_New(0);
        PyList_Append(list, (PyObject*)item);
        PyDict_SetItemString(dict, key, list);
        Py_DECREF(list);
    }
present:
    Py_INCREF(Py_None);
    return Py_None;
}

PyTypeObject dictlist_add_value_type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,
        "Function",
        sizeof(FunctionObject),
        0,
        /* methods */
        (destructor)Function_dealloc,
        0,              /* tp_print */
        0,              /* tp_getattr */
        0,              /* tp_setattr */
        0,              /* tp_compare */
        0,              /* tp_repr */
        0,              /* tp_as_number */
        0,              /* tp_as_sequence */
        0,              /* tp_as_mapping */
        0,              /* tp_hash */
        dictlist_add_value,     /* tp_call */
};

static PyObject * Get_PyClass(const std::string & package,
                              const std::string & type)
{
    std::string classname(type);
    classname[0] = toupper(classname[0]);
    PyObject * package_name = PyString_FromString((char *)package.c_str());
    PyObject * module = PyImport_Import(package_name);
    Py_DECREF(package_name);
    if (module == NULL) {
        std::string msg = std::string("Missing python module ") + package;
        log(ERROR, msg.c_str());
        PyErr_Print();
        return NULL;
    }
    PyObject * pyClass = PyObject_GetAttrString(module, (char *)classname.c_str());
    Py_DECREF(module);
    if (pyClass == NULL) {
        std::string msg = std::string("Could not find python class ")
                        + package + "." + classname;
        log(ERROR, msg.c_str());
        PyErr_Print();
        return NULL;
    }
    if (PyCallable_Check(pyClass) == 0) {
        std::string msg = std::string("Could not instance python class ")
                        + package + "." + classname;
        log(ERROR, msg.c_str());
        Py_DECREF(pyClass);
        return NULL;
    }
#if 0
    // In later versions of python using PyType_* will become the right thing to do.
    if (PyType_Check(pyClass) == 0) {
        std::cerr << "PyCallable_Check returned true, but PyType_Check returned false " << package << "." << type << std::endl << std::flush;
    } else {
        std::cerr << "PyType_Check returned true" << std::endl << std::flush;
    }
#endif
    return pyClass;
}

PyObject * Create_PyScript(PyObject * pyEntity, PyObject * pyClass)
{
    PyObject * pyob = PyEval_CallFunction(pyClass,"(O)", pyEntity);
    
    if (pyob == NULL) {
        if (PyErr_Occurred() == NULL) {
            log(ERROR, "Could not create python instance");
        } else {
            log(ERROR, "Reporting python error");
            PyErr_Print();
        }
    }
    Py_DECREF(pyEntity);
    return pyob;
}

void Subscribe_Script(Entity * entity, PyObject * pyclass,
                      const std::string& package)
{
#if 0
    PyObject * dmap = PyObject_GetAttrString(pyclass, "__dict__");
    if (dmap == NULL) {
        std::string msg = std::string( "Python class for ") + package
                        + " has no __dict__";
        log(ERROR, msg.c_str());
        return;
    }
    if (!PyDict_Check(dmap)) {
        std::string msg = std::string( "Python class for ") + package
                        + " is malformed";
        log(ERROR, msg.c_str());
        return;
    }
    PyObject * keys = PyDict_Keys(dmap);
#else
    PyObject * keys = PyObject_Dir(pyclass);
#endif
    if (keys == NULL) {
        std::string msg = std::string("Error getting attribute list of Python class for ") + package;
        log(ERROR, msg.c_str());
        return;
    }
    for(int i = 0; i < PyList_Size(keys); i++) {
        std::string method(PyString_AsString(PyList_GetItem(keys, i)));
        std::string::size_type l = method.find("_operation", 0, 10);
        if (l == std::string::npos) {
            debug(std::cout << method << " is not a method" << std::endl
                            << std::flush;);
        } else {
            std::string op_name = method.substr(0,l);
            debug(std::cout << method << " is a method, it contains _op.. at "
                            << l << " so we can register for "
                            << method.substr(0,l) << std::endl << std::flush;);
            entity->scriptSubscribe(op_name);
        }
    }
    Py_DECREF(keys);
}

void Create_PyEntity(Entity * entity, const std::string & package,
                                      const std::string & type)
{
    PyObject * pyClass = Get_PyClass(package, type);
    if (pyClass == NULL) { return; }
    PyEntity * pyEntity = newPyEntity();
    pyEntity->m_entity = entity;
    Subscribe_Script(entity, pyClass, package);
    PyObject * o = Create_PyScript((PyObject *)pyEntity, pyClass);
    Py_DECREF(pyClass);

    if (o != NULL) {
        entity->setScript(new PythonEntityScript(o, *entity));
    }
}

void Create_PyMind(BaseMind * mind, const std::string & package,
                                    const std::string & type)
{
    PyObject * pyClass = Get_PyClass(package, type);
    if (pyClass == NULL) { return; }
    PyMind * pyMind = newPyMind();
    pyMind->m_mind = mind;
    Subscribe_Script(mind, pyClass, package);
    PyObject * o = Create_PyScript((PyObject *)pyMind, pyClass);
    Py_DECREF(pyClass);

    if (o != NULL) {
        mind->setScript(new PythonMindScript(o, *mind));
    }
}

static PyObject * is_location(PyObject * self, PyObject * args)
{
    PyObject * loc;
    if (!PyArg_ParseTuple(args, "O", &loc)) {
        return NULL;
    }
    if (PyLocation_Check(loc)) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    Py_INCREF(Py_False);
    return Py_False;
}

static PyObject * location_new(PyObject * self, PyObject * args)
{
    PyLocation *o;
    // We need to deal with actual args here
    PyObject * refO = NULL, * coordsO = NULL;
    Entity * ref_ent = NULL;
    bool decrefO = false;
    if (!PyArg_ParseTuple(args, "|OO", &refO, &coordsO)) {
        return NULL;
    }
    if (refO != NULL) {
        if ((!PyEntity_Check(refO)) && (!PyWorld_Check(refO)) && (!PyMind_Check(refO))) {
            if (PyObject_HasAttrString(refO, "cppthing")) {
                refO = PyObject_GetAttrString(refO, "cppthing");
                decrefO = true;
            }
            if (!PyEntity_Check(refO) && !PyMind_Check(refO)) {
                PyErr_SetString(PyExc_TypeError, "Arg ref required");
                if (decrefO) { Py_DECREF(refO); }
                return NULL;
            }
        }
        if ((coordsO != NULL) && (!PyPoint3D_Check(coordsO))) {
            PyErr_SetString(PyExc_TypeError, "Arg coords required");
            if (decrefO) { Py_DECREF(refO); }
            return NULL;
        }
    
        if (PyWorld_Check(refO)) {
            PyWorld * ref = (PyWorld*)refO;
#ifndef NDEBUG
            if (ref->world == NULL) {
                PyErr_SetString(PyExc_AssertionError, "Parent world is invalid");
                if (decrefO) { Py_DECREF(refO); }
                return NULL;
            }
#endif // NDEBUG
            ref_ent = &ref->world->m_gameWorld;
        } else if (PyMind_Check(refO)) {
            PyMind * ref = (PyMind*)refO;
#ifndef NDEBUG
            if (ref->m_mind == NULL) {
                PyErr_SetString(PyExc_AssertionError, "Parent mind is invalid");
                if (decrefO) { Py_DECREF(refO); }
                return NULL;
            }
#endif // NDEBUG
            ref_ent = ref->m_mind;
        } else {
            PyEntity * ref = (PyEntity*)refO;
#ifndef NDEBUG
            if (ref->m_entity == NULL) {
                PyErr_SetString(PyExc_AssertionError, "Parent thing is invalid");
                if (decrefO) { Py_DECREF(refO); }
                return NULL;
            }
#endif // NDEBUG
            ref_ent = ref->m_entity;
        }
    }
    if (decrefO) { Py_DECREF(refO); }
    PyPoint3D * coords = (PyPoint3D*)coordsO;
    o = newPyLocation();
    if ( o == NULL ) {
        return NULL;
    }
    o->own = 1;
    if (coords == NULL) {
        o->location = new Location(ref_ent);
    } else {
        o->location = new Location(ref_ent, coords->coords);
    }
    return (PyObject *)o;
}

static PyObject * distance_to(PyObject * self, PyObject * args)
{
    PyObject * near, * other;
    if (!PyArg_ParseTuple(args, "OO", &near, &other)) {
        return NULL;
    }
    if (!PyLocation_Check(near) || !PyLocation_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Arg Location required");
        return NULL;
    }
    PyLocation * sloc = (PyLocation *)near,
               * oloc = (PyLocation *)other;
#ifndef NDEBUG
    if (sloc->location == NULL || oloc == NULL) {
        PyErr_SetString(PyExc_AssertionError, "Null location pointer");
        return NULL;
    }
#endif // NDEBUG
    PyVector3D * ret = newPyVector3D();
    if (ret == NULL) {
        return NULL;
    }
    ret->coords = distanceTo(*sloc->location, *oloc->location);
    return (PyObject *)ret;
}

static PyObject * square_distance(PyObject * self, PyObject * args)
{
    PyObject * near, * other;
    if (!PyArg_ParseTuple(args, "OO", &near, &other)) {
        return NULL;
    }
    if (!PyLocation_Check(near) || !PyLocation_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Arg Location required");
        return NULL;
    }
    PyLocation * sloc = (PyLocation *)near,
               * oloc = (PyLocation *)other;
#ifndef NDEBUG
    if (sloc->location == NULL || oloc == NULL) {
        PyErr_SetString(PyExc_AssertionError, "Null location pointer");
        return NULL;
    }
#endif // NDEBUG
    return PyFloat_FromDouble(squareDistance(*sloc->location, *oloc->location));
}

static PyObject * vector3d_new(PyObject * self, PyObject * args)
{
        PyVector3D *o;
        Vector3D val;
        // We need to deal with actual args here
        PyObject * clist;
        switch (PyTuple_Size(args)) {
            case 0:
                break;
            case 1:
                clist = PyTuple_GetItem(args, 0);
                if ((!PyList_Check(clist)) || (PyList_Size(clist) != 3)) {
                    PyErr_SetString(PyExc_TypeError, "Vector3D() from single value must a list 3 long");
                    return NULL;
                }
                for(int i = 0; i < 3; i++) {
                    PyObject * item = PyList_GetItem(clist, i);
                    if (PyInt_Check(item)) {
                        val[i] = (float)PyInt_AsLong(item);
                    } else if (PyFloat_Check(item)) {
                        val[i] = PyFloat_AsDouble(item);
                    } else if (PyMessageElement_Check(item)) {
                        PyMessageElement * mitem = (PyMessageElement*)item;
                        if (!mitem->m_obj->isNum()) {
                            PyErr_SetString(PyExc_TypeError, "Vector3D() must take list of floats, or ints");
                            return NULL;
                        }
                        val[i] = mitem->m_obj->asNum();
                    } else {
                        PyErr_SetString(PyExc_TypeError, "Vector3D() must take list of floats, or ints");
                        return NULL;
                    }
                }
                val.setValid();
                break;
            case 3:
                for(int i = 0; i < 3; i++) {
                    PyObject * item = PyTuple_GetItem(args, i);
                    if (PyInt_Check(item)) {
                        val[i] = (float)PyInt_AsLong(item);
                    } else if (PyFloat_Check(item)) {
                        val[i] = PyFloat_AsDouble(item);
                    } else {
                        PyErr_SetString(PyExc_TypeError, "Vector3D() must take list of floats, or ints");
                        return NULL;
                    }
                }
                val.setValid();
                break;
            default:
                PyErr_SetString(PyExc_TypeError, "Vector3D must take list of floats, or ints, 3 ints or 3 floats");
                return NULL;
                break;
        }
            
        o = newPyVector3D();
        if ( o == NULL ) {
                return NULL;
        }
        o->coords = val;
        return (PyObject *)o;
}

static PyObject * point3d_new(PyObject * self, PyObject * args)
{
        PyPoint3D *o;
        Point3D val;
        // We need to deal with actual args here
        PyObject * clist;
        switch (PyTuple_Size(args)) {
            case 0:
                break;
            case 1:
                clist = PyTuple_GetItem(args, 0);
                if ((!PyList_Check(clist)) || (PyList_Size(clist) != 3)) {
                    PyErr_SetString(PyExc_TypeError, "Point3D() from single value must a list 3 long");
                    return NULL;
                }
                for(int i = 0; i < 3; i++) {
                    PyObject * item = PyList_GetItem(clist, i);
                    if (PyInt_Check(item)) {
                        val[i] = (float)PyInt_AsLong(item);
                    } else if (PyFloat_Check(item)) {
                        val[i] = PyFloat_AsDouble(item);
                    } else if (PyMessageElement_Check(item)) {
                        PyMessageElement * mitem = (PyMessageElement*)item;
                        if (!mitem->m_obj->isNum()) {
                            PyErr_SetString(PyExc_TypeError, "Point3D() must take list of floats, or ints");
                            return NULL;
                        }
                        val[i] = mitem->m_obj->asNum();
                    } else {
                        PyErr_SetString(PyExc_TypeError, "Point3D() must take list of floats, or ints");
                        return NULL;
                    }
                }
                val.setValid();
                break;
            case 3:
                for(int i = 0; i < 3; i++) {
                    PyObject * item = PyTuple_GetItem(args, i);
                    if (PyInt_Check(item)) {
                        val[i] = (float)PyInt_AsLong(item);
                    } else if (PyFloat_Check(item)) {
                        val[i] = PyFloat_AsDouble(item);
                    } else {
                        PyErr_SetString(PyExc_TypeError, "Point3D() must take list of floats, or ints");
                        return NULL;
                    }
                }
                val.setValid();
                break;
            default:
                PyErr_SetString(PyExc_TypeError, "Point3D must take list of floats, or ints, 3 ints or 3 floats");
                return NULL;
                break;
        }
            
        o = newPyPoint3D();
        if ( o == NULL ) {
                return NULL;
        }
        o->coords = val;
        return (PyObject *)o;
}

static PyObject * bbox_new(PyObject * self, PyObject * args)
{
    std::vector<float> val;

    PyObject * clist;
    int tuple_size = PyTuple_Size(args);
    switch(tuple_size) {
        case 0:
            break;
        case 1:
            clist = PyTuple_GetItem(args, 0);
            int clist_size = PyList_Size(clist);
            if (!PyList_Check(clist) || (clist_size != 3 && clist_size != 6)) {
                PyErr_SetString(PyExc_TypeError, "BBox() from single value must a list 3 or 6 long");
                return NULL;
            }
            
            val.resize(clist_size);
            for(int i = 0; i < clist_size; i++) {
                PyObject * item = PyList_GetItem(clist, i);
                if (PyInt_Check(item)) {
                    val[i] = (float)PyInt_AsLong(item);
                } else if (PyFloat_Check(item)) {
                    val[i] = PyFloat_AsDouble(item);
                } else if (PyMessageElement_Check(item)) {
                    PyMessageElement * mitem = (PyMessageElement*)item;
                    if (!mitem->m_obj->isNum()) {
                        PyErr_SetString(PyExc_TypeError, "BBox() must take list of floats, or ints");
                        return NULL;
                    }
                    val[i] = mitem->m_obj->asNum();
                } else {
                    PyErr_SetString(PyExc_TypeError, "BBox() must take list of floats, or ints");
                    return NULL;
                }
            }
            break;
        case 3:
        case 6:
            val.resize(tuple_size);
            for(int i = 0; i < tuple_size; i++) {
                PyObject * item = PyTuple_GetItem(args, i);
                if (PyInt_Check(item)) {
                    val[i] = (float)PyInt_AsLong(item);
                } else if (PyFloat_Check(item)) {
                    val[i] = PyFloat_AsDouble(item);
                } else {
                    PyErr_SetString(PyExc_TypeError, "BBox() must take list of floats, or ints");
                    return NULL;
                }
            }
            break;
        default:
            PyErr_SetString(PyExc_TypeError, "Point3D must take list of floats, or ints, 3 ints or 3 floats");
            return NULL;
            break;
    }
        
    PyBBox * o = newPyBBox();
    if ( o == NULL ) {
            return NULL;
    }
    if (val.size() == 3) {
        o->box = BBox(WFMath::Point<3>(0.f, 0.f, 0.f),
                      WFMath::Point<3>(val[0], val[1], val[2]));
    } else {
        o->box = BBox(WFMath::Point<3>(val[0], val[1], val[2]),
                      WFMath::Point<3>(val[3], val[4], val[5]));
    }
    return (PyObject *)o;
}

static PyObject * quaternion_new(PyObject * self, PyObject * args)
{
        PyQuaternion *o;
        Quaternion val;

        PyObject * clist;
        switch (PyTuple_Size(args)) {
            case 0:
                break;
            case 1:
                clist = PyTuple_GetItem(args, 0);
                if ((!PyList_Check(clist)) || (PyList_Size(clist) != 4)) {
                    PyErr_SetString(PyExc_TypeError, "Quaternion() from single value must a list 4 long");
                    return NULL;
                }
                {
                float quaternion[4];
                for(int i = 0; i < 4; i++) {
                    PyObject * item = PyList_GetItem(clist, i);
                    if (PyInt_Check(item)) {
                        quaternion[i] = (WFMath::CoordType)PyInt_AsLong(item);
                    } else if (PyFloat_Check(item)) {
                        quaternion[i] = PyFloat_AsDouble(item);
                    } else {
                        PyErr_SetString(PyExc_TypeError, "Quaternion() must take list of floats, or ints");
                        return NULL;
                    }
                }
                val = Quaternion(quaternion[3], quaternion[0],
                                 quaternion[1], quaternion[2]);
                }
                break;
            case 2:
                {
                PyObject * v1 = PyTuple_GetItem(args, 0);
                PyObject * v2 = PyTuple_GetItem(args, 1);
                if (!PyVector3D_Check(v1) || !PyVector3D_Check(v2)) {
                    PyErr_SetString(PyExc_TypeError, "Quaternion(a,b) must take two vectors");
                    return NULL;
                }
                PyVector3D * from = (PyVector3D *)v1;
                PyVector3D * to = (PyVector3D *)v2;
                val = quaternionFromTo(from->coords, to->coords);
                }
                break;
            case 4:
                {
                float quaternion[4];
                for(int i = 0; i < 4; i++) {
                    PyObject * item = PyTuple_GetItem(args, i);
                    if (PyInt_Check(item)) {
                        quaternion[i] = (WFMath::CoordType)PyInt_AsLong(item);
                    } else if (PyFloat_Check(item)) {
                        quaternion[i] = PyFloat_AsDouble(item);
                    } else {
                        PyErr_SetString(PyExc_TypeError, "Quaternion() must take list of floats, or ints");
                        return NULL;
                    }
                }
                val = Quaternion(quaternion[3], quaternion[0],
                                 quaternion[1], quaternion[2]);
                }
                break;
            default:
                PyErr_SetString(PyExc_TypeError, "Quaternion must take list of floats, or ints, 4 ints or 4 floats");
                return NULL;
                break;
        }

        o = newPyQuaternion();
        if ( o == NULL ) {
                return NULL;
        }
        o->rotation = val;
        return (PyObject *)o;
}

static PyObject * worldtime_new(PyObject * self, PyObject * args)
{
        PyWorldTime *o;
                
        int seconds;
        if (!PyArg_ParseTuple(args, "i", &seconds)) {
                return NULL;
        }
        o = newPyWorldTime();
        if ( o == NULL ) {
                return NULL;
        }
        o->time = new WorldTime(seconds);
        o->own = true;
        return (PyObject *)o;
}

static inline void addToOplist(PyOperation * op, PyOplist * o)
{
    if (op != NULL) {
       if (PyOperation_Check(op)) {
           o->ops->push_back(op->operation);
           op->own = 0;
       } else if ((PyObject*)op != Py_None) {
           PyErr_SetString(PyExc_TypeError, "Argument must be an op");
           return;
       }
    }
}


static PyObject * oplist_new(PyObject * self, PyObject * args)
{
        PyOplist *o;
        
        PyOperation *op1 = NULL, *op2 = NULL, *op3 = NULL, *op4 = NULL;
        if (!PyArg_ParseTuple(args, "|OOOO", &op1, &op2, &op3, &op4)) {
                return NULL;
        }
        o = newPyOplist();
        if ( o == NULL ) {
                return NULL;
        }
        o->ops = new OpVector();
        addToOplist(op1, o);
        addToOplist(op2, o);
        addToOplist(op3, o);
        addToOplist(op4, o);
        return (PyObject *)o;
}

static PyObject * entity_new(PyObject * self, PyObject * args, PyObject * kwds)
{
    PyMessageElement *o;
    char * id = NULL;
    
    if (!PyArg_ParseTuple(args, "|s", &id)) {
        return NULL;
    }
    MapType omap;
    if (id != NULL) {
        omap["id"] = std::string(id);
    }
    if ((kwds != NULL) && (PyDict_Check(kwds))) {
        PyObject * keys = PyDict_Keys(kwds);
        PyObject * vals = PyDict_Values(kwds);
        if ((keys == NULL) || (vals == NULL)) {
            PyErr_SetString(PyExc_RuntimeError, "Error in keywords");
            return NULL;
        }
        int i, size = PyList_Size(keys); 
        for(i = 0; i < size; i++) {
            char * key = PyString_AsString(PyList_GetItem(keys, i));
            PyObject * val = PyList_GetItem(vals, i);
            if ((strcmp(key, "location") == 0) && (PyLocation_Check(val))) {
                PyLocation * loc = (PyLocation*)val;
                loc->location->addToMessage(omap);
            } else if (strcmp(key, "xyz") == 0) {
                omap["pos"] = PyObject_asMessageElement(val);
            } else if ((strcmp(key, "parent") == 0) && (PyString_Check(val))) {
                omap["loc"] = PyString_AsString(val);
            } else if ((strcmp(key, "type") == 0) && (PyString_Check(val))) {
                omap["parents"] = ListType(1,std::string(PyString_AsString(val)));
                omap["objtype"] = "obj";
            } else {
                Element val_obj = PyObject_asMessageElement(val);
                if (val_obj.getType() == Element::TYPE_NONE) {
                    Py_DECREF(keys);
                    Py_DECREF(vals);
                    PyErr_SetString(PyExc_TypeError, "Arg has no type.");
                    return NULL;
                }
                omap[key] = val_obj;
            }
        }
        Py_DECREF(keys);
        Py_DECREF(vals);
    }

    o = newPyMessageElement();
    if ( o == NULL ) {
        return NULL;
    }
    o->m_obj = new Element(omap);
    return (PyObject *)o;
}

static inline void addToArgs(ListType & args, PyObject * ent)
{
    if (ent == NULL) {
        return;
    }
    if (PyMessageElement_Check(ent)) {
        PyMessageElement * obj = (PyMessageElement*)ent;
        if (obj->m_obj == NULL) {
            log(ERROR, "Operation() Null element object added to new operation arguments.");
            return;
        }
        Element o(*obj->m_obj);
        if (o.isMap() && (obj->Object_attr != NULL)) {
            MapType & ent = o.asMap();
            MapType ent2 = PyDictObject_asElementMap(obj->Object_attr);
            MapType::const_iterator Iend = ent2.end();
            for (MapType::const_iterator I = ent2.begin(); I != Iend; ++I) {
                if (ent.find(I->first) != ent.end()) {
                    ent[I->first] = I->second;
                }
            }
        }
        args.push_back(o);
    } else if (PyOperation_Check(ent)) {
        PyOperation * op = (PyOperation*)ent;
        if (op->operation == NULL) {
            log(ERROR, "Operation() Null operation object added to new operation arguments.");
            return;
        }
        args.push_back(op->operation->asObject());
    } else {
        log(ERROR, "Operation() Unknown object added to operation arguments.");
    }
}

static PyObject * operation_new(PyObject * self, PyObject * args, PyObject * kwds)
{
    PyOperation * op;

    char * type;
    PyObject * arg1 = NULL;
    PyObject * arg2 = NULL;
    PyObject * arg3 = NULL;

    if (!PyArg_ParseTuple(args, "s|OOO", &type, &arg1, &arg2, &arg3)) {
        return NULL;
    }
    op = newPyOperation();
    if (op == NULL) {
        return NULL;
    }
    if (strcmp(type, "tick") == 0) {
        op->operation = new Tick();
    } else if (strcmp(type, "sight") == 0) {
        op->operation = new Sight();
    } else if (strcmp(type, "set") == 0) {
        op->operation = new Set();
    } else if (strcmp(type, "burn") == 0) {
        op->operation = new Burn();
    } else if (strcmp(type, "action") == 0) {
        op->operation = new Action();
    } else if (strcmp(type, "chop") == 0) {
        op->operation = new Chop();
    } else if (strcmp(type, "cut") == 0) {
        op->operation = new Cut();
    } else if (strcmp(type, "create") == 0) {
        op->operation = new Create();
    } else if (strcmp(type, "setup") == 0) {
        op->operation = new Setup();
    } else if (strcmp(type, "look") == 0) {
        op->operation = new Look();
    } else if (strcmp(type, "move") == 0) {
        op->operation = new Move();
    } else if (strcmp(type, "talk") == 0) {
        op->operation = new Talk();
    } else if (strcmp(type, "touch") == 0) {
        op->operation = new Touch();
    } else if (strcmp(type, "eat") == 0) {
        op->operation = new Eat();
    } else if (strcmp(type, "nourish") == 0) {
        op->operation = new Nourish();
    } else if (strcmp(type, "info") == 0) {
        op->operation = new Info();
    } else if (strcmp(type, "thought") == 0 ||
               strcmp(type, "goal_info") == 0) {
        Py_DECREF(op);
        Py_INCREF(Py_None);
        return Py_None;
    } else {
        op->operation = new Generic(type);
    }
    op->own = 1;
    if (PyMapping_HasKeyString(kwds, "to")) {
        PyObject * to = PyMapping_GetItemString(kwds, "to");
        PyObject * to_id = 0;
        if (PyString_Check(to)) {
            to_id = to;
        } else if ((to_id = PyObject_GetAttrString(to, "id")) == NULL) {
            Py_DECREF(to);
            PyErr_SetString(PyExc_TypeError, "to is not a string and has no id");
            return NULL;
        } else {
            // to_id == to.getattr("id") and to is finished with
            Py_DECREF(to);
        }
        if (!PyString_Check(to_id)) {
            Py_DECREF(to_id);
            PyErr_SetString(PyExc_TypeError, "id of to is not a string");
            return NULL;
        }
        op->operation->setTo(PyString_AsString(to_id));
        Py_DECREF(to_id);
    }
    if (PyMapping_HasKeyString(kwds, "from_")) {
        PyObject * from = PyMapping_GetItemString(kwds, "from_");
        PyObject * from_id = 0;
        if (PyString_Check(from)) {
            from_id = from;
        } else if ((from_id = PyObject_GetAttrString(from, "id")) == NULL) {
            Py_DECREF(from);
            PyErr_SetString(PyExc_TypeError, "from is not a string and has no id");
            return NULL;
        } else {
            Py_DECREF(from);
        }
        if (!PyString_Check(from_id)) {
            Py_DECREF(from_id);
            PyErr_SetString(PyExc_TypeError, "id of from is not a string");
            return NULL;
        }
        op->operation->setFrom(PyString_AsString(from_id));
        Py_DECREF(from_id);
    }
    ListType args_list;
    addToArgs(args_list, arg1);
    addToArgs(args_list, arg2);
    addToArgs(args_list, arg3);
    op->operation->setArgs(args_list);
    return (PyObject *)op;
}

static PyObject * set_kw(PyObject * meth_self, PyObject * args)
{
    // Takes self, kw, name, default = None
    PyObject * self;
    PyObject * kw;
    char * name;
    PyObject * def = NULL;

    if (!PyArg_ParseTuple(args, "OOs|O", &self, &kw, &name, &def)) {
        return NULL;
    }
    PyObject * attr = PyObject_GetAttrString(self, "attributes");
    if (attr == NULL) {
        PyErr_SetString(PyExc_AttributeError, "SET_KW: No attributes list");
        return NULL;
    }
    int i = PyList_Size(attr);
    char * entry;
    PyObject * item;
    for(i = 0; i < PyList_Size(attr); i++) {
        item = PyList_GetItem(attr, i);
        if (!PyString_Check(item)) {
            continue;
        }
        entry = PyString_AsString(item);
        if (strcmp(entry, name) == 0) {
            goto list_contains_it;
        }
        // Should I free entry at this point?
    }
    {
      PyObject * namestr = PyString_FromString(name);
      PyList_Append(attr, namestr);
      Py_DECREF(namestr);
    }
list_contains_it:
    Py_DECREF(attr);
    if (!PyDict_Check(kw)) {
        PyErr_SetString(PyExc_TypeError, "SET_KW: kw not a dict");
        return NULL;
    }
    PyObject * value = NULL;
    bool decvalue = false;
    if ((value = PyDict_GetItemString(kw, name)) == NULL) {
        PyObject * copy = PyDict_GetItemString(kw, "copy");
        if ((copy != NULL) && (PyObject_HasAttrString(copy, name))) {
            value = PyObject_GetAttrString(copy, name);
            decvalue = true;
        } else {
            value = def;
        }
    }
    if (value == NULL) {
        Py_INCREF(Py_None);
        value = Py_None;
    }
    PyObject_SetAttrString(self, name, value);

    if (decvalue) { Py_DECREF(value); }
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef atlas_methods[] = {
    {"Operation",  (PyCFunction)operation_new,  METH_VARARGS|METH_KEYWORDS},
    {"isLocation", is_location,                 METH_VARARGS},
    {"Location",   location_new,                METH_VARARGS},
    {"Entity",     (PyCFunction)entity_new,     METH_VARARGS|METH_KEYWORDS},
    {"Message",    oplist_new,                  METH_VARARGS},
    {NULL,          NULL}                       /* Sentinel */
};

static PyMethodDef physics_methods[] = {
    {"distance_to",distance_to,                 METH_VARARGS},
    {"square_distance",square_distance,         METH_VARARGS},
    {NULL,          NULL}                       /* Sentinel */
};

static PyMethodDef vector3d_methods[] = {
    {"Vector3D",    vector3d_new,               METH_VARARGS},
    {NULL,          NULL}                       /* Sentinel */
};

static PyMethodDef point3d_methods[] = {
    {"Point3D",     point3d_new,                 METH_VARARGS},
    {NULL,          NULL}                       /* Sentinel */
};

static PyMethodDef bbox_methods[] = {
    {"BBox",        bbox_new,                 METH_VARARGS},
    {NULL,          NULL}                       /* Sentinel */
};

static PyMethodDef quaternion_methods[] = {
    {"Quaternion",  quaternion_new,             METH_VARARGS},
    {NULL,          NULL}                       /* Sentinel */
};

static PyMethodDef server_methods[] = {
    {"WorldTime",   worldtime_new,              METH_VARARGS},
    {NULL,          NULL}                       /* Sentinel */
};

static PyMethodDef common_methods[] = {
    //{"null",      null_new,                   METH_VARARGS},
    {NULL,          NULL}                       /* Sentinel */
};

static PyMethodDef misc_methods[] = {
    {"set_kw",      set_kw,                     METH_VARARGS},
    {NULL,          NULL}                       /* Sentinel */
};

void init_python_api()
{
    std::string importCmd("ruleset_import_hooks.install([");
    std::vector<std::string>::const_iterator Ibeg = rulesets.begin();
    std::vector<std::string>::const_iterator Iend = rulesets.end();
    for (std::vector<std::string>::const_iterator I = Ibeg; I != Iend; ++I) {
        if (I != Ibeg) {
            importCmd = importCmd + ",";
        }
        importCmd = importCmd + "\"" + *I + "\"";
    }
    importCmd = importCmd + "])\n";

    Py_Initialize();

    PyObject * sys_name = PyString_FromString("sys");
    PyObject * sys_module = PyImport_Import(sys_name);
    Py_DECREF(sys_name);

    if (sys_module == 0) {
        log(CRITICAL, "Python could not import sys module");
        return;
    }

    PyObject * outLogger = (PyObject*)PyObject_NEW(PyLogger, &PyOutLogger_Type);
    PyObject_SetAttrString(sys_module, "stdout", outLogger);
    Py_DECREF(outLogger);

    PyObject * errLogger = (PyObject*)PyObject_NEW(PyLogger, &PyErrLogger_Type);
    PyObject_SetAttrString(sys_module, "stderr", errLogger);
    Py_DECREF(errLogger);

    PyObject * sys_path = PyObject_GetAttrString(sys_module, "path");
    if (sys_path != 0) {
        if (PyList_Check(sys_path)) {
            std::vector<std::string>::const_iterator I = Ibeg;
            for (; I != Iend; ++I) {
                std::string p = share_directory + "/cyphesis/rulesets/" + *I;
                PyObject * path = PyString_FromString(p.c_str());
                PyList_Append(sys_path, path);
                Py_DECREF(path);
            }
        } else {
            log(CRITICAL, "Python sys.path is not a list");
        }
    } else {
        log(CRITICAL, "Python could not import sys.path");
    }
    Py_DECREF(sys_module);

    if (Py_InitModule("atlas", atlas_methods) == NULL) {
        log(CRITICAL, "Python init failed to create atlas module\n");
        return;
    }

    if (Py_InitModule("physics", physics_methods) == NULL) {
        log(CRITICAL, "Python init failed to create physics module\n");
        return;
    }

    if (Py_InitModule("Vector3D", vector3d_methods) == NULL) {
        log(CRITICAL, "Python init failed to create Vector3D module\n");
        return;
    }

    if (Py_InitModule("Point3D", point3d_methods) == NULL) {
        log(CRITICAL, "Python init failed to create Point3D module\n");
        return;
    }

    if (Py_InitModule("BBox", bbox_methods) == NULL) {
        log(CRITICAL, "Python init failed to create BBox module\n");
        return;
    }

    if (Py_InitModule("Quaternion", quaternion_methods) == NULL) {
        log(CRITICAL, "Python init failed to create Quaternion module\n");
        return;
    }

    if (Py_InitModule("misc", misc_methods) == NULL) {
        log(CRITICAL, "Python init failed to create misc module\n");
        return;
    }

    PyObject * common = Py_InitModule("common", common_methods);
    if (common == NULL) {
        log(CRITICAL, "Python init failed to create common module\n");
        return;
    }

    PyObject * common_dict = PyModule_GetDict(common);

    /// Create the common.log module
    PyObject * log_mod = PyModule_New("log");
    PyDict_SetItemString(common_dict, "log", log_mod);

    PyObject * debug = (PyObject*)PyObject_NEW(FunctionObject, &log_debug_type);
    PyObject_SetAttrString(log_mod, "debug", debug);
    Py_DECREF(debug);
    Py_DECREF(log_mod);

    PyObject * o;

    /// Create the common.const module
    PyObject * _const = PyModule_New("const");
    PyDict_SetItemString(common_dict, "const", _const);

    o = PyInt_FromLong(0);
    PyObject_SetAttrString(_const, "server_python", o);
    Py_DECREF(o);
    o = PyInt_FromLong(consts::debug_level);
    PyObject_SetAttrString(_const, "debug_level", o);
    Py_DECREF(o);
    o = PyInt_FromLong(consts::debug_thinking);
    PyObject_SetAttrString(_const, "debug_thinking", o);
    Py_DECREF(o);

    o = PyFloat_FromDouble(consts::time_multiplier);
    PyObject_SetAttrString(_const, "time_multiplier", o);
    Py_DECREF(o);
    o = PyFloat_FromDouble(consts::base_velocity_coefficient);
    PyObject_SetAttrString(_const, "base_velocity_coefficient", o);
    Py_DECREF(o);
    o = PyFloat_FromDouble(consts::base_velocity);
    PyObject_SetAttrString(_const, "base_velocity", o);
    Py_DECREF(o);

    o = PyFloat_FromDouble(consts::basic_tick);
    PyObject_SetAttrString(_const, "basic_tick", o);
    Py_DECREF(o);

    o = PyFloat_FromDouble(consts::sight_range);
    PyObject_SetAttrString(_const, "sight_range", o);
    Py_DECREF(o);
    o = PyFloat_FromDouble(consts::hearing_range);
    PyObject_SetAttrString(_const, "hearing_range", o);
    Py_DECREF(o);
    o = PyInt_FromLong(consts::enable_ranges);
    PyObject_SetAttrString(_const, "enable_ranges", o);
    Py_DECREF(o);
    Py_DECREF(_const);

    /// Create the common.globals module
    PyObject * globals = PyModule_New("globals");
    PyDict_SetItemString(common_dict, "globals", globals);
    o = PyString_FromString(share_directory.c_str());
    PyObject_SetAttrString(globals, "share_directory", o);
    Py_DECREF(o);
    Py_DECREF(globals);

    PyObject * server;
    if ((server = Py_InitModule("server", server_methods)) == NULL) {
        log(CRITICAL, "Python init failed to create server module\n");
        return;
    }
    PyObject * server_dict = PyModule_GetDict(server);
    PyObject * dictlist = PyModule_New("dictlist");
    PyObject * add_value = (PyObject *)PyObject_NEW(FunctionObject, &dictlist_add_value_type);
    PyObject_SetAttrString(dictlist, "add_value", add_value);
    Py_DECREF(add_value);
    PyObject * remove_value = (PyObject *)PyObject_NEW(FunctionObject, &dictlist_remove_value_type);
    PyObject_SetAttrString(dictlist, "remove_value", remove_value);
    Py_DECREF(remove_value);
    PyDict_SetItemString(server_dict, "dictlist", dictlist);
    Py_DECREF(dictlist);

    PyRun_SimpleString("from hooks import ruleset_import_hooks\n");
    PyRun_SimpleString((char *)importCmd.c_str());

    debug(std::cout << Py_GetPath() << std::endl << std::flush;);
}

void shutdown_python_api()
{
    
    Py_Finalize();
}
